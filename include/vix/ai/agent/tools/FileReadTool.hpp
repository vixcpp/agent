/**
 *
 *  @file FileReadTool.hpp
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
#ifndef VIX_AI_AGENT_TOOLS_FILEREADTOOL_HPP
#define VIX_AI_AGENT_TOOLS_FILEREADTOOL_HPP

#include <string_view>

#include <vix/ai/agent/workspace/FileReader.hpp>
#include <vix/ai/agent/tools/Tool.hpp>
#include <vix/ai/agent/tools/ToolCall.hpp>
#include <vix/ai/agent/tools/ToolResult.hpp>

namespace vix::ai::agent
{
  /**
   * @class FileReadTool
   * @brief Tool that reads a text file from the agent workspace.
   *
   * Tool name:
   * - "file.read"
   *
   * Expected arguments:
   * @code
   * {
   *   "path": "src/main.cpp"
   * }
   * @endcode
   *
   * The file path is always resolved through AgentWorkspace and FileReader.
   * This keeps reads inside the workspace boundary and applies FileScanPolicy
   * before reading.
   */
  class FileReadTool final : public Tool
  {
  public:
    /**
     * @brief Construct a file read tool.
     *
     * @param reader Safe workspace file reader.
     */
    explicit FileReadTool(FileReader reader);

    /**
     * @brief Return the stable tool name.
     */
    [[nodiscard]] std::string_view name() const noexcept override;

    /**
     * @brief Return a short tool description.
     */
    [[nodiscard]] std::string_view description() const noexcept override;

    /**
     * @brief Execute the file read tool.
     *
     * Reads arguments.path and returns the file content plus metadata.
     */
    [[nodiscard]] AgentResult<ToolResult> run(
        const ToolCall &call) override;

    /**
     * @brief Access the underlying file reader.
     */
    [[nodiscard]] const FileReader &reader() const noexcept;

  private:
    FileReader reader_;
  };

} // namespace vix::ai::agent

#endif // VIX_AI_AGENT_TOOLS_FILEREADTOOL_HPP
