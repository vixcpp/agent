/**
 *
 *  @file Agent.cpp
 *  @author Gaspard Kirira
 *
 *  Copyright 2026, Gaspard Kirira.
 *  All rights reserved.
 *  https://github.com/vixcpp/vix
 *
 *  Use of this source code is governed by a MIT license
 *  that can be found in the License file.
 *
 *  Vix.cpp
 *
 */
#include <vix/ai/agent/Agent.hpp>

#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <chrono>
#include <optional>

#include <vix/ai/agent/AgentError.hpp>
#include <vix/ai/agent/AgentRunTimer.hpp>
#include <vix/ai/agent/crypto/AgentId.hpp>
#include <vix/ai/agent/model/OllamaProvider.hpp>
#include <vix/ai/agent/tools/CommandTool.hpp>
#include <vix/ai/agent/tools/FileReadTool.hpp>
#include <vix/ai/agent/workspace/FileReader.hpp>
#include <vix/ai/agent/workspace/FileScanPolicy.hpp>
#include <vix/ai/agent/workspace/ProjectScanner.hpp>
#include <vix/ai/agent/AgentConfigValidator.hpp>
#include <vix/ai/agent/tools/ToolResult.hpp>
#include <vix/ai/agent/AgentRunStore.hpp>
#include <vix/ai/agent/crypto/AgentFingerprint.hpp>
#include <vix/cache/Cache.hpp>
#include <vix/cache/CacheContext.hpp>
#include <vix/cache/CacheEntry.hpp>
#include <vix/cache/CachePolicy.hpp>
#include <vix/cache/FileStore.hpp>
#include <vix/json/json.hpp>

namespace vix::ai::agent
{
  namespace
  {
    [[nodiscard]] std::string mode_to_instruction(
        AgentRequestMode mode)
    {
      switch (mode)
      {
      case AgentRequestMode::Run:
        return "You are Vix Agent. Answer clearly and focus on the user's request.";

      case AgentRequestMode::Analyze:
        return "You are Vix Agent. Analyze the workspace and explain the important findings.";

      case AgentRequestMode::Explain:
        return "You are Vix Agent. Explain the topic clearly without modifying files.";

      case AgentRequestMode::Chat:
        return "You are Vix Agent. Continue the conversation in a helpful and practical way.";
      }

      return "You are Vix Agent.";
    }

    [[nodiscard]] std::string build_file_list_context(
        const ProjectScanResult &scan)
    {
      std::string context;
      context += "Workspace root:\n";
      context += scan.root;
      context += "\n\nFiles:\n";

      for (const auto &file : scan.files)
      {
        context += "- ";
        context += file.relative_path;
        context += " (";
        context += std::to_string(file.size);
        context += " bytes)\n";
      }

      if (scan.truncated)
      {
        context += "\nFile list was truncated because max_files was reached.\n";
      }

      return context;
    }

    [[nodiscard]] std::int64_t now_ms()
    {
      const auto now = std::chrono::system_clock::now();
      const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
          now.time_since_epoch());

      return static_cast<std::int64_t>(ms.count());
    }

    [[nodiscard]] vix::json::Json agent_response_to_json(
        const AgentResponse &response)
    {
      vix::json::Json json = vix::json::Json::object();

      json["text"] = response.text;
      json["run_id"] = response.run_id;
      json["model"] = response.model;
      json["provider"] = response.provider;
      json["duration_ms"] = response.duration_ms;
      json["from_cache"] = response.from_cache;
      json["metadata"] = response.metadata;

      switch (response.status)
      {
      case AgentResponseStatus::Completed:
        json["status"] = "completed";
        break;

      case AgentResponseStatus::Failed:
        json["status"] = "failed";
        break;

      case AgentResponseStatus::Cancelled:
        json["status"] = "cancelled";
        break;

      case AgentResponseStatus::Partial:
        json["status"] = "partial";
        break;
      }

      vix::json::Json tools = vix::json::Json::array();

      for (const auto &tool : response.tools)
      {
        vix::json::Json item = vix::json::Json::object();
        item["id"] = tool.id;
        item["name"] = tool.name;
        item["ok"] = tool.ok;
        item["duration_ms"] = tool.duration_ms;
        item["error"] = tool.error;

        tools.push_back(std::move(item));
      }

      json["tools"] = std::move(tools);

      return json;
    }

    [[nodiscard]] AgentResponse agent_response_from_json(
        const vix::json::Json &json)
    {
      AgentResponse response;

      if (json.contains("text") && json["text"].is_string())
      {
        response.text = json["text"].get<std::string>();
      }

      if (json.contains("run_id") && json["run_id"].is_string())
      {
        response.run_id = json["run_id"].get<std::string>();
      }

      if (json.contains("model") && json["model"].is_string())
      {
        response.model = json["model"].get<std::string>();
      }

      if (json.contains("provider") && json["provider"].is_string())
      {
        response.provider = json["provider"].get<std::string>();
      }

      if (json.contains("duration_ms") && json["duration_ms"].is_number_unsigned())
      {
        response.duration_ms = json["duration_ms"].get<std::uint64_t>();
      }

      if (json.contains("metadata") && json["metadata"].is_object())
      {
        response.metadata = json["metadata"];
      }

      response.from_cache = true;
      response.status = AgentResponseStatus::Completed;

      if (json.contains("status") && json["status"].is_string())
      {
        const std::string status = json["status"].get<std::string>();

        if (status == "failed")
        {
          response.status = AgentResponseStatus::Failed;
        }
        else if (status == "cancelled")
        {
          response.status = AgentResponseStatus::Cancelled;
        }
        else if (status == "partial")
        {
          response.status = AgentResponseStatus::Partial;
        }
      }

      if (json.contains("tools") && json["tools"].is_array())
      {
        for (const auto &item : json["tools"])
        {
          if (!item.is_object())
          {
            continue;
          }

          AgentToolSummary tool;

          if (item.contains("id") && item["id"].is_string())
          {
            tool.id = item["id"].get<std::string>();
          }

          if (item.contains("name") && item["name"].is_string())
          {
            tool.name = item["name"].get<std::string>();
          }

          if (item.contains("ok") && item["ok"].is_boolean())
          {
            tool.ok = item["ok"].get<bool>();
          }

          if (item.contains("duration_ms") &&
              item["duration_ms"].is_number_unsigned())
          {
            tool.duration_ms = item["duration_ms"].get<std::uint64_t>();
          }

          if (item.contains("error") && item["error"].is_string())
          {
            tool.error = item["error"].get<std::string>();
          }

          response.tools.push_back(std::move(tool));
        }
      }

      return response;
    }

    [[nodiscard]] vix::cache::Cache make_agent_response_cache(
        const AgentWorkspace &workspace,
        const AgentConfig &config)
    {
      auto cache_dir = workspace.cache_dir();

      vix::cache::FileStore::Config store_config;

      if (cache_dir)
      {
        store_config.file_path = cache_dir.value() + "/model_responses.json";
      }

      store_config.pretty_json = true;

      vix::cache::CachePolicy policy;
      policy.ttl_ms = static_cast<std::int64_t>(config.cache_ttl_ms);
      policy.stale_if_error_ms = static_cast<std::int64_t>(config.cache_ttl_ms);
      policy.stale_if_offline_ms = static_cast<std::int64_t>(config.cache_ttl_ms);
      policy.allow_stale_if_error = false;
      policy.allow_stale_if_offline = false;

      return vix::cache::Cache(
          policy,
          std::make_shared<vix::cache::FileStore>(std::move(store_config)));
    }

  } // namespace

  Agent::Agent()
      : Agent(AgentConfig{})
  {
  }

  Agent::Agent(AgentConfig config)
      : config_(std::move(config))
  {
    ensure_default_provider();
  }

  Agent::Agent(
      AgentConfig config,
      std::shared_ptr<ModelProvider> provider)
      : config_(std::move(config)),
        provider_(std::move(provider))
  {
    ensure_default_provider();
  }

  AgentResult<AgentResponse> Agent::run(
      const AgentRequest &request)
  {
    AgentRunTimer timer;

    auto config_error = AgentConfigValidator::validate(config_);
    if (config_error)
    {
      return config_error;
    }

    auto validation = validate_request(request);
    if (validation)
    {
      return validation;
    }

    auto run_id = AgentId::run_id();
    if (!run_id)
    {
      return run_id.error();
    }

    auto workspace = prepare_workspace(request);
    if (!workspace)
    {
      return workspace.error();
    }

    AgentRunStore run_store(workspace.value());

    if (config_.persist_memory)
    {
      auto err = run_store.create_run(run_id.value(), request);
      if (err)
      {
        return err;
      }
    }

    if (tools_.empty())
    {
      FileScanPolicy policy(config_);
      FileReader reader(workspace.value(), policy);

      auto file_tool = std::make_shared<FileReadTool>(reader);
      auto err = tools_.add(file_tool);
      if (err)
      {
        return err;
      }

      if (config_.allow_process && request.allow_process)
      {
        auto command_tool = std::make_shared<CommandTool>(
            workspace.value(),
            config_);

        err = tools_.add(command_tool);
        if (err)
        {
          return err;
        }
      }
    }

    ensure_default_provider();

    if (!provider_)
    {
      return make_agent_error(
          AgentErrorCode::ModelUnavailable,
          "no model provider configured");
    }

    auto model_request = build_model_request(request, workspace.value());
    if (!model_request)
    {
      return model_request.error();
    }

    auto current_request = model_request.value();
    std::vector<ToolResult> all_tool_results;

    if (config_.persist_memory)
    {
      auto err = run_store.save_prompt(run_id.value(), current_request);
      if (err)
      {
        return err;
      }
    }

    if (config_.use_cache && request.use_cache && request.allow_tools == false)
    {
      auto cached = try_load_cached_response(
          workspace.value(),
          current_request,
          run_id.value());

      if (!cached)
      {
        return cached.error();
      }

      if (cached.value().has_value())
      {
        return cached.value().value();
      }
    }

    ModelResponse final_model_response;

    for (unsigned round = 0; round < config_.max_tool_rounds; ++round)
    {
      auto model_response = provider_->generate(current_request);
      if (!model_response)
      {
        return model_response.error();
      }

      final_model_response = model_response.value();

      if (!final_model_response.ok())
      {
        return make_agent_error(
            AgentErrorCode::ModelRequestFailed,
            final_model_response.error.empty()
                ? "model response failed"
                : final_model_response.error);
      }

      if (!request.allow_tools ||
          final_model_response.tool_calls.empty())
      {
        AgentResponse response = build_response(
            request,
            final_model_response,
            run_id.value(),
            timer.elapsed_ms());

        response.tools = build_tool_summaries(all_tool_results);

        if (config_.persist_memory)
        {
          auto err = run_store.save_model_response(
              run_id.value(),
              final_model_response);

          if (err)
          {
            return err;
          }

          err = run_store.save_final_response(
              run_id.value(),
              response);

          if (err)
          {
            return err;
          }
        }

        if (config_.use_cache && request.use_cache && response.tools.empty())
        {
          auto err = store_cached_response(
              workspace.value(),
              current_request,
              response);

          if (err)
          {
            return err;
          }
        }

        return response;
      }

      auto tool_results = run_tool_calls(final_model_response);
      if (!tool_results)
      {
        return tool_results.error();
      }

      for (const auto &tool_result : tool_results.value())
      {
        all_tool_results.push_back(tool_result);
      }

      append_tool_results(current_request, tool_results.value());

      if (config_.persist_memory)
      {
        auto err = run_store.save_tool_results(run_id.value(), all_tool_results);
        if (err)
        {
          return err;
        }
      }
    }

    AgentResponse response = build_response(
        request,
        final_model_response,
        run_id.value(),
        timer.elapsed_ms());

    response.status = AgentResponseStatus::Partial;
    response.tools = build_tool_summaries(all_tool_results);

    if (config_.persist_memory)
    {
      auto err = run_store.save_model_response(
          run_id.value(),
          final_model_response);

      if (err)
      {
        return err;
      }

      err = run_store.save_final_response(
          run_id.value(),
          response);

      if (err)
      {
        return err;
      }
    }

    if (config_.use_cache && request.use_cache && response.tools.empty())
    {
      auto err = store_cached_response(
          workspace.value(),
          current_request,
          response);

      if (err)
      {
        return err;
      }
    }

    return response;
  }

  void Agent::set_model_provider(
      std::shared_ptr<ModelProvider> provider)
  {
    provider_ = std::move(provider);
  }

  std::shared_ptr<ModelProvider> Agent::model_provider() const noexcept
  {
    return provider_;
  }

  ToolRegistry &Agent::tools() noexcept
  {
    return tools_;
  }

  const ToolRegistry &Agent::tools() const noexcept
  {
    return tools_;
  }

  const AgentConfig &Agent::config() const noexcept
  {
    return config_;
  }

  void Agent::set_config(AgentConfig config)
  {
    config_ = std::move(config);
  }

  vix::error::Error Agent::validate_request(
      const AgentRequest &request) const
  {
    if (request.input.empty())
    {
      return make_agent_error(
          AgentErrorCode::EmptyInput,
          "agent request input cannot be empty");
    }

    if (!config_.allow_file_read && request.allow_file_read)
    {
      return make_agent_error(
          AgentErrorCode::ToolNotAllowed,
          "file reading is disabled by agent configuration");
    }

    if (!config_.allow_process && request.allow_process)
    {
      return make_agent_error(
          AgentErrorCode::ToolNotAllowed,
          "process execution is disabled by agent configuration");
    }

    if (!config_.allow_file_write && request.allow_file_write)
    {
      return make_agent_error(
          AgentErrorCode::ToolNotAllowed,
          "file writing is disabled by agent configuration");
    }

    return {};
  }

  AgentResult<AgentWorkspace> Agent::prepare_workspace(
      const AgentRequest &request) const
  {
    return AgentWorkspace::open(request.workspace, config_);
  }

  AgentResult<ModelRequest> Agent::build_model_request(
      const AgentRequest &request,
      const AgentWorkspace &workspace) const
  {
    ModelRequest out;

    out.model = request.model_override.empty()
                    ? config_.model
                    : request.model_override;

    out.timeout_ms = config_.timeout_ms;
    out.stream = false;

    out.system_prompt = mode_to_instruction(request.mode);

    std::string context;

    if (!request.context.empty())
    {
      context += "User provided context:\n";
      context += request.context;
      context += "\n\n";
    }

    if (request.allow_file_read && config_.allow_file_read)
    {
      FileScanPolicy policy(config_);
      ProjectScanner scanner(workspace, policy);

      auto scan = scanner.scan();
      if (scan)
      {
        context += build_file_list_context(scan.value());
        context += "\n";
      }
    }

    out.prompt.reserve(
        request.input.size() +
        context.size() +
        out.system_prompt.size() +
        64);

    out.prompt += out.system_prompt;
    out.prompt += "\n\n";

    if (!context.empty())
    {
      out.prompt += context;
      out.prompt += "\n";
    }

    out.prompt += "User request:\n";
    out.prompt += request.input;

    if (out.prompt.size() > config_.max_context_chars)
    {
      out.prompt.resize(config_.max_context_chars);
      out.prompt += "\n[context truncated]";
    }

    ModelMessage user_message;
    user_message.role = ModelMessageRole::User;
    user_message.content = out.prompt;

    out.messages.push_back(std::move(user_message));

    return out;
  }

  AgentResponse Agent::build_response(
      const AgentRequest &request,
      const ModelResponse &model_response,
      std::string run_id,
      std::uint64_t duration_ms) const
  {
    (void)request;

    AgentResponse response;
    response.text = model_response.text;
    response.run_id = std::move(run_id);
    response.model = model_response.model;
    response.provider = model_response.provider;
    response.duration_ms = duration_ms;

    switch (model_response.status)
    {
    case ModelResponseStatus::Completed:
      response.status = AgentResponseStatus::Completed;
      break;

    case ModelResponseStatus::Failed:
      response.status = AgentResponseStatus::Failed;
      break;

    case ModelResponseStatus::Cancelled:
      response.status = AgentResponseStatus::Cancelled;
      break;

    case ModelResponseStatus::Partial:
      response.status = AgentResponseStatus::Partial;
      break;
    }

    response.metadata = vix::json::Json::object();
    response.metadata["model_duration_ms"] = model_response.duration_ms;
    response.metadata["input_tokens"] = model_response.usage.input_tokens;
    response.metadata["output_tokens"] = model_response.usage.output_tokens;
    response.metadata["total_tokens"] = model_response.usage.total_tokens;

    return response;
  }

  AgentResult<std::vector<ToolResult>> Agent::run_tool_calls(
      const ModelResponse &model_response) const
  {
    std::vector<ToolResult> results;
    results.reserve(model_response.tool_calls.size());

    for (const auto &call : model_response.tool_calls)
    {
      if (!call.valid())
      {
        return make_agent_error(
            AgentErrorCode::ToolFailed,
            "model produced an invalid tool call");
      }

      auto result = tools_.run(call);
      if (!result)
      {
        return result.error();
      }

      results.push_back(result.value());
    }

    return results;
  }

  void Agent::append_tool_results(
      ModelRequest &request,
      const std::vector<ToolResult> &tool_results) const
  {
    for (const auto &result : tool_results)
    {
      ModelMessage message;
      message.role = ModelMessageRole::Tool;
      message.tool_name = result.name;
      message.tool_call_id = result.id;

      if (result.ok)
      {
        message.content = result.output;
      }
      else
      {
        message.content = "Tool failed: " + result.error;
      }

      request.messages.push_back(std::move(message));
    }

    request.prompt.clear();
  }

  std::vector<AgentToolSummary> Agent::build_tool_summaries(
      const std::vector<ToolResult> &tool_results) const
  {
    std::vector<AgentToolSummary> summaries;
    summaries.reserve(tool_results.size());

    for (const auto &result : tool_results)
    {
      AgentToolSummary summary;
      summary.id = result.id;
      summary.name = result.name;
      summary.ok = result.ok;
      summary.duration_ms = result.duration_ms;
      summary.error = result.error;

      summaries.push_back(std::move(summary));
    }

    return summaries;
  }

  AgentResult<std::string> Agent::build_model_cache_key(
      const ModelRequest &request) const
  {
    std::string material;

    material += request.model;
    material += "\n";
    material += request.system_prompt;
    material += "\n";
    material += request.prompt;
    material += "\n";

    for (const auto &message : request.messages)
    {
      material += std::to_string(static_cast<int>(message.role));
      material += "\n";
      material += message.tool_name;
      material += "\n";
      material += message.tool_call_id;
      material += "\n";
      material += message.content;
      material += "\n";
    }

    auto hash = AgentFingerprint::prompt_fingerprint(
        config_.provider,
        request.model,
        material);

    if (!hash)
    {
      return hash.error();
    }

    return AgentFingerprint::model_response_key(hash.value());
  }

  AgentResult<std::optional<AgentResponse>> Agent::try_load_cached_response(
      const AgentWorkspace &workspace,
      const ModelRequest &request,
      std::string_view run_id) const
  {
    if (!config_.use_cache)
    {
      return std::optional<AgentResponse>{};
    }

    auto cache_key = build_model_cache_key(request);
    if (!cache_key)
    {
      return cache_key.error();
    }

    auto cache = make_agent_response_cache(workspace, config_);

    auto entry = cache.get(
        cache_key.value(),
        now_ms(),
        vix::cache::CacheContext::Online());

    if (!entry)
    {
      return std::optional<AgentResponse>{};
    }

    try
    {
      auto json = vix::json::Json::parse(entry->body);

      AgentResponse response = agent_response_from_json(json);
      response.run_id = std::string(run_id);
      response.from_cache = true;

      return std::optional<AgentResponse>{std::move(response)};
    }
    catch (...)
    {
      return std::optional<AgentResponse>{};
    }
  }

  vix::error::Error Agent::store_cached_response(
      const AgentWorkspace &workspace,
      const ModelRequest &request,
      const AgentResponse &response) const
  {
    if (!config_.use_cache)
    {
      return {};
    }

    if (!response.ok())
    {
      return {};
    }

    if (!response.tools.empty())
    {
      return {};
    }

    auto cache_key = build_model_cache_key(request);
    if (!cache_key)
    {
      return cache_key.error();
    }

    auto cache = make_agent_response_cache(workspace, config_);

    vix::cache::CacheEntry entry;
    entry.status = 200;
    entry.body = agent_response_to_json(response).dump(2);
    entry.created_at_ms = now_ms();
    entry.headers["content-type"] = "application/json";

    cache.put(cache_key.value(), entry);

    return {};
  }

  void Agent::ensure_default_provider()
  {
    if (provider_)
    {
      return;
    }

    if (config_.provider == "ollama")
    {
      provider_ = std::make_shared<OllamaProvider>(config_);
    }
  }

} // namespace vix::ai::agent
