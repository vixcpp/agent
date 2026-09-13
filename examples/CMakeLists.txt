cmake_minimum_required(VERSION 3.20)

project(vix_example_agent LANGUAGES CXX)

if (NOT TARGET vix::ai_agent)
  find_package(Vix CONFIG REQUIRED)
endif()

set(_vix_agent_examples
  agent_with_cache
  agent_with_command_tool
  basic_agent
  custom_provider
  ollama_agent
  public_api
  scan_project
)

foreach (_example IN LISTS _vix_agent_examples)
  add_executable("agent_${_example}" "${_example}.cpp")
  target_compile_features("agent_${_example}" PRIVATE cxx_std_20)
  target_link_libraries("agent_${_example}" PRIVATE vix::ai_agent vix::io)
endforeach()

# These providers are fully local and do not require a model service or credentials.
enable_testing()
foreach (_example IN ITEMS agent_with_cache custom_provider scan_project)
  add_test(NAME "agent.${_example}" COMMAND "agent_${_example}")
endforeach()
