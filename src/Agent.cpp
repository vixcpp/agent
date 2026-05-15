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

#include <vix/ai/agent/AgentError.hpp>
#include <vix/ai/agent/AgentRunTimer.hpp>
#include <vix/ai/agent/crypto/AgentId.hpp>
#include <vix/ai/agent/model/OllamaProvider.hpp>
#include <vix/ai/agent/tools/CommandTool.hpp>
#include <vix/ai/agent/tools/FileReadTool.hpp>
#include <vix/ai/agent/workspace/FileReader.hpp>
#include <vix/ai/agent/workspace/FileScanPolicy.hpp>
#include <vix/ai/agent/workspace/ProjectScanner.hpp>

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

    auto model_response = provider_->generate(model_request.value());
    if (!model_response)
    {
      return model_response.error();
    }

    return build_response(
        request,
        model_response.value(),
        run_id.value(),
        timer.elapsed_ms());
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
