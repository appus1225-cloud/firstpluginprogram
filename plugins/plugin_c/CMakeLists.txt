# plugins/plugin_c/CMakeLists.txt
add_library(plugin_c SHARED plugin_c.cpp)

set_target_properties(plugin_c PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins
    ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins
    OUTPUT_NAME "plugin_c"
)