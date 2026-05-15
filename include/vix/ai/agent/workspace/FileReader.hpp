/**
 *
 *  @file FileReader.hpp
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
#ifndef VIX_AI_AGENT_WORKSPACE_FILEREADER_HPP
#define VIX_AI_AGENT_WORKSPACE_FILEREADER_HPP

#include <cstdint>
#include <string>
#include <string_view>

#include <vix/ai/agent/AgentResult.hpp>
#include <vix/ai/agent/AgentWorkspace.hpp>
#include <vix/ai/agent/workspace/FileScanPolicy.hpp>

namespace vix::ai::agent
{
  /**
   * @struct ReadFileResult
   * @brief Result of a workspace file read operation.
   */
  struct ReadFileResult
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
     * @brief File content as text.
     */
    std::string content;

    /**
     * @brief File size in bytes.
     */
    std::uintmax_t size{0};
  };

  /**
   * @class FileReader
   * @brief Safely reads text files from an AgentWorkspace.
   *
   * FileReader always resolves paths through AgentWorkspace before accessing
   * the filesystem. This keeps file reads inside the workspace boundary.
   *
   * It also applies FileScanPolicy before reading to avoid large, generated,
   * binary, or unsupported files.
   */
  class FileReader
  {
  public:
    /**
     * @brief Construct a file reader.
     *
     * @param workspace Agent workspace.
     * @param policy File scan/read policy.
     */
    FileReader(AgentWorkspace workspace, FileScanPolicy policy);

    /**
     * @brief Read a text file from the workspace.
     *
     * @param path User-provided path, relative or absolute.
     * @return ReadFileResult on success.
     */
    [[nodiscard]] AgentResult<ReadFileResult> read_text(
        std::string_view path) const;

    /**
     * @brief Check whether a file can be read safely.
     *
     * @param path User-provided path, relative or absolute.
     * @return true if the file exists, is inside workspace, and passes policy.
     */
    [[nodiscard]] AgentResult<bool> can_read(
        std::string_view path) const;

    /**
     * @brief Access the workspace used by this reader.
     */
    [[nodiscard]] const AgentWorkspace &workspace() const noexcept;

    /**
     * @brief Access the policy used by this reader.
     */
    [[nodiscard]] const FileScanPolicy &policy() const noexcept;

  private:
    AgentWorkspace workspace_{};
    FileScanPolicy policy_{};
  };

} // namespace vix::ai::agent

#endif // VIX_AI_AGENT_WORKSPACE_FILEREADER_HPP
