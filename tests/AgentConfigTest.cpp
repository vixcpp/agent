/**
 *
 *  @file AgentConfigTest.cpp
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
#include <iostream>
#include <string>

#include <vix/ai/agent/AgentConfig.hpp>
#include <vix/ai/agent/AgentConfigLoader.hpp>
#include <vix/env/Set.hpp>
#include <vix/env/Unset.hpp>

namespace
{
  void test_default_config()
  {
    vix::ai::agent::AgentConfig config;

    assert(config.provider == "ollama");
    assert(!config.model.empty());
    assert(!config.model_url.empty());

    assert(!config.memory_dir.empty());
    assert(!config.cache_dir.empty());
    assert(!config.runs_dir.empty());
    assert(!config.logs_dir.empty());

    assert(config.timeout_ms > 0);
    assert(config.max_files > 0);
    assert(config.max_file_size > 0);
    assert(config.max_tool_output > 0);
    assert(config.max_context_chars > 0);

    assert(config.allow_file_read);
    assert(!config.allow_file_write);
    assert(!config.allow_process);
  }

  void test_environment_loader_defaults()
  {
    auto config = vix::ai::agent::AgentConfigLoader::from_environment("VIX_AGENT_TEST_");

    assert(config.provider == "ollama");
    assert(!config.model.empty());
    assert(!config.model_url.empty());
  }

  void test_environment_loader_overrides()
  {
    vix::env::set("VIX_AGENT_TEST_PROVIDER", "ollama", true);
    vix::env::set("VIX_AGENT_TEST_MODEL", "qwen2.5-coder", true);
    vix::env::set("VIX_AGENT_TEST_MODEL_URL", "http://127.0.0.1:11434", true);
    vix::env::set("VIX_AGENT_TEST_TIMEOUT_MS", "15000", true);
    vix::env::set("VIX_AGENT_TEST_MAX_FILES", "42", true);
    vix::env::set("VIX_AGENT_TEST_ALLOW_PROCESS", "true", true);
    vix::env::set("VIX_AGENT_TEST_ALLOW_FILE_WRITE", "false", true);

    auto config = vix::ai::agent::AgentConfigLoader::from_environment("VIX_AGENT_TEST_");

    assert(config.provider == "ollama");
    assert(config.model == "qwen2.5-coder");
    assert(config.model_url == "http://127.0.0.1:11434");
    assert(config.timeout_ms == 15000);
    assert(config.max_files == 42);
    assert(config.allow_process);
    assert(!config.allow_file_write);

    vix::env::unset("VIX_AGENT_TEST_PROVIDER");
    vix::env::unset("VIX_AGENT_TEST_MODEL");
    vix::env::unset("VIX_AGENT_TEST_MODEL_URL");
    vix::env::unset("VIX_AGENT_TEST_TIMEOUT_MS");
    vix::env::unset("VIX_AGENT_TEST_MAX_FILES");
    vix::env::unset("VIX_AGENT_TEST_ALLOW_PROCESS");
    vix::env::unset("VIX_AGENT_TEST_ALLOW_FILE_WRITE");
  }
}

void test_agent_config()
{
  test_default_config();
  test_environment_loader_defaults();
  test_environment_loader_overrides();
}

void test_agent_workspace();

int main()
{
  test_agent_config();
  test_agent_workspace();
  test_project_scanner();
  test_tool_registry();
  test_agent_run();

  std::cout << "vix_ai_agent tests passed\n";
  return 0;
}
