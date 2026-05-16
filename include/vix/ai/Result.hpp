/**
 *
 *  @file Result.hpp
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
#ifndef VIX_AI_RESULT_HPP
#define VIX_AI_RESULT_HPP

#include <string>
#include <string_view>
#include <utility>

#include <vix/ai/agent/AgentResponse.hpp>

namespace vix::ai
{
  /**
   * @class Result
   * @brief High-level result returned by vix::ai::Agent.
   *
   * Result is a small public wrapper around the lower-level
   * vix::ai::agent::AgentResponse.
   *
   * It exposes the most common information through simple accessor methods
   * while keeping access to the raw response for advanced users.
   */
  class Result
  {
  public:
    /**
     * @brief Construct an empty AI result.
     */
    Result() = default;

    /**
     * @brief Construct a result from a low-level agent response.
     *
     * @param response Low-level agent response.
     */
    explicit Result(vix::ai::agent::AgentResponse response)
        : response_(std::move(response))
    {
    }

    /**
     * @brief Return the generated text.
     *
     * @return Text produced by the agent.
     */
    [[nodiscard]] std::string_view text() const noexcept
    {
      return response_.text;
    }

    /**
     * @brief Return the unique run identifier.
     *
     * @return Run identifier generated for this agent execution.
     */
    [[nodiscard]] std::string_view run_id() const noexcept
    {
      return response_.run_id;
    }

    /**
     * @brief Return whether the result came from cache.
     *
     * @return true if the response was loaded from cache.
     */
    [[nodiscard]] bool from_cache() const noexcept
    {
      return response_.from_cache;
    }

    /**
     * @brief Access the underlying low-level agent response.
     *
     * @return Constant reference to the raw agent response.
     */
    [[nodiscard]] const vix::ai::agent::AgentResponse &raw() const noexcept
    {
      return response_;
    }

  private:
    /**
     * @brief Wrapped low-level agent response.
     */
    vix::ai::agent::AgentResponse response_{};
  };

} // namespace vix::ai

#endif // VIX_AI_RESULT_HPP
