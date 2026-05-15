/**
 *
 *  @file ToolCall.hpp
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
#ifndef VIX_AI_AGENT_TOOLS_TOOLCALL_HPP
#define VIX_AI_AGENT_TOOLS_TOOLCALL_HPP

#include <string>

#include <vix/json/json.hpp>

namespace vix::ai::agent
{
  /**
   * @struct ToolCall
   * @brief Request to execute a tool.
   *
   * ToolCall represents a structured tool invocation produced by the agent,
   * by a model response, or by the caller.
   */
  struct ToolCall
  {
    /**
     * @brief Unique tool call identifier.
     */
    std::string id;

    /**
     * @brief Tool name.
     *
     * Examples:
     * - "file.read"
     * - "project.scan"
     * - "command.run"
     */
    std::string name;

    /**
     * @brief Tool arguments as structured JSON.
     */
    vix::json::Json arguments{vix::json::Json::object()};

    /**
     * @brief Optional user-facing reason for this tool call.
     */
    std::string reason;

    /**
     * @brief Return true if the call has the minimum required fields.
     */
    [[nodiscard]] bool valid() const noexcept
    {
      return !id.empty() && !name.empty();
    }
  };

} // namespace vix::ai::agent

#endif // VIX_AI_AGENT_TOOLS_TOOLCALL_HPP
