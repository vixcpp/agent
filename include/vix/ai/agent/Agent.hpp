/**
 *
 *  @file Agent.hpp
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
#ifndef VIX_AI_AGENT_AGENT_HPP
#define VIX_AI_AGENT_AGENT_HPP

#include <memory>
#include <string>
#include <vector>
#include <optional>

#include <vix/ai/agent/AgentConfig.hpp>
#include <vix/ai/agent/AgentRequest.hpp>
#include <vix/ai/agent/AgentResponse.hpp>
#include <vix/ai/agent/AgentResult.hpp>
#include <vix/ai/agent/AgentWorkspace.hpp>
#include <vix/ai/agent/model/ModelProvider.hpp>
#include <vix/ai/agent/tools/ToolRegistry.hpp>
#include <vix/ai/agent/tools/ToolResult.hpp>

namespace vix::ai::agent
{
  /**
   * @class Agent
   * @brief Main orchestrator of the Vix AI Agent runtime.
   *
   * Agent receives a request, prepares the workspace, builds model context,
   * optionally executes controlled tools, and returns a structured response.
   *
   * The first implementation is local-first and designed around:
   * - local workspace scanning
   * - local model providers such as Ollama
   * - controlled tools
   * - structured errors
   * - local cache and memory later
   */
  class Agent
  {
  public:
    /**
     * @brief Construct an agent using default configuration.
     */
    Agent();

    /**
     * @brief Construct an agent from configuration.
     *
     * @param config Agent configuration.
     */
    explicit Agent(AgentConfig config);

    /**
     * @brief Construct an agent with explicit model provider.
     *
     * @param config Agent configuration.
     * @param provider Model provider.
     */
    Agent(
        AgentConfig config,
        std::shared_ptr<ModelProvider> provider);

    /**
     * @brief Run the agent for a single request.
     *
     * @param request User request.
     * @return AgentResponse on success, or structured error.
     */
    [[nodiscard]] AgentResult<AgentResponse> run(
        const AgentRequest &request);

    /**
     * @brief Set the model provider.
     */
    void set_model_provider(std::shared_ptr<ModelProvider> provider);

    /**
     * @brief Access the model provider.
     */
    [[nodiscard]] std::shared_ptr<ModelProvider> model_provider() const noexcept;

    /**
     * @brief Access the tool registry.
     */
    [[nodiscard]] ToolRegistry &tools() noexcept;

    /**
     * @brief Access the tool registry.
     */
    [[nodiscard]] const ToolRegistry &tools() const noexcept;

    /**
     * @brief Access the agent configuration.
     */
    [[nodiscard]] const AgentConfig &config() const noexcept;

    /**
     * @brief Update the agent configuration.
     */
    void set_config(AgentConfig config);

  private:
    /**
     * @brief Validate an incoming request.
     */
    [[nodiscard]] vix::error::Error validate_request(
        const AgentRequest &request) const;

    /**
     * @brief Prepare the workspace for the request.
     */
    [[nodiscard]] AgentResult<AgentWorkspace> prepare_workspace(
        const AgentRequest &request) const;

    /**
     * @brief Build the model request from the agent request and workspace.
     */
    [[nodiscard]] AgentResult<ModelRequest> build_model_request(
        const AgentRequest &request,
        const AgentWorkspace &workspace) const;

    /**
     * @brief Convert a model response into an agent response.
     */
    [[nodiscard]] AgentResponse build_response(
        const AgentRequest &request,
        const ModelResponse &model_response,
        std::string run_id,
        std::uint64_t duration_ms) const;

    /**
     * @brief Execute model-requested tool calls.
     */
    [[nodiscard]] AgentResult<std::vector<ToolResult>> run_tool_calls(
        const ModelResponse &model_response) const;

    /**
     * @brief Append tool results to a model request.
     */
    void append_tool_results(
        ModelRequest &request,
        const std::vector<ToolResult> &tool_results) const;

    /**
     * @brief Convert tool results to public response summaries.
     */
    [[nodiscard]] std::vector<AgentToolSummary> build_tool_summaries(
        const std::vector<ToolResult> &tool_results) const;

    /**
     * @brief Build a stable cache key for a model request.
     */
    [[nodiscard]] AgentResult<std::string> build_model_cache_key(
        const ModelRequest &request) const;

    /**
     * @brief Try to load an agent response from cache.
     */
    [[nodiscard]] AgentResult<std::optional<AgentResponse>> try_load_cached_response(
        const AgentWorkspace &workspace,
        const ModelRequest &request,
        std::string_view run_id) const;

    /**
     * @brief Store an agent response in cache.
     */
    [[nodiscard]] vix::error::Error store_cached_response(
        const AgentWorkspace &workspace,
        const ModelRequest &request,
        const AgentResponse &response) const;

    /**
     * @brief Ensure a default local model provider exists.
     */
    void ensure_default_provider();

  private:
    AgentConfig config_{};
    std::shared_ptr<ModelProvider> provider_{};
    ToolRegistry tools_{};
  };

} // namespace vix::ai::agent

#endif // VIX_AI_AGENT_AGENT_HPP
