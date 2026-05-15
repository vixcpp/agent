/**
 *
 *  @file ToolRegistry.cpp
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
#include <vix/ai/agent/tools/ToolRegistry.hpp>

#include <memory>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <vix/ai/agent/AgentError.hpp>

namespace vix::ai::agent
{
  vix::error::Error ToolRegistry::add(std::shared_ptr<Tool> tool)
  {
    if (!tool)
    {
      return make_agent_error(
          AgentErrorCode::ToolNotFound,
          "cannot register a null tool");
    }

    const std::string tool_name(tool->name());
    if (tool_name.empty())
    {
      return make_agent_error(
          AgentErrorCode::ToolNotFound,
          "cannot register a tool with an empty name");
    }

    tools_[tool_name] = std::move(tool);
    return {};
  }

  bool ToolRegistry::contains(std::string_view name) const noexcept
  {
    return find(name) != nullptr;
  }

  std::shared_ptr<Tool> ToolRegistry::find(
      std::string_view name) const noexcept
  {
    if (name.empty())
    {
      return nullptr;
    }

    const auto it = tools_.find(std::string(name));
    if (it == tools_.end())
    {
      return nullptr;
    }

    return it->second;
  }

  AgentResult<ToolResult> ToolRegistry::run(
      const ToolCall &call) const
  {
    if (!call.valid())
    {
      return make_agent_error(
          AgentErrorCode::ToolFailed,
          "tool call is invalid");
    }

    auto tool = find(call.name);
    if (!tool)
    {
      return make_agent_error(
          AgentErrorCode::ToolNotFound,
          "tool was not found: " + call.name);
    }

    return tool->run(call);
  }

  std::vector<ToolInfo> ToolRegistry::list() const
  {
    std::vector<ToolInfo> out;
    out.reserve(tools_.size());

    for (const auto &[name, tool] : tools_)
    {
      ToolInfo info;
      info.name = name;

      if (tool)
      {
        info.description = std::string(tool->description());
      }

      out.push_back(std::move(info));
    }

    return out;
  }

  std::size_t ToolRegistry::size() const noexcept
  {
    return tools_.size();
  }

  bool ToolRegistry::empty() const noexcept
  {
    return tools_.empty();
  }

} // namespace vix::ai::agent
