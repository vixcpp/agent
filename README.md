# Vix AI Agent

Local-first AI agent module for Vix.cpp.

`vix::ai::agent` provides a small, explicit, and production-oriented foundation for building AI-powered developer tools in C++.

It is designed around:

- local model providers
- safe workspace access
- controlled tools
- structured errors
- run history
- conservative caching
- explicit configuration

The current stable public API is:

```cpp
Agent::run(const AgentRequest &request)
```

Other helpers such as `chat()`, `analyze()`, `explain()`, or `run_with_tools()` are intentionally not exposed yet as stable APIs.

## Public header

```cpp
#include <vix/ai/agent/agent.hpp>
```

This umbrella header exposes the public agent API:

```cpp
#include <vix/ai/agent/Agent.hpp>
#include <vix/ai/agent/AgentConfig.hpp>
#include <vix/ai/agent/AgentConfigLoader.hpp>
#include <vix/ai/agent/AgentConfigValidator.hpp>
#include <vix/ai/agent/AgentError.hpp>
#include <vix/ai/agent/AgentRequest.hpp>
#include <vix/ai/agent/AgentResponse.hpp>
#include <vix/ai/agent/AgentResult.hpp>
#include <vix/ai/agent/AgentRunStore.hpp>
#include <vix/ai/agent/AgentRunTimer.hpp>
#include <vix/ai/agent/AgentWorkspace.hpp>
```

## Basic usage

```cpp
#include <vix/ai/agent/agent.hpp>
#include <vix/print.hpp>

int main()
{
  vix::ai::agent::AgentConfig config;

  config.provider = "ollama";
  config.model = "llama3";
  config.model_url = "http://127.0.0.1:11434";

  config.allow_file_read = true;
  config.allow_process = false;
  config.allow_file_write = false;

  vix::ai::agent::Agent agent(config);

  vix::ai::agent::AgentRequest request;

  request.input = "Explain this project in simple words.";
  request.workspace = ".";
  request.mode = vix::ai::agent::AgentRequestMode::Analyze;

  request.allow_tools = true;
  request.allow_file_read = true;
  request.allow_process = false;
  request.allow_file_write = false;

  auto result = agent.run(request);

  if (!result)
  {
    vix::print("Agent error:", result.error().message());
    return 1;
  }

  vix::print(result.value().text);

  return 0;
}
```

## Design goals

Vix AI Agent is designed to be:

- local-first by default
- safe by default
- explicit about permissions
- deterministic where possible
- observable through run history
- cache-aware but conservative
- usable from C++ without hiding behavior

The agent does not silently read or modify files outside the workspace.

## Current status

Current stable surface:

```
Agent::run(...)
```

Current internal features:

- local Ollama provider
- model provider abstraction
- workspace scanner
- safe file reader
- controlled tool registry
- `file.read` tool
- `command.run` tool with allowlist
- run history
- conservative model response cache
- structured errors through `vix::error::Result<T>`

## Architecture

```
agent/
├── Agent
├── AgentConfig
├── AgentConfigLoader
├── AgentConfigValidator
├── AgentRequest
├── AgentResponse
├── AgentRunStore
├── AgentWorkspace
├── crypto/
│   ├── AgentFingerprint
│   └── AgentId
├── model/
│   ├── ModelProvider
│   ├── ModelRequest
│   ├── ModelResponse
│   └── OllamaProvider
├── tools/
│   ├── Tool
│   ├── ToolCall
│   ├── ToolRegistry
│   ├── ToolResult
│   ├── FileReadTool
│   └── CommandTool
└── workspace/
    ├── FileReader
    ├── FileScanPolicy
    └── ProjectScanner
```

## Main concepts

### Agent

`Agent` is the main orchestrator.

It receives an `AgentRequest`, prepares the workspace, builds model context, optionally executes controlled tools, and returns an `AgentResponse`.

```cpp
vix::ai::agent::Agent agent(config);

auto result = agent.run(request);
```

### AgentConfig

`AgentConfig` controls the agent runtime.

```cpp
vix::ai::agent::AgentConfig config;

config.provider = "ollama";
config.model = "llama3";
config.model_url = "http://127.0.0.1:11434";

config.timeout_ms = 30'000;
config.tool_timeout_ms = 30'000;

config.max_files = 2'000;
config.max_file_size = 512 * 1024;
config.max_tool_output = 20'000;
config.max_context_chars = 120'000;
config.max_tool_rounds = 3;

config.offline = true;

config.allow_file_read = true;
config.allow_process = false;
config.allow_file_write = false;

config.use_cache = true;
config.cache_ttl_ms = 5 * 60 * 1000;

config.persist_memory = true;
```

### AgentRequest

`AgentRequest` represents one user request.

```cpp
vix::ai::agent::AgentRequest request;

request.input = "Explain this project.";
request.workspace = ".";
request.mode = vix::ai::agent::AgentRequestMode::Analyze;

request.allow_tools = true;
request.allow_file_read = true;
request.allow_process = false;
request.allow_file_write = false;
request.use_cache = true;
```

### AgentResponse

`AgentResponse` contains the final text, metadata, model information, run id, cache status, and tool summaries.

```cpp
auto result = agent.run(request);

if (result)
{
  const auto &response = result.value();

  vix::print("run:", response.run_id);
  vix::print("provider:", response.provider);
  vix::print("model:", response.model);
  vix::print("from cache:", response.from_cache);
  vix::print(response.text);
}
```

## Request modes

`AgentRequestMode` controls the high-level behavior of the request.

```cpp
request.mode = vix::ai::agent::AgentRequestMode::Run;
request.mode = vix::ai::agent::AgentRequestMode::Analyze;
request.mode = vix::ai::agent::AgentRequestMode::Explain;
request.mode = vix::ai::agent::AgentRequestMode::Chat;
```

| Mode | Purpose |
|------|---------|
| `Run` | Normal single request |
| `Analyze` | Analyze a workspace or project |
| `Explain` | Explain something without modifying files |
| `Chat` | Chat-style interaction |

## Local-first model provider

The default provider is Ollama.

```cpp
config.provider = "ollama";
config.model = "llama3";
config.model_url = "http://127.0.0.1:11434";
```

The current `OllamaProvider` targets:

```
POST /api/generate
```

Example local setup:

```bash
ollama serve
ollama pull llama3
```

Then run your Vix AI Agent program.

## Safety controls

The agent is explicit about local capabilities.

```cpp
config.allow_file_read = true;
config.allow_process = false;
config.allow_file_write = false;
```

Per request, permissions can be restricted further:

```cpp
request.allow_tools = true;
request.allow_file_read = true;
request.allow_process = false;
request.allow_file_write = false;
```

The effective permission is restrictive.

If the global config disables a capability, the request cannot enable it.

For example, this fails:

```cpp
config.allow_process = false;
request.allow_process = true;
```

## Workspace boundary

All file operations are resolved through `AgentWorkspace`.

The workspace protects the agent from reading outside the project root.

It rejects:

- empty paths
- parent directory escapes
- paths outside the workspace
- invalid workspace roots

Default internal directories:

```
.vix/agent/memory
.vix/agent/cache
.vix/agent/runs
.vix/agent/logs
```

## File scanning policy

`FileScanPolicy` controls what the agent may scan or read.

It rejects common generated or unsafe paths such as:

```
.git
.vix
.cache
.idea
.vscode
build
build-ninja
build-release
build-debug
cmake-build-debug
cmake-build-release
node_modules
vendor
dist
out
target
__pycache__
```

It also rejects:

- hidden files
- unsupported extensions
- files larger than `config.max_file_size`

Common allowed extensions include:

```
.c
.cc
.cpp
.cxx
.h
.hh
.hpp
.hxx
.cmake
.txt
.md
.json
.yaml
.yml
.toml
.js
.jsx
.ts
.tsx
.vue
.py
.rs
.go
.java
.php
.rb
.sh
.zsh
.bash
.html
.css
.scss
.sql
```

## Tool system

The agent has a controlled tool system.

Current tools:

| Tool | Purpose |
|------|---------|
| `file.read` | Read a safe text file inside the workspace |
| `command.run` | Run an allowed local command inside the workspace |

Tools are registered in `ToolRegistry`.

The agent can execute model-requested tool calls through a controlled loop:

```
model request
tool calls
tool execution
tool results
final model response
```

### File read tool

Tool name:

```
file.read
```

Expected arguments:

```json
{
  "path": "src/main.cpp"
}
```

The path is resolved through `AgentWorkspace` and checked by `FileScanPolicy`.

### Command tool

Tool name:

```
command.run
```

Expected arguments:

```json
{
  "program": "vix",
  "args": ["build"],
  "working_directory": "."
}
```

`command.run` is disabled by default.

To enable it:

```cpp
config.allow_process = true;
config.allowed_programs = {
    "vix",
    "cmake",
    "ninja",
    "git",
    "ls",
    "cat",
    "echo"
};
```

Programs not present in `allowed_programs` are rejected.

Dangerous programs are also blocked:

```
rm
rmdir
mv
dd
mkfs
shutdown
reboot
poweroff
sudo
su
```

The command working directory must stay inside the workspace.

### Tool limits

Important tool-related limits:

```cpp
config.tool_timeout_ms = 30'000;
config.max_tool_output = 20'000;
config.max_tool_rounds = 3;
```

Current status:

- `max_tool_output` is enforced by `CommandTool`
- `max_tool_rounds` is enforced by `Agent`
- `tool_timeout_ms` is part of the API and validation
- real process timeout will be enforced when `vix::process` supports command timeouts

## Run history

When `persist_memory` is enabled, the agent stores local run history.

```cpp
config.persist_memory = true;
```

Run history is written under:

```
.vix/agent/runs/<run_id>/
```

A run may include:

```
run.json
prompt.txt
model_response.json
tools.json
response.json
response.txt
error.json
```

This makes local debugging, auditing, and replay support easier to build later.

## Cache

When `use_cache` is enabled, the agent can reuse safe cached model responses.

```cpp
config.use_cache = true;
config.cache_ttl_ms = 5 * 60 * 1000;
```

The first cache implementation is conservative:

- only successful responses are cached
- responses involving tools are not cached
- cache is stored locally under `.vix/agent/cache`
- cache keys are based on provider, model, prompt, and context fingerprint

## Environment configuration

`AgentConfigLoader` can load configuration from environment variables.

```cpp
auto config =
    vix::ai::agent::AgentConfigLoader::from_environment();
```

Default prefix:

```
VIX_AGENT_
```

Supported variables include:

```
VIX_AGENT_PROVIDER
VIX_AGENT_MODEL
VIX_AGENT_MODEL_URL
VIX_AGENT_MEMORY_DIR
VIX_AGENT_CACHE_DIR
VIX_AGENT_RUNS_DIR
VIX_AGENT_LOGS_DIR
VIX_AGENT_TIMEOUT_MS
VIX_AGENT_MAX_FILES
VIX_AGENT_MAX_FILE_SIZE
VIX_AGENT_MAX_TOOL_OUTPUT
VIX_AGENT_MAX_CONTEXT_CHARS
VIX_AGENT_OFFLINE
VIX_AGENT_ALLOW_PROCESS
VIX_AGENT_ALLOW_FILE_READ
VIX_AGENT_ALLOW_FILE_WRITE
VIX_AGENT_USE_CACHE
VIX_AGENT_PERSIST_MEMORY
```

Example:

```bash
export VIX_AGENT_PROVIDER=ollama
export VIX_AGENT_MODEL=llama3
export VIX_AGENT_MODEL_URL=http://127.0.0.1:11434
export VIX_AGENT_ALLOW_PROCESS=false
```

Then:

```cpp
auto config = vix::ai::agent::AgentConfigLoader::from_environment();
vix::ai::agent::Agent agent(config);
```

## Configuration validation

Use `AgentConfigValidator` to validate a config before running the agent.

```cpp
vix::ai::agent::AgentConfig config;

auto err = vix::ai::agent::AgentConfigValidator::validate(config);

if (err)
{
  vix::print("Invalid config:", err.message());
  return 1;
}
```

The agent also validates its configuration at runtime before executing a request.

## Custom model provider

You can inject your own provider by implementing `ModelProvider`.

```cpp
class MyProvider final : public vix::ai::agent::ModelProvider
{
public:
  [[nodiscard]] std::string_view name() const noexcept override
  {
    return "my-provider";
  }

  [[nodiscard]] bool local() const noexcept override
  {
    return true;
  }

  [[nodiscard]] vix::ai::agent::AgentResult<bool> available() const override
  {
    return true;
  }

  [[nodiscard]] vix::ai::agent::AgentResult<vix::ai::agent::ModelResponse> generate(
      const vix::ai::agent::ModelRequest &request) override
  {
    vix::ai::agent::ModelResponse response;

    response.text = "Hello from my provider";
    response.model = request.model;
    response.provider = "my-provider";
    response.status = vix::ai::agent::ModelResponseStatus::Completed;

    return response;
  }
};
```

Use it with `Agent`:

```cpp
auto provider = std::make_shared<MyProvider>();

vix::ai::agent::Agent agent(config, provider);
```

## Custom tools

Create a tool by implementing `Tool`.

```cpp
class EchoTool final : public vix::ai::agent::Tool
{
public:
  [[nodiscard]] std::string_view name() const noexcept override
  {
    return "test.echo";
  }

  [[nodiscard]] std::string_view description() const noexcept override
  {
    return "Echo test tool.";
  }

  [[nodiscard]] vix::ai::agent::AgentResult<vix::ai::agent::ToolResult> run(
      const vix::ai::agent::ToolCall &call) override
  {
    return vix::ai::agent::ToolResult::success(
        call.id,
        "test.echo",
        "echo");
  }
};
```

Register it:

```cpp
vix::ai::agent::Agent agent(config);

auto err = agent.tools().add(std::make_shared<EchoTool>());

if (err)
{
  vix::print("Tool error:", err.message());
}
```

## Error handling

The agent uses:

```cpp
vix::ai::agent::AgentResult<T>
```

which is an alias around:

```cpp
vix::error::Result<T>
```

Example:

```cpp
auto result = agent.run(request);

if (!result)
{
  const auto &err = result.error();

  vix::print("error:", err.message());
  return 1;
}

vix::print(result.value().text);
```

Agent-specific errors include:

```
empty_input
invalid_workspace
path_outside_workspace
model_unavailable
model_request_failed
model_response_invalid
tool_not_found
tool_not_allowed
tool_failed
tool_timeout
memory_unavailable
memory_write_failed
memory_read_failed
config_invalid
internal_failure
```

## Testing

From the module directory:

```bash
cd ~/vixcpp/vix/modules/agent
cmake --build build-ninja
./build-ninja/vix_ai_agent_tests
```

Current test areas:

- `AgentConfig`
- `AgentConfigValidator`
- `AgentWorkspace`
- `ProjectScanner`
- `FileScanPolicy`
- `FileReader`
- `ToolRegistry`
- `CommandTool`
- `AgentRun`
- `AgentRunStore`
- `AgentCache`
- `AgentPublicApi`

## Build

From the module directory:

```bash
cd ~/vixcpp/vix/modules/agent
cmake --build build-ninja
```

From the Vix root:

```bash
cd ~/vixcpp/vix
cmake --build build-ninja
```

## Example with Ollama

```cpp
#include <vix/ai/agent/agent.hpp>
#include <vix/print.hpp>

int main()
{
  vix::ai::agent::AgentConfig config;

  config.provider = "ollama";
  config.model = "llama3";
  config.model_url = "http://127.0.0.1:11434";

  config.allow_file_read = true;
  config.allow_process = false;
  config.allow_file_write = false;

  config.use_cache = true;
  config.persist_memory = true;

  vix::ai::agent::Agent agent(config);

  vix::ai::agent::AgentRequest request;

  request.workspace = ".";
  request.input = "Explain what this project does.";
  request.mode = vix::ai::agent::AgentRequestMode::Analyze;

  request.allow_tools = true;
  request.allow_file_read = true;
  request.allow_process = false;
  request.allow_file_write = false;

  auto result = agent.run(request);

  if (!result)
  {
    vix::print("Agent failed:", result.error().message());
    return 1;
  }

  vix::print(result.value().text);

  return 0;
}
```

## Roadmap

Planned improvements:

- replace temporary curl usage in `OllamaProvider` with a native Vix HTTP client
- enforce real process timeouts once `vix::process` supports command timeout
- add streaming responses
- add structured tool schemas
- add safe file write support
- add richer run replay support
- add memory store for long-term agent context
- add more local providers
- add OpenAI-compatible provider later
- expose stable helper APIs only after `Agent::run(...)` is fully settled

## License
