/**
 *
 *  @file FileReadTool.cpp
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
#include <vix/ai/agent/tools/FileReadTool.hpp>

#include <string>
#include <string_view>
#include <utility>

#include <vix/ai/agent/AgentError.hpp>
#include <vix/ai/agent/AgentRunTimer.hpp>

namespace vix::ai::agent
{
  FileReadTool::FileReadTool(FileReader reader)
      : reader_(std::move(reader))
  {
  }

  std::string_view FileReadTool::name() const noexcept
  {
    return "file.read";
  }

  std::string_view FileReadTool::description() const noexcept
  {
    return "Read a text file from the agent workspace.";
  }

  AgentResult<ToolResult> FileReadTool::run(
      const ToolCall &call)
  {
    AgentRunTimer timer;

    if (!call.valid())
    {
      return make_agent_error(
          AgentErrorCode::ToolFailed,
          "file.read received an invalid tool call");
    }

    if (!call.arguments.is_object())
    {
      return make_agent_error(
          AgentErrorCode::ToolFailed,
          "file.read arguments must be a JSON object");
    }

    if (!call.arguments.contains("path") ||
        !call.arguments["path"].is_string())
    {
      return make_agent_error(
          AgentErrorCode::ToolFailed,
          "file.read requires a string argument: path");
    }

    const std::string path = call.arguments["path"].get<std::string>();

    auto read = reader_.read_text(path);
    if (!read)
    {
      ToolResult failed = ToolResult::failure(
          call.id,
          std::string(name()),
          std::string(read.error().message()));

      failed.duration_ms = timer.elapsed_ms();
      return failed;
    }

    vix::json::Json data = vix::json::Json::object();
    data["path"] = read.value().path;
    data["relative_path"] = read.value().relative_path;
    data["size"] = read.value().size;

    ToolResult result = ToolResult::success(
        call.id,
        std::string(name()),
        read.value().content,
        std::move(data));

    result.duration_ms = timer.elapsed_ms();
    return result;
  }

  const FileReader &FileReadTool::reader() const noexcept
  {
    return reader_;
  }

} // namespace vix::ai::agent
