/**
 *
 *  @file ToolRegistryTest.cpp
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
#include <memory>
#include <string_view>

#include <vix/ai/agent/tools/Tool.hpp>
#include <vix/ai/agent/tools/ToolCall.hpp>
#include <vix/ai/agent/tools/ToolRegistry.hpp>
#include <vix/ai/agent/tools/ToolResult.hpp>

namespace
{
  class EchoTool final : public vix::ai::agent::Tool
  {
  public:
    [[nodiscard]] std::string_view name() const noexcept override
    {
      return "test.echo";
    }

    [[nodiscard]] std::string_view description() const noexcept override
    {
      return "Echo test tool.";
    }

    [[nodiscard]] vix::ai::agent::AgentResult<vix::ai::agent::ToolResult> run(
        const vix::ai::agent::ToolCall &call) override
    {
      return vix::ai::agent::ToolResult::success(
          call.id,
          "test.echo",
          "echo");
    }
  };

  void test_register_tool()
  {
    vix::ai::agent::ToolRegistry registry;

    auto err = registry.add(std::make_shared<EchoTool>());
    assert(!err);

    assert(!registry.empty());
    assert(registry.size() == 1);
    assert(registry.contains("test.echo"));
    assert(registry.find("test.echo") != nullptr);
  }

  void test_list_tools()
  {
    vix::ai::agent::ToolRegistry registry;

    auto err = registry.add(std::make_shared<EchoTool>());
    assert(!err);

    auto tools = registry.list();
    assert(tools.size() == 1);
    assert(tools[0].name == "test.echo");
    assert(!tools[0].description.empty());
  }

  void test_run_tool()
  {
    vix::ai::agent::ToolRegistry registry;

    auto err = registry.add(std::make_shared<EchoTool>());
    assert(!err);

    vix::ai::agent::ToolCall call;
    call.id = "tool_1";
    call.name = "test.echo";

    auto result = registry.run(call);
    assert(result);
    assert(result.value().ok);
    assert(result.value().id == "tool_1");
    assert(result.value().name == "test.echo");
    assert(result.value().output == "echo");
  }

  void test_missing_tool_fails()
  {
    vix::ai::agent::ToolRegistry registry;

    vix::ai::agent::ToolCall call;
    call.id = "tool_missing";
    call.name = "missing.tool";

    auto result = registry.run(call);
    assert(!result);
  }

  void test_invalid_tool_call_fails()
  {
    vix::ai::agent::ToolRegistry registry;

    auto err = registry.add(std::make_shared<EchoTool>());
    assert(!err);

    vix::ai::agent::ToolCall call;
    call.name = "test.echo";

    auto result = registry.run(call);
    assert(!result);
  }
}

void test_tool_registry()
{
  test_register_tool();
  test_list_tools();
  test_run_tool();
  test_missing_tool_fails();
  test_invalid_tool_call_fails();
}
