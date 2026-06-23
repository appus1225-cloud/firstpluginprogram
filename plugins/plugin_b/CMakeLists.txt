# plugins/plugin_b/CMakeLists.txt
# plugins/plugin_b/CMakeLists.txt
add_library(plugin_b SHARED plugin_b.cpp)

set_target_properties(plugin_b PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins
    OUTPUT_NAME "plugin_b"
)
