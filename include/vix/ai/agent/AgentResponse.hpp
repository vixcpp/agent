/**
 *
 *  @file AgentResponse.hpp
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
#ifndef VIX_AI_AGENT_AGENTRESPONSE_HPP
#define VIX_AI_AGENT_AGENTRESPONSE_HPP

#include <cstdint>
#include <string>
#include <vector>

#include <vix/json.hpp>

namespace vix::ai::agent
{
  /**
   * @enum AgentResponseStatus
   * @brief Final status of an agent response.
   */
  enum class AgentResponseStatus
  {
    /**
     * @brief Request completed successfully.
     */
    Completed = 0,

    /**
     * @brief Request failed.
     */
    Failed,

    /**
     * @brief Request was cancelled before completion.
     */
    Cancelled,

    /**
     * @brief Request completed partially.
     */
    Partial
  };

  /**
   * @struct AgentToolSummary
   * @brief Lightweight public summary of a tool execution.
   */
  struct AgentToolSummary
  {
    /**
     * @brief Tool call identifier.
     */
    std::string id;

    /**
     * @brief Tool name.
     */
    std::string name;

    /**
     * @brief Whether the tool execution succeeded.
     */
    bool ok{false};

    /**
     * @brief Tool execution duration in milliseconds.
     */
    std::uint64_t duration_ms{0};

    /**
     * @brief Short error message when the tool failed.
     */
    std::string error;
  };

  /**
   * @struct AgentResponse
   * @brief Output returned by the Vix AI Agent.
   *
   * AgentResponse contains the final text produced by the agent, execution
   * metadata, optional structured JSON, and a summary of tools used during
   * the run.
   */
  struct AgentResponse
  {
    /**
     * @brief Final response text.
     */
    std::string text;

    /**
     * @brief Unique run identifier.
     */
    std::string run_id;

    /**
     * @brief Model used to generate the response.
     */
    std::string model;

    /**
     * @brief Provider used to generate the response.
     */
    std::string provider;

    /**
     * @brief Final response status.
     */
    AgentResponseStatus status{AgentResponseStatus::Completed};

    /**
     * @brief Total run duration in milliseconds.
     */
    std::uint64_t duration_ms{0};

    /**
     * @brief Whether the response was loaded from cache.
     */
    bool from_cache{false};

    /**
     * @brief Tool executions performed during the run.
     */
    std::vector<AgentToolSummary> tools;

    /**
     * @brief Optional structured metadata for logs, reports, or future sync.
     */
    vix::json::Json metadata{vix::json::Json::object()};

    /**
     * @brief Return true if the agent run completed successfully.
     */
    [[nodiscard]] bool ok() const noexcept
    {
      return status == AgentResponseStatus::Completed ||
             status == AgentResponseStatus::Partial;
    }
  };

} // namespace vix::ai::agent

#endif // VIX_AI_AGENT_AGENTRESPONSE_HPP
