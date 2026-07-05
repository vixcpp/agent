/**
 *
 *  @file AgentCacheTest.cpp
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

#include <vix/ai/agent/Agent.hpp>
#include <vix/ai/agent/AgentConfig.hpp>
#include <vix/ai/agent/AgentRequest.hpp>
#include <vix/ai/agent/model/ModelProvider.hpp>
#include <vix/ai/agent/model/ModelRequest.hpp>
#include <vix/ai/agent/model/ModelResponse.hpp>
#include <vix/fs/EnsureDirectory.hpp>

namespace
{
  class CountingProvider final : public vix::ai::agent::ModelProvider
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
      response.text = "cached response";
      response.model = request.model;
      response.provider = "fake";
      response.status = vix::ai::agent::ModelResponseStatus::Completed;

      return response;
    }

    [[nodiscard]] int calls() const noexcept
    {
      return calls_;
    }

  private:
    int calls_{0};
  };

  void test_agent_reuses_cached_model_response()
  {
    assert(vix::fs::ensure_directory(".vix-agent-cache-test"));

    vix::ai::agent::AgentConfig config;
    config.provider = "fake";
    config.model = "fake-model";
    config.allow_file_read = false;
    config.allow_process = false;
    config.allow_file_write = false;
    config.use_cache = true;
    config.persist_memory = true;
    config.cache_ttl_ms = 60'000;

    auto provider = std::make_shared<CountingProvider>();

    vix::ai::agent::Agent agent(config, provider);

    vix::ai::agent::AgentRequest request;
    request.workspace = ".vix-agent-cache-test";
    request.input = "Explain cache.";
    request.allow_tools = false;
    request.allow_file_read = false;
    request.allow_process = false;
    request.allow_file_write = false;
    request.use_cache = true;

    auto first = agent.run(request);
    assert(first);
    assert(first.value().text == "cached response");
    assert(!first.value().from_cache);
    assert(provider->calls() == 1);

    auto second = agent.run(request);
    assert(second);
    assert(second.value().text == "cached response");
    assert(second.value().from_cache);
    assert(provider->calls() == 1);
  }
}

void test_agent_cache()
{
  test_agent_reuses_cached_model_response();
}
