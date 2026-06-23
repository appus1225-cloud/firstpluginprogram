// plugins/pluginA/pluginA.cpp
// Example plugin A. Exports two C-style functions:
// - const char* plugin_name()
// - int plugin_value()
// The test runner expects pluginA -> value 42.
extern "C" {

const char* plugin_name() {
    return "pluginA";
}

int plugin_value() {
    // Minimal deterministic value used by the CI test.
    return 42;
}

} // extern "C"