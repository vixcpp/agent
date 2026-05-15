cmake_minimum_required(VERSION 3.20)

project(vix_ai_agent
  VERSION 0.1.0
  DESCRIPTION "Local-first AI agent module for Vix.cpp"
  LANGUAGES CXX
)

option(VIX_AI_AGENT_BUILD_TESTS "Build vix ai agent tests" ON)
option(VIX_AI_AGENT_BUILD_EXAMPLES "Build vix ai agent examples" ON)

add_library(vix_ai_agent
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
)

add_library(vix::ai_agent ALIAS vix_ai_agent)

target_compile_features(vix_ai_agent
  PUBLIC
    cxx_std_20
)

target_include_directories(vix_ai_agent
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
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
)

set_target_properties(vix_ai_agent
  PROPERTIES
    OUTPUT_NAME vix_ai_agent
    EXPORT_NAME ai_agent
)

if(MSVC)
  target_compile_options(vix_ai_agent PRIVATE /W4)
else()
  target_compile_options(vix_ai_agent PRIVATE -Wall -Wextra -Wpedantic)
endif()

include(GNUInstallDirs)

install(
  TARGETS vix_ai_agent
  EXPORT vix_ai_agent_targets
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(
  DIRECTORY include/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
  EXPORT vix_ai_agent_targets
  NAMESPACE vix::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/vix_ai_agent
)

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

if(VIX_AI_AGENT_BUILD_TESTS AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests")
  enable_testing()

  add_executable(vix_ai_agent_tests
    tests/AgentConfigTest.cpp
    tests/AgentWorkspaceTest.cpp
    tests/ProjectScannerTest.cpp
    tests/ToolRegistryTest.cpp
    tests/AgentRunTest.cpp
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
