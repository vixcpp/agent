/**
 *
 *  @file public_api.cpp
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

  auto err = vix::ai::agent::AgentConfigValidator::validate(config);

  if (err)
  {
    vix::print("Invalid config:", err.message());
    return 1;
  }

  vix::ai::agent::Agent agent(config);

  vix::ai::agent::AgentRequest request;

  request.workspace = ".";
  request.input = "What is the role of Agent::run?";
  request.mode = vix::ai::agent::AgentRequestMode::Explain;

  request.allow_tools = false;
  request.allow_file_read = false;
  request.allow_process = false;
  request.allow_file_write = false;

  auto result = agent.run(request);

  if (!result)
  {
    vix::print("Agent failed:", result.error().message());
    return 1;
  }

  const auto &response = result.value();

  vix::print(response.text);
  vix::print();
  vix::print("Stable API: Agent::run(const AgentRequest&)");
  vix::print("Run id:", response.run_id);

  return 0;
}
