/**
 *
 *  @file AgentId.cpp
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
#include <vix/ai/agent/crypto/AgentId.hpp>

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

#include <vix/ai/agent/AgentError.hpp>
#include <vix/crypto/hex.hpp>
#include <vix/crypto/random.hpp>

namespace vix::ai::agent
{
  AgentResult<std::string> AgentId::random(
      std::string_view prefix,
      std::size_t random_bytes)
  {
    if (random_bytes == 0)
    {
      return make_agent_error(
          AgentErrorCode::InternalFailure,
          "random id byte size must be greater than zero");
    }

    std::vector<std::uint8_t> bytes(random_bytes);

    auto generated = vix::crypto::random_bytes(bytes);
    if (!generated.ok())
    {
      return make_agent_error(
          AgentErrorCode::InternalFailure,
          std::string(generated.error().message));
    }

    std::string id = normalize_prefix(prefix);
    id += "_";
    id += vix::crypto::hex_lower(bytes);

    return id;
  }

  AgentResult<std::string> AgentId::run_id()
  {
    return random("run");
  }

  AgentResult<std::string> AgentId::tool_call_id()
  {
    return random("tool");
  }

  AgentResult<std::string> AgentId::operation_id()
  {
    return random("op");
  }

  std::string AgentId::normalize_prefix(
      std::string_view prefix)
  {
    if (prefix.empty())
    {
      return "id";
    }

    std::string out;
    out.reserve(prefix.size());

    for (const char c : prefix)
    {
      if ((c >= 'a' && c <= 'z') ||
          (c >= 'A' && c <= 'Z') ||
          (c >= '0' && c <= '9'))
      {
        out.push_back(c);
        continue;
      }

      if (c == '_' || c == '-')
      {
        out.push_back('_');
      }
    }

    if (out.empty())
    {
      return "id";
    }

    return out;
  }

} // namespace vix::ai::agent
