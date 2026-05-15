/**
 *
 *  @file AgentConfigLoader.hpp
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
#ifndef VIX_AI_AGENT_AGENTCONFIGLOADER_HPP
#define VIX_AI_AGENT_AGENTCONFIGLOADER_HPP

#include <string_view>

#include <vix/ai/agent/AgentConfig.hpp>

namespace vix::ai::agent
{
  /**
   * @class AgentConfigLoader
   * @brief Loads AgentConfig from environment variables.
   *
   * AgentConfigLoader centralizes configuration loading for the agent.
   * It reads stable VIX_AGENT_* variables and falls back to AgentConfig
   * defaults when values are missing or invalid.
   *
   * The loader does not throw. Invalid optional values are ignored and
   * the default value from AgentConfig is kept.
   */
  class AgentConfigLoader
  {
  public:
    /**
     * @brief Load agent configuration from the current process environment.
     *
     * Supported variables:
     * - VIX_AGENT_PROVIDER
     * - VIX_AGENT_MODEL
     * - VIX_AGENT_MODEL_URL
     * - VIX_AGENT_MEMORY_DIR
     * - VIX_AGENT_CACHE_DIR
     * - VIX_AGENT_RUNS_DIR
     * - VIX_AGENT_LOGS_DIR
     * - VIX_AGENT_TIMEOUT_MS
     * - VIX_AGENT_MAX_FILES
     * - VIX_AGENT_MAX_FILE_SIZE
     * - VIX_AGENT_MAX_TOOL_OUTPUT
     * - VIX_AGENT_MAX_CONTEXT_CHARS
     * - VIX_AGENT_OFFLINE
     * - VIX_AGENT_ALLOW_PROCESS
     * - VIX_AGENT_ALLOW_FILE_READ
     * - VIX_AGENT_ALLOW_FILE_WRITE
     * - VIX_AGENT_USE_CACHE
     * - VIX_AGENT_PERSIST_MEMORY
     *
     * @return AgentConfig populated from environment variables.
     */
    [[nodiscard]] static AgentConfig from_environment();

    /**
     * @brief Load agent configuration from environment variables using a prefix.
     *
     * Example:
     * prefix = "VIX_AGENT_"
     * keys become:
     * - VIX_AGENT_PROVIDER
     * - VIX_AGENT_MODEL
     * - VIX_AGENT_TIMEOUT_MS
     *
     * @param prefix Environment variable prefix.
     * @return AgentConfig populated from matching environment variables.
     */
    [[nodiscard]] static AgentConfig from_environment(
        std::string_view prefix);

  private:
    /**
     * @brief Build an environment variable name from a prefix and key.
     */
    [[nodiscard]] static std::string make_key(
        std::string_view prefix,
        std::string_view name);
  };

} // namespace vix::ai::agent

#endif // VIX_AI_AGENT_AGENTCONFIGLOADER_HPP
