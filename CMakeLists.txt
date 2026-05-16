#  @file CMakeLists.txt
#  @author Gaspard Kirira
#
#  Copyright 2026, Gaspard Kirira. All rights reserved.
#  https://github.com/vixcpp/vix
#  Use of this source code is governed by a MIT license
#  that can be found in the License file.
#
# ====================================================================
# Vix.cpp - AI Agent Module
# ====================================================================
# Purpose:
#   Local-first AI agent module for Vix.cpp.
#   Provides a controlled agent runtime capable of:
#     - preparing and scanning a local workspace
#     - building model requests
#     - using local model providers such as Ollama
#     - exposing safe tools such as file reading and command execution
#     - returning structured results and errors
#
#   The module is designed to be local-first, explicit, and safe by
#   default. It does not require a cloud model provider. The first
#   provider is Ollama, accessed through local process execution.
#
# Public Targets:
#   - vix_ai_agent   : The actual library target
#   - vix::ai_agent  : Namespaced alias for consumers
#
# Public Dependencies:
#   - vix::error
#   - vix::json
#   - vix::fs
#   - vix::path
#   - vix::process
#   - vix::env
#   - vix::time
#   - vix::crypto
#   - vix::log
#   - vix::cache
#
# Installation/Export:
#   - Umbrella build exports into: VixTargets
#   - Standalone build exports into: vix_ai_agentTargets
# ====================================================================

cmake_minimum_required(VERSION 3.20)

project(vix_ai_agent
  VERSION 0.1.0
  DESCRIPTION "Local-first AI agent module for Vix.cpp"
  LANGUAGES CXX
)

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# --------------------------------------------------------------------
# Global settings
# --------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# --------------------------------------------------------------------
# Options
# --------------------------------------------------------------------
option(VIX_AI_AGENT_BUILD_TESTS "Build vix ai agent tests" OFF)
option(VIX_AI_AGENT_BUILD_EXAMPLES "Build vix ai agent examples" OFF)

option(VIX_AI_AGENT_FETCH_ERROR "Auto-fetch vix::error if missing" ON)
option(VIX_AI_AGENT_FETCH_JSON "Auto-fetch vix::json if missing" ON)
option(VIX_AI_AGENT_FETCH_FS "Auto-fetch vix::fs if missing" ON)
option(VIX_AI_AGENT_FETCH_PATH "Auto-fetch vix::path if missing" ON)
option(VIX_AI_AGENT_FETCH_PROCESS "Auto-fetch vix::process if missing" ON)
option(VIX_AI_AGENT_FETCH_ENV "Auto-fetch vix::env if missing" ON)
option(VIX_AI_AGENT_FETCH_TIME "Auto-fetch vix::time if missing" ON)
option(VIX_AI_AGENT_FETCH_CRYPTO "Auto-fetch vix::crypto if missing" ON)
option(VIX_AI_AGENT_FETCH_LOG "Auto-fetch vix::log if missing" ON)
option(VIX_AI_AGENT_FETCH_CACHE "Auto-fetch vix::cache if missing" ON)

# --------------------------------------------------------------------
# Export set selection
# --------------------------------------------------------------------
if(DEFINED VIX_UMBRELLA_BUILD AND VIX_UMBRELLA_BUILD)
  set(VIX_AI_AGENT_EXPORT_SET VixTargets)
else()
  set(VIX_AI_AGENT_EXPORT_SET vix_ai_agentTargets)
endif()

# --------------------------------------------------------------------
# Umbrella build policy
# --------------------------------------------------------------------
# When building inside the Vix umbrella, dependencies are managed by the
# umbrella project. The AI agent module must not FetchContent nor add
# sibling subdirectories in umbrella mode.
# --------------------------------------------------------------------
if(DEFINED VIX_UMBRELLA_BUILD AND VIX_UMBRELLA_BUILD)
  set(VIX_AI_AGENT_FETCH_ERROR OFF CACHE BOOL "Auto-fetch vix::error if missing" FORCE)
  set(VIX_AI_AGENT_FETCH_JSON OFF CACHE BOOL "Auto-fetch vix::json if missing" FORCE)
  set(VIX_AI_AGENT_FETCH_FS OFF CACHE BOOL "Auto-fetch vix::fs if missing" FORCE)
  set(VIX_AI_AGENT_FETCH_PATH OFF CACHE BOOL "Auto-fetch vix::path if missing" FORCE)
  set(VIX_AI_AGENT_FETCH_PROCESS OFF CACHE BOOL "Auto-fetch vix::process if missing" FORCE)
  set(VIX_AI_AGENT_FETCH_ENV OFF CACHE BOOL "Auto-fetch vix::env if missing" FORCE)
  set(VIX_AI_AGENT_FETCH_TIME OFF CACHE BOOL "Auto-fetch vix::time if missing" FORCE)
  set(VIX_AI_AGENT_FETCH_CRYPTO OFF CACHE BOOL "Auto-fetch vix::crypto if missing" FORCE)
  set(VIX_AI_AGENT_FETCH_LOG OFF CACHE BOOL "Auto-fetch vix::log if missing" FORCE)
  set(VIX_AI_AGENT_FETCH_CACHE OFF CACHE BOOL "Auto-fetch vix::cache if missing" FORCE)
endif()

# --------------------------------------------------------------------
# Dependency helper
# --------------------------------------------------------------------
function(vix_ai_agent_require_dependency target package_name sibling_dir fetch_option repo_url git_tag)
  if(TARGET ${target})
    return()
  endif()

  if(DEFINED VIX_UMBRELLA_BUILD AND VIX_UMBRELLA_BUILD)
    message(FATAL_ERROR "[ai_agent] Umbrella build: ${target} must be provided before ai_agent.")
  endif()

  if(EXISTS "${CMAKE_CURRENT_LIST_DIR}/../${sibling_dir}/CMakeLists.txt")
    message(STATUS "[ai_agent] Adding ${sibling_dir} from sibling: ../${sibling_dir}")

    add_subdirectory(
      "${CMAKE_CURRENT_LIST_DIR}/../${sibling_dir}"
      "${CMAKE_CURRENT_BINARY_DIR}/deps/${sibling_dir}"
      EXCLUDE_FROM_ALL
    )
  else()
    find_package(${package_name} CONFIG QUIET)
  endif()

  if(TARGET ${target})
    return()
  endif()

  if(${fetch_option})
    include(FetchContent)

    message(STATUS "[ai_agent] Fetching ${target} via FetchContent")

    FetchContent_Declare(${package_name}
      GIT_REPOSITORY ${repo_url}
      GIT_TAG        ${git_tag}
    )

    FetchContent_MakeAvailable(${package_name})
  endif()

  if(NOT TARGET ${target})
    message(FATAL_ERROR
      "[ai_agent] ${target} not found. Install ${package_name}, provide the target, "
      "add sibling module, or enable ${fetch_option}=ON."
    )
  endif()
endfunction()

# --------------------------------------------------------------------
# Nested dependency policy
# --------------------------------------------------------------------
if(NOT TARGET vix::utils)
  set(VIX_CRYPTO_FETCH_UTILS OFF CACHE BOOL "Auto-fetch vix::utils if missing" FORCE)
endif()

vix_ai_agent_require_dependency(
  vix::error
  vix_error
  error
  VIX_AI_AGENT_FETCH_ERROR
  https://github.com/vixcpp/error.git
  v0.1.0
)

vix_ai_agent_require_dependency(
  vix::json
  vix_json
  json
  VIX_AI_AGENT_FETCH_JSON
  https://github.com/vixcpp/json.git
  v0.1.0
)

vix_ai_agent_require_dependency(
  vix::fs
  vix_fs
  fs
  VIX_AI_AGENT_FETCH_FS
  https://github.com/vixcpp/fs.git
  v0.1.0
)

vix_ai_agent_require_dependency(
  vix::path
  vix_path
  path
  VIX_AI_AGENT_FETCH_PATH
  https://github.com/vixcpp/path.git
  v0.1.0
)

vix_ai_agent_require_dependency(
  vix::process
  vix_process
  process
  VIX_AI_AGENT_FETCH_PROCESS
  https://github.com/vixcpp/process.git
  v0.1.0
)

vix_ai_agent_require_dependency(
  vix::env
  vix_env
  env
  VIX_AI_AGENT_FETCH_ENV
  https://github.com/vixcpp/env.git
  v0.1.0
)

vix_ai_agent_require_dependency(
  vix::time
  vix_time
  time
  VIX_AI_AGENT_FETCH_TIME
  https://github.com/vixcpp/time.git
  v0.1.0
)

vix_ai_agent_require_dependency(
  vix::crypto
  vix_crypto
  crypto
  VIX_AI_AGENT_FETCH_CRYPTO
  https://github.com/vixcpp/crypto.git
  v0.1.0
)

vix_ai_agent_require_dependency(
  vix::log
  vix_log
  log
  VIX_AI_AGENT_FETCH_LOG
  https://github.com/vixcpp/log.git
  v0.1.0
)

vix_ai_agent_require_dependency(
  vix::cache
  vix_cache
  cache
  VIX_AI_AGENT_FETCH_CACHE
  https://github.com/vixcpp/cache.git
  v0.1.0
)

# --------------------------------------------------------------------
# Sources
# --------------------------------------------------------------------
set(VIX_AI_AGENT_SOURCES
  src/Agent.cpp
  src/AgentConfigLoader.cpp
  src/AgentError.cpp
  src/AgentRunTimer.cpp
  src/AgentWorkspace.cpp

  src/crypto/AgentFingerprint.cpp
  src/crypto/AgentId.cpp

  src/workspace/FileScanPolicy.cpp
  src/workspace/FileReader.cpp
  src/workspace/ProjectScanner.cpp

  src/tools/ToolRegistry.cpp
  src/tools/FileReadTool.cpp
  src/tools/CommandTool.cpp

  src/model/OllamaProvider.cpp
  src/AgentConfigValidator.cpp
  src/AgentRunStore.cpp
)

# --------------------------------------------------------------------
# Library target
# --------------------------------------------------------------------
add_library(vix_ai_agent STATIC
  ${VIX_AI_AGENT_SOURCES}
)

add_library(vix::ai_agent ALIAS vix_ai_agent)

target_compile_features(vix_ai_agent
  PUBLIC
    cxx_std_20
)

target_include_directories(vix_ai_agent
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)

target_link_libraries(vix_ai_agent
  PUBLIC
    vix::error
    vix::json
    vix::fs
    vix::path
    vix::process
    vix::env
    vix::time
    vix::crypto
    vix::log
    vix::cache
)

set_target_properties(vix_ai_agent
  PROPERTIES
    OUTPUT_NAME vix_ai_agent
    VERSION ${PROJECT_VERSION}
    SOVERSION 0
    EXPORT_NAME ai_agent
)

# --------------------------------------------------------------------
# Warnings
# --------------------------------------------------------------------
if(MSVC)
  target_compile_options(vix_ai_agent PRIVATE /W4)
else()
  target_compile_options(vix_ai_agent PRIVATE
    -Wall
    -Wextra
    -Wpedantic
  )
endif()

# --------------------------------------------------------------------
# Sanitizers
# --------------------------------------------------------------------
if(VIX_ENABLE_SANITIZERS AND NOT MSVC)
  target_compile_options(vix_ai_agent PRIVATE
    -fno-omit-frame-pointer
    -fsanitize=address,undefined
  )

  target_link_options(vix_ai_agent PRIVATE
    -fsanitize=address,undefined
  )
endif()

# --------------------------------------------------------------------
# Install target and standalone package config
# --------------------------------------------------------------------
if(VIX_AI_AGENT_ENABLE_INSTALL)
  install(TARGETS vix_ai_agent
    EXPORT ${VIX_AI_AGENT_EXPORT_SET}
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  )

  if(NOT (DEFINED VIX_UMBRELLA_BUILD AND VIX_UMBRELLA_BUILD))
    install(DIRECTORY include/
      DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
      FILES_MATCHING
        PATTERN "*.hpp"
        PATTERN "*.h"
    )

    configure_package_config_file(
      "${CMAKE_CURRENT_SOURCE_DIR}/cmake/vix_ai_agentConfig.cmake.in"
      "${CMAKE_CURRENT_BINARY_DIR}/vix_ai_agentConfig.cmake"
      INSTALL_DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/vix_ai_agent"
    )

    write_basic_package_version_file(
      "${CMAKE_CURRENT_BINARY_DIR}/vix_ai_agentConfigVersion.cmake"
      VERSION ${PROJECT_VERSION}
      COMPATIBILITY SameMajorVersion
    )

    install(FILES
      "${CMAKE_CURRENT_BINARY_DIR}/vix_ai_agentConfig.cmake"
      "${CMAKE_CURRENT_BINARY_DIR}/vix_ai_agentConfigVersion.cmake"
      DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/vix_ai_agent"
    )

    install(EXPORT vix_ai_agentTargets
      FILE vix_ai_agentTargets.cmake
      NAMESPACE vix::
      DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/vix_ai_agent"
    )
  endif()
endif()
# --------------------------------------------------------------------
# Examples
# --------------------------------------------------------------------
if(VIX_AI_AGENT_BUILD_EXAMPLES AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/examples")
  add_executable(vix_ai_agent_basic_agent
    examples/basic_agent.cpp
  )

  target_link_libraries(vix_ai_agent_basic_agent
    PRIVATE
      vix::ai_agent
  )

  add_executable(vix_ai_agent_scan_project
    examples/scan_project.cpp
  )

  target_link_libraries(vix_ai_agent_scan_project
    PRIVATE
      vix::ai_agent
  )

  add_executable(vix_ai_agent_ollama_agent
    examples/ollama_agent.cpp
  )

  target_link_libraries(vix_ai_agent_ollama_agent
    PRIVATE
      vix::ai_agent
  )
endif()

# --------------------------------------------------------------------
# Tests
# --------------------------------------------------------------------
if(VIX_AI_AGENT_BUILD_TESTS AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests")
  include(CTest)
  enable_testing()

  add_executable(vix_ai_agent_tests
    tests/AgentConfigTest.cpp
    tests/AgentWorkspaceTest.cpp
    tests/ProjectScannerTest.cpp
    tests/ToolRegistryTest.cpp
    tests/AgentRunTest.cpp
    tests/FileScanPolicyTest.cpp
    tests/FileReaderTest.cpp
    tests/CommandToolTest.cpp
    tests/AgentRunStoreTest.cpp
    tests/AgentCacheTest.cpp
    tests/AgentPublicApiTest.cpp
  )

  target_link_libraries(vix_ai_agent_tests
    PRIVATE
      vix::ai_agent
  )

  add_test(
    NAME vix_ai_agent_tests
    COMMAND vix_ai_agent_tests
  )
endif()

# --------------------------------------------------------------------
# Summary
# --------------------------------------------------------------------
message(STATUS "------------------------------------------------------")
message(STATUS "vix::ai_agent configured (${PROJECT_VERSION})")
message(STATUS "Mode: STATIC")
message(STATUS "Export set: ${VIX_AI_AGENT_EXPORT_SET}")
message(STATUS "Error target:   vix::error")
message(STATUS "Json target:    vix::json")
message(STATUS "Fs target:      vix::fs")
message(STATUS "Path target:    vix::path")
message(STATUS "Process target: vix::process")
message(STATUS "Env target:     vix::env")
message(STATUS "Time target:    vix::time")
message(STATUS "Crypto target:  vix::crypto")
message(STATUS "Log target:     vix::log")
message(STATUS "Cache target:   vix::cache")
message(STATUS "Install/export: ${VIX_AI_AGENT_ENABLE_INSTALL}")
message(STATUS "Include dir: ${CMAKE_CURRENT_SOURCE_DIR}/include")
message(STATUS "Build tests: ${VIX_AI_AGENT_BUILD_TESTS}")
message(STATUS "Build examples: ${VIX_AI_AGENT_BUILD_EXAMPLES}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "------------------------------------------------------")
