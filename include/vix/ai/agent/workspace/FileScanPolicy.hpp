/**
 *
 *  @file FileScanPolicy.hpp
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
#ifndef VIX_AI_AGENT_WORKSPACE_FILESCANPOLICY_HPP
#define VIX_AI_AGENT_WORKSPACE_FILESCANPOLICY_HPP

#include <cstdint>
#include <string>
#include <string_view>
#include <unordered_set>

#include <vix/ai/agent/AgentConfig.hpp>
#include <vix/fs/FsEntry.hpp>

namespace vix::ai::agent
{
  /**
   * @class FileScanPolicy
   * @brief Decides which workspace files and directories the agent may scan.
   *
   * FileScanPolicy prevents the agent from reading unnecessary, generated,
   * hidden, large, or risky files.
   *
   * It is used by ProjectScanner and FileReader before reading project content.
   */
  class FileScanPolicy
  {
  public:
    /**
     * @brief Construct a policy from default limits.
     */
    FileScanPolicy();

    /**
     * @brief Construct a policy from AgentConfig.
     *
     * @param config Agent configuration.
     */
    explicit FileScanPolicy(const AgentConfig &config);

    /**
     * @brief Return true if a directory name should be skipped.
     *
     * Examples:
     * - .git
     * - .vix
     * - build
     * - node_modules
     */
    [[nodiscard]] bool should_ignore_directory(
        std::string_view name) const;

    /**
     * @brief Return true if a file name should be skipped by name.
     */
    [[nodiscard]] bool should_ignore_file_name(
        std::string_view name) const;

    /**
     * @brief Return true if a file extension is allowed for text analysis.
     */
    [[nodiscard]] bool is_allowed_extension(
        std::string_view extension) const;

    /**
     * @brief Return true if the entry can be read by the agent.
     */
    [[nodiscard]] bool should_read_file(
        const vix::fs::FsEntry &entry) const;

    /**
     * @brief Return true if the file size is accepted.
     */
    [[nodiscard]] bool accepts_size(std::uintmax_t size) const noexcept;

    /**
     * @brief Maximum number of files the scanner may return.
     */
    [[nodiscard]] std::size_t max_files() const noexcept;

    /**
     * @brief Maximum accepted file size in bytes.
     */
    [[nodiscard]] std::uintmax_t max_file_size() const noexcept;

    /**
     * @brief Add an ignored directory name.
     */
    void ignore_directory(std::string name);

    /**
     * @brief Add an ignored file name.
     */
    void ignore_file_name(std::string name);

    /**
     * @brief Add an allowed extension.
     *
     * The extension may be passed with or without a leading dot.
     */
    void allow_extension(std::string extension);

  private:
    /**
     * @brief Normalize extension to a stable form with leading dot.
     */
    [[nodiscard]] static std::string normalize_extension(
        std::string_view extension);

    /**
     * @brief Extract extension from a filename.
     */
    [[nodiscard]] static std::string extension_of(
        std::string_view name);

  private:
    std::size_t max_files_{2'000};
    std::uintmax_t max_file_size_{512 * 1024};

    std::unordered_set<std::string> ignored_directories_{};
    std::unordered_set<std::string> ignored_file_names_{};
    std::unordered_set<std::string> allowed_extensions_{};
  };

} // namespace vix::ai::agent

#endif // VIX_AI_AGENT_WORKSPACE_FILESCANPOLICY_HPP
