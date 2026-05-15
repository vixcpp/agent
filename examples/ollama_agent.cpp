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
#include <iostream>
#include <memory>

#include <vix/ai/agent/agent.hpp>

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

  auto provider = std::make_shared<vix::ai::agent::OllamaProvider>(config);

  auto available = provider->available();
  if (!available || !available.value())
  {
    std::cerr << "Ollama is not available.\n";
    std::cerr << "Start it with: ollama serve\n";
    return 1;
  }

  vix::ai::agent::Agent agent(config, provider);

  vix::ai::agent::AgentRequest request;
  request.workspace = ".";
  request.mode = vix::ai::agent::AgentRequestMode::Analyze;
  request.input =
      "Look at this project structure and explain what the most important files are.";
  request.allow_file_read = true;
  request.allow_process = false;
  request.allow_file_write = false;

  auto response = agent.run(request);
  if (!response)
  {
    std::cerr << "Agent error: " << response.error().message() << '\n';
    return 1;
  }

  std::cout << response.value().text << '\n';
  std::cout << "\n---\n";
  std::cout << "Run id: " << response.value().run_id << '\n';
  std::cout << "Provider: " << response.value().provider << '\n';
  std::cout << "Model: " << response.value().model << '\n';
  std::cout << "Duration: " << response.value().duration_ms << " ms\n";

  return 0;
}
