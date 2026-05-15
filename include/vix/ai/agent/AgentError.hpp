/**
 *
 *  @file AgentError.hpp
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
#ifndef VIX_AI_AGENT_AGENTERROR_HPP
#define VIX_AI_AGENT_AGENTERROR_HPP

#include <string>
#include <utility>

#include <vix/error/Error.hpp>
#include <vix/error/ErrorCategory.hpp>
#include <vix/error/ErrorCode.hpp>

namespace vix::ai::agent
{
  /**
   * @enum AgentErrorCode
   * @brief Agent-specific semantic error codes.
   *
   * These codes describe failures that belong specifically to the AI agent
   * layer: workspace validation, model calls, tools, memory, configuration,
   * and internal orchestration.
   */
  enum class AgentErrorCode
  {
    None = 0,

    EmptyInput,
    InvalidWorkspace,
    PathOutsideWorkspace,

    ModelUnavailable,
    ModelRequestFailed,
    ModelResponseInvalid,

    ToolNotFound,
    ToolNotAllowed,
    ToolFailed,
    ToolTimeout,

    MemoryUnavailable,
    MemoryWriteFailed,
    MemoryReadFailed,

    ConfigInvalid,
    InternalFailure
  };

  /**
   * @brief Return the default agent error category.
   */
  [[nodiscard]] inline constexpr vix::error::ErrorCategory agent_error_category() noexcept
  {
    return vix::error::ErrorCategory("agent");
  }

  /**
   * @brief Convert an AgentErrorCode to a generic Vix ErrorCode.
   */
  [[nodiscard]] inline constexpr vix::error::ErrorCode to_error_code(
      AgentErrorCode code) noexcept
  {
    using vix::error::ErrorCode;

    switch (code)
    {
    case AgentErrorCode::None:
      return ErrorCode::Ok;

    case AgentErrorCode::EmptyInput:
    case AgentErrorCode::InvalidWorkspace:
    case AgentErrorCode::PathOutsideWorkspace:
    case AgentErrorCode::ConfigInvalid:
      return ErrorCode::InvalidArgument;

    case AgentErrorCode::ModelUnavailable:
    case AgentErrorCode::ModelRequestFailed:
    case AgentErrorCode::ToolFailed:
      return ErrorCode::ExternalError;

    case AgentErrorCode::ModelResponseInvalid:
      return ErrorCode::ParseError;

    case AgentErrorCode::ToolNotFound:
      return ErrorCode::NotFound;

    case AgentErrorCode::ToolNotAllowed:
      return ErrorCode::PermissionDenied;

    case AgentErrorCode::ToolTimeout:
      return ErrorCode::Timeout;

    case AgentErrorCode::MemoryUnavailable:
      return ErrorCode::InvalidState;

    case AgentErrorCode::MemoryWriteFailed:
    case AgentErrorCode::MemoryReadFailed:
      return ErrorCode::IoError;

    case AgentErrorCode::InternalFailure:
      return ErrorCode::InternalError;
    }

    return ErrorCode::Unknown;
  }

  /**
   * @brief Convert an AgentErrorCode to a stable string name.
   */
  [[nodiscard]] inline const char *to_string(AgentErrorCode code) noexcept
  {
    switch (code)
    {
    case AgentErrorCode::None:
      return "none";

    case AgentErrorCode::EmptyInput:
      return "empty_input";

    case AgentErrorCode::InvalidWorkspace:
      return "invalid_workspace";

    case AgentErrorCode::PathOutsideWorkspace:
      return "path_outside_workspace";

    case AgentErrorCode::ModelUnavailable:
      return "model_unavailable";

    case AgentErrorCode::ModelRequestFailed:
      return "model_request_failed";

    case AgentErrorCode::ModelResponseInvalid:
      return "model_response_invalid";

    case AgentErrorCode::ToolNotFound:
      return "tool_not_found";

    case AgentErrorCode::ToolNotAllowed:
      return "tool_not_allowed";

    case AgentErrorCode::ToolFailed:
      return "tool_failed";

    case AgentErrorCode::ToolTimeout:
      return "tool_timeout";

    case AgentErrorCode::MemoryUnavailable:
      return "memory_unavailable";

    case AgentErrorCode::MemoryWriteFailed:
      return "memory_write_failed";

    case AgentErrorCode::MemoryReadFailed:
      return "memory_read_failed";

    case AgentErrorCode::ConfigInvalid:
      return "config_invalid";

    case AgentErrorCode::InternalFailure:
      return "internal_failure";
    }

    return "unknown";
  }

  /**
   * @brief Build a structured Vix error from an AgentErrorCode.
   *
   * @param code Agent-specific error code.
   * @param message Human-readable error message.
   * @return Structured vix::error::Error.
   */
  [[nodiscard]] inline vix::error::Error make_agent_error(
      AgentErrorCode code,
      std::string message)
  {
    return vix::error::Error(
        to_error_code(code),
        agent_error_category(),
        std::move(message));
  }

} // namespace vix::ai::agent

#endif // VIX_AI_AGENT_AGENTERROR_HPP
