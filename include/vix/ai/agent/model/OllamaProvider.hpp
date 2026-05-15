/**
 *
 *  @file OllamaProvider.hpp
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
#ifndef VIX_AI_AGENT_MODEL_OLLAMAPROVIDER_HPP
#define VIX_AI_AGENT_MODEL_OLLAMAPROVIDER_HPP

#include <string>
#include <string_view>

#include <vix/ai/agent/AgentConfig.hpp>
#include <vix/ai/agent/model/ModelProvider.hpp>
#include <vix/ai/agent/model/ModelRequest.hpp>
#include <vix/ai/agent/model/ModelResponse.hpp>

namespace vix::ai::agent
{
  /**
   * @class OllamaProvider
   * @brief Model provider for a local Ollama-compatible server.
   *
   * OllamaProvider is the first local provider for Vix Agent.
   * It targets a local Ollama server, usually running at:
   * http://127.0.0.1:11434
   *
   * The provider is local-first and suitable for offline developer workflows.
   */
  class OllamaProvider final : public ModelProvider
  {
  public:
    /**
     * @brief Construct an Ollama provider from AgentConfig.
     *
     * @param config Agent configuration.
     */
    explicit OllamaProvider(AgentConfig config);

    /**
     * @brief Construct an Ollama provider from endpoint and default model.
     *
     * @param endpoint Ollama server endpoint.
     * @param default_model Default model name.
     */
    OllamaProvider(std::string endpoint, std::string default_model);

    /**
     * @brief Return provider name.
     */
    [[nodiscard]] std::string_view name() const noexcept override;

    /**
     * @brief Return true because Ollama is a local-first provider.
     */
    [[nodiscard]] bool local() const noexcept override;

    /**
     * @brief Check whether the Ollama provider is available.
     *
     * The implementation may call the Ollama API or use a lightweight
     * local process/network check.
     */
    [[nodiscard]] AgentResult<bool> available() const override;

    /**
     * @brief Generate a model response through Ollama.
     *
     * @param request Provider-neutral model request.
     * @return ModelResponse on success.
     */
    [[nodiscard]] AgentResult<ModelResponse> generate(
        const ModelRequest &request) override;

    /**
     * @brief Return configured endpoint.
     */
    [[nodiscard]] const std::string &endpoint() const noexcept;

    /**
     * @brief Return default model.
     */
    [[nodiscard]] const std::string &default_model() const noexcept;

  private:
    /**
     * @brief Build the effective model name for a request.
     */
    [[nodiscard]] std::string effective_model(
        const ModelRequest &request) const;

    /**
     * @brief Build the effective prompt for a request.
     */
    [[nodiscard]] std::string effective_prompt(
        const ModelRequest &request) const;

  private:
    std::string endpoint_{"http://127.0.0.1:11434"};
    std::string default_model_{"llama3"};
    AgentConfig config_{};
  };

} // namespace vix::ai::agent

#endif // VIX_AI_AGENT_MODEL_OLLAMAPROVIDER_HPP
