# src/CMakeLists.txt
add_executable(plugin_loader main.cpp)

# Link with dl (required for dlopen/dlsym) and nlohmann_json
target_link_libraries(plugin_loader PRIVATE dl nlohmann_json::nlohmann_json)

set_target_properties(plugin_loader PROPERTIES
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

enable_testing()

# Use the generated config file (in build/config/config.json)
add_test(NAME RunPluginLoader
         COMMAND $<TARGET_FILE:plugin_loader> ${CMAKE_BINARY_DIR}/config/config.json)

set_tests_properties(RunPluginLoader PROPERTIES
    PASS_REGULAR_EXPRESSION "All enabled plugins loaded and verified successfully"
)
