/**
 *
 *  @file AgentId.hpp
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
#ifndef VIX_AI_AGENT_CRYPTO_AGENTID_HPP
#define VIX_AI_AGENT_CRYPTO_AGENTID_HPP

#include <cstddef>
#include <string>
#include <string_view>

#include <vix/ai/agent/AgentResult.hpp>

namespace vix::ai::agent
{
  /**
   * @class AgentId
   * @brief Generates stable random identifiers for agent runs and tool calls.
   *
   * AgentId uses vix::crypto secure randomness and hex encoding.
   *
   * Generated IDs are intended for:
   * - run_id
   * - tool_call_id
   * - operation_id
   * - cache scopes
   * - local trace correlation
   *
   * Example output:
   * - run_4f91a2b7c81e9d31
   * - tool_91ab52c09f10aa03
   */
  class AgentId
  {
  public:
    /**
     * @brief Generate a random id using a prefix.
     *
     * @param prefix Stable textual prefix, for example "run" or "tool".
     * @param random_bytes Number of secure random bytes before hex encoding.
     * @return Generated id on success.
     */
    [[nodiscard]] static AgentResult<std::string> random(
        std::string_view prefix,
        std::size_t random_bytes = 16);

    /**
     * @brief Generate a run identifier.
     */
    [[nodiscard]] static AgentResult<std::string> run_id();

    /**
     * @brief Generate a tool call identifier.
     */
    [[nodiscard]] static AgentResult<std::string> tool_call_id();

    /**
     * @brief Generate an operation identifier.
     */
    [[nodiscard]] static AgentResult<std::string> operation_id();

  private:
    /**
     * @brief Normalize a prefix for id generation.
     *
     * Empty prefixes are replaced by "id".
     */
    [[nodiscard]] static std::string normalize_prefix(
        std::string_view prefix);
  };

} // namespace vix::ai::agent

#endif // VIX_AI_AGENT_CRYPTO_AGENTID_HPP
