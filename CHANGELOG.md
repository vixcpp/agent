# Changelog

All notable changes to the `vix::ai_agent` module will be documented in this file.

The format is inspired by Keep a Changelog, and this module follows the Vix.cpp release style.

## v0.1.0

### Added
- Added the initial `vix::ai_agent` module.
- Added the `vix::ai::agent` namespace.
- Added the core agent runtime:
  - `Agent`
  - `AgentConfig`
  - `AgentConfigLoader`
  - `AgentRequest`
  - `AgentResponse`
  - `AgentResult`
  - `AgentWorkspace`
- Added local-first model provider support.
- Added `OllamaProvider` for local Ollama-compatible model execution.
- Added provider-neutral model types:
  - `ModelProvider`
  - `ModelRequest`
  - `ModelResponse`
- Added controlled tool execution primitives:
  - `Tool`
  - `ToolCall`
  - `ToolResult`
  - `ToolRegistry`
- Added built-in tools:
  - `FileReadTool`
  - `CommandTool`
- Added workspace utilities:
  - `FileReader`
  - `FileScanPolicy`
  - `ProjectScanner`
- Added agent identity helpers:
  - `AgentId`
  - `AgentFingerprint`
- Added standalone CMake build support.
- Added Vix umbrella build integration support.
- Added examples for basic agent usage, project scanning, and Ollama usage.
- Added initial tests for configuration, workspace handling, project scanning, tool registry behavior, and agent runs.

### Safety
- File reading is enabled by default.
- File writing is disabled by default.
- Local process execution is disabled by default.
- Command execution must be explicitly enabled through configuration.
- Tool output is bounded by `max_tool_output`.
- Project scanning is bounded by `max_files`, `max_file_size`, and `max_context_chars`.

### Build
- Added the `vix_ai_agent` target.
- Added the `vix::ai_agent` CMake alias.
- Added support for required Vix module dependencies:
  - `vix::error`
  - `vix::json`
  - `vix::fs`
  - `vix::path`
  - `vix::process`
  - `vix::env`
  - `vix::time`
  - `vix::crypto`
- Added CMake options:
  - `VIX_AI_AGENT_BUILD_TESTS`
  - `VIX_AI_AGENT_BUILD_EXAMPLES`
  - `VIX_AI_AGENT_ENABLE_INSTALL`

### Compatibility
- Initial release.
- Designed to work as a standalone Vix module.
- Designed to integrate into the Vix umbrella build through `vix::ai_agent`.
