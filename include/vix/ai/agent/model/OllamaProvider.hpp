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

#include <memory>
#include <string>
#include <string_view>

#include <vix/ai/agent/AgentConfig.hpp>
#include <vix/ai/agent/model/ModelProvider.hpp>
#include <vix/ai/agent/model/ModelRequest.hpp>
#include <vix/ai/agent/model/ModelResponse.hpp>
#include <vix/net/http/Client.hpp>

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
   *
   * The HTTP transport is abstracted through vix::net::http::Client.
   * By default, the provider creates a curl-backed HTTP client, but callers
   * may inject another implementation later without changing the provider API.
   */
  class OllamaProvider final : public ModelProvider
  {
  public:
    /**
     * @brief Construct an Ollama provider from AgentConfig.
     *
     * The endpoint and default model are read from the agent configuration.
     * A default HTTP client is created automatically when none is injected.
     *
     * @param config Agent configuration.
     */
    explicit OllamaProvider(AgentConfig config);

    /**
     * @brief Construct an Ollama provider from endpoint and default model.
     *
     * A default HTTP client is created automatically when none is injected.
     *
     * @param endpoint Ollama server endpoint.
     * @param default_model Default model name.
     */
    OllamaProvider(std::string endpoint, std::string default_model);

    /**
     * @brief Construct an Ollama provider from AgentConfig and HTTP client.
     *
     * This overload allows tests or advanced users to inject a custom HTTP
     * client implementation.
     *
     * @param config Agent configuration.
     * @param http_client HTTP client used to send Ollama requests.
     */
    OllamaProvider(
        AgentConfig config,
        std::shared_ptr<vix::net::http::Client> http_client);

    /**
     * @brief Construct an Ollama provider from endpoint, model and HTTP client.
     *
     * This overload allows the provider to use a custom HTTP transport while
     * keeping the provider logic independent from the concrete backend.
     *
     * @param endpoint Ollama server endpoint.
     * @param default_model Default model name.
     * @param http_client HTTP client used to send Ollama requests.
     */
    OllamaProvider(
        std::string endpoint,
        std::string default_model,
        std::shared_ptr<vix::net::http::Client> http_client);

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

    /**
     * @brief Return the HTTP client used by the provider.
     */
    [[nodiscard]] std::shared_ptr<vix::net::http::Client>
    http_client() const noexcept;

    /**
     * @brief Replace the HTTP client used by the provider.
     *
     * If the provided client is null, the provider recreates its default
     * curl-backed HTTP client.
     *
     * @param client HTTP client implementation.
     */
    void set_http_client(
        std::shared_ptr<vix::net::http::Client> client);

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

    /**
     * @brief Ensure the provider has a usable HTTP client.
     */
    void ensure_http_client();

  private:
    std::string endpoint_{"http://127.0.0.1:11434"};
    std::string default_model_{"llama3"};
    AgentConfig config_{};
    std::shared_ptr<vix::net::http::Client> http_client_{};
  };

} // namespace vix::ai::agent

#endif // VIX_AI_AGENT_MODEL_OLLAMAPROVIDER_HPP
