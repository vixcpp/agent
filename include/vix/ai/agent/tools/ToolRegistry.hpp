/**
 *
 *  @file ToolRegistry.hpp
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
#ifndef VIX_AI_AGENT_TOOLS_TOOLREGISTRY_HPP
#define VIX_AI_AGENT_TOOLS_TOOLREGISTRY_HPP

#include <memory>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include <vix/ai/agent/AgentResult.hpp>
#include <vix/ai/agent/tools/Tool.hpp>
#include <vix/ai/agent/tools/ToolCall.hpp>
#include <vix/ai/agent/tools/ToolResult.hpp>

namespace vix::ai::agent
{
  /**
   * @struct ToolInfo
   * @brief Public metadata describing a registered tool.
   */
  struct ToolInfo
  {
    /**
     * @brief Tool name.
     */
    std::string name;

    /**
     * @brief Tool description.
     */
    std::string description;
  };

  /**
   * @class ToolRegistry
   * @brief Stores and executes agent tools by name.
   *
   * ToolRegistry is the central registry for controlled agent capabilities.
   * The agent should only execute tools that are explicitly registered here.
   */
  class ToolRegistry
  {
  public:
    /**
     * @brief Construct an empty registry.
     */
    ToolRegistry() = default;

    /**
     * @brief Register a tool.
     *
     * If a tool with the same name already exists, it is replaced.
     *
     * @param tool Tool instance.
     * @return Success or structured error.
     */
    [[nodiscard]] vix::error::Error add(std::shared_ptr<Tool> tool);

    /**
     * @brief Return true if a tool exists.
     */
    [[nodiscard]] bool contains(std::string_view name) const noexcept;

    /**
     * @brief Find a tool by name.
     *
     * @param name Tool name.
     * @return Shared tool pointer, or nullptr when not found.
     */
    [[nodiscard]] std::shared_ptr<Tool> find(
        std::string_view name) const noexcept;

    /**
     * @brief Execute a tool call by resolving its tool name.
     *
     * @param call Tool call.
     * @return ToolResult on success, or structured error if the tool is missing
     * or the call is invalid.
     */
    [[nodiscard]] AgentResult<ToolResult> run(
        const ToolCall &call) const;

    /**
     * @brief Return metadata for all registered tools.
     */
    [[nodiscard]] std::vector<ToolInfo> list() const;

    /**
     * @brief Return number of registered tools.
     */
    [[nodiscard]] std::size_t size() const noexcept;

    /**
     * @brief Return true if the registry has no tools.
     */
    [[nodiscard]] bool empty() const noexcept;

  private:
    std::unordered_map<std::string, std::shared_ptr<Tool>> tools_{};
  };

} // namespace vix::ai::agent

#endif // VIX_AI_AGENT_TOOLS_TOOLREGISTRY_HPP
