# Top-level CMakeLists.txt (Linux-only)
cmake_minimum_required(VERSION 3.15)
project(PluginLoaderLinux LANGUAGES CXX)

# Use C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# We are Linux-only
set(SHARED_LIB_EXT ".so")

# Plugin filenames (must match OUTPUT_NAME in plugin CMakeLists)
set(PLUGIN_A_FILENAME "plugin_a${SHARED_LIB_EXT}")
set(PLUGIN_B_FILENAME "plugin_b${SHARED_LIB_EXT}")
set(PLUGIN_C_FILENAME "plugin_c${SHARED_LIB_EXT}")

# Configure config.json into build/config/config.json
configure_file(
  "${CMAKE_SOURCE_DIR}/config/config.json.in"
  "${CMAKE_BINARY_DIR}/config/config.json"
  @ONLY
)

# Bring in nlohmann::json (header-only) for JSON parsing
include(FetchContent)
FetchContent_Declare(
  nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.2
)
FetchContent_MakeAvailable(nlohmann_json)

# (Optional) Vendor handling macro: prefer local vendor dir, else FetchContent (replace URLs with real ones)
macro(maybe_add_vendor name url tag)
  if (EXISTS "${CMAKE_SOURCE_DIR}/vendor/${name}/CMakeLists.txt")
    message(STATUS "Using vendor/${name} from local source")
    add_subdirectory("vendor/${name}")
  else()
    message(STATUS "Fetching vendor ${name} from ${url}")
    FetchContent_Declare(${name} GIT_REPOSITORY ${url} GIT_TAG ${tag})
    FetchContent_MakeAvailable(${name})
  endif()
endmacro()

# Example: add vendors if needed
# maybe_add_vendor(vendorA https://example.com/vendorA.git main)
# maybe_add_vendor(vendorB https://example.com/vendorB.git main)

# Project subdirectories
add_subdirectory(src)
add_subdirectory(plugins)