// src/main.cpp
// Linux-only plugin loader that reads JSON config and loads enabled plugins using dlopen/dlsym.
// Uses nlohmann::json (provided as a header-only dependency via CMake FetchContent).
//
// Expected config schema (build/config/config.json):
// {
//   "plugins": [
//     { "name": "plugin_a", "path": "/full/path/to/build/plugins/plugin_a.so", "enabled": true, "expected_value": 42 },
//     ...
//   ]
// }
//
// Usage:
//   ./plugin_loader /path/to/config.json
// If no path provided it defaults to "config/config.json" relative to current working directory.

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include <cstdlib>
#include <dlfcn.h>
#include <nlohmann/json.hpp>

namespace fs = std::filesystem;
using json = nlohmann::json;

// plugin function types
using plugin_name_fn = const char* (*)();
using plugin_value_fn = int (*)();

struct PluginEntry {
    std::string name;
    fs::path path;
    bool enabled = true;
    bool has_expected = false;
    int expected_value = 0;
};

bool load_config(const fs::path& config_path, std::vector<PluginEntry>& out, std::string& errmsg) {
    std::ifstream ifs(config_path);
    if (!ifs) {
        errmsg = "Cannot open config file: " + config_path.string();
        return false;
    }
    json j;
    try {
        ifs >> j;
    } catch (const std::exception& ex) {
        errmsg = std::string("JSON parse error: ") + ex.what();
        return false;
    }
    if (!j.contains("plugins") || !j["plugins"].is_array()) {
        errmsg = "Config must contain a top-level \"plugins\" array";
        return false;
    }
    for (const auto& item : j["plugins"]) {
        PluginEntry e;
        if (item.contains("name") && item["name"].is_string()) e.name = item["name"].get<std::string>();
        if (!item.contains("path") || !item["path"].is_string()) {
            errmsg = "Each plugin entry must contain a string \"path\" field";
            return false;
        }
        e.path = fs::path(item["path"].get<std::string>());
        if (item.contains("enabled") && item["enabled"].is_boolean()) e.enabled = item["enabled"].get<bool>();
        if (item.contains("expected_value") && item["expected_value"].is_number_integer()) {
            e.has_expected = true;
            e.expected_value = item["expected_value"].get<int>();
        }
        out.push_back(std::move(e));
    }
    return true;
}

int main(int argc, char** argv) {
    fs::path cfg = (argc > 1) ? fs::path(argv[1]) : fs::path("config/config.json");
    std::cout << "Plugin loader: reading config: " << cfg << "\n";

    std::vector<PluginEntry> plugins;
    std::string err;
    if (!load_config(cfg, plugins, err)) {
        std::cerr << "Failed to load config: " << err << "\n";
        return EXIT_FAILURE;
    }
    if (plugins.empty()) {
        std::cerr << "No plugins defined in config\n";
        return EXIT_FAILURE;
    }

    bool all_ok = true;

    for (const auto& p : plugins) {
        std::cout << "Entry: name='" << p.name << "' path='" << p.path << "' enabled=" << (p.enabled ? "true" : "false") << "\n";
        if (!p.enabled) {
            std::cout << "  Skipping (disabled by config)\n";
            continue;
        }
        if (!fs::exists(p.path) || !fs::is_regular_file(p.path)) {
            std::cerr << "  Plugin file missing: " << p.path << "\n";
            all_ok = false;
            continue;
        }

        // Load
        dlerror(); // clear
        void* handle = dlopen(p.path.c_str(), RTLD_LAZY);
        if (!handle) {
            const char* d = dlerror();
            std::cerr << "  dlopen failed: " << (d ? d : "unknown") << "\n";
            all_ok = false;
            continue;
        }

        // Resolve symbols
        dlerror();
        plugin_name_fn name_fn = reinterpret_cast<plugin_name_fn>(dlsym(handle, "plugin_name"));
        const char* d1 = dlerror();
        if (!name_fn || d1) {
            std::cerr << "  dlsym(plugin_name) failed: " << (d1 ? d1 : "unknown") << "\n";
            dlclose(handle);
            all_ok = false;
            continue;
        }
        dlerror();
        plugin_value_fn value_fn = reinterpret_cast<plugin_value_fn>(dlsym(handle, "plugin_value"));
        const char* d2 = dlerror();
        if (!value_fn || d2) {
            std::cerr << "  dlsym(plugin_value) failed: " << (d2 ? d2 : "unknown") << "\n";
            dlclose(handle);
            all_ok = false;
            continue;
        }

        // Call plugin functions
        const char* reported_name = nullptr;
        int value = 0;
        try {
            reported_name = name_fn();
            value = value_fn();
        } catch (const std::exception& ex) {
            std::cerr << "  Exception calling plugin functions: " << ex.what() << "\n";
            dlclose(handle);
            all_ok = false;
            continue;
        } catch (...) {
            std::cerr << "  Unknown exception calling plugin functions\n";
            dlclose(handle);
            all_ok = false;
            continue;
        }

        std::string key = p.name.empty() ? (reported_name ? reported_name : std::string("<unnamed>")) : p.name;
        std::cout << "  Loaded plugin '" << key << "' (reported name='" << (reported_name ? reported_name : "<null>") << "') -> value=" << value << "\n";

        if (!p.has_expected) {
            std::cerr << "  No expected_value in config for plugin '" << key << "'. Treating as failure in CI.\n";
            dlclose(handle);
            all_ok = false;
            continue;
        } else {
            if (p.expected_value != value) {
                std::cerr << "  Unexpected value for plugin '" << key << "': got " << value << " expected " << p.expected_value << "\n";
                dlclose(handle);
                all_ok = false;
                continue;
            } else {
                std::cout << "  Plugin '" << key << "' returned expected value " << value << "\n";
            }
        }

        dlclose(handle);
    }

    if (all_ok) {
        std::cout << "All enabled plugins loaded and verified successfully.\n";
        return EXIT_SUCCESS;
    } else {
        std::cerr << "One or more plugins failed verification.\n";
        return EXIT_FAILURE;
    }
}
