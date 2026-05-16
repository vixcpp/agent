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
#include <vix/ai/agent/AgentConfigValidator.hpp>
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

    assert(config.tool_timeout_ms > 0);
    assert(!config.allowed_programs.empty());

    assert(config.cache_ttl_ms > 0);
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

  void test_config_validator_accepts_default_config()
  {
    vix::ai::agent::AgentConfig config;

    auto err = vix::ai::agent::AgentConfigValidator::validate(config);

    assert(!err);
  }

  void test_config_validator_rejects_empty_provider()
  {
    vix::ai::agent::AgentConfig config;
    config.provider.clear();

    auto err = vix::ai::agent::AgentConfigValidator::validate(config);

    assert(err);
  }

  void test_config_validator_rejects_empty_model()
  {
    vix::ai::agent::AgentConfig config;
    config.model.clear();

    auto err = vix::ai::agent::AgentConfigValidator::validate(config);

    assert(err);
  }

  void test_config_validator_rejects_empty_model_url()
  {
    vix::ai::agent::AgentConfig config;
    config.model_url.clear();

    auto err = vix::ai::agent::AgentConfigValidator::validate(config);

    assert(err);
  }

  void test_config_validator_rejects_zero_timeout()
  {
    vix::ai::agent::AgentConfig config;
    config.timeout_ms = 0;

    auto err = vix::ai::agent::AgentConfigValidator::validate(config);

    assert(err);
  }

  void test_config_validator_rejects_zero_limits()
  {
    vix::ai::agent::AgentConfig config;

    config.max_files = 0;
    assert(vix::ai::agent::AgentConfigValidator::validate(config));

    config = vix::ai::agent::AgentConfig{};
    config.max_file_size = 0;
    assert(vix::ai::agent::AgentConfigValidator::validate(config));

    config = vix::ai::agent::AgentConfig{};
    config.max_context_chars = 0;
    assert(vix::ai::agent::AgentConfigValidator::validate(config));

    config = vix::ai::agent::AgentConfig{};
    config.max_tool_output = 0;
    assert(vix::ai::agent::AgentConfigValidator::validate(config));

    config = vix::ai::agent::AgentConfig{};
    config.tool_timeout_ms = 0;
    assert(vix::ai::agent::AgentConfigValidator::validate(config));

    config = vix::ai::agent::AgentConfig{};
    config.cache_ttl_ms = 0;
    assert(vix::ai::agent::AgentConfigValidator::validate(config));
  }

  void test_config_validator_rejects_process_without_allowed_programs()
  {
    vix::ai::agent::AgentConfig config;
    config.allow_process = true;
    config.allowed_programs.clear();

    auto err = vix::ai::agent::AgentConfigValidator::validate(config);

    assert(err);
  }
}

void test_agent_config()
{
  test_default_config();
  test_environment_loader_defaults();
  test_environment_loader_overrides();

  test_config_validator_accepts_default_config();
  test_config_validator_rejects_empty_provider();
  test_config_validator_rejects_empty_model();
  test_config_validator_rejects_empty_model_url();
  test_config_validator_rejects_zero_timeout();
  test_config_validator_rejects_zero_limits();
  test_config_validator_rejects_process_without_allowed_programs();
}

void test_agent_workspace();
void test_project_scanner();
void test_tool_registry();
void test_agent_run();
void test_file_scan_policy();
void test_file_reader();
void test_command_tool();
void test_agent_run_store();
void test_agent_cache();
void test_agent_public_api();

int main()
{
  test_agent_config();
  test_agent_workspace();
  test_project_scanner();
  test_tool_registry();
  test_agent_run();
  test_file_scan_policy();
  test_file_reader();
  test_command_tool();
  test_agent_run_store();
  test_agent_cache();
  test_agent_public_api();

  std::cout << "vix_ai_agent tests passed\n";
  return 0;
}
