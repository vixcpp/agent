/**
 *
 *  @file OllamaProviderTest.cpp
 *  @author Gaspard Kirira
 *
 *  Copyright 2026, Gaspard Kirira.
 *  All rights reserved.
 *  https://github.com/vixcpp/vix
 *
 *  Use of this source code is governed by a MIT license
 *  that can be found in the LICENSE file.
 *
 *  Vix.cpp
 *
 */
#include <cassert>
#include <initializer_list>
#include <memory>
#include <optional>
#include <string>

#include <vix/ai/agent/AgentConfig.hpp>
#include <vix/ai/agent/AgentError.hpp>
#include <vix/ai/agent/model/OllamaProvider.hpp>
#include <vix/error/ErrorCode.hpp>
#include <vix/json/json.hpp>

#include "model/detail/OllamaHttpTransport.hpp"

namespace
{
  class RecordingHttpTransport final
  {
  public:
    [[nodiscard]] vix::ai::agent::detail::OllamaHttpResult send(
        const vix::ai::agent::detail::OllamaHttpRequest &request) const
    {
      sent = true;
      last_request = request;

      if (failure.has_value())
      {
        return *failure;
      }

      return response;
    }

    mutable bool sent{false};
    mutable vix::ai::agent::detail::OllamaHttpRequest last_request{};
    vix::ai::agent::detail::OllamaHttpResponse response{};
    std::optional<vix::error::Error> failure{};
  };

  void set_http_transport(
      vix::ai::agent::OllamaProvider &provider,
      const std::shared_ptr<RecordingHttpTransport> &transport)
  {
    vix::ai::agent::detail::OllamaProviderTestAccess::set_http_transport(
        provider,
        [transport](const vix::ai::agent::detail::OllamaHttpRequest &request)
        {
          return transport->send(request);
        });
  }

  [[nodiscard]] vix::ai::agent::AgentConfig test_config()
  {
    vix::ai::agent::AgentConfig config;
    config.model = "test-model";
    config.model_url = "http://ollama.test/";
    config.timeout_ms = 4'321;
    return config;
  }

  [[nodiscard]] vix::ai::agent::ModelRequest test_request()
  {
    vix::ai::agent::ModelRequest request;
    request.prompt = "Explain the current contract.";
    return request;
  }

  void test_ollama_request_construction_and_request_timeout()
  {
    auto transport = std::make_shared<RecordingHttpTransport>();
    transport->response.status_code = 200;
    transport->response.body = R"({"response":"ok"})";

    vix::ai::agent::OllamaProvider provider(test_config());
    set_http_transport(provider, transport);

    auto request = test_request();
    request.stream = true;
    request.system_prompt = "Respond concisely.";
    request.max_tokens = 42;
    request.timeout_ms = 9'876;
    request.options = vix::json::Json::object();
    assert(request.options.is_object());
    request.options["temperature"] = 0.25;

    const auto result = provider.generate(request);

    assert(result);
    assert(transport->sent);
    assert(transport->last_request.method == "POST");
    assert(transport->last_request.url == "http://ollama.test/api/generate");
    assert(transport->last_request.headers.at("Content-Type") == "application/json");
    assert(transport->last_request.timeout_ms == request.timeout_ms);

    const auto payload = vix::json::Json::parse(transport->last_request.body);
    assert(payload.is_object());
    assert(payload.contains("model"));
    assert(payload.contains("prompt"));
    assert(payload.contains("stream"));
    assert(payload.contains("system"));
    assert(payload.contains("options"));
    assert(payload.at("model") == "test-model");
    assert(payload.at("prompt") == request.prompt);
    assert(payload.at("stream") == true);
    assert(payload.at("system") == request.system_prompt);

    const auto &options = payload.at("options");
    assert(options.is_object());
    assert(options.contains("num_predict"));
    assert(options.contains("temperature"));
    assert(options.at("num_predict") == request.max_tokens);
    assert(options.at("temperature") == 0.25);
  }

  void test_ollama_config_timeout_fallback()
  {
    auto transport = std::make_shared<RecordingHttpTransport>();
    transport->response.status_code = 200;
    transport->response.body = R"({"response":"ok"})";

    const auto config = test_config();
    vix::ai::agent::OllamaProvider provider(config);
    set_http_transport(provider, transport);

    auto request = test_request();
    request.timeout_ms = 0;

    const auto result = provider.generate(request);

    assert(result);
    assert(transport->last_request.timeout_ms == config.timeout_ms);
  }

  void test_ollama_successful_response_mapping()
  {
    auto transport = std::make_shared<RecordingHttpTransport>();
    transport->response.status_code = 200;
    transport->response.body = R"({"model":"returned-model","response":"generated text","total_duration":2500000,"prompt_eval_count":3,"eval_count":5})";

    vix::ai::agent::OllamaProvider provider(test_config());
    set_http_transport(provider, transport);

    const auto result = provider.generate(test_request());

    assert(result);
    assert(result.value().status == vix::ai::agent::ModelResponseStatus::Completed);
    assert(result.value().text == "generated text");
    assert(result.value().model == "returned-model");
    assert(result.value().provider == "ollama");
    assert(result.value().duration_ms == 2);
    assert(result.value().usage.input_tokens == 3);
    assert(result.value().usage.output_tokens == 5);
    assert(result.value().usage.total_tokens == 8);
  }

  void test_ollama_client_failure_is_model_request_failed()
  {
    auto transport = std::make_shared<RecordingHttpTransport>();
    transport->failure = vix::ai::agent::make_agent_error(
        vix::ai::agent::AgentErrorCode::ModelRequestFailed,
        "Ollama backend is unavailable");

    vix::ai::agent::OllamaProvider provider(test_config());
    set_http_transport(provider, transport);

    const auto result = provider.generate(test_request());

    assert(!result);
    assert(result.error().code() == vix::error::ErrorCode::ExternalError);
    assert(result.error().message() == "Ollama backend is unavailable");
  }

  void test_ollama_backend_failures_are_failed_model_responses()
  {
    auto transport = std::make_shared<RecordingHttpTransport>();
    transport->response.status_code = 503;
    transport->response.body = "service unavailable";
    transport->response.error = "Ollama backend unavailable";

    vix::ai::agent::OllamaProvider provider(test_config());
    set_http_transport(provider, transport);

    const auto result = provider.generate(test_request());

    assert(result);
    assert(result.value().status == vix::ai::agent::ModelResponseStatus::Failed);
    assert(result.value().error == "Ollama backend unavailable");

    transport->response.status_code = 200;
    transport->response.error.clear();
    transport->response.body = R"({"error":"Ollama model unavailable"})";

    const auto model_error_result = provider.generate(test_request());

    assert(model_error_result);
    assert(model_error_result.value().status ==
           vix::ai::agent::ModelResponseStatus::Failed);
    assert(model_error_result.value().error == "Ollama model unavailable");
  }

  void test_ollama_invalid_json_is_model_response_invalid()
  {
    auto transport = std::make_shared<RecordingHttpTransport>();
    transport->response.status_code = 200;
    transport->response.body = "not valid JSON";

    vix::ai::agent::OllamaProvider provider(test_config());
    set_http_transport(provider, transport);

    const auto result = provider.generate(test_request());

    assert(!result);
    assert(result.error().code() == vix::error::ErrorCode::ParseError);
    assert(result.error().message() == "failed to parse Ollama response as JSON");
  }

  void test_ollama_missing_or_empty_response_is_model_response_invalid()
  {
    for (const std::string body : {
             std::string{"{}"},
             std::string{R"({"response":""})"}})
    {
      auto transport = std::make_shared<RecordingHttpTransport>();
      transport->response.status_code = 200;
      transport->response.body = body;

      vix::ai::agent::OllamaProvider provider(test_config());
      set_http_transport(provider, transport);
      const auto result = provider.generate(test_request());

      assert(!result);
      assert(result.error().code() == vix::error::ErrorCode::ParseError);
      assert(result.error().message() ==
             "Ollama response does not contain a valid response field");
    }
  }
} // namespace

void test_ollama_provider()
{
  test_ollama_request_construction_and_request_timeout();
  test_ollama_config_timeout_fallback();
  test_ollama_successful_response_mapping();
  test_ollama_client_failure_is_model_request_failed();
  test_ollama_backend_failures_are_failed_model_responses();
  test_ollama_invalid_json_is_model_response_invalid();
  test_ollama_missing_or_empty_response_is_model_response_invalid();
}
