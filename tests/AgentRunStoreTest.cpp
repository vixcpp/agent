/**
 *
 *  @file AgentRunStoreTest.cpp
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
#include <vix/ai/agent/AgentRequest.hpp>
#include <vix/ai/agent/AgentResponse.hpp>
#include <vix/ai/agent/AgentRunStore.hpp>
#include <vix/ai/agent/AgentWorkspace.hpp>
#include <vix/ai/agent/model/ModelRequest.hpp>
#include <vix/fs/EnsureDirectory.hpp>
#include <vix/fs/Exists.hpp>

namespace
{
  void test_agent_run_store_writes_run_files()
  {
    vix::fs::ensure_directory(".vix-agent-run-store-test");

    vix::ai::agent::AgentConfig config;
    auto workspace =
        vix::ai::agent::AgentWorkspace::open(".vix-agent-run-store-test", config);

    assert(workspace);

    vix::ai::agent::AgentRunStore store(workspace.value());

    vix::ai::agent::AgentRequest request;
    request.input = "Test run store";
    request.workspace = ".vix-agent-run-store-test";

    auto err = store.create_run("run_test", request);
    assert(!err);

    vix::ai::agent::ModelRequest model_request;
    model_request.model = "fake-model";
    model_request.prompt = "prompt text";

    err = store.save_prompt("run_test", model_request);
    assert(!err);

    vix::ai::agent::AgentResponse response;
    response.run_id = "run_test";
    response.text = "final response";
    response.model = "fake-model";
    response.provider = "fake";

    err = store.save_final_response("run_test", response);
    assert(!err);

    auto run_json = workspace.value().run_dir("run_test");
    assert(run_json);

    auto exists = vix::fs::exists(run_json.value());
    assert(exists && exists.value());
  }
}

void test_agent_run_store()
{
  test_agent_run_store_writes_run_files();
}
