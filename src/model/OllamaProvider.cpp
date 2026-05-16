/**
 *
 *  @file OllamaProvider.cpp
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
#include <vix/ai/agent/model/OllamaProvider.hpp>

#include <algorithm>
#include <string>
#include <string_view>
#include <utility>

#include <vix/ai/agent/AgentError.hpp>
#include <vix/ai/agent/AgentRunTimer.hpp>
#include <vix/json/json.hpp>
#include <vix/net/http/ClientRequest.hpp>
#include <vix/net/http/CurlClient.hpp>
#include <vix/net/http/Method.hpp>
#include <vix/process/Command.hpp>
#include <vix/process/Output.hpp>
#include <vix/process/PipeMode.hpp>

namespace vix::ai::agent
{
  namespace
  {
    [[nodiscard]] std::string role_to_string(ModelMessageRole role)
    {
      switch (role)
      {
      case ModelMessageRole::System:
        return "system";
      case ModelMessageRole::User:
        return "user";
      case ModelMessageRole::Assistant:
        return "assistant";
      case ModelMessageRole::Tool:
        return "tool";
      }

      return "user";
    }

    [[nodiscard]] bool starts_with_http_endpoint(
        std::string_view endpoint) noexcept
    {
      return endpoint.rfind("http://", 0) == 0 ||
             endpoint.rfind("https://", 0) == 0;
    }

    [[nodiscard]] std::string trim_trailing_slashes(
        std::string endpoint)
    {
      while (!endpoint.empty() && endpoint.back() == '/')
      {
        endpoint.pop_back();
      }

      return endpoint;
    }

    [[nodiscard]] std::string json_string_or_empty(
        const vix::json::Json &json,
        const char *key)
    {
      if (json.contains(key) && json[key].is_string())
      {
        return json[key].get<std::string>();
      }

      return {};
    }

    [[nodiscard]] std::uint64_t json_uint64_or_zero(
        const vix::json::Json &json,
        const char *key)
    {
      if (json.contains(key) && json[key].is_number_unsigned())
      {
        return json[key].get<std::uint64_t>();
      }

      if (json.contains(key) && json[key].is_number_integer())
      {
        const auto value = json[key].get<std::int64_t>();
        return value > 0 ? static_cast<std::uint64_t>(value) : 0;
      }

      return 0;
    }

    [[nodiscard]] std::uint64_t effective_timeout_ms(
        const AgentConfig &config,
        const ModelRequest &request) noexcept
    {
      if (request.timeout_ms > 0)
      {
        return request.timeout_ms;
      }

      return config.timeout_ms;
    }

    [[nodiscard]] std::string build_chat_prompt(
        const ModelRequest &request)
    {
      std::string prompt;

      if (!request.system_prompt.empty())
      {
        prompt += "System:\n";
        prompt += request.system_prompt;
        prompt += "\n\n";
      }

      for (const auto &message : request.messages)
      {
        prompt += role_to_string(message.role);
        prompt += ":\n";

        if (!message.tool_name.empty())
        {
          prompt += "[tool: ";
          prompt += message.tool_name;
          prompt += "]\n";
        }

        if (!message.tool_call_id.empty())
        {
          prompt += "[tool_call_id: ";
          prompt += message.tool_call_id;
          prompt += "]\n";
        }

        prompt += message.content;
        prompt += "\n\n";
      }

      return prompt;
    }

    [[nodiscard]] vix::json::Json build_ollama_payload(
        const std::string &model,
        const std::string &prompt,
        const ModelRequest &request)
    {
      vix::json::Json payload = vix::json::Json::object();
      payload["model"] = model;
      payload["prompt"] = prompt;
      payload["stream"] = request.stream;

      if (!request.system_prompt.empty())
      {
        payload["system"] = request.system_prompt;
      }

      if (request.max_tokens > 0)
      {
        if (!payload.contains("options") || !payload["options"].is_object())
        {
          payload["options"] = vix::json::Json::object();
        }

        payload["options"]["num_predict"] = request.max_tokens;
      }

      if (request.options.is_object() && !request.options.empty())
      {
        if (!payload.contains("options") || !payload["options"].is_object())
        {
          payload["options"] = vix::json::Json::object();
        }

        for (auto it = request.options.begin(); it != request.options.end(); ++it)
        {
          payload["options"][it.key()] = it.value();
        }
      }

      return payload;
    }
  } // namespace

  OllamaProvider::OllamaProvider(AgentConfig config)
      : endpoint_(trim_trailing_slashes(config.model_url)),
        default_model_(config.model),
        config_(std::move(config)),
        http_client_(nullptr)
  {
    ensure_http_client();
  }

  OllamaProvider::OllamaProvider(
      std::string endpoint,
      std::string default_model)
      : endpoint_(trim_trailing_slashes(std::move(endpoint))),
        default_model_(std::move(default_model)),
        config_(),
        http_client_(nullptr)
  {
    ensure_http_client();
  }

  OllamaProvider::OllamaProvider(
      AgentConfig config,
      std::shared_ptr<vix::net::http::Client> http_client)
      : endpoint_(trim_trailing_slashes(config.model_url)),
        default_model_(config.model),
        config_(std::move(config)),
        http_client_(std::move(http_client))
  {
    ensure_http_client();
  }

  OllamaProvider::OllamaProvider(
      std::string endpoint,
      std::string default_model,
      std::shared_ptr<vix::net::http::Client> http_client)
      : endpoint_(trim_trailing_slashes(std::move(endpoint))),
        default_model_(std::move(default_model)),
        config_(),
        http_client_(std::move(http_client))
  {
    ensure_http_client();
  }

  std::string_view OllamaProvider::name() const noexcept
  {
    return "ollama";
  }

  bool OllamaProvider::local() const noexcept
  {
    return true;
  }

  AgentResult<bool> OllamaProvider::available() const
  {
    vix::process::Command command("ollama");

    command.arg("list")
        .stdout_mode(vix::process::PipeMode::Pipe)
        .stderr_mode(vix::process::PipeMode::Pipe)
        .stdin_mode(vix::process::PipeMode::Null)
        .search_in_path(true)
        .detach(false)
        .inherit_environment(true);

    auto output = vix::process::output(command);
    if (!output)
    {
      return false;
    }

    return output.value().success();
  }

  AgentResult<ModelResponse> OllamaProvider::generate(
      const ModelRequest &request)
  {
    AgentRunTimer timer;

    const std::string model = effective_model(request);
    const std::string prompt = effective_prompt(request);

    if (model.empty())
    {
      return make_agent_error(
          AgentErrorCode::ConfigInvalid,
          "Ollama model cannot be empty");
    }

    if (prompt.empty())
    {
      return make_agent_error(
          AgentErrorCode::EmptyInput,
          "Ollama prompt cannot be empty");
    }

    if (endpoint_.empty() || !starts_with_http_endpoint(endpoint_))
    {
      return make_agent_error(
          AgentErrorCode::ConfigInvalid,
          "Ollama endpoint must start with http:// or https://");
    }

    ensure_http_client();

    if (!http_client_)
    {
      return make_agent_error(
          AgentErrorCode::ModelUnavailable,
          "Ollama HTTP client is not configured");
    }

    const vix::json::Json payload =
        build_ollama_payload(model, prompt, request);

    vix::net::http::ClientRequest http_request;

    http_request
        .set_method(vix::net::http::Method::Post)
        .set_url(endpoint_ + "/api/generate")
        .set_header("Content-Type", "application/json")
        .set_body(payload.dump())
        .set_timeout_ms(effective_timeout_ms(config_, request));

    auto http_response = http_client_->send(http_request);
    if (!http_response)
    {
      return make_agent_error(
          AgentErrorCode::ModelRequestFailed,
          std::string(http_response.error().message()));
    }

    ModelResponse response;
    response.model = model;
    response.provider = std::string(name());
    response.duration_ms = timer.elapsed_ms();

    if (!http_response.value().success())
    {
      response.status = ModelResponseStatus::Failed;
      response.error = http_response.value().error.empty()
                           ? http_response.value().body
                           : http_response.value().error;
      response.raw = vix::json::Json::object();
      return response;
    }

    try
    {
      response.raw = vix::json::Json::parse(http_response.value().body);

      const std::string ollama_error =
          json_string_or_empty(response.raw, "error");

      if (!ollama_error.empty())
      {
        response.status = ModelResponseStatus::Failed;
        response.error = ollama_error;
        return response;
      }

      response.text = json_string_or_empty(response.raw, "response");

      if (response.text.empty())
      {
        return make_agent_error(
            AgentErrorCode::ModelResponseInvalid,
            "Ollama response does not contain a valid response field");
      }

      const std::string response_model =
          json_string_or_empty(response.raw, "model");

      if (!response_model.empty())
      {
        response.model = response_model;
      }

      const std::uint64_t total_duration =
          json_uint64_or_zero(response.raw, "total_duration");

      if (total_duration > 0)
      {
        response.duration_ms = total_duration / 1'000'000;
      }
      else
      {
        response.duration_ms = timer.elapsed_ms();
      }

      response.usage.input_tokens =
          json_uint64_or_zero(response.raw, "prompt_eval_count");

      response.usage.output_tokens =
          json_uint64_or_zero(response.raw, "eval_count");

      response.usage.total_tokens =
          response.usage.input_tokens + response.usage.output_tokens;

      response.status = ModelResponseStatus::Completed;
      return response;
    }
    catch (...)
    {
      return make_agent_error(
          AgentErrorCode::ModelResponseInvalid,
          "failed to parse Ollama response as JSON");
    }
  }

  const std::string &OllamaProvider::endpoint() const noexcept
  {
    return endpoint_;
  }

  const std::string &OllamaProvider::default_model() const noexcept
  {
    return default_model_;
  }

  std::shared_ptr<vix::net::http::Client>
  OllamaProvider::http_client() const noexcept
  {
    return http_client_;
  }

  void OllamaProvider::set_http_client(
      std::shared_ptr<vix::net::http::Client> client)
  {
    http_client_ = std::move(client);
    ensure_http_client();
  }

  std::string OllamaProvider::effective_model(
      const ModelRequest &request) const
  {
    if (!request.model.empty())
    {
      return request.model;
    }

    return default_model_;
  }

  std::string OllamaProvider::effective_prompt(
      const ModelRequest &request) const
  {
    if (!request.prompt.empty())
    {
      return request.prompt;
    }

    return build_chat_prompt(request);
  }

  void OllamaProvider::ensure_http_client()
  {
    if (!http_client_)
    {
      http_client_ = std::make_shared<vix::net::http::CurlClient>();
    }
  }

} // namespace vix::ai::agent
