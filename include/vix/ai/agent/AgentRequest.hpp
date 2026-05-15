/**
 *
 *  @file AgentRequest.hpp
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
#ifndef VIX_AI_AGENT_AGENTREQUEST_HPP
#define VIX_AI_AGENT_AGENTREQUEST_HPP

#include <string>
#include <vector>

namespace vix::ai::agent
{
  /**
   * @enum AgentRequestMode
   * @brief High-level execution mode for an agent request.
   */
  enum class AgentRequestMode
  {
    /**
     * @brief Normal single-turn request.
     */
    Run = 0,

    /**
     * @brief Analyze the current workspace or a selected path.
     */
    Analyze,

    /**
     * @brief Ask the agent to explain something without modifying files.
     */
    Explain,

    /**
     * @brief Interactive chat-style request.
     */
    Chat
  };

  /**
   * @struct AgentRequest
   * @brief Input request passed to the Vix AI Agent.
   *
   * AgentRequest represents a single user request. It contains the natural
   * language input, the workspace root, optional selected files, and safety
   * controls for this specific run.
   */
  struct AgentRequest
  {
    /**
     * @brief User instruction or question.
     *
     * Examples:
     * - "Analyze this project"
     * - "Explain the last build error"
     * - "Read src/main.cpp and summarize it"
     */
    std::string input;

    /**
     * @brief Workspace root for the request.
     *
     * Empty means the agent may use the current process directory.
     */
    std::string workspace{"."};

    /**
     * @brief Optional model override for this request.
     *
     * Empty means AgentConfig::model is used.
     */
    std::string model_override{};

    /**
     * @brief Optional provider override for this request.
     *
     * Empty means AgentConfig::provider is used.
     */
    std::string provider_override{};

    /**
     * @brief Optional selected files for focused analysis.
     *
     * These paths are resolved relative to the workspace unless already
     * absolute. They must still pass workspace safety checks.
     */
    std::vector<std::string> files;

    /**
     * @brief Optional extra context provided by the caller.
     *
     * This may contain build logs, compiler errors, terminal output,
     * or user-provided notes.
     */
    std::string context;

    /**
     * @brief Request execution mode.
     */
    AgentRequestMode mode{AgentRequestMode::Run};

    /**
     * @brief Whether tools may be used for this request.
     */
    bool allow_tools{true};

    /**
     * @brief Whether local command execution may be used for this request.
     */
    bool allow_process{true};

    /**
     * @brief Whether file reading may be used for this request.
     */
    bool allow_file_read{true};

    /**
     * @brief Whether file writing may be used for this request.
     */
    bool allow_file_write{false};

    /**
     * @brief Whether cached results may be reused for this request.
     */
    bool use_cache{true};
  };

} // namespace vix::ai::agent

#endif // VIX_AI_AGENT_AGENTREQUEST_HPP
