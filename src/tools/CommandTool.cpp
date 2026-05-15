/**
 *
 *  @file CommandTool.cpp
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
#include <vix/ai/agent/tools/CommandTool.hpp>

#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <vix/ai/agent/AgentError.hpp>
#include <vix/ai/agent/AgentRunTimer.hpp>
#include <vix/process/Command.hpp>
#include <vix/process/Output.hpp>
#include <vix/process/PipeMode.hpp>
#include <vix/process/ProcessOptions.hpp>

namespace vix::ai::agent
{
  namespace
  {
    [[nodiscard]] bool is_dangerous_program_name(
        std::string_view program) noexcept
    {
      return program == "rm" ||
             program == "rmdir" ||
             program == "mv" ||
             program == "dd" ||
             program == "mkfs" ||
             program == "shutdown" ||
             program == "reboot" ||
             program == "poweroff" ||
             program == "sudo" ||
             program == "su";
    }

    [[nodiscard]] std::vector<std::string> read_args(
        const vix::json::Json &arguments)
    {
      std::vector<std::string> args;

      if (!arguments.contains("args") || !arguments["args"].is_array())
      {
        return args;
      }

      for (const auto &item : arguments["args"])
      {
        if (item.is_string())
        {
          args.push_back(item.get<std::string>());
        }
      }

      return args;
    }
  } // namespace

  CommandTool::CommandTool(
      AgentWorkspace workspace,
      AgentConfig config)
      : workspace_(std::move(workspace)),
        config_(std::move(config))
  {
    block_program("rm");
    block_program("rmdir");
    block_program("mv");
    block_program("dd");
    block_program("mkfs");
    block_program("shutdown");
    block_program("reboot");
    block_program("poweroff");
    block_program("sudo");
    block_program("su");
  }

  std::string_view CommandTool::name() const noexcept
  {
    return "command.run";
  }

  std::string_view CommandTool::description() const noexcept
  {
    return "Run a controlled local command inside the agent workspace.";
  }

  AgentResult<ToolResult> CommandTool::run(
      const ToolCall &call)
  {
    AgentRunTimer timer;

    if (!config_.allow_process)
    {
      return make_agent_error(
          AgentErrorCode::ToolNotAllowed,
          "local command execution is disabled");
    }

    if (!call.valid())
    {
      return make_agent_error(
          AgentErrorCode::ToolFailed,
          "command.run received an invalid tool call");
    }

    if (!call.arguments.is_object())
    {
      return make_agent_error(
          AgentErrorCode::ToolFailed,
          "command.run arguments must be a JSON object");
    }

    if (!call.arguments.contains("program") ||
        !call.arguments["program"].is_string())
    {
      return make_agent_error(
          AgentErrorCode::ToolFailed,
          "command.run requires a string argument: program");
    }

    const std::string program = call.arguments["program"].get<std::string>();
    if (program.empty())
    {
      return make_agent_error(
          AgentErrorCode::ToolFailed,
          "command.run program cannot be empty");
    }

    if (!is_allowed_program(program))
    {
      return make_agent_error(
          AgentErrorCode::ToolNotAllowed,
          "program is blocked by the agent command policy: " + program);
    }

    std::string working_directory = ".";
    if (call.arguments.contains("working_directory") &&
        call.arguments["working_directory"].is_string())
    {
      working_directory = call.arguments["working_directory"].get<std::string>();
    }

    auto resolved_workdir = workspace_.resolve_inside(working_directory);
    if (!resolved_workdir)
    {
      return resolved_workdir.error();
    }

    vix::process::Command command(program);

    command.args(read_args(call.arguments))
        .stdout_mode(vix::process::PipeMode::Pipe)
        .stderr_mode(vix::process::PipeMode::Pipe)
        .stdin_mode(vix::process::PipeMode::Null)
        .search_in_path(true)
        .detach(false)
        .inherit_environment(true)
        .cwd(resolved_workdir.value());

    auto output = vix::process::output(command);
    if (!output)
    {
      ToolResult failed = ToolResult::failure(
          call.id,
          std::string(name()),
          std::string(output.error().message()));

      failed.duration_ms = timer.elapsed_ms();
      return failed;
    }

    std::string combined;
    combined.reserve(
        output.value().stdout_text.size() +
        output.value().stderr_text.size() +
        64);

    combined += output.value().stdout_text;

    if (!output.value().stderr_text.empty())
    {
      if (!combined.empty())
      {
        combined += "\n";
      }

      combined += output.value().stderr_text;
    }

    combined = truncate_output(std::move(combined));

    vix::json::Json data = vix::json::Json::object();
    data["program"] = program;
    data["working_directory"] = resolved_workdir.value();
    data["exit_code"] = output.value().exit_code;
    data["success"] = output.value().success();

    ToolResult result = ToolResult::success(
        call.id,
        std::string(name()),
        std::move(combined),
        std::move(data));

    result.duration_ms = timer.elapsed_ms();
    return result;
  }

  bool CommandTool::is_allowed_program(
      std::string_view program) const noexcept
  {
    if (program.empty())
    {
      return false;
    }

    if (is_dangerous_program_name(program))
    {
      return false;
    }

    return blocked_programs_.find(std::string(program)) ==
           blocked_programs_.end();
  }

  void CommandTool::block_program(std::string program)
  {
    if (program.empty())
    {
      return;
    }

    blocked_programs_.insert(std::move(program));
  }

  const AgentWorkspace &CommandTool::workspace() const noexcept
  {
    return workspace_;
  }

  const AgentConfig &CommandTool::config() const noexcept
  {
    return config_;
  }

  std::string CommandTool::truncate_output(
      std::string output) const
  {
    if (config_.max_tool_output == 0)
    {
      return {};
    }

    if (output.size() <= config_.max_tool_output)
    {
      return output;
    }

    output.resize(config_.max_tool_output);
    output += "\n[output truncated]";
    return output;
  }

} // namespace vix::ai::agent
