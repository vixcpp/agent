/**
 *
 *  @file ModelRequest.hpp
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
#ifndef VIX_AI_AGENT_MODEL_MODELREQUEST_HPP
#define VIX_AI_AGENT_MODEL_MODELREQUEST_HPP

#include <string>
#include <vector>

#include <vix/json/json.hpp>

namespace vix::ai::agent
{
  /**
   * @enum ModelMessageRole
   * @brief Role of a message sent to a model provider.
   */
  enum class ModelMessageRole
  {
    System = 0,
    User,
    Assistant,
    Tool
  };

  /**
   * @struct ModelMessage
   * @brief Single chat-style message for a model request.
   */
  struct ModelMessage
  {
    /**
     * @brief Message role.
     */
    ModelMessageRole role{ModelMessageRole::User};

    /**
     * @brief Message textual content.
     */
    std::string content;

    /**
     * @brief Optional tool name when role is Tool.
     */
    std::string tool_name;

    /**
     * @brief Optional tool call id when role is Tool.
     */
    std::string tool_call_id;
  };

  /**
   * @struct ModelRequest
   * @brief Request sent to a model provider.
   *
   * ModelRequest is provider-neutral. It contains the selected model,
   * messages, optional structured parameters, and execution limits.
   */
  struct ModelRequest
  {
    /**
     * @brief Model name.
     */
    std::string model;

    /**
     * @brief Ordered messages sent to the model.
     */
    std::vector<ModelMessage> messages;

    /**
     * @brief Optional plain prompt.
     *
     * This is useful for simple completion-style providers.
     */
    std::string prompt;

    /**
     * @brief Optional system prompt.
     */
    std::string system_prompt;

    /**
     * @brief Maximum response tokens, when supported by the provider.
     */
    unsigned max_tokens{0};

    /**
     * @brief Request timeout in milliseconds.
     */
    unsigned timeout_ms{30'000};

    /**
     * @brief Whether the provider should stream output.
     */
    bool stream{false};

    /**
     * @brief Optional provider-specific parameters.
     */
    vix::json::Json options{vix::json::Json::object()};

    /**
     * @brief Return true if the request has enough information to be sent.
     */
    [[nodiscard]] bool valid() const noexcept
    {
      return !model.empty() && (!prompt.empty() || !messages.empty());
    }
  };

} // namespace vix::ai::agent

#endif // VIX_AI_AGENT_MODEL_MODELREQUEST_HPP
