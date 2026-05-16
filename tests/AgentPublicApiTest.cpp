/**
 *
 *  @file AgentPublicApiTest.cpp
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

#include <vix/ai/agent/agent.hpp>

namespace
{
  class PublicApiFakeProvider final : public vix::ai::agent::ModelProvider
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
      vix::ai::agent::ModelResponse response;
      response.text = "public api ok";
      response.model = request.model;
      response.provider = "fake";
      response.status = vix::ai::agent::ModelResponseStatus::Completed;
      return response;
    }
  };

  void test_public_agent_run_api()
  {
    vix::ai::agent::AgentConfig config;
    config.provider = "fake";
    config.model = "fake-model";
    config.allow_file_read = false;
    config.allow_process = false;
    config.allow_file_write = false;
    config.use_cache = false;
    config.persist_memory = false;

    auto provider = std::make_shared<PublicApiFakeProvider>();

    vix::ai::agent::Agent agent(config, provider);

    vix::ai::agent::AgentRequest request;
    request.workspace = ".";
    request.input = "Test public API.";
    request.allow_tools = false;
    request.allow_file_read = false;
    request.allow_process = false;
    request.allow_file_write = false;
    request.use_cache = false;

    auto response = agent.run(request);

    assert(response);
    assert(response.value().ok());
    assert(response.value().text == "public api ok");
    assert(response.value().provider == "fake");
  }
}

void test_agent_public_api()
{
  test_public_agent_run_api();
}
