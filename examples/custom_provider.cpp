/**
 *
 *  @file custom_provider.cpp
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
#include <vix/print.hpp>

namespace
{
  class EchoProvider final : public vix::ai::agent::ModelProvider
  {
  public:
    [[nodiscard]] std::string_view name() const noexcept override
    {
      return "echo";
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
      vix::ai::agent::ModelResponse response;

      response.text = "Echo provider received: " + request.prompt;
      response.model = request.model;
      response.provider = "echo";
      response.status = vix::ai::agent::ModelResponseStatus::Completed;

      return response;
    }
  };
}

int main()
{
  vix::ai::agent::AgentConfig config;

  config.provider = "echo";
  config.model = "echo-model";

  config.allow_file_read = false;
  config.allow_process = false;
  config.allow_file_write = false;

  config.use_cache = false;
  config.persist_memory = false;

  auto provider = std::make_shared<EchoProvider>();

  vix::ai::agent::Agent agent(config, provider);

  vix::ai::agent::AgentRequest request;

  request.workspace = ".";
  request.input = "Hello custom provider.";
  request.mode = vix::ai::agent::AgentRequestMode::Chat;

  request.allow_tools = false;
  request.allow_file_read = false;
  request.allow_process = false;
  request.allow_file_write = false;
  request.use_cache = false;

  auto response = agent.run(request);

  if (!response)
  {
    vix::print("Agent error:", response.error().message());
    return 1;
  }

  vix::print(response.value().text);

  return 0;
}
