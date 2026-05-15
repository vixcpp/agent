/**
 *
 *  @file AgentWorkspace.hpp
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
#ifndef VIX_AI_AGENT_AGENTWORKSPACE_HPP
#define VIX_AI_AGENT_AGENTWORKSPACE_HPP

#include <string>
#include <string_view>

#include <vix/ai/agent/AgentConfig.hpp>
#include <vix/ai/agent/AgentResult.hpp>

namespace vix::ai::agent
{
  /**
   * @class AgentWorkspace
   * @brief Represents and protects the workspace used by an agent run.
   *
   * AgentWorkspace is responsible for:
   * - resolving the project root
   * - keeping all file operations inside the workspace
   * - creating local .vix/agent directories
   * - building stable internal paths for memory, cache, runs, and logs
   *
   * The workspace is the security boundary of the local agent.
   */
  class AgentWorkspace
  {
  public:
    /**
     * @brief Construct an empty workspace.
     */
    AgentWorkspace() = default;

    /**
     * @brief Construct a workspace from a root path and config.
     *
     * @param root Workspace root path.
     * @param config Agent configuration used for internal directories.
     */
    AgentWorkspace(std::string root, AgentConfig config);

    /**
     * @brief Open and validate a workspace.
     *
     * If root is empty, "." is used.
     *
     * @param root Workspace root path.
     * @param config Agent configuration.
     * @return AgentWorkspace on success, structured error on failure.
     */
    [[nodiscard]] static AgentResult<AgentWorkspace> open(
        std::string_view root,
        const AgentConfig &config);

    /**
     * @brief Ensure internal agent directories exist.
     *
     * Creates:
     * - memory_dir
     * - cache_dir
     * - runs_dir
     * - logs_dir
     *
     * The paths are resolved inside the workspace.
     */
    [[nodiscard]] vix::error::Error ensure_layout() const;

    /**
     * @brief Return the absolute workspace root path.
     */
    [[nodiscard]] const std::string &root() const noexcept;

    /**
     * @brief Return true if the workspace has a non-empty root.
     */
    [[nodiscard]] bool valid() const noexcept;

    /**
     * @brief Resolve a user path inside the workspace.
     *
     * Relative paths are resolved against root().
     * Absolute paths are normalized and then checked against root().
     *
     * @param path User-provided path.
     * @return Absolute path inside the workspace, or an error.
     */
    [[nodiscard]] AgentResult<std::string> resolve_inside(
        std::string_view path) const;

    /**
     * @brief Convert an absolute workspace path to a path relative to root().
     *
     * @param path Path to convert.
     * @return Project-relative path, or an error if outside workspace.
     */
    [[nodiscard]] AgentResult<std::string> relative_to_root(
        std::string_view path) const;

    /**
     * @brief Check whether a path is inside the workspace.
     */
    [[nodiscard]] bool contains(std::string_view path) const;

    /**
     * @brief Return the resolved memory directory path.
     */
    [[nodiscard]] AgentResult<std::string> memory_dir() const;

    /**
     * @brief Return the resolved cache directory path.
     */
    [[nodiscard]] AgentResult<std::string> cache_dir() const;

    /**
     * @brief Return the resolved runs directory path.
     */
    [[nodiscard]] AgentResult<std::string> runs_dir() const;

    /**
     * @brief Return the resolved logs directory path.
     */
    [[nodiscard]] AgentResult<std::string> logs_dir() const;

    /**
     * @brief Build a path inside the run directory for a specific run id.
     */
    [[nodiscard]] AgentResult<std::string> run_dir(
        std::string_view run_id) const;

  private:
    [[nodiscard]] AgentResult<std::string> resolve_config_path(
        std::string_view configured_path) const;

  private:
    std::string root_{};
    AgentConfig config_{};
  };

} // namespace vix::ai::agent

#endif // VIX_AI_AGENT_AGENTWORKSPACE_HPP
