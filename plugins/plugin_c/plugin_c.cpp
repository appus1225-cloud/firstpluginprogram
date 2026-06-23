extern "C" {
const char* plugin_name() {
    return "plugin_c";
}
int plugin_value() {
    return 123;
}
}