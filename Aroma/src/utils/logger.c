// https://github.com/wiiu-env/WiiUPluginSystem/blob/a789f3234e95795cc6578e5ba66dea0fa1e0c7c3/plugins/example_plugin_cpp/src/utils/logger.c
#ifdef DEBUG
#include <stdint.h>
#include <whb/log_cafe.h>
#include <whb/log_module.h>
#include <whb/log_udp.h>

uint32_t moduleLogInit = false;
uint32_t cafeLogInit   = false;
uint32_t udpLogInit    = false;
#endif // DEBUG

void initLogging() {
#ifdef DEBUG
    if (!(moduleLogInit = WHBLogModuleInit())) {
        cafeLogInit = WHBLogCafeInit();
        udpLogInit  = WHBLogUdpInit();
    }
#endif // DEBUG
}

void deinitLogging() {
#ifdef DEBUG
    if (moduleLogInit) {
        WHBLogModuleDeinit();
        moduleLogInit = false;
    }
    if (cafeLogInit) {
        WHBLogCafeDeinit();
        cafeLogInit = false;
    }
    if (udpLogInit) {
        WHBLogUdpDeinit();
        udpLogInit = false;
    }
#endif // DEBUG
}
