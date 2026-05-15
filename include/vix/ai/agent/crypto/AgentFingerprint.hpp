/**
 *
 *  @file AgentFingerprint.hpp
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
#ifndef VIX_AI_AGENT_CRYPTO_AGENTFINGERPRINT_HPP
#define VIX_AI_AGENT_CRYPTO_AGENTFINGERPRINT_HPP

#include <string>
#include <string_view>

#include <vix/ai/agent/AgentResult.hpp>

namespace vix::ai::agent
{
  /**
   * @class AgentFingerprint
   * @brief Builds stable hashes and cache keys for agent data.
   *
   * AgentFingerprint is used to create deterministic identifiers for:
   * - file content
   * - prompts
   * - model requests
   * - tool inputs
   * - workspace snapshots
   * - cache entries
   *
   * The implementation uses vix::crypto hashing and hexadecimal encoding.
   */
  class AgentFingerprint
  {
  public:
    /**
     * @brief Compute SHA-256 and return it as lowercase hexadecimal text.
     *
     * @param data Input text or binary-safe string view.
     * @return Hexadecimal SHA-256 digest.
     */
    [[nodiscard]] static AgentResult<std::string> sha256_hex(
        std::string_view data);

    /**
     * @brief Compute a stable fingerprint for a file content.
     *
     * @param relative_path Workspace-relative file path.
     * @param content File content.
     * @return Stable file fingerprint.
     */
    [[nodiscard]] static AgentResult<std::string> file_fingerprint(
        std::string_view relative_path,
        std::string_view content);

    /**
     * @brief Compute a stable fingerprint for a prompt.
     *
     * @param provider Model provider.
     * @param model Model name.
     * @param prompt Prompt content.
     * @param context_hash Optional context hash.
     * @return Stable prompt fingerprint.
     */
    [[nodiscard]] static AgentResult<std::string> prompt_fingerprint(
        std::string_view provider,
        std::string_view model,
        std::string_view prompt,
        std::string_view context_hash = {});

    /**
     * @brief Build a cache key for a file summary.
     *
     * @param file_hash File fingerprint.
     * @return Cache key string.
     */
    [[nodiscard]] static std::string file_summary_key(
        std::string_view file_hash);

    /**
     * @brief Build a cache key for a model response.
     *
     * @param prompt_hash Prompt fingerprint.
     * @return Cache key string.
     */
    [[nodiscard]] static std::string model_response_key(
        std::string_view prompt_hash);

    /**
     * @brief Build a cache key for a tool result.
     *
     * @param tool_name Tool name.
     * @param arguments_hash Tool arguments fingerprint.
     * @return Cache key string.
     */
    [[nodiscard]] static std::string tool_result_key(
        std::string_view tool_name,
        std::string_view arguments_hash);
  };

} // namespace vix::ai::agent

#endif // VIX_AI_AGENT_CRYPTO_AGENTFINGERPRINT_HPP
