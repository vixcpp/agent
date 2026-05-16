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
#ifndef VIX_AI_AGENT_FACADE_HPP
#define VIX_AI_AGENT_FACADE_HPP

#include <string>
#include <string_view>
#include <vector>
#include <cstdint>

#include <vix/ai/Result.hpp>
#include <vix/ai/agent/Agent.hpp>
#include <vix/ai/agent/AgentConfig.hpp>
#include <vix/error/Result.hpp>

namespace vix::ai
{
  /**
   * @class Agent
   * @brief Simple public AI agent facade for Vix.cpp.
   *
   * vix::ai::Agent is the high-level public API intended for application
   * developers. It provides a small and expressive interface on top of the
   * lower-level vix::ai::agent runtime.
   *
   * The facade hides most configuration details while still allowing users to:
   * - select a model
   * - choose a workspace
   * - enable safe tools
   * - enable local memory
   * - run a prompt with a simple string
   *
   * Example:
   * @code
   * vix::ai::Agent agent;
   *
   * agent.set_model("local:llama");
   * agent.add_tool("filesystem");
   * agent.add_tool("shell");
   * agent.add_memory(".vix/ai/memory");
   *
   * auto result = agent.run("Analyze this project.");
   * @endcode
   */
  class Agent
  {
  public:
    /**
     * @brief Construct an AI agent with default local-first settings.
     *
     * Defaults are intentionally conservative:
     * - local Ollama-compatible provider
     * - file reads disabled until a filesystem tool is added
     * - process execution disabled until a shell/git tool is added
     * - file writes disabled
     * - cache and memory enabled
     */
    Agent();

    /**
     * @brief Set the model used by the agent.
     *
     * Supported shorthand values:
     * - "local:llama" maps to the default local Ollama model
     * - "ollama:<model>" selects a specific Ollama model
     *
     * Any other value is stored as the raw model name.
     *
     * @param model Model selector or model name.
     * @return Reference to this agent for chaining.
     */
    Agent &set_model(std::string model);

    /**
     * @brief Set the maximum model request timeout in milliseconds.
     *
     * This is useful for local models that may take more time to load on the
     * first request, especially with Ollama on CPU or small machines.
     *
     * @param timeout_ms Timeout in milliseconds.
     * @return Reference to this agent for chaining.
     */
    Agent &set_timeout(std::uint64_t timeout_ms);

    /**
     * @brief Set the workspace used by the agent.
     *
     * The workspace is the security boundary for filesystem and command tools.
     * All file operations are resolved inside this directory.
     *
     * @param workspace Workspace path.
     * @return Reference to this agent for chaining.
     */
    Agent &set_workspace(std::string workspace);

    /**
     * @brief Enable a named tool for the agent.
     *
     * Current public tool names:
     * - "filesystem": enables safe workspace file reading
     * - "shell": enables safe command execution with a small allowlist
     * - "git": enables Git commands through the command tool
     *
     * Unknown tool names are ignored for now.
     *
     * @param tool Tool name.
     * @return Reference to this agent for chaining.
     */
    Agent &add_tool(std::string tool);

    /**
     * @brief Enable persistent local memory for the agent.
     *
     * The provided directory is used as the agent memory directory.
     *
     * @param memory_dir Local memory directory.
     * @return Reference to this agent for chaining.
     */
    Agent &add_memory(std::string memory_dir);

    /**
     * @brief Run the agent with a single prompt.
     *
     * This is the main high-level API:
     * @code
     * auto result = agent.run("Explain this project.");
     * @endcode
     *
     * @param prompt User prompt or instruction.
     * @return Result containing vix::ai::Result on success, or a structured error.
     */
    [[nodiscard]] vix::error::Result<Result> run(
        std::string_view prompt);

    /**
     * @brief Access the underlying low-level agent configuration.
     *
     * This is useful for inspection, debugging, or advanced integration.
     *
     * @return Current underlying agent configuration.
     */
    [[nodiscard]] const vix::ai::agent::AgentConfig &config() const noexcept;

  private:
    /**
     * @brief Apply the behavior associated with a public tool name.
     *
     * This updates the underlying low-level AgentConfig.
     *
     * @param tool Tool name.
     */
    void apply_tool(std::string_view tool);

  private:
    /**
     * @brief Low-level agent configuration used by the facade.
     */
    vix::ai::agent::AgentConfig config_{};

    /**
     * @brief Workspace path used for agent runs.
     */
    std::string workspace_{"."};

    /**
     * @brief Public tool names enabled through add_tool().
     */
    std::vector<std::string> tools_{};
  };

} // namespace vix::ai

#endif // VIX_AI_AGENT_FACADE_HPP
