/**
 *
 *  @file ToolResult.hpp
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
#ifndef VIX_AI_AGENT_TOOLS_TOOLRESULT_HPP
#define VIX_AI_AGENT_TOOLS_TOOLRESULT_HPP

#include <cstdint>
#include <string>

#include <vix/json/json.hpp>

namespace vix::ai::agent
{
  /**
   * @struct ToolResult
   * @brief Result produced by a tool execution.
   *
   * ToolResult keeps both human-readable output and optional structured data.
   * The text output is useful for model context, while JSON data is useful for
   * logs, reports, cache, and future sync.
   */
  struct ToolResult
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
     * @brief Human-readable output.
     */
    std::string output;

    /**
     * @brief Error message when ok is false.
     */
    std::string error;

    /**
     * @brief Tool execution duration in milliseconds.
     */
    std::uint64_t duration_ms{0};

    /**
     * @brief Optional structured data returned by the tool.
     */
    vix::json::Json data{vix::json::Json::object()};

    /**
     * @brief Build a successful tool result.
     */
    [[nodiscard]] static ToolResult success(
        std::string id,
        std::string name,
        std::string output,
        vix::json::Json data = vix::json::Json::object())
    {
      ToolResult result;
      result.id = std::move(id);
      result.name = std::move(name);
      result.ok = true;
      result.output = std::move(output);
      result.data = std::move(data);
      return result;
    }

    /**
     * @brief Build a failed tool result.
     */
    [[nodiscard]] static ToolResult failure(
        std::string id,
        std::string name,
        std::string error)
    {
      ToolResult result;
      result.id = std::move(id);
      result.name = std::move(name);
      result.ok = false;
      result.error = std::move(error);
      return result;
    }
  };

} // namespace vix::ai::agent

#endif // VIX_AI_AGENT_TOOLS_TOOLRESULT_HPP
