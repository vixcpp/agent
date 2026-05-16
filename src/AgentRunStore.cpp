/**
 *
 *  @file AgentRunStore.cpp
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
#include <vix/ai/agent/AgentRunStore.hpp>

#include <string>
#include <string_view>
#include <utility>

#include <vix/ai/agent/AgentError.hpp>
#include <vix/fs/EnsureDirectory.hpp>
#include <vix/fs/WriteText.hpp>
#include <vix/json/json.hpp>
#include <vix/path/Join.hpp>

namespace vix::ai::agent
{
  namespace
  {
    [[nodiscard]] const char *status_to_string(
        AgentResponseStatus status) noexcept
    {
      switch (status)
      {
      case AgentResponseStatus::Completed:
        return "completed";
      case AgentResponseStatus::Failed:
        return "failed";
      case AgentResponseStatus::Cancelled:
        return "cancelled";
      case AgentResponseStatus::Partial:
        return "partial";
      }

      return "unknown";
    }

    [[nodiscard]] const char *model_status_to_string(
        ModelResponseStatus status) noexcept
    {
      switch (status)
      {
      case ModelResponseStatus::Completed:
        return "completed";
      case ModelResponseStatus::Failed:
        return "failed";
      case ModelResponseStatus::Cancelled:
        return "cancelled";
      case ModelResponseStatus::Partial:
        return "partial";
      }

      return "unknown";
    }

    [[nodiscard]] const char *mode_to_string(
        AgentRequestMode mode) noexcept
    {
      switch (mode)
      {
      case AgentRequestMode::Run:
        return "run";
      case AgentRequestMode::Analyze:
        return "analyze";
      case AgentRequestMode::Explain:
        return "explain";
      case AgentRequestMode::Chat:
        return "chat";
      }

      return "unknown";
    }
  } // namespace

  AgentRunStore::AgentRunStore(AgentWorkspace workspace)
      : workspace_(std::move(workspace))
  {
  }

  vix::error::Error AgentRunStore::create_run(
      std::string_view run_id,
      const AgentRequest &request) const
  {
    auto dir = workspace_.run_dir(run_id);
    if (!dir)
    {
      return dir.error();
    }

    auto created = vix::fs::ensure_directory(dir.value());
    if (!created)
    {
      return created.error();
    }

    vix::json::Json json = vix::json::Json::object();
    json["run_id"] = std::string(run_id);
    json["input"] = request.input;
    json["workspace"] = request.workspace;
    json["mode"] = mode_to_string(request.mode);
    json["allow_tools"] = request.allow_tools;
    json["allow_process"] = request.allow_process;
    json["allow_file_read"] = request.allow_file_read;
    json["allow_file_write"] = request.allow_file_write;
    json["use_cache"] = request.use_cache;

    return write_text_file(run_id, "run.json", json.dump(2));
  }

  vix::error::Error AgentRunStore::save_prompt(
      std::string_view run_id,
      const ModelRequest &request) const
  {
    return write_text_file(run_id, "prompt.txt", request.prompt);
  }

  vix::error::Error AgentRunStore::save_model_response(
      std::string_view run_id,
      const ModelResponse &response) const
  {
    vix::json::Json json = vix::json::Json::object();

    json["text"] = response.text;
    json["model"] = response.model;
    json["provider"] = response.provider;
    json["status"] = model_status_to_string(response.status);
    json["error"] = response.error;
    json["duration_ms"] = response.duration_ms;
    json["input_tokens"] = response.usage.input_tokens;
    json["output_tokens"] = response.usage.output_tokens;
    json["total_tokens"] = response.usage.total_tokens;
    json["raw"] = response.raw;

    return write_text_file(run_id, "model_response.json", json.dump(2));
  }

  vix::error::Error AgentRunStore::save_tool_results(
      std::string_view run_id,
      const std::vector<ToolResult> &results) const
  {
    vix::json::Json items = vix::json::Json::array();

    for (const auto &result : results)
    {
      vix::json::Json item = vix::json::Json::object();
      item["id"] = result.id;
      item["name"] = result.name;
      item["ok"] = result.ok;
      item["output"] = result.output;
      item["error"] = result.error;
      item["duration_ms"] = result.duration_ms;
      item["data"] = result.data;

      items.push_back(std::move(item));
    }

    return write_text_file(run_id, "tools.json", items.dump(2));
  }

  vix::error::Error AgentRunStore::save_final_response(
      std::string_view run_id,
      const AgentResponse &response) const
  {
    vix::json::Json json = vix::json::Json::object();

    json["text"] = response.text;
    json["run_id"] = response.run_id;
    json["model"] = response.model;
    json["provider"] = response.provider;
    json["status"] = status_to_string(response.status);
    json["duration_ms"] = response.duration_ms;
    json["from_cache"] = response.from_cache;
    json["metadata"] = response.metadata;

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

    auto err = write_text_file(run_id, "response.json", json.dump(2));
    if (err)
    {
      return err;
    }

    return write_text_file(run_id, "response.txt", response.text);
  }

  vix::error::Error AgentRunStore::save_error(
      std::string_view run_id,
      const vix::error::Error &error) const
  {
    vix::json::Json json = vix::json::Json::object();

    json["message"] = std::string(error.message());
    json["category"] = std::string(error.category().name());
    json["code"] = static_cast<int>(error.code());

    return write_text_file(run_id, "error.json", json.dump(2));
  }

  AgentResult<std::string> AgentRunStore::run_file(
      std::string_view run_id,
      std::string_view name) const
  {
    auto dir = workspace_.run_dir(run_id);
    if (!dir)
    {
      return dir.error();
    }

    auto path = vix::path::join(dir.value(), name);
    if (!path)
    {
      return path.error();
    }

    return path.value();
  }

  vix::error::Error AgentRunStore::write_text_file(
      std::string_view run_id,
      std::string_view name,
      std::string_view content) const
  {
    auto path = run_file(run_id, name);
    if (!path)
    {
      return path.error();
    }

    auto written = vix::fs::write_text(path.value(), std::string(content));
    if (!written)
    {
      return written.error();
    }

    return {};
  }

} // namespace vix::ai::agent
