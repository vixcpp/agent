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
#include <vix/ai/agent/AgentRuntime.hpp>
#include <vix/print.hpp>

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
    vix::print("Workspace error:", workspace.error().message());
    return 1;
  }

  vix::ai::agent::FileScanPolicy policy(config);
  vix::ai::agent::ProjectScanner scanner(workspace.value(), policy);

  auto scan = scanner.scan();

  if (!scan)
  {
    vix::print("Scan error:", scan.error().message());
    return 1;
  }

  vix::print("Workspace:", scan.value().root);
  vix::print("Files:", scan.value().files.size());
  vix::print("Skipped:", scan.value().skipped);
  vix::print("Truncated:", scan.value().truncated ? "yes" : "no");
  vix::print();

  for (const auto &file : scan.value().files)
  {
    vix::print("-", file.relative_path, "(", file.size, "bytes )");
  }

  return 0;
}
