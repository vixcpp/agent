/**
 *
 *  @file AgentRunStore.hpp
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
#ifndef VIX_AI_AGENT_AGENTRUNSTORE_HPP
#define VIX_AI_AGENT_AGENTRUNSTORE_HPP

#include <string>
#include <string_view>
#include <vector>

#include <vix/ai/agent/AgentRequest.hpp>
#include <vix/ai/agent/AgentResponse.hpp>
#include <vix/ai/agent/AgentResult.hpp>
#include <vix/ai/agent/AgentWorkspace.hpp>
#include <vix/ai/agent/model/ModelRequest.hpp>
#include <vix/ai/agent/model/ModelResponse.hpp>
#include <vix/ai/agent/tools/ToolResult.hpp>
#include <vix/error/Error.hpp>

namespace vix::ai::agent
{
  /**
   * @class AgentRunStore
   * @brief Persists local agent run history inside .vix/agent/runs.
   */
  class AgentRunStore
  {
  public:
    explicit AgentRunStore(AgentWorkspace workspace);

    [[nodiscard]] vix::error::Error create_run(
        std::string_view run_id,
        const AgentRequest &request) const;

    [[nodiscard]] vix::error::Error save_prompt(
        std::string_view run_id,
        const ModelRequest &request) const;

    [[nodiscard]] vix::error::Error save_model_response(
        std::string_view run_id,
        const ModelResponse &response) const;

    [[nodiscard]] vix::error::Error save_tool_results(
        std::string_view run_id,
        const std::vector<ToolResult> &results) const;

    [[nodiscard]] vix::error::Error save_final_response(
        std::string_view run_id,
        const AgentResponse &response) const;

    [[nodiscard]] vix::error::Error save_error(
        std::string_view run_id,
        const vix::error::Error &error) const;

  private:
    [[nodiscard]] AgentResult<std::string> run_file(
        std::string_view run_id,
        std::string_view name) const;

    [[nodiscard]] vix::error::Error write_text_file(
        std::string_view run_id,
        std::string_view name,
        std::string_view content) const;

  private:
    AgentWorkspace workspace_{};
  };

} // namespace vix::ai::agent

#endif // VIX_AI_AGENT_AGENTRUNSTORE_HPP
