# plugins/pluginA/CMakeLists.txt
add_library(pluginA SHARED pluginA.cpp)

# Ensure plugin is placed under build/plugins for easy discovery by the test.
set_target_properties(pluginA PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins
    OUTPUT_NAME "pluginA"
)