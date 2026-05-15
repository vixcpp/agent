/**
 *
 *  @file ModelResponse.hpp
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
#ifndef VIX_AI_AGENT_MODEL_MODELRESPONSE_HPP
#define VIX_AI_AGENT_MODEL_MODELRESPONSE_HPP

#include <cstdint>
#include <string>
#include <vector>

#include <vix/ai/agent/tools/ToolCall.hpp>
#include <vix/json/json.hpp>

namespace vix::ai::agent
{
  /**
   * @enum ModelResponseStatus
   * @brief Final status of a model response.
   */
  enum class ModelResponseStatus
  {
    Completed = 0,
    Failed,
    Cancelled,
    Partial
  };

  /**
   * @struct ModelUsage
   * @brief Token usage reported by a model provider when available.
   */
  struct ModelUsage
  {
    /**
     * @brief Number of input tokens.
     */
    std::uint64_t input_tokens{0};

    /**
     * @brief Number of output tokens.
     */
    std::uint64_t output_tokens{0};

    /**
     * @brief Total number of tokens.
     */
    std::uint64_t total_tokens{0};
  };

  /**
   * @struct ModelResponse
   * @brief Provider-neutral response returned by a model.
   */
  struct ModelResponse
  {
    /**
     * @brief Generated text.
     */
    std::string text;

    /**
     * @brief Model name used by the provider.
     */
    std::string model;

    /**
     * @brief Provider name.
     */
    std::string provider;

    /**
     * @brief Response status.
     */
    ModelResponseStatus status{ModelResponseStatus::Completed};

    /**
     * @brief Error message when the response failed.
     */
    std::string error;

    /**
     * @brief Duration reported or measured for the model call.
     */
    std::uint64_t duration_ms{0};

    /**
     * @brief Token usage if available.
     */
    ModelUsage usage{};

    /**
     * @brief Tool calls requested by the model.
     */
    std::vector<ToolCall> tool_calls;

    /**
     * @brief Optional raw provider response.
     */
    vix::json::Json raw{vix::json::Json::object()};

    /**
     * @brief Return true if the model response completed successfully.
     */
    [[nodiscard]] bool ok() const noexcept
    {
      return status == ModelResponseStatus::Completed ||
             status == ModelResponseStatus::Partial;
    }
  };

} // namespace vix::ai::agent

#endif // VIX_AI_AGENT_MODEL_MODELRESPONSE_HPP
