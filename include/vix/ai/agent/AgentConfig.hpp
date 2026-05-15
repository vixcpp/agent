/**
 *
 *  @file AgentConfig.hpp
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
#ifndef VIX_AI_AGENT_AGENTCONFIG_HPP
#define VIX_AI_AGENT_AGENTCONFIG_HPP

#include <string>

namespace vix::ai::agent
{
  /**
   * @struct AgentConfig
   * @brief Runtime configuration for the Vix AI Agent.
   *
   * AgentConfig contains the stable configuration needed by the agent
   * runtime: model provider, model name, local model URL, workspace-related
   * paths, execution limits, and safety flags.
   *
   * The default configuration is local-first:
   * - local provider by default
   * - Ollama-compatible local URL
   * - local memory/cache directories under .vix/agent
   * - offline mode enabled by default
   */
  struct AgentConfig
  {
    /**
     * @brief Model provider name.
     *
     * Examples:
     * - "ollama"
     * - "llama.cpp"
     * - "openai" later
     */
    std::string provider{"ollama"};

    /**
     * @brief Model name used by the provider.
     *
     * Example:
     * - "llama3"
     * - "qwen2.5-coder"
     * - "deepseek-coder"
     */
    std::string model{"llama3"};

    /**
     * @brief Local or remote model endpoint.
     *
     * For Ollama, the default endpoint is:
     * http://127.0.0.1:11434
     */
    std::string model_url{"http://127.0.0.1:11434"};

    /**
     * @brief Local directory used for persistent agent memory.
     */
    std::string memory_dir{".vix/agent/memory"};

    /**
     * @brief Local directory used for cache entries.
     */
    std::string cache_dir{".vix/agent/cache"};

    /**
     * @brief Local directory used to store run history.
     */
    std::string runs_dir{".vix/agent/runs"};

    /**
     * @brief Local directory used for agent logs or event traces.
     */
    std::string logs_dir{".vix/agent/logs"};

    /**
     * @brief Default timeout for model calls and tool execution, in milliseconds.
     */
    unsigned timeout_ms{30'000};

    /**
     * @brief Maximum number of project files the agent may scan.
     */
    unsigned max_files{2'000};

    /**
     * @brief Maximum size of a single file the agent may read, in bytes.
     */
    unsigned max_file_size{512 * 1024};

    /**
     * @brief Maximum number of characters kept from a tool output.
     */
    unsigned max_tool_output{20'000};

    /**
     * @brief Maximum context size prepared for a model request.
     */
    unsigned max_context_chars{120'000};

    /**
     * @brief Whether the agent should avoid cloud/network providers by default.
     */
    bool offline{true};

    /**
     * @brief Whether the agent may execute controlled local commands.
     */
    bool allow_process{true};

    /**
     * @brief Whether the agent may read files from the workspace.
     */
    bool allow_file_read{true};

    /**
     * @brief Whether the agent may write files.
     *
     * This should stay false in the early MVP unless explicitly enabled.
     */
    bool allow_file_write{false};

    /**
     * @brief Whether the agent should use cache when possible.
     */
    bool use_cache{true};

    /**
     * @brief Whether the agent should persist memory.
     */
    bool persist_memory{true};
  };

} // namespace vix::ai::agent

#endif // VIX_AI_AGENT_AGENTCONFIG_HPP
