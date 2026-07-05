/**
 *
 *  @file FileReaderTest.cpp
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
#include <string>

#include <vix/ai/agent/AgentConfig.hpp>
#include <vix/ai/agent/AgentWorkspace.hpp>
#include <vix/ai/agent/workspace/FileReader.hpp>
#include <vix/ai/agent/workspace/FileScanPolicy.hpp>
#include <vix/fs/EnsureDirectory.hpp>
#include <vix/fs/WriteText.hpp>

namespace
{
  vix::ai::agent::FileReader make_reader(
      const std::string &root,
      vix::ai::agent::AgentConfig config = {})
  {
    auto workspace = vix::ai::agent::AgentWorkspace::open(root, config);
    assert(workspace);

    vix::ai::agent::FileScanPolicy policy(config);
    return vix::ai::agent::FileReader(workspace.value(), policy);
  }

  void test_file_reader_reads_allowed_text_file()
  {
    assert(vix::fs::ensure_directory(".vix-agent-reader-test/src"));
    assert(vix::fs::write_text(
        ".vix-agent-reader-test/src/main.cpp",
        "int main() { return 0; }\n"));

    auto reader = make_reader(".vix-agent-reader-test");

    auto result = reader.read_text("src/main.cpp");

    assert(result);
    assert(result.value().relative_path == "src/main.cpp");
    assert(result.value().content == "int main() { return 0; }\n");
    assert(result.value().size > 0);
  }

  void test_file_reader_rejects_path_outside_workspace()
  {
    assert(vix::fs::ensure_directory(".vix-agent-reader-outside"));
    assert(vix::fs::write_text(".vix-agent-reader-outside/main.cpp", "int main() {}\n"));

    auto reader = make_reader(".vix-agent-reader-outside");

    auto result = reader.read_text("../main.cpp");

    assert(!result);
  }

  void test_file_reader_rejects_hidden_file()
  {
    assert(vix::fs::ensure_directory(".vix-agent-reader-hidden"));
    assert(vix::fs::write_text(".vix-agent-reader-hidden/.env", "SECRET=value\n"));

    auto reader = make_reader(".vix-agent-reader-hidden");

    auto result = reader.read_text(".env");

    assert(!result);
  }

  void test_file_reader_rejects_large_file()
  {
    assert(vix::fs::ensure_directory(".vix-agent-reader-large"));
    assert(vix::fs::write_text(".vix-agent-reader-large/large.cpp", "0123456789"));

    vix::ai::agent::AgentConfig config;
    config.max_file_size = 4;

    auto reader = make_reader(".vix-agent-reader-large", config);

    auto result = reader.read_text("large.cpp");

    assert(!result);
  }

  void test_file_reader_rejects_unknown_extension()
  {
    assert(vix::fs::ensure_directory(".vix-agent-reader-extension"));
    assert(vix::fs::write_text(".vix-agent-reader-extension/file.bin", "binary"));

    auto reader = make_reader(".vix-agent-reader-extension");

    auto result = reader.read_text("file.bin");

    assert(!result);
  }
}

void test_file_reader()
{
  test_file_reader_reads_allowed_text_file();
  test_file_reader_rejects_path_outside_workspace();
  test_file_reader_rejects_hidden_file();
  test_file_reader_rejects_large_file();
  test_file_reader_rejects_unknown_extension();
}
