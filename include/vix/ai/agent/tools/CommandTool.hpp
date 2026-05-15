/**
 *
 *  @file CommandTool.hpp
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
#ifndef VIX_AI_AGENT_TOOLS_COMMANDTOOL_HPP
#define VIX_AI_AGENT_TOOLS_COMMANDTOOL_HPP

#include <string>
#include <string_view>
#include <unordered_set>

#include <vix/ai/agent/AgentConfig.hpp>
#include <vix/ai/agent/AgentWorkspace.hpp>
#include <vix/ai/agent/tools/Tool.hpp>
#include <vix/ai/agent/tools/ToolCall.hpp>
#include <vix/ai/agent/tools/ToolResult.hpp>

namespace vix::ai::agent
{
  /**
   * @class CommandTool
   * @brief Tool that executes controlled local commands inside the workspace.
   *
   * Tool name:
   * - "command.run"
   *
   * Expected arguments:
   * @code
   * {
   *   "program": "vix",
   *   "args": ["build"],
   *   "working_directory": "."
   * }
   * @endcode
   *
   * CommandTool is intentionally restricted:
   * - it only runs when process execution is allowed
   * - it runs inside the agent workspace
   * - it may reject dangerous programs
   * - it truncates large outputs according to AgentConfig
   */
  class CommandTool final : public Tool
  {
  public:
    /**
     * @brief Construct a command tool.
     *
     * @param workspace Agent workspace.
     * @param config Agent configuration.
     */
    CommandTool(AgentWorkspace workspace, AgentConfig config);

    /**
     * @brief Return the stable tool name.
     */
    [[nodiscard]] std::string_view name() const noexcept override;

    /**
     * @brief Return a short tool description.
     */
    [[nodiscard]] std::string_view description() const noexcept override;

    /**
     * @brief Execute the command tool.
     */
    [[nodiscard]] AgentResult<ToolResult> run(
        const ToolCall &call) override;

    /**
     * @brief Return true if a program is allowed by the command policy.
     */
    [[nodiscard]] bool is_allowed_program(
        std::string_view program) const noexcept;

    /**
     * @brief Add a blocked program name.
     */
    void block_program(std::string program);

    /**
     * @brief Access the workspace used by this tool.
     */
    [[nodiscard]] const AgentWorkspace &workspace() const noexcept;

    /**
     * @brief Access the config used by this tool.
     */
    [[nodiscard]] const AgentConfig &config() const noexcept;

  private:
    /**
     * @brief Truncate command output according to AgentConfig.
     */
    [[nodiscard]] std::string truncate_output(
        std::string output) const;

  private:
    AgentWorkspace workspace_{};
    AgentConfig config_{};

    std::unordered_set<std::string> blocked_programs_{};
  };

} // namespace vix::ai::agent

#endif // VIX_AI_AGENT_TOOLS_COMMANDTOOL_HPP
