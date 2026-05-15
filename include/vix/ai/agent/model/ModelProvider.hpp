/**
 *
 *  @file ModelProvider.hpp
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
#ifndef VIX_AI_AGENT_MODEL_MODELPROVIDER_HPP
#define VIX_AI_AGENT_MODEL_MODELPROVIDER_HPP

#include <string_view>

#include <vix/ai/agent/AgentResult.hpp>
#include <vix/ai/agent/model/ModelRequest.hpp>
#include <vix/ai/agent/model/ModelResponse.hpp>

namespace vix::ai::agent
{
  /**
   * @class ModelProvider
   * @brief Base interface for model providers used by the Vix AI Agent.
   *
   * A ModelProvider is responsible for sending a provider-neutral
   * ModelRequest to a concrete AI backend and returning a provider-neutral
   * ModelResponse.
   *
   * Examples:
   * - Ollama
   * - llama.cpp server
   * - OpenAI-compatible server later
   */
  class ModelProvider
  {
  public:
    /**
     * @brief Virtual destructor.
     */
    virtual ~ModelProvider() = default;

    /**
     * @brief Return the stable provider name.
     *
     * Examples:
     * - "ollama"
     * - "llama.cpp"
     * - "openai"
     */
    [[nodiscard]] virtual std::string_view name() const noexcept = 0;

    /**
     * @brief Return whether the provider is local-first.
     *
     * Local providers can usually work without cloud dependencies.
     */
    [[nodiscard]] virtual bool local() const noexcept = 0;

    /**
     * @brief Return whether the provider is available.
     *
     * This may check configuration, local server availability, or required
     * runtime dependencies.
     */
    [[nodiscard]] virtual AgentResult<bool> available() const = 0;

    /**
     * @brief Generate a response from a model request.
     *
     * @param request Provider-neutral model request.
     * @return ModelResponse on success, or structured error.
     */
    [[nodiscard]] virtual AgentResult<ModelResponse> generate(
        const ModelRequest &request) = 0;
  };

} // namespace vix::ai::agent

#endif // VIX_AI_AGENT_MODEL_MODELPROVIDER_HPP
