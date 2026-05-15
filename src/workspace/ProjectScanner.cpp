/**
 *
 *  @file ProjectScanner.cpp
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
#include <vix/ai/agent/workspace/ProjectScanner.hpp>

#include <string>
#include <utility>

#include <vix/ai/agent/AgentError.hpp>
#include <vix/fs/FsOptions.hpp>
#include <vix/fs/ListDirectory.hpp>

namespace vix::ai::agent
{
  ProjectScanner::ProjectScanner(
      AgentWorkspace workspace,
      FileScanPolicy policy)
      : workspace_(std::move(workspace)),
        policy_(std::move(policy))
  {
  }

  AgentResult<ProjectScanResult> ProjectScanner::scan() const
  {
    if (!workspace_.valid())
    {
      return make_agent_error(
          AgentErrorCode::InvalidWorkspace,
          "workspace is not initialized");
    }

    vix::fs::FsOptions options;
    options.recursive = true;
    options.include_hidden = false;
    options.follow_symlinks = false;
    options.list_mode = vix::fs::DirectoryListMode::Recursive;

    auto entries = vix::fs::list_directory(workspace_.root(), options);
    if (!entries)
    {
      return entries.error();
    }

    ProjectScanResult result;
    result.root = workspace_.root();

    for (const auto &entry : entries.value())
    {
      if (result.files.size() >= policy_.max_files())
      {
        result.truncated = true;
        ++result.skipped;
        continue;
      }

      if (entry.type == vix::fs::FsEntryType::Directory)
      {
        if (policy_.should_ignore_directory(entry.name))
        {
          ++result.skipped;
        }

        continue;
      }

      if (!policy_.should_read_file(entry))
      {
        ++result.skipped;
        continue;
      }

      auto relative = workspace_.relative_to_root(entry.path);
      if (!relative)
      {
        ++result.skipped;
        continue;
      }

      ProjectFile file;
      file.path = entry.path;
      file.relative_path = relative.value();
      file.name = entry.name;
      file.size = entry.size;

      result.files.push_back(std::move(file));
    }

    return result;
  }

  const AgentWorkspace &ProjectScanner::workspace() const noexcept
  {
    return workspace_;
  }

  const FileScanPolicy &ProjectScanner::policy() const noexcept
  {
    return policy_;
  }

} // namespace vix::ai::agent
