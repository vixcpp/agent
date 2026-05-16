/**
 *
 *  @file Agent.cpp
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
#include <vix/ai/Agent.hpp>

#include <algorithm>
#include <string>
#include <string_view>
#include <utility>

namespace vix::ai
{
  namespace
  {
    bool contains_program(
        const std::vector<std::string> &programs,
        std::string_view program)
    {
      return std::find(
                 programs.begin(),
                 programs.end(),
                 std::string(program)) != programs.end();
    }

    void add_program_once(
        std::vector<std::string> &programs,
        std::string program)
    {
      if (!contains_program(programs, program))
      {
        programs.push_back(std::move(program));
      }
    }
  }

  Agent::Agent()
  {
    config_.provider = "ollama";
    config_.model = "llama3";
    config_.model_url = "http://127.0.0.1:11434";

    config_.allow_file_read = false;
    config_.allow_process = false;
    config_.allow_file_write = false;

    config_.use_cache = true;
    config_.persist_memory = true;
  }

  Agent &Agent::set_model(std::string model)
  {
    if (model == "local:llama")
    {
      config_.provider = "ollama";
      config_.model = "llama3";
      config_.model_url = "http://127.0.0.1:11434";
      return *this;
    }

    if (model.rfind("ollama:", 0) == 0)
    {
      config_.provider = "ollama";
      config_.model = model.substr(std::string("ollama:").size());
      return *this;
    }

    config_.model = std::move(model);
    return *this;
  }

  Agent &Agent::set_timeout(std::uint64_t timeout_ms)
  {
    config_.timeout_ms = timeout_ms;
    return *this;
  }

  Agent &Agent::set_workspace(std::string workspace)
  {
    workspace_ = std::move(workspace);
    return *this;
  }

  Agent &Agent::add_tool(std::string tool)
  {
    apply_tool(tool);
    tools_.push_back(std::move(tool));
    return *this;
  }

  Agent &Agent::add_memory(std::string memory_dir)
  {
    config_.memory_dir = std::move(memory_dir);
    config_.persist_memory = true;
    return *this;
  }

  vix::error::Result<Result> Agent::run(
      std::string_view prompt)
  {
    vix::ai::agent::Agent inner(config_);

    vix::ai::agent::AgentRequest request;
    request.input = std::string(prompt);
    request.workspace = workspace_;
    request.mode = vix::ai::agent::AgentRequestMode::Analyze;

    request.allow_tools = !tools_.empty();
    request.allow_file_read = config_.allow_file_read;
    request.allow_process = config_.allow_process;
    request.allow_file_write = false;
    request.use_cache = config_.use_cache;
    request.timeout_ms = config_.timeout_ms;

    auto response = inner.run(request);

    if (!response)
    {
      return response.error();
    }

    return Result(std::move(response.value()));
  }

  const vix::ai::agent::AgentConfig &Agent::config() const noexcept
  {
    return config_;
  }

  void Agent::apply_tool(std::string_view tool)
  {
    if (tool == "filesystem")
    {
      config_.allow_file_read = true;
      return;
    }

    if (tool == "shell")
    {
      config_.allow_process = true;

      add_program_once(config_.allowed_programs, "echo");
      add_program_once(config_.allowed_programs, "ls");
      add_program_once(config_.allowed_programs, "cat");
      add_program_once(config_.allowed_programs, "vix");
      add_program_once(config_.allowed_programs, "cmake");
      add_program_once(config_.allowed_programs, "ninja");

      return;
    }

    if (tool == "git")
    {
      config_.allow_process = true;
      add_program_once(config_.allowed_programs, "git");
      return;
    }
  }

} // namespace vix::ai
