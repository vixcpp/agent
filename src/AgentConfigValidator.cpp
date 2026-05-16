/**
 *
 *  @file AgentConfigValidator.cpp
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
#include <vix/ai/agent/AgentConfigValidator.hpp>
#include <vix/ai/agent/AgentError.hpp>

namespace vix::ai::agent
{
  vix::error::Error AgentConfigValidator::validate(
      const AgentConfig &config)
  {
    if (config.provider.empty())
    {
      return make_agent_error(
          AgentErrorCode::ConfigInvalid,
          "agent provider cannot be empty");
    }

    if (config.model.empty())
    {
      return make_agent_error(
          AgentErrorCode::ConfigInvalid,
          "agent model cannot be empty");
    }

    if (config.model_url.empty())
    {
      return make_agent_error(
          AgentErrorCode::ConfigInvalid,
          "agent model_url cannot be empty");
    }

    if (config.memory_dir.empty())
    {
      return make_agent_error(
          AgentErrorCode::ConfigInvalid,
          "agent memory_dir cannot be empty");
    }

    if (config.cache_dir.empty())
    {
      return make_agent_error(
          AgentErrorCode::ConfigInvalid,
          "agent cache_dir cannot be empty");
    }

    if (config.runs_dir.empty())
    {
      return make_agent_error(
          AgentErrorCode::ConfigInvalid,
          "agent runs_dir cannot be empty");
    }

    if (config.logs_dir.empty())
    {
      return make_agent_error(
          AgentErrorCode::ConfigInvalid,
          "agent logs_dir cannot be empty");
    }

    if (config.timeout_ms == 0)
    {
      return make_agent_error(
          AgentErrorCode::ConfigInvalid,
          "agent timeout_ms must be greater than zero");
    }

    if (config.max_files == 0)
    {
      return make_agent_error(
          AgentErrorCode::ConfigInvalid,
          "agent max_files must be greater than zero");
    }

    if (config.max_file_size == 0)
    {
      return make_agent_error(
          AgentErrorCode::ConfigInvalid,
          "agent max_file_size must be greater than zero");
    }

    if (config.max_context_chars == 0)
    {
      return make_agent_error(
          AgentErrorCode::ConfigInvalid,
          "agent max_context_chars must be greater than zero");
    }

    if (config.max_tool_output == 0)
    {
      return make_agent_error(
          AgentErrorCode::ConfigInvalid,
          "agent max_tool_output must be greater than zero");
    }

    if (config.tool_timeout_ms == 0)
    {
      return make_agent_error(
          AgentErrorCode::ConfigInvalid,
          "agent tool_timeout_ms must be greater than zero");
    }

    if (config.max_tool_rounds == 0)
    {
      return make_agent_error(
          AgentErrorCode::ConfigInvalid,
          "agent max_tool_rounds must be greater than zero");
    }

    if (config.use_cache && config.cache_ttl_ms == 0)
    {
      return make_agent_error(
          AgentErrorCode::ConfigInvalid,
          "agent cache_ttl_ms must be greater than zero when cache is enabled");
    }

    if (config.allow_process && config.allowed_programs.empty())
    {
      return make_agent_error(
          AgentErrorCode::ConfigInvalid,
          "agent allow_process requires at least one allowed program");
    }

    return {};
  }

} // namespace vix::ai::agent
