/**
 *
 *  @file AgentRunTimer.cpp
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
#include <vix/ai/agent/AgentRunTimer.hpp>

#include <cstdint>
#include <limits>

namespace vix::ai::agent
{
  AgentRunTimer::AgentRunTimer() noexcept
  {
    restart();
  }

  void AgentRunTimer::restart() noexcept
  {
    started_at_ = vix::time::SystemClock::now();
    steady_start_ = vix::time::SteadyClock::now_chrono();
  }

  vix::time::Timestamp AgentRunTimer::started_at() const noexcept
  {
    return started_at_;
  }

  vix::time::Timestamp AgentRunTimer::now() const noexcept
  {
    return vix::time::SystemClock::now();
  }

  vix::time::Duration AgentRunTimer::elapsed() const noexcept
  {
    return vix::time::SteadyClock::since(steady_start_);
  }

  std::uint64_t AgentRunTimer::elapsed_ms() const noexcept
  {
    const auto ms = elapsed().count_ms();

    if (ms <= 0)
    {
      return 0;
    }

    const auto max_value = std::numeric_limits<std::uint64_t>::max();

    if (static_cast<unsigned long long>(ms) > max_value)
    {
      return max_value;
    }

    return static_cast<std::uint64_t>(ms);
  }

} // namespace vix::ai::agent
