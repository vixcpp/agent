/**
 *
 *  @file CommandToolTest.cpp
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
#include <cassert>
#include <string>

#include <vix/ai/agent/AgentConfig.hpp>
#include <vix/ai/agent/AgentWorkspace.hpp>
#include <vix/ai/agent/tools/CommandTool.hpp>
#include <vix/ai/agent/tools/ToolCall.hpp>
#include <vix/fs/EnsureDirectory.hpp>
#include <vix/json/json.hpp>

namespace
{
  vix::ai::agent::CommandTool make_command_tool(
      const std::string &root,
      vix::ai::agent::AgentConfig config)
  {
    auto workspace = vix::ai::agent::AgentWorkspace::open(root, config);
    assert(workspace);

    return vix::ai::agent::CommandTool(workspace.value(), config);
  }

  vix::ai::agent::ToolCall make_command_call(
      const std::string &program)
  {
    vix::ai::agent::ToolCall call;
    call.id = "tool_command_test";
    call.name = "command.run";
    call.arguments = vix::json::Json::object();
    call.arguments["program"] = program;
    call.arguments["args"] = vix::json::Json::array();
    call.arguments["working_directory"] = ".";

    return call;
  }

  void test_command_tool_rejects_when_process_disabled()
  {
    vix::fs::ensure_directory(".vix-agent-command-disabled");

    vix::ai::agent::AgentConfig config;
    config.allow_process = false;

    auto tool = make_command_tool(".vix-agent-command-disabled", config);
    auto call = make_command_call("echo");

    auto result = tool.run(call);

    assert(!result);
  }

  void test_command_tool_rejects_invalid_tool_call()
  {
    vix::fs::ensure_directory(".vix-agent-command-invalid");

    vix::ai::agent::AgentConfig config;
    config.allow_process = true;

    auto tool = make_command_tool(".vix-agent-command-invalid", config);

    vix::ai::agent::ToolCall call;
    call.name = "command.run";

    auto result = tool.run(call);

    assert(!result);
  }

  void test_command_tool_rejects_blocked_program()
  {
    vix::fs::ensure_directory(".vix-agent-command-blocked");

    vix::ai::agent::AgentConfig config;
    config.allow_process = true;

    auto tool = make_command_tool(".vix-agent-command-blocked", config);
    auto call = make_command_call("rm");

    auto result = tool.run(call);

    assert(!result);
  }

  void test_command_tool_runs_simple_allowed_command()
  {
    vix::fs::ensure_directory(".vix-agent-command-allowed");

    vix::ai::agent::AgentConfig config;
    config.allow_process = true;

    auto tool = make_command_tool(".vix-agent-command-allowed", config);

    auto call = make_command_call("echo");
    call.arguments["args"].push_back("hello");

    auto result = tool.run(call);

    assert(result);
    assert(result.value().ok);
    assert(result.value().name == "command.run");
    assert(result.value().output.find("hello") != std::string::npos);
  }

  void test_command_tool_rejects_workdir_outside_workspace()
  {
    vix::fs::ensure_directory(".vix-agent-command-workdir");

    vix::ai::agent::AgentConfig config;
    config.allow_process = true;

    auto tool = make_command_tool(".vix-agent-command-workdir", config);

    auto call = make_command_call("echo");
    call.arguments["args"].push_back("hello");
    call.arguments["working_directory"] = "..";

    auto result = tool.run(call);

    assert(!result);
  }

  void test_command_tool_truncates_output()
  {
    vix::fs::ensure_directory(".vix-agent-command-truncate");

    vix::ai::agent::AgentConfig config;
    config.allow_process = true;
    config.max_tool_output = 4;

    auto tool = make_command_tool(".vix-agent-command-truncate", config);

    auto call = make_command_call("echo");
    call.arguments["args"].push_back("hello world");

    auto result = tool.run(call);

    assert(result);
    assert(result.value().ok);
    assert(result.value().output.find("[output truncated]") != std::string::npos);
  }

  void test_command_tool_rejects_program_not_in_allowlist()
  {
    vix::fs::ensure_directory(".vix-agent-command-not-allowed");

    vix::ai::agent::AgentConfig config;
    config.allow_process = true;
    config.allowed_programs = {"echo"};

    auto tool = make_command_tool(".vix-agent-command-not-allowed", config);
    auto call = make_command_call("git");

    auto result = tool.run(call);

    assert(!result);
  }
}

void test_command_tool()
{
  test_command_tool_rejects_when_process_disabled();
  test_command_tool_rejects_invalid_tool_call();
  test_command_tool_rejects_blocked_program();
  test_command_tool_runs_simple_allowed_command();
  test_command_tool_rejects_workdir_outside_workspace();
  test_command_tool_truncates_output();
  test_command_tool_rejects_program_not_in_allowlist();
}
