/**
 *
 *  @file ProjectScanner.hpp
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
#ifndef VIX_AI_AGENT_WORKSPACE_PROJECTSCANNER_HPP
#define VIX_AI_AGENT_WORKSPACE_PROJECTSCANNER_HPP

#include <cstdint>
#include <string>
#include <vector>

#include <vix/ai/agent/AgentResult.hpp>
#include <vix/ai/agent/AgentWorkspace.hpp>
#include <vix/ai/agent/workspace/FileScanPolicy.hpp>

namespace vix::ai::agent
{
  /**
   * @struct ProjectFile
   * @brief Lightweight description of a file discovered in a workspace.
   */
  struct ProjectFile
  {
    /**
     * @brief Absolute file path.
     */
    std::string path;

    /**
     * @brief Workspace-relative file path.
     */
    std::string relative_path;

    /**
     * @brief File name only.
     */
    std::string name;

    /**
     * @brief File size in bytes.
     */
    std::uintmax_t size{0};
  };

  /**
   * @struct ProjectScanResult
   * @brief Result of a project scan operation.
   */
  struct ProjectScanResult
  {
    /**
     * @brief Workspace root used for the scan.
     */
    std::string root;

    /**
     * @brief Files accepted by the scan policy.
     */
    std::vector<ProjectFile> files;

    /**
     * @brief Number of entries skipped because of policy or limits.
     */
    std::size_t skipped{0};

    /**
     * @brief Whether the scan stopped because max_files was reached.
     */
    bool truncated{false};
  };

  /**
   * @class ProjectScanner
   * @brief Scans an AgentWorkspace and returns files useful for AI context.
   *
   * ProjectScanner lists the real filesystem through vix::fs, then applies
   * FileScanPolicy to avoid generated, hidden, large, unsupported, or risky
   * files.
   */
  class ProjectScanner
  {
  public:
    /**
     * @brief Construct a project scanner.
     *
     * @param workspace Agent workspace.
     * @param policy File scan policy.
     */
    ProjectScanner(AgentWorkspace workspace, FileScanPolicy policy);

    /**
     * @brief Scan the workspace recursively.
     *
     * @return ProjectScanResult on success.
     */
    [[nodiscard]] AgentResult<ProjectScanResult> scan() const;

    /**
     * @brief Access the workspace used by this scanner.
     */
    [[nodiscard]] const AgentWorkspace &workspace() const noexcept;

    /**
     * @brief Access the policy used by this scanner.
     */
    [[nodiscard]] const FileScanPolicy &policy() const noexcept;

  private:
    AgentWorkspace workspace_{};
    FileScanPolicy policy_{};
  };

} // namespace vix::ai::agent

#endif // VIX_AI_AGENT_WORKSPACE_PROJECTSCANNER_HPP
