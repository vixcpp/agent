/**
 *
 *  @file AgentConfigLoader.cpp
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
#include <vix/ai/agent/AgentConfigLoader.hpp>

#include <cstdint>
#include <string>
#include <string_view>

#include <vix/env/Get.hpp>
#include <vix/env/GetBool.hpp>
#include <vix/env/GetUint.hpp>

namespace vix::ai::agent
{
  namespace
  {
    void load_string(
        std::string &target,
        std::string_view key)
    {
      auto value = vix::env::get(key);
      if (value && !value.value().empty())
      {
        target = value.value();
      }
    }

    void load_uint(
        unsigned &target,
        std::string_view key)
    {
      auto value = vix::env::get_uint(key);
      if (value)
      {
        target = value.value();
      }
    }

    void load_uint64(
        std::uint64_t &target,
        std::string_view key)
    {
      auto value = vix::env::get_uint(key);
      if (value)
      {
        target = static_cast<std::uint64_t>(value.value());
      }
    }

    void load_bool(
        bool &target,
        std::string_view key)
    {
      auto value = vix::env::get_bool(key);
      if (value)
      {
        target = value.value();
      }
    }
  } // namespace

  AgentConfig AgentConfigLoader::from_environment()
  {
    return from_environment("VIX_AGENT_");
  }

  AgentConfig AgentConfigLoader::from_environment(
      std::string_view prefix)
  {
    AgentConfig config;

    load_string(config.provider, make_key(prefix, "PROVIDER"));
    load_string(config.model, make_key(prefix, "MODEL"));
    load_string(config.model_url, make_key(prefix, "MODEL_URL"));

    load_string(config.memory_dir, make_key(prefix, "MEMORY_DIR"));
    load_string(config.cache_dir, make_key(prefix, "CACHE_DIR"));
    load_string(config.runs_dir, make_key(prefix, "RUNS_DIR"));
    load_string(config.logs_dir, make_key(prefix, "LOGS_DIR"));

    load_uint64(config.timeout_ms, make_key(prefix, "TIMEOUT_MS"));
    load_uint(config.max_files, make_key(prefix, "MAX_FILES"));
    load_uint(config.max_file_size, make_key(prefix, "MAX_FILE_SIZE"));
    load_uint(config.max_tool_output, make_key(prefix, "MAX_TOOL_OUTPUT"));
    load_uint(config.max_context_chars, make_key(prefix, "MAX_CONTEXT_CHARS"));

    load_bool(config.offline, make_key(prefix, "OFFLINE"));
    load_bool(config.allow_process, make_key(prefix, "ALLOW_PROCESS"));
    load_bool(config.allow_file_read, make_key(prefix, "ALLOW_FILE_READ"));
    load_bool(config.allow_file_write, make_key(prefix, "ALLOW_FILE_WRITE"));
    load_bool(config.use_cache, make_key(prefix, "USE_CACHE"));
    load_bool(config.persist_memory, make_key(prefix, "PERSIST_MEMORY"));

    return config;
  }

  std::string AgentConfigLoader::make_key(
      std::string_view prefix,
      std::string_view name)
  {
    std::string key;
    key.reserve(prefix.size() + name.size());
    key.append(prefix.data(), prefix.size());
    key.append(name.data(), name.size());
    return key;
  }

} // namespace vix::ai::agent
