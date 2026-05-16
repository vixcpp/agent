/**
 *
 *  @file ai.hpp
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
#ifndef VIX_AI_MODULE_HPP
#define VIX_AI_MODULE_HPP

/**
 * @defgroup vix_ai Vix AI
 * @brief High-level AI APIs for Vix.cpp.
 *
 * The Vix AI module exposes a simple public facade for building
 * AI-powered tools with Vix.cpp.
 *
 * Main entry point:
 * @code
 * #include <vix/ai.hpp>
 *
 * int main()
 * {
 *   vix::ai::Agent agent;
 *
 *   agent.set_model("local:llama");
 *   agent.add_tool("filesystem");
 *
 *   auto result = agent.run("Explain this project.");
 *
 *   if (!result)
 *   {
 *     return 1;
 *   }
 *
 *   vix::print(result.value().text());
 *   return 0;
 * }
 * @endcode
 *
 * Lower-level agent internals remain available under:
 * @code
 * vix::ai::agent
 * @endcode
 */

#include <vix/ai/Agent.hpp>
#include <vix/ai/Result.hpp>

#endif // VIX_AI_MODULE_HPP
