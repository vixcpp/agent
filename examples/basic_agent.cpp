/**
 *
 *  @file basic_agent.cpp
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

#include <vix/ai/agent/AgentRuntime.hpp>
#include <vix/print.hpp>
#include <memory>

int main()
{
  vix::ai::agent::AgentConfig config;

  config.provider = "ollama";
  config.model = "llama3";
  config.model_url = "http://127.0.0.1:11434";

  config.allow_file_read = false;
  config.allow_process = false;
  config.allow_file_write = false;

  config.use_cache = false;
  config.persist_memory = false;

  auto provider = std::make_shared<vix::ai::agent::OllamaProvider>(config);

  auto available = provider->available();
  if (!available || !available.value())
  {
    vix::print("Ollama is not available.");
    vix::print("Start it with: ollama serve");
    return 1;
  }

  vix::ai::agent::Agent agent(config, provider);

  vix::ai::agent::AgentRequest request;

  request.input = "Explain what Vix.cpp is in simple words.";
  request.workspace = ".";
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
  vix::print();
  vix::print("Run id:", response.value().run_id);
  vix::print("Provider:", response.value().provider);
  vix::print("Model:", response.value().model);
  vix::print("Duration:", response.value().duration_ms, "ms");

  return 0;
}
