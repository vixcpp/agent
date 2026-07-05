/**
 *
 *  @file AgentRunTest.cpp
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
#include <string_view>

#include <vix/ai/agent/Agent.hpp>
#include <vix/ai/agent/AgentConfig.hpp>
#include <vix/ai/agent/AgentRequest.hpp>
#include <vix/ai/agent/model/ModelProvider.hpp>
#include <vix/ai/agent/model/ModelRequest.hpp>
#include <vix/ai/agent/model/ModelResponse.hpp>
#include <vix/fs/EnsureDirectory.hpp>
#include <vix/fs/WriteText.hpp>
#include <vix/json/json.hpp>

namespace
{
  class FakeProvider final : public vix::ai::agent::ModelProvider
  {
  public:
    [[nodiscard]] std::string_view name() const noexcept override
    {
      return "fake";
    }

    [[nodiscard]] bool local() const noexcept override
    {
      return true;
    }

    [[nodiscard]] vix::ai::agent::AgentResult<bool> available() const override
    {
      return true;
    }

    [[nodiscard]] vix::ai::agent::AgentResult<vix::ai::agent::ModelResponse> generate(
        const vix::ai::agent::ModelRequest &request) override
    {
      assert(request.valid());
      assert(!request.prompt.empty());

      vix::ai::agent::ModelResponse response;
      response.text = "fake model response";
      response.model = request.model.empty() ? "fake-model" : request.model;
      response.provider = "fake";
      response.status = vix::ai::agent::ModelResponseStatus::Completed;
      response.duration_ms = 1;

      return response;
    }
  };

  class ToolCallingFakeProvider final : public vix::ai::agent::ModelProvider
  {
  public:
    [[nodiscard]] std::string_view name() const noexcept override
    {
      return "fake";
    }

    [[nodiscard]] bool local() const noexcept override
    {
      return true;
    }

    [[nodiscard]] vix::ai::agent::AgentResult<bool> available() const override
    {
      return true;
    }

    [[nodiscard]] vix::ai::agent::AgentResult<vix::ai::agent::ModelResponse> generate(
        const vix::ai::agent::ModelRequest &request) override
    {
      ++calls_;

      vix::ai::agent::ModelResponse response;
      response.model = request.model.empty() ? "fake-model" : request.model;
      response.provider = "fake";
      response.status = vix::ai::agent::ModelResponseStatus::Completed;
      response.duration_ms = 1;

      if (calls_ == 1)
      {
        vix::ai::agent::ToolCall call;
        call.id = "tool_1";
        call.name = "file.read";
        call.arguments = vix::json::Json::object();
        call.arguments["path"] = "main.cpp";

        response.tool_calls.push_back(std::move(call));
        response.text = "I need to read a file.";
        return response;
      }

      response.text = "final answer after tool";
      return response;
    }

  private:
    int calls_{0};
  };

  void test_agent_rejects_empty_input()
  {
    vix::ai::agent::AgentConfig config;
    config.provider = "fake";
    config.model = "fake-model";
    config.allow_file_read = false;
    config.allow_process = false;
    config.allow_file_write = false;

    auto provider = std::make_shared<FakeProvider>();
    vix::ai::agent::Agent agent(config, provider);

    vix::ai::agent::AgentRequest request;
    request.workspace = ".";
    request.input = "";

    auto response = agent.run(request);
    assert(!response);
  }

  void test_agent_runs_with_fake_provider()
  {
    assert(vix::fs::ensure_directory(".vix-agent-run-test"));
    assert(vix::fs::write_text(".vix-agent-run-test/main.cpp", "int main() { return 0; }\n"));

    vix::ai::agent::AgentConfig config;
    config.provider = "fake";
    config.model = "fake-model";
    config.allow_file_read = true;
    config.allow_process = false;
    config.allow_file_write = false;
    config.max_files = 20;
    config.max_file_size = 64 * 1024;
    config.max_context_chars = 8000;

    auto provider = std::make_shared<FakeProvider>();
    vix::ai::agent::Agent agent(config, provider);

    vix::ai::agent::AgentRequest request;
    request.workspace = ".vix-agent-run-test";
    request.input = "Explain this project.";
    request.mode = vix::ai::agent::AgentRequestMode::Analyze;
    request.allow_file_read = true;
    request.allow_process = false;
    request.allow_file_write = false;

    auto response = agent.run(request);
    assert(response);
    assert(response.value().status == vix::ai::agent::AgentResponseStatus::Completed);
    assert(response.value().text == "fake model response");
    assert(response.value().provider == "fake");
    assert(response.value().model == "fake-model");
    assert(!response.value().run_id.empty());
  }

  void test_agent_rejects_process_when_disabled()
  {
    vix::ai::agent::AgentConfig config;
    config.provider = "fake";
    config.model = "fake-model";
    config.allow_file_read = false;
    config.allow_process = false;
    config.allow_file_write = false;

    auto provider = std::make_shared<FakeProvider>();
    vix::ai::agent::Agent agent(config, provider);

    vix::ai::agent::AgentRequest request;
    request.workspace = ".";
    request.input = "Run a command.";
    request.allow_process = true;

    auto response = agent.run(request);
    assert(!response);
  }

  void test_agent_rejects_file_write_when_disabled()
  {
    vix::ai::agent::AgentConfig config;
    config.provider = "fake";
    config.model = "fake-model";
    config.allow_file_read = false;
    config.allow_process = false;
    config.allow_file_write = false;

    auto provider = std::make_shared<FakeProvider>();
    vix::ai::agent::Agent agent(config, provider);

    vix::ai::agent::AgentRequest request;
    request.workspace = ".";
    request.input = "Write a file.";
    request.allow_file_write = true;

    auto response = agent.run(request);
    assert(!response);
  }

  void test_agent_runs_tool_loop_with_fake_provider()
  {
    assert(vix::fs::ensure_directory(".vix-agent-tool-loop-test"));
    assert(vix::fs::write_text(
        ".vix-agent-tool-loop-test/main.cpp",
        "int main() { return 0; }\n"));

    vix::ai::agent::AgentConfig config;
    config.provider = "fake";
    config.model = "fake-model";
    config.allow_file_read = true;
    config.allow_process = false;
    config.allow_file_write = false;
    config.max_tool_rounds = 3;

    auto provider = std::make_shared<ToolCallingFakeProvider>();
    vix::ai::agent::Agent agent(config, provider);

    vix::ai::agent::AgentRequest request;
    request.workspace = ".vix-agent-tool-loop-test";
    request.input = "Read main.cpp and explain it.";
    request.allow_tools = true;
    request.allow_file_read = true;
    request.allow_process = false;
    request.allow_file_write = false;

    auto response = agent.run(request);

    assert(response);
    assert(response.value().text == "final answer after tool");
    assert(response.value().tools.size() == 1);
    assert(response.value().tools[0].name == "file.read");
    assert(response.value().tools[0].ok);
  }
}

void test_agent_run()
{
  test_agent_rejects_empty_input();
  test_agent_runs_with_fake_provider();
  test_agent_rejects_process_when_disabled();
  test_agent_rejects_file_write_when_disabled();
  test_agent_runs_tool_loop_with_fake_provider();
}
