/**
 *
 *  @file AgentWorkspaceTest.cpp
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
#include <vix/fs/Exists.hpp>

namespace
{
  void test_open_current_workspace()
  {
    vix::ai::agent::AgentConfig config;

    auto workspace = vix::ai::agent::AgentWorkspace::open(".", config);

    assert(workspace);
    assert(workspace.value().valid());
    assert(!workspace.value().root().empty());
  }

  void test_workspace_layout_is_created()
  {
    vix::ai::agent::AgentConfig config;

    auto workspace = vix::ai::agent::AgentWorkspace::open(".", config);
    assert(workspace);

    auto memory_dir = workspace.value().memory_dir();
    auto cache_dir = workspace.value().cache_dir();
    auto runs_dir = workspace.value().runs_dir();
    auto logs_dir = workspace.value().logs_dir();

    assert(memory_dir);
    assert(cache_dir);
    assert(runs_dir);
    assert(logs_dir);

    auto memory_exists = vix::fs::exists(memory_dir.value());
    auto cache_exists = vix::fs::exists(cache_dir.value());
    auto runs_exists = vix::fs::exists(runs_dir.value());
    auto logs_exists = vix::fs::exists(logs_dir.value());

    assert(memory_exists && memory_exists.value());
    assert(cache_exists && cache_exists.value());
    assert(runs_exists && runs_exists.value());
    assert(logs_exists && logs_exists.value());
  }

  void test_resolve_inside_workspace()
  {
    vix::ai::agent::AgentConfig config;

    auto workspace = vix::ai::agent::AgentWorkspace::open(".", config);
    assert(workspace);

    auto resolved = workspace.value().resolve_inside(".");
    assert(resolved);
    assert(!resolved.value().empty());

    assert(workspace.value().contains(resolved.value()));
  }

  void test_reject_empty_path()
  {
    vix::ai::agent::AgentConfig config;

    auto workspace = vix::ai::agent::AgentWorkspace::open(".", config);
    assert(workspace);

    auto resolved = workspace.value().resolve_inside("");
    assert(!resolved);
  }

  void test_run_dir()
  {
    vix::ai::agent::AgentConfig config;

    auto workspace = vix::ai::agent::AgentWorkspace::open(".", config);
    assert(workspace);

    auto run_dir = workspace.value().run_dir("run_test");
    assert(run_dir);
    assert(!run_dir.value().empty());
    assert(workspace.value().contains(run_dir.value()));
  }

  void test_reject_parent_directory_escape()
  {
    vix::ai::agent::AgentConfig config;

    auto workspace = vix::ai::agent::AgentWorkspace::open(".", config);
    assert(workspace);

    auto resolved = workspace.value().resolve_inside("../");
    assert(!resolved);
  }

  void test_reject_absolute_path_outside_workspace()
  {
    vix::ai::agent::AgentConfig config;

    auto workspace = vix::ai::agent::AgentWorkspace::open(".", config);
    assert(workspace);

    auto resolved = workspace.value().resolve_inside("/tmp");
    assert(!resolved);
  }

  void test_accept_absolute_path_inside_workspace()
  {
    vix::ai::agent::AgentConfig config;

    auto workspace = vix::ai::agent::AgentWorkspace::open(".", config);
    assert(workspace);

    auto root = workspace.value().root();
    auto resolved = workspace.value().resolve_inside(root);

    assert(resolved);
    assert(resolved.value() == root);
    assert(workspace.value().contains(resolved.value()));
  }

  void test_reject_empty_run_id()
  {
    vix::ai::agent::AgentConfig config;

    auto workspace = vix::ai::agent::AgentWorkspace::open(".", config);
    assert(workspace);

    auto run_dir = workspace.value().run_dir("");
    assert(!run_dir);
  }

  void test_reject_missing_workspace()
  {
    vix::ai::agent::AgentConfig config;

    auto workspace = vix::ai::agent::AgentWorkspace::open(
        ".vix-agent-missing-workspace",
        config);

    assert(!workspace);
  }
}

void test_agent_workspace()
{
  test_open_current_workspace();
  test_workspace_layout_is_created();
  test_resolve_inside_workspace();
  test_reject_empty_path();
  test_run_dir();

  test_reject_parent_directory_escape();
  test_reject_absolute_path_outside_workspace();
  test_accept_absolute_path_inside_workspace();
  test_reject_empty_run_id();
  test_reject_missing_workspace();
}
