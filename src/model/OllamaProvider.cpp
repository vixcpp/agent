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

#include <string>
#include <string_view>
#include <utility>

#include <vix/ai/agent/AgentError.hpp>
#include <vix/ai/agent/AgentRunTimer.hpp>
#include <vix/json/json.hpp>
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
      payload["stream"] = false;

      if (!request.system_prompt.empty())
      {
        payload["system"] = request.system_prompt;
      }

      if (request.options.is_object() && !request.options.empty())
      {
        payload["options"] = request.options;
      }

      return payload;
    }
  } // namespace

  OllamaProvider::OllamaProvider(AgentConfig config)
      : endpoint_(config.model_url),
        default_model_(config.model),
        config_(std::move(config))
  {
  }

  OllamaProvider::OllamaProvider(
      std::string endpoint,
      std::string default_model)
      : endpoint_(std::move(endpoint)),
        default_model_(std::move(default_model))
  {
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
    vix::process::Command command;
    command.program = "ollama";
    command.args = {"list"};
    command.options.stdout_mode = vix::process::PipeMode::Pipe;
    command.options.stderr_mode = vix::process::PipeMode::Pipe;
    command.options.stdin_mode = vix::process::PipeMode::Null;
    command.options.search_in_path = true;
    command.options.detach = false;
    command.options.inherit_environment = true;

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

    if (!request.valid())
    {
      return make_agent_error(
          AgentErrorCode::ModelRequestFailed,
          "model request is invalid");
    }

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

    const vix::json::Json payload =
        build_ollama_payload(model, prompt, request);

    vix::process::Command command;
    command.program = "curl";
    command.args = {
        "-s",
        "-X",
        "POST",
        endpoint_ + "/api/generate",
        "-H",
        "Content-Type: application/json",
        "-d",
        payload.dump()};

    command.options.stdout_mode = vix::process::PipeMode::Pipe;
    command.options.stderr_mode = vix::process::PipeMode::Pipe;
    command.options.stdin_mode = vix::process::PipeMode::Null;
    command.options.search_in_path = true;
    command.options.detach = false;
    command.options.inherit_environment = true;

    auto output = vix::process::output(command);
    if (!output)
    {
      return make_agent_error(
          AgentErrorCode::ModelRequestFailed,
          std::string(output.error().message()));
    }

    ModelResponse response;
    response.model = model;
    response.provider = std::string(name());
    response.duration_ms = timer.elapsed_ms();

    if (!output.value().success())
    {
      response.status = ModelResponseStatus::Failed;
      response.error = output.value().stderr_text.empty()
                           ? output.value().stdout_text
                           : output.value().stderr_text;
      response.raw = vix::json::Json::object();
      return response;
    }

    try
    {
      response.raw = vix::json::Json::parse(output.value().stdout_text);

      if (response.raw.contains("response") &&
          response.raw["response"].is_string())
      {
        response.text = response.raw["response"].get<std::string>();
      }

      if (response.raw.contains("model") &&
          response.raw["model"].is_string())
      {
        response.model = response.raw["model"].get<std::string>();
      }

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

} // namespace vix::ai::agent
