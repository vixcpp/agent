/**
 *
 *  @file FileScanPolicyTest.cpp
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
#include <cassert>

#include <vix/ai/agent/AgentConfig.hpp>
#include <vix/ai/agent/workspace/FileScanPolicy.hpp>
#include <vix/fs/FsEntry.hpp>

namespace
{
  vix::fs::FsEntry make_file(
      const std::string &name,
      std::uintmax_t size,
      bool hidden = false)
  {
    vix::fs::FsEntry entry;
    entry.path = name;
    entry.name = name;
    entry.type = vix::fs::FsEntryType::File;
    entry.size = size;
    entry.hidden = hidden;
    return entry;
  }

  void test_rejects_git_directory()
  {
    vix::ai::agent::FileScanPolicy policy;

    assert(policy.should_ignore_directory(".git"));
  }

  void test_rejects_node_modules_directory()
  {
    vix::ai::agent::FileScanPolicy policy;

    assert(policy.should_ignore_directory("node_modules"));
  }

  void test_rejects_hidden_file()
  {
    vix::ai::agent::FileScanPolicy policy;

    auto entry = make_file(".env", 12, true);

    assert(!policy.should_read_file(entry));
  }

  void test_accepts_cpp_extension()
  {
    vix::ai::agent::FileScanPolicy policy;

    auto entry = make_file("main.cpp", 128);

    assert(policy.should_read_file(entry));
  }

  void test_rejects_unknown_extension()
  {
    vix::ai::agent::FileScanPolicy policy;

    auto entry = make_file("binary.unknownext", 128);

    assert(!policy.should_read_file(entry));
  }

  void test_rejects_large_file()
  {
    vix::ai::agent::AgentConfig config;
    config.max_file_size = 4;

    vix::ai::agent::FileScanPolicy policy(config);

    auto entry = make_file("large.cpp", 10);

    assert(!policy.should_read_file(entry));
  }
}

void test_file_scan_policy()
{
  test_rejects_git_directory();
  test_rejects_node_modules_directory();
  test_rejects_hidden_file();
  test_accepts_cpp_extension();
  test_rejects_unknown_extension();
  test_rejects_large_file();
}
