/**
 *
 *  @file AgentFingerprint.cpp
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
#include <vix/ai/agent/crypto/AgentFingerprint.hpp>

#include <array>
#include <cstdint>
#include <string>
#include <string_view>

#include <vix/ai/agent/AgentError.hpp>
#include <vix/crypto/hash.hpp>
#include <vix/crypto/hex.hpp>

namespace vix::ai::agent
{
  namespace
  {
    [[nodiscard]] std::string join_fingerprint_parts(
        std::string_view a,
        std::string_view b,
        std::string_view c = {},
        std::string_view d = {})
    {
      std::string out;
      out.reserve(a.size() + b.size() + c.size() + d.size() + 8);

      out += a;
      out += '\n';
      out += b;
      out += '\n';
      out += c;
      out += '\n';
      out += d;

      return out;
    }
  } // namespace

  AgentResult<std::string> AgentFingerprint::sha256_hex(
      std::string_view data)
  {
    std::array<std::uint8_t, 32> digest{};

    auto result = vix::crypto::sha256(data, digest);
    if (!result.ok())
    {
      return make_agent_error(
          AgentErrorCode::InternalFailure,
          std::string(result.error().message));
    }

    return vix::crypto::hex_lower(digest);
  }

  AgentResult<std::string> AgentFingerprint::file_fingerprint(
      std::string_view relative_path,
      std::string_view content)
  {
    if (relative_path.empty())
    {
      return make_agent_error(
          AgentErrorCode::InvalidWorkspace,
          "file fingerprint requires a relative path");
    }

    const std::string data = join_fingerprint_parts(
        "vix.agent.file",
        relative_path,
        content);

    return sha256_hex(data);
  }

  AgentResult<std::string> AgentFingerprint::prompt_fingerprint(
      std::string_view provider,
      std::string_view model,
      std::string_view prompt,
      std::string_view context_hash)
  {
    if (provider.empty())
    {
      return make_agent_error(
          AgentErrorCode::ConfigInvalid,
          "prompt fingerprint requires a provider");
    }

    if (model.empty())
    {
      return make_agent_error(
          AgentErrorCode::ConfigInvalid,
          "prompt fingerprint requires a model");
    }

    if (prompt.empty())
    {
      return make_agent_error(
          AgentErrorCode::EmptyInput,
          "prompt fingerprint requires prompt content");
    }

    const std::string data = join_fingerprint_parts(
        provider,
        model,
        prompt,
        context_hash);

    return sha256_hex(data);
  }

  std::string AgentFingerprint::file_summary_key(
      std::string_view file_hash)
  {
    std::string key;
    key.reserve(file_hash.size() + 19);
    key += "agent:file:summary:";
    key += file_hash;
    return key;
  }

  std::string AgentFingerprint::model_response_key(
      std::string_view prompt_hash)
  {
    std::string key;
    key.reserve(prompt_hash.size() + 21);
    key += "agent:model:response:";
    key += prompt_hash;
    return key;
  }

  std::string AgentFingerprint::tool_result_key(
      std::string_view tool_name,
      std::string_view arguments_hash)
  {
    std::string key;
    key.reserve(tool_name.size() + arguments_hash.size() + 20);
    key += "agent:tool:";
    key += tool_name;
    key += ":";
    key += arguments_hash;
    return key;
  }

} // namespace vix::ai::agent
