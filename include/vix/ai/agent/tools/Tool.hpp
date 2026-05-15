/**
 *
 *  @file Tool.hpp
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
#ifndef VIX_AI_AGENT_TOOLS_TOOL_HPP
#define VIX_AI_AGENT_TOOLS_TOOL_HPP

#include <string>
#include <string_view>

#include <vix/ai/agent/AgentResult.hpp>
#include <vix/ai/agent/tools/ToolCall.hpp>
#include <vix/ai/agent/tools/ToolResult.hpp>

namespace vix::ai::agent
{
  /**
   * @class Tool
   * @brief Base interface for all agent tools.
   *
   * A tool is a controlled action that the agent may execute.
   * Examples:
   * - read a file
   * - scan a project
   * - run a local command
   *
   * Tools must be explicit, named, and policy-aware. They should never perform
   * hidden side effects outside their documented behavior.
   */
  class Tool
  {
  public:
    /**
     * @brief Virtual destructor.
     */
    virtual ~Tool() = default;

    /**
     * @brief Return the stable tool name.
     *
     * Examples:
     * - "file.read"
     * - "project.scan"
     * - "command.run"
     */
    [[nodiscard]] virtual std::string_view name() const noexcept = 0;

    /**
     * @brief Return a short human-readable tool description.
     */
    [[nodiscard]] virtual std::string_view description() const noexcept = 0;

    /**
     * @brief Execute the tool.
     *
     * @param call Structured tool call.
     * @return ToolResult on success, or a structured error when execution
     * cannot even produce a normal ToolResult.
     */
    [[nodiscard]] virtual AgentResult<ToolResult> run(
        const ToolCall &call) = 0;
  };

} // namespace vix::ai::agent

#endif // VIX_AI_AGENT_TOOLS_TOOL_HPP
