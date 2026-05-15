/**
 *
 *  @file AgentResult.hpp
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
#ifndef VIX_AI_AGENT_AGENTRESULT_HPP
#define VIX_AI_AGENT_AGENTRESULT_HPP

#include <vix/error/Result.hpp>

namespace vix::ai::agent
{
  /**
   * @brief Standard result type used by the Vix AI Agent module.
   *
   * AgentResult is an alias around vix::error::Result<T>.
   * It is used across the agent module to return either:
   * - a successful value
   * - a structured vix::error::Error
   *
   * The agent module follows the Vix style:
   * - no exceptions for normal control flow
   * - explicit error propagation
   * - structured diagnostics
   */
  template <typename T>
  using AgentResult = vix::error::Result<T>;

} // namespace vix::ai::agent

#endif // VIX_AI_AGENT_AGENTRESULT_HPP
