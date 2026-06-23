// plugins/pluginB/pluginB.cpp
// Example plugin B. Exports plugin_name() and plugin_value().
// The test runner expects pluginB -> value 7.
extern "C" {

const char* plugin_name() {
    return "pluginB";
}

int plugin_value() {
    return 7;
}

} // extern "C"