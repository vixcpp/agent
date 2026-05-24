/**
 *
 *  @file agent_with_command_tool.cpp
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
#include <memory>
#include <string_view>

#include <vix/ai/agent/AgentRuntime.hpp>
#include <vix/json/json.hpp>
#include <vix/print.hpp>

namespace
{
  class CommandToolProvider final : public vix::ai::agent::ModelProvider
  {
  public:
    [[nodiscard]] std::string_view name() const noexcept override
    {
      return "command-tool-demo";
    }

    [[nodiscard]] bool local() const noexcept override
    {
      return true;
    }

    [[nodiscard]] vix::ai::agent::AgentResult<bool> available() const override
    {
      return true;
    }

    [[nodiscard]] vix::ai::agent::AgentResult<vix::ai::agent::ModelResponse>
    generate(const vix::ai::agent::ModelRequest &request) override
    {
      ++calls_;

      vix::ai::agent::ModelResponse response;
      response.model = request.model;
      response.provider = "command-tool-demo";
      response.status = vix::ai::agent::ModelResponseStatus::Completed;

      if (calls_ == 1)
      {
        vix::ai::agent::ToolCall call;
        call.id = "tool_echo_1";
        call.name = "command.run";
        call.reason = "Run a safe command to demonstrate the command tool.";
        call.arguments = vix::json::Json::object();
        call.arguments["program"] = "echo";
        call.arguments["args"] = vix::json::Json::array();
        call.arguments["args"].push_back("hello from command.run");
        call.arguments["working_directory"] = ".";

        response.text = "I need to run a safe command first.";
        response.tool_calls.push_back(std::move(call));

        return response;
      }

      response.text =
          "The command tool was executed successfully and its result was "
          "returned to the model context.";

      return response;
    }

  private:
    int calls_{0};
  };
}

int main()
{
  vix::ai::agent::AgentConfig config;

  config.provider = "command-tool-demo";
  config.model = "command-tool-demo-model";

  config.allow_file_read = false;
  config.allow_process = true;
  config.allow_file_write = false;

  config.allowed_programs = {"echo"};

  config.max_tool_output = 20'000;
  config.max_tool_rounds = 3;

  config.use_cache = false;
  config.persist_memory = true;

  auto provider = std::make_shared<CommandToolProvider>();

  vix::ai::agent::Agent agent(config, provider);

  vix::ai::agent::AgentRequest request;

  request.workspace = ".";
  request.mode = vix::ai::agent::AgentRequestMode::Run;
  request.input = "Demonstrate command.run with a safe command.";

  request.allow_tools = true;
  request.allow_file_read = false;
  request.allow_process = true;
  request.allow_file_write = false;
  request.use_cache = false;

  auto response = agent.run(request);

  if (!response)
  {
    vix::print("Agent error:", response.error().message());
    return 1;
  }

  vix::print(response.value().text);
  vix::print();

  vix::print("Tools used:", response.value().tools.size());

  for (const auto &tool : response.value().tools)
  {
    vix::print("-", tool.name, tool.ok ? "ok" : "failed");
  }

  return 0;
}
