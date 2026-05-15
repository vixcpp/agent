/**
 *
 *  @file scan_project.cpp
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
#include <iostream>

#include <vix/ai/agent/agent.hpp>

int main()
{
  vix::ai::agent::AgentConfig config;
  config.max_files = 100;
  config.max_file_size = 128 * 1024;
  config.allow_file_read = true;
  config.allow_process = false;
  config.allow_file_write = false;

  auto workspace = vix::ai::agent::AgentWorkspace::open(".", config);
  if (!workspace)
  {
    std::cerr << "Workspace error: " << workspace.error().message() << '\n';
    return 1;
  }

  vix::ai::agent::FileScanPolicy policy(config);
  vix::ai::agent::ProjectScanner scanner(workspace.value(), policy);

  auto scan = scanner.scan();
  if (!scan)
  {
    std::cerr << "Scan error: " << scan.error().message() << '\n';
    return 1;
  }

  std::cout << "Workspace: " << scan.value().root << '\n';
  std::cout << "Files: " << scan.value().files.size() << '\n';
  std::cout << "Skipped: " << scan.value().skipped << '\n';
  std::cout << "Truncated: " << (scan.value().truncated ? "yes" : "no") << '\n';
  std::cout << '\n';

  for (const auto &file : scan.value().files)
  {
    std::cout << "- " << file.relative_path
              << " (" << file.size << " bytes)"
              << '\n';
  }

  return 0;
}
