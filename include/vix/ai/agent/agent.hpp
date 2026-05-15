/**
 *
 *  @file agent.hpp
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
#ifndef VIX_AI_AGENT_AGENT_HPP
#define VIX_AI_AGENT_AGENT_HPP

#include <vix/ai/agent/Agent.hpp>
#include <vix/ai/agent/AgentConfig.hpp>
#include <vix/ai/agent/AgentConfigLoader.hpp>
#include <vix/ai/agent/AgentError.hpp>
#include <vix/ai/agent/AgentRequest.hpp>
#include <vix/ai/agent/AgentResponse.hpp>
#include <vix/ai/agent/AgentResult.hpp>
#include <vix/ai/agent/AgentRunTimer.hpp>
#include <vix/ai/agent/AgentWorkspace.hpp>

#include <vix/ai/agent/crypto/AgentFingerprint.hpp>
#include <vix/ai/agent/crypto/AgentId.hpp>

#include <vix/ai/agent/model/ModelProvider.hpp>
#include <vix/ai/agent/model/ModelRequest.hpp>
#include <vix/ai/agent/model/ModelResponse.hpp>
#include <vix/ai/agent/model/OllamaProvider.hpp>

#include <vix/ai/agent/tools/CommandTool.hpp>
#include <vix/ai/agent/tools/FileReadTool.hpp>
#include <vix/ai/agent/tools/Tool.hpp>
#include <vix/ai/agent/tools/ToolCall.hpp>
#include <vix/ai/agent/tools/ToolRegistry.hpp>
#include <vix/ai/agent/tools/ToolResult.hpp>

#include <vix/ai/agent/workspace/FileReader.hpp>
#include <vix/ai/agent/workspace/FileScanPolicy.hpp>
#include <vix/ai/agent/workspace/ProjectScanner.hpp>

#endif // VIX_AI_AGENT_AGENT_HPP
