/**
 *
 *  @file ProjectScannerTest.cpp
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
#include <vix/ai/agent/AgentWorkspace.hpp>
#include <vix/ai/agent/workspace/FileScanPolicy.hpp>
#include <vix/ai/agent/workspace/ProjectScanner.hpp>
#include <vix/fs/EnsureDirectory.hpp>
#include <vix/fs/WriteText.hpp>

namespace
{
  void test_project_scanner_finds_source_files()
  {
    vix::fs::ensure_directory(".vix-agent-test/src");
    vix::fs::write_text(".vix-agent-test/src/main.cpp", "int main() { return 0; }\n");
    vix::fs::write_text(".vix-agent-test/README.md", "# test\n");

    vix::ai::agent::AgentConfig config;
    config.max_files = 50;
    config.max_file_size = 64 * 1024;

    auto workspace = vix::ai::agent::AgentWorkspace::open(".vix-agent-test", config);
    assert(workspace);

    vix::ai::agent::FileScanPolicy policy(config);
    vix::ai::agent::ProjectScanner scanner(workspace.value(), policy);

    auto scan = scanner.scan();
    assert(scan);
    assert(!scan.value().root.empty());
    assert(!scan.value().files.empty());

    bool found_main = false;
    bool found_readme = false;

    for (const auto &file : scan.value().files)
    {
      if (file.relative_path == "src/main.cpp")
      {
        found_main = true;
      }

      if (file.relative_path == "README.md")
      {
        found_readme = true;
      }
    }

    assert(found_main);
    assert(found_readme);
  }

  void test_project_scanner_respects_max_files()
  {
    vix::fs::ensure_directory(".vix-agent-test-limit");

    vix::fs::write_text(".vix-agent-test-limit/a.cpp", "int a = 1;\n");
    vix::fs::write_text(".vix-agent-test-limit/b.cpp", "int b = 2;\n");
    vix::fs::write_text(".vix-agent-test-limit/c.cpp", "int c = 3;\n");

    vix::ai::agent::AgentConfig config;
    config.max_files = 1;
    config.max_file_size = 64 * 1024;

    auto workspace = vix::ai::agent::AgentWorkspace::open(".vix-agent-test-limit", config);
    assert(workspace);

    vix::ai::agent::FileScanPolicy policy(config);
    vix::ai::agent::ProjectScanner scanner(workspace.value(), policy);

    auto scan = scanner.scan();
    assert(scan);
    assert(scan.value().files.size() == 1);
    assert(scan.value().truncated);
  }

  void test_file_scan_policy_rejects_large_files()
  {
    vix::fs::ensure_directory(".vix-agent-test-large");
    vix::fs::write_text(".vix-agent-test-large/large.cpp", "0123456789");

    vix::ai::agent::AgentConfig config;
    config.max_files = 50;
    config.max_file_size = 4;

    auto workspace = vix::ai::agent::AgentWorkspace::open(".vix-agent-test-large", config);
    assert(workspace);

    vix::ai::agent::FileScanPolicy policy(config);
    vix::ai::agent::ProjectScanner scanner(workspace.value(), policy);

    auto scan = scanner.scan();
    assert(scan);

    for (const auto &file : scan.value().files)
    {
      assert(file.name != "large.cpp");
    }
  }
}

void test_project_scanner()
{
  test_project_scanner_finds_source_files();
  test_project_scanner_respects_max_files();
  test_file_scan_policy_rejects_large_files();
}
