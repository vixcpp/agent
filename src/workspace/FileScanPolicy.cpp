/**
 *
 *  @file FileScanPolicy.cpp
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
#include <vix/ai/agent/workspace/FileScanPolicy.hpp>

#include <algorithm>
#include <cctype>
#include <cstdint>
#include <string>
#include <string_view>
#include <utility>

namespace vix::ai::agent
{
  namespace
  {
    [[nodiscard]] std::string to_lower_copy(std::string_view value)
    {
      std::string out;
      out.reserve(value.size());

      for (const char c : value)
      {
        out.push_back(
            static_cast<char>(
                std::tolower(static_cast<unsigned char>(c))));
      }

      return out;
    }

    void add_default_ignored_directories(
        std::unordered_set<std::string> &dirs)
    {
      dirs.insert(".git");
      dirs.insert(".vix");
      dirs.insert(".cache");
      dirs.insert(".idea");
      dirs.insert(".vscode");

      dirs.insert("build");
      dirs.insert("build-ninja");
      dirs.insert("build-release");
      dirs.insert("build-debug");
      dirs.insert("cmake-build-debug");
      dirs.insert("cmake-build-release");

      dirs.insert("node_modules");
      dirs.insert("vendor");
      dirs.insert("dist");
      dirs.insert("out");
      dirs.insert("target");
      dirs.insert("__pycache__");
    }

    void add_default_ignored_files(
        std::unordered_set<std::string> &files)
    {
      files.insert(".DS_Store");
      files.insert("Thumbs.db");
      files.insert("compile_commands.json");
      files.insert("CMakeCache.txt");
      files.insert("package-lock.json");
      files.insert("yarn.lock");
      files.insert("pnpm-lock.yaml");
    }

    void add_default_allowed_extensions(
        std::unordered_set<std::string> &exts)
    {
      exts.insert(".c");
      exts.insert(".cc");
      exts.insert(".cpp");
      exts.insert(".cxx");
      exts.insert(".h");
      exts.insert(".hh");
      exts.insert(".hpp");
      exts.insert(".hxx");

      exts.insert(".cmake");
      exts.insert(".txt");
      exts.insert(".md");
      exts.insert(".json");
      exts.insert(".yaml");
      exts.insert(".yml");
      exts.insert(".toml");

      exts.insert(".js");
      exts.insert(".jsx");
      exts.insert(".ts");
      exts.insert(".tsx");
      exts.insert(".vue");

      exts.insert(".py");
      exts.insert(".rs");
      exts.insert(".go");
      exts.insert(".java");
      exts.insert(".php");
      exts.insert(".rb");
      exts.insert(".sh");
      exts.insert(".zsh");
      exts.insert(".bash");

      exts.insert(".html");
      exts.insert(".css");
      exts.insert(".scss");
      exts.insert(".sql");
    }
  } // namespace

  FileScanPolicy::FileScanPolicy()
  {
    add_default_ignored_directories(ignored_directories_);
    add_default_ignored_files(ignored_file_names_);
    add_default_allowed_extensions(allowed_extensions_);
  }

  FileScanPolicy::FileScanPolicy(const AgentConfig &config)
      : FileScanPolicy()
  {
    max_files_ = config.max_files;
    max_file_size_ = config.max_file_size;
  }

  bool FileScanPolicy::should_ignore_directory(
      std::string_view name) const
  {
    if (name.empty())
    {
      return true;
    }

    const std::string normalized = to_lower_copy(name);
    return ignored_directories_.find(normalized) != ignored_directories_.end();
  }

  bool FileScanPolicy::should_ignore_file_name(
      std::string_view name) const
  {
    if (name.empty())
    {
      return true;
    }

    const std::string stable_name(name);

    if (!stable_name.empty() && stable_name.front() == '.')
    {
      return true;
    }

    return ignored_file_names_.find(stable_name) != ignored_file_names_.end();
  }

  bool FileScanPolicy::is_allowed_extension(
      std::string_view extension) const
  {
    const std::string normalized = normalize_extension(extension);
    if (normalized.empty())
    {
      return false;
    }

    return allowed_extensions_.find(normalized) != allowed_extensions_.end();
  }

  bool FileScanPolicy::should_read_file(
      const vix::fs::FsEntry &entry) const
  {
    if (entry.type != vix::fs::FsEntryType::File)
    {
      return false;
    }

    if (entry.hidden)
    {
      return false;
    }

    if (should_ignore_file_name(entry.name))
    {
      return false;
    }

    if (!accepts_size(entry.size))
    {
      return false;
    }

    return is_allowed_extension(extension_of(entry.name));
  }

  bool FileScanPolicy::accepts_size(std::uintmax_t size) const noexcept
  {
    return size <= max_file_size_;
  }

  std::size_t FileScanPolicy::max_files() const noexcept
  {
    return max_files_;
  }

  std::uintmax_t FileScanPolicy::max_file_size() const noexcept
  {
    return max_file_size_;
  }

  void FileScanPolicy::ignore_directory(std::string name)
  {
    if (name.empty())
    {
      return;
    }

    ignored_directories_.insert(to_lower_copy(name));
  }

  void FileScanPolicy::ignore_file_name(std::string name)
  {
    if (name.empty())
    {
      return;
    }

    ignored_file_names_.insert(std::move(name));
  }

  void FileScanPolicy::allow_extension(std::string extension)
  {
    std::string normalized = normalize_extension(extension);
    if (normalized.empty())
    {
      return;
    }

    allowed_extensions_.insert(std::move(normalized));
  }

  std::string FileScanPolicy::normalize_extension(
      std::string_view extension)
  {
    if (extension.empty())
    {
      return {};
    }

    std::string out = to_lower_copy(extension);

    if (!out.empty() && out.front() != '.')
    {
      out.insert(out.begin(), '.');
    }

    return out;
  }

  std::string FileScanPolicy::extension_of(
      std::string_view name)
  {
    const std::size_t pos = name.rfind('.');
    if (pos == std::string_view::npos || pos == 0)
    {
      return {};
    }

    return normalize_extension(name.substr(pos));
  }

} // namespace vix::ai::agent
