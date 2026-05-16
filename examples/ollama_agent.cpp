/**
 *
 *  @file ollama_agent.cpp
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

#include <vix/ai/agent/agent.hpp>
#include <vix/print.hpp>

int main()
{
  vix::ai::agent::AgentConfig config;

  config.provider = "ollama";
  config.model = "llama3";
  config.model_url = "http://127.0.0.1:11434";

  config.allow_file_read = true;
  config.allow_process = false;
  config.allow_file_write = false;

  config.max_files = 200;
  config.max_file_size = 256 * 1024;
  config.max_context_chars = 32'000;
  config.timeout_ms = 30'000;

  config.use_cache = true;
  config.persist_memory = true;

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

  request.workspace = ".";
  request.mode = vix::ai::agent::AgentRequestMode::Analyze;
  request.input =
      "Look at this project structure and explain the most important files.";

  request.allow_tools = true;
  request.allow_file_read = true;
  request.allow_process = false;
  request.allow_file_write = false;
  request.use_cache = true;

  auto response = agent.run(request);

  if (!response)
  {
    vix::print("Agent error:", response.error().message());
    return 1;
  }

  vix::print(response.value().text);
  vix::print();
  vix::print("---");
  vix::print("Run id:", response.value().run_id);
  vix::print("Provider:", response.value().provider);
  vix::print("Model:", response.value().model);
  vix::print("From cache:", response.value().from_cache);
  vix::print("Duration:", response.value().duration_ms, "ms");

  return 0;
}
