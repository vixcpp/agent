/**
 *
 *  @file FileReader.cpp
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
#include <vix/ai/agent/workspace/FileReader.hpp>

#include <string>
#include <string_view>
#include <utility>

#include <vix/ai/agent/AgentError.hpp>
#include <vix/fs/Exists.hpp>
#include <vix/fs/FsEntry.hpp>
#include <vix/fs/IsFile.hpp>
#include <vix/fs/ReadText.hpp>
#include <vix/fs/Size.hpp>
#include <vix/path/Filename.hpp>
#include <vix/path/Extension.hpp>

namespace vix::ai::agent
{
  namespace
  {
    [[nodiscard]] AgentResult<vix::fs::FsEntry> build_entry(
        const AgentWorkspace &workspace,
        std::string_view path)
    {
      auto resolved = workspace.resolve_inside(path);
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
            "file does not exist");
      }

      auto is_file = vix::fs::is_file(resolved.value());
      if (!is_file)
      {
        return is_file.error();
      }

      if (!is_file.value())
      {
        return make_agent_error(
            AgentErrorCode::InvalidWorkspace,
            "path is not a regular file");
      }

      auto file_size = vix::fs::size(resolved.value());
      if (!file_size)
      {
        return file_size.error();
      }

      auto file_name = vix::path::filename(resolved.value());
      if (!file_name)
      {
        return file_name.error();
      }

      vix::fs::FsEntry entry;
      entry.path = resolved.value();
      entry.name = file_name.value();
      entry.type = vix::fs::FsEntryType::File;
      entry.size = file_size.value();
      entry.hidden = !entry.name.empty() && entry.name.front() == '.';

      return entry;
    }
  } // namespace

  FileReader::FileReader(
      AgentWorkspace workspace,
      FileScanPolicy policy)
      : workspace_(std::move(workspace)),
        policy_(std::move(policy))
  {
  }

  AgentResult<ReadFileResult> FileReader::read_text(
      std::string_view path) const
  {
    auto entry = build_entry(workspace_, path);
    if (!entry)
    {
      return entry.error();
    }

    if (!policy_.should_read_file(entry.value()))
    {
      return make_agent_error(
          AgentErrorCode::ToolNotAllowed,
          "file is not allowed by the agent file scan policy");
    }

    auto content = vix::fs::read_text(entry.value().path);
    if (!content)
    {
      return content.error();
    }

    auto relative = workspace_.relative_to_root(entry.value().path);
    if (!relative)
    {
      return relative.error();
    }

    ReadFileResult result;
    result.path = entry.value().path;
    result.relative_path = relative.value();
    result.content = content.value();
    result.size = entry.value().size;

    return result;
  }

  AgentResult<bool> FileReader::can_read(
      std::string_view path) const
  {
    auto entry = build_entry(workspace_, path);
    if (!entry)
    {
      return entry.error();
    }

    return policy_.should_read_file(entry.value());
  }

  const AgentWorkspace &FileReader::workspace() const noexcept
  {
    return workspace_;
  }

  const FileScanPolicy &FileReader::policy() const noexcept
  {
    return policy_;
  }

} // namespace vix::ai::agent
