/**
 *
 *  @file AgentConfigValidator.hpp
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
#ifndef VIX_AI_AGENT_AGENTCONFIGVALIDATOR_HPP
#define VIX_AI_AGENT_AGENTCONFIGVALIDATOR_HPP

#include <vix/ai/agent/AgentConfig.hpp>
#include <vix/error/Error.hpp>

namespace vix::ai::agent
{
  /**
   * @class AgentConfigValidator
   * @brief Validates AgentConfig before the agent starts.
   */
  class AgentConfigValidator
  {
  public:
    [[nodiscard]] static vix::error::Error validate(
        const AgentConfig &config);
  };

} // namespace vix::ai::agent

#endif // VIX_AI_AGENT_AGENTCONFIGVALIDATOR_HPP
