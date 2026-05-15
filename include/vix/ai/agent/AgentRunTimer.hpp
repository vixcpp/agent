/**
 *
 *  @file AgentRunTimer.hpp
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
#ifndef VIX_AI_AGENT_AGENTRUNTIMER_HPP
#define VIX_AI_AGENT_AGENTRUNTIMER_HPP

#include <cstdint>

#include <vix/time/Clock.hpp>
#include <vix/time/Duration.hpp>
#include <vix/time/Timestamp.hpp>

namespace vix::ai::agent
{
  /**
   * @class AgentRunTimer
   * @brief Tracks wall-clock timestamps and monotonic elapsed time for an agent run.
   *
   * AgentRunTimer uses two different clocks:
   * - SystemClock for persisted timestamps such as started_at and finished_at
   * - SteadyClock for measuring elapsed duration safely
   *
   * This avoids using wall time for duration measurement, because wall time may
   * jump due to NTP sync, manual clock changes, VM suspend/resume, or system
   * time adjustments.
   */
  class AgentRunTimer
  {
  public:
    /**
     * @brief Start a new timer immediately.
     */
    AgentRunTimer() noexcept;

    /**
     * @brief Restart the timer.
     *
     * This resets the wall-clock start timestamp and the monotonic start point.
     */
    void restart() noexcept;

    /**
     * @brief Return the wall-clock timestamp when the timer started.
     */
    [[nodiscard]] vix::time::Timestamp started_at() const noexcept;

    /**
     * @brief Return the current wall-clock timestamp.
     *
     * This is useful when recording a finished_at value.
     */
    [[nodiscard]] vix::time::Timestamp now() const noexcept;

    /**
     * @brief Return elapsed monotonic duration since the timer started.
     */
    [[nodiscard]] vix::time::Duration elapsed() const noexcept;

    /**
     * @brief Return elapsed monotonic duration in milliseconds.
     */
    [[nodiscard]] std::uint64_t elapsed_ms() const noexcept;

  private:
    vix::time::Timestamp started_at_{};
    vix::time::SteadyClock::chrono_tp steady_start_{};
  };

} // namespace vix::ai::agent

#endif // VIX_AI_AGENT_AGENTRUNTIMER_HPP
