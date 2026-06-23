# plugins/plugin_a/CMakeLists.txt
# plugins/plugin_a/CMakeLists.txt
add_library(plugin_a SHARED plugin_a.cpp)

set_target_properties(plugin_a PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins
    OUTPUT_NAME "plugin_a"
)
