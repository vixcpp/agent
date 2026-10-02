/**
 *
 *  @file OllamaHttpTransport.hpp
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
#ifndef VIX_AI_AGENT_MODEL_DETAIL_OLLAMAHTTPTRANSPORT_HPP
#define VIX_AI_AGENT_MODEL_DETAIL_OLLAMAHTTPTRANSPORT_HPP

#include <cstdint>
#include <functional>
#include <string>
#include <unordered_map>

#include <vix/error/Result.hpp>

namespace vix::ai::agent
{
  class OllamaProvider;

  namespace detail
  {
    struct OllamaHttpRequest
    {
      std::string method;
      std::string url;
      std::unordered_map<std::string, std::string> headers;
      std::string body;
      std::uint64_t timeout_ms{0};
    };

    struct OllamaHttpResponse
    {
      int status_code{0};
      std::unordered_map<std::string, std::string> headers;
      std::string body;
      std::string error;

      [[nodiscard]] bool success() const noexcept
      {
        return status_code >= 200 && status_code < 300;
      }
    };

    using OllamaHttpResult = vix::error::Result<OllamaHttpResponse>;
    using OllamaHttpTransport = std::function<OllamaHttpResult(
        const OllamaHttpRequest &request)>;

    /**
     * @brief Test-only access to OllamaProvider's private HTTP seam.
     *
     * This header is kept under Agent's private source tree and is not
     * installed as part of the Agent public API.
     */
    class OllamaProviderTestAccess
    {
    public:
      static void set_http_transport(
          OllamaProvider &provider,
          OllamaHttpTransport transport);
    };
  } // namespace detail
} // namespace vix::ai::agent

#endif // VIX_AI_AGENT_MODEL_DETAIL_OLLAMAHTTPTRANSPORT_HPP
