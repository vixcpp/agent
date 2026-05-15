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
#include <iostream>

#include <vix/ai/agent/agent.hpp>

int main()
{
  vix::ai::agent::AgentConfig config;
  config.provider = "ollama";
  config.model = "llama3";
  config.model_url = "http://127.0.0.1:11434";
  config.allow_file_read = false;
  config.allow_process = false;
  config.allow_file_write = false;

  vix::ai::agent::Agent agent(config);

  vix::ai::agent::AgentRequest request;
  request.input = "Explain what Vix.cpp is in simple words.";
  request.workspace = ".";
  request.mode = vix::ai::agent::AgentRequestMode::Chat;
  request.allow_file_read = false;
  request.allow_process = false;
  request.allow_file_write = false;

  auto response = agent.run(request);
  if (!response)
  {
    std::cerr << "Agent error: " << response.error().message() << '\n';
    return 1;
  }

  std::cout << response.value().text << '\n';
  std::cout << "\nRun id: " << response.value().run_id << '\n';
  std::cout << "Provider: " << response.value().provider << '\n';
  std::cout << "Model: " << response.value().model << '\n';
  std::cout << "Duration: " << response.value().duration_ms << " ms\n";

  return 0;
}
