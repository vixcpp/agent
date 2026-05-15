/**
 *
 *  @file AgentWorkspace.cpp
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
#include <vix/ai/agent/AgentWorkspace.hpp>

#include <string>
#include <string_view>
#include <utility>

#include <vix/ai/agent/AgentError.hpp>
#include <vix/fs/EnsureDirectory.hpp>
#include <vix/fs/Exists.hpp>
#include <vix/fs/IsDirectory.hpp>
#include <vix/fs/CurrentPath.hpp>
#include <vix/path/Absolute.hpp>
#include <vix/path/IsAbsolute.hpp>
#include <vix/path/Join.hpp>
#include <vix/path/Normalize.hpp>
#include <vix/path/Relative.hpp>

namespace vix::ai::agent
{
  namespace
  {
    [[nodiscard]] bool starts_with_path(
        std::string_view value,
        std::string_view prefix) noexcept
    {
      if (value.size() < prefix.size())
      {
        return false;
      }

      if (value.substr(0, prefix.size()) != prefix)
      {
        return false;
      }

      if (value.size() == prefix.size())
      {
        return true;
      }

      const char next = value[prefix.size()];
      return next == '/' || next == '\\';
    }

    [[nodiscard]] AgentResult<std::string> absolute_from_current(
        std::string_view path)
    {
      auto current = vix::fs::current_path();
      if (!current)
      {
        return current.error();
      }

      auto resolved = vix::path::absolute(path, current.value());
      if (!resolved)
      {
        return resolved.error();
      }

      return resolved.value();
    }
  } // namespace

  AgentWorkspace::AgentWorkspace(std::string root, AgentConfig config)
      : root_(std::move(root)),
        config_(std::move(config))
  {
  }

  AgentResult<AgentWorkspace> AgentWorkspace::open(
      std::string_view root,
      const AgentConfig &config)
  {
    const std::string_view requested_root = root.empty() ? "." : root;

    auto resolved = absolute_from_current(requested_root);
    if (!resolved)
    {
      return resolved.error();
    }

    auto exists = vix::fs::exists(resolved.value());
    if (!exists)
    {
      return exists.error();
    }

    if (!exists.value())
    {
      return make_agent_error(
          AgentErrorCode::InvalidWorkspace,
          "workspace path does not exist");
    }

    auto is_dir = vix::fs::is_directory(resolved.value());
    if (!is_dir)
    {
      return is_dir.error();
    }

    if (!is_dir.value())
    {
      return make_agent_error(
          AgentErrorCode::InvalidWorkspace,
          "workspace path is not a directory");
    }

    AgentWorkspace workspace(resolved.value(), config);

    auto layout_error = workspace.ensure_layout();
    if (layout_error)
    {
      return layout_error;
    }

    return workspace;
  }

  vix::error::Error AgentWorkspace::ensure_layout() const
  {
    auto memory = memory_dir();
    if (!memory)
    {
      return memory.error();
    }

    auto cache = cache_dir();
    if (!cache)
    {
      return cache.error();
    }

    auto runs = runs_dir();
    if (!runs)
    {
      return runs.error();
    }

    auto logs = logs_dir();
    if (!logs)
    {
      return logs.error();
    }

    auto ok = vix::fs::ensure_directory(memory.value());
    if (!ok)
    {
      return ok.error();
    }

    ok = vix::fs::ensure_directory(cache.value());
    if (!ok)
    {
      return ok.error();
    }

    ok = vix::fs::ensure_directory(runs.value());
    if (!ok)
    {
      return ok.error();
    }

    ok = vix::fs::ensure_directory(logs.value());
    if (!ok)
    {
      return ok.error();
    }

    return {};
  }

  const std::string &AgentWorkspace::root() const noexcept
  {
    return root_;
  }

  bool AgentWorkspace::valid() const noexcept
  {
    return !root_.empty();
  }

  AgentResult<std::string> AgentWorkspace::resolve_inside(
      std::string_view path) const
  {
    if (!valid())
    {
      return make_agent_error(
          AgentErrorCode::InvalidWorkspace,
          "workspace is not initialized");
    }

    if (path.empty())
    {
      return make_agent_error(
          AgentErrorCode::PathOutsideWorkspace,
          "path cannot be empty");
    }

    AgentResult<std::string> resolved =
        vix::path::is_absolute(path)
            ? vix::path::normalize(path)
            : vix::path::absolute(path, root_);

    if (!resolved)
    {
      return resolved.error();
    }

    if (!contains(resolved.value()))
    {
      return make_agent_error(
          AgentErrorCode::PathOutsideWorkspace,
          "path is outside the agent workspace");
    }

    return resolved.value();
  }

  AgentResult<std::string> AgentWorkspace::relative_to_root(
      std::string_view path) const
  {
    auto resolved = resolve_inside(path);
    if (!resolved)
    {
      return resolved.error();
    }

    auto relative = vix::path::relative(resolved.value(), root_);
    if (!relative)
    {
      return relative.error();
    }

    return relative.value();
  }

  bool AgentWorkspace::contains(std::string_view path) const
  {
    if (root_.empty() || path.empty())
    {
      return false;
    }

    auto normalized = vix::path::normalize(path);
    if (!normalized)
    {
      return false;
    }

    return starts_with_path(normalized.value(), root_);
  }

  AgentResult<std::string> AgentWorkspace::memory_dir() const
  {
    return resolve_config_path(config_.memory_dir);
  }

  AgentResult<std::string> AgentWorkspace::cache_dir() const
  {
    return resolve_config_path(config_.cache_dir);
  }

  AgentResult<std::string> AgentWorkspace::runs_dir() const
  {
    return resolve_config_path(config_.runs_dir);
  }

  AgentResult<std::string> AgentWorkspace::logs_dir() const
  {
    return resolve_config_path(config_.logs_dir);
  }

  AgentResult<std::string> AgentWorkspace::run_dir(
      std::string_view run_id) const
  {
    if (run_id.empty())
    {
      return make_agent_error(
          AgentErrorCode::InvalidWorkspace,
          "run id cannot be empty");
    }

    auto base = runs_dir();
    if (!base)
    {
      return base.error();
    }

    auto joined = vix::path::join(base.value(), run_id);
    if (!joined)
    {
      return joined.error();
    }

    if (!contains(joined.value()))
    {
      return make_agent_error(
          AgentErrorCode::PathOutsideWorkspace,
          "run directory is outside the workspace");
    }

    return joined.value();
  }

  AgentResult<std::string> AgentWorkspace::resolve_config_path(
      std::string_view configured_path) const
  {
    if (configured_path.empty())
    {
      return make_agent_error(
          AgentErrorCode::InvalidWorkspace,
          "configured workspace path cannot be empty");
    }

    return resolve_inside(configured_path);
  }

} // namespace vix::ai::agent
