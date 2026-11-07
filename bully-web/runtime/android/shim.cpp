// runtime/android/shim.cpp
#include "shim.h"
#include <cstdio>
#include <cstdarg>
#include <cstring>
#include <string>
#include <unordered_map>

namespace bully::android {

using arm64::CPUState;

// ---- liblog ----
static int android_log_print(int prio, const char* tag, const char* fmt, ...) {
    char buf[2048];
    va_list ap;
    va_start(ap, fmt);
    int n = vsnprintf(buf, sizeof(buf), fmt ? fmt : "", ap);
    va_end(ap);
    fprintf(stderr, "[log:%d] %s: %s\n", prio, tag ? tag : "", buf);
    return n;
}

// HostFn wrapper para __android_log_print (variadico: args em X0,X1,X2,X3..X7)
// Para funcoes variadicas, passamos os primeiros 8 args como inteiros.
static uint64_t h_android_log_print(CPUState& cpu) {
    int prio = (int)cpu.x[0];
    const char* tag = (const char*)cpu.x[1];
    const char* fmt = (const char*)cpu.x[2];
    // Args variadicos: X3..X7
    return (uint64_t)android_log_print(prio, tag, fmt,
        (void*)cpu.x[3], (void*)cpu.x[4], (void*)cpu.x[5],
        (void*)cpu.x[6], (void*)cpu.x[7]);
}

// ---- AAssetManager ----
static std::string g_assets_dir;

struct AAsset {
    FILE* fp;
    int64_t length;
};

struct AAssetManager {
    int dummy;
};

static AAssetManager* asset_manager_from_java(void* env, void* assetManager) {
    static AAssetManager mgr;
    return &mgr;
}

static AAsset* asset_manager_open(AAssetManager* mgr, const char* filename, int mode) {
    if (!filename) return nullptr;
    std::string path = g_assets_dir + "/" + filename;
    FILE* fp = fopen(path.c_str(), "rb");
    if (!fp) {
        fprintf(stderr, "[asset] not found: %s\n", filename);
        return nullptr;
    }
    fseek(fp, 0, SEEK_END);
    int64_t len = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    return new AAsset{fp, len};
}

static int asset_read(AAsset* asset, void* buf, size_t count) {
    if (!asset || !asset->fp) return 0;
    return (int)fread(buf, 1, count, asset->fp);
}

static int64_t asset_seek64(AAsset* asset, int64_t offset, int whence) {
    if (!asset || !asset->fp) return -1;
    return fseek(asset->fp, (long)offset, whence);
}

static int64_t asset_get_length64(AAsset* asset) {
    return asset ? asset->length : 0;
}

static int64_t asset_get_remaining_length64(AAsset* asset) {
    if (!asset || !asset->fp) return 0;
    int64_t cur = ftell(asset->fp);
    return asset->length - cur;
}

static int asset_close(AAsset* asset) {
    if (!asset) return -1;
    if (asset->fp) fclose(asset->fp);
    delete asset;
    return 0;
}

void set_assets_dir(const char* dir) {
    g_assets_dir = dir ? dir : "";
}

// HostFn wrappers
static uint64_t h_AAssetManager_fromJava(CPUState& cpu) {
    return (uint64_t)asset_manager_from_java((void*)cpu.x[0], (void*)cpu.x[1]);
}
static uint64_t h_AAssetManager_open(CPUState& cpu) {
    return (uint64_t)asset_manager_open((AAssetManager*)cpu.x[0],
        (const char*)cpu.x[1], (int)cpu.x[2]);
}
static uint64_t h_AAsset_read(CPUState& cpu) {
    return (uint64_t)asset_read((AAsset*)cpu.x[0], (void*)cpu.x[1], (size_t)cpu.x[2]);
}
static uint64_t h_AAsset_seek64(CPUState& cpu) {
    return (uint64_t)asset_seek64((AAsset*)cpu.x[0], (int64_t)cpu.x[1], (int)cpu.x[2]);
}
static uint64_t h_AAsset_getLength64(CPUState& cpu) {
    return (uint64_t)asset_get_length64((AAsset*)cpu.x[0]);
}
static uint64_t h_AAsset_getRemainingLength64(CPUState& cpu) {
    return (uint64_t)asset_get_remaining_length64((AAsset*)cpu.x[0]);
}
static uint64_t h_AAsset_close(CPUState& cpu) {
    return (uint64_t)asset_close((AAsset*)cpu.x[0]);
}

// ---- ANativeWindow ----
struct ANativeWindow {
    int width;
    int height;
    int format;
};

static ANativeWindow* g_window = nullptr;

static ANativeWindow* native_window_from_surface(void* env, void* surface) {
    if (!g_window) g_window = new ANativeWindow{1280, 720, 0};
    return g_window;
}
static int native_window_get_width(ANativeWindow* w) { return w ? w->width : 1280; }
static int native_window_get_height(ANativeWindow* w) { return w ? w->height : 720; }
static void native_window_release(ANativeWindow* window) { /* no-op */ }
static int native_window_set_buffers_geometry(ANativeWindow* w, int wdt, int h, int fmt) {
    if (w) { w->width = wdt; w->height = h; w->format = fmt; }
    return 0;
}

static uint64_t h_ANativeWindow_fromSurface(CPUState& cpu) {
    return (uint64_t)native_window_from_surface((void*)cpu.x[0], (void*)cpu.x[1]);
}
static uint64_t h_ANativeWindow_getWidth(CPUState& cpu) {
    return (uint64_t)native_window_get_width((ANativeWindow*)cpu.x[0]);
}
static uint64_t h_ANativeWindow_getHeight(CPUState& cpu) {
    return (uint64_t)native_window_get_height((ANativeWindow*)cpu.x[0]);
}
static uint64_t h_ANativeWindow_release(CPUState& cpu) {
    native_window_release((ANativeWindow*)cpu.x[0]);
    return 0;
}
static uint64_t h_ANativeWindow_setBuffersGeometry(CPUState& cpu) {
    return (uint64_t)native_window_set_buffers_geometry((ANativeWindow*)cpu.x[0],
        (int)cpu.x[1], (int)cpu.x[2], (int)cpu.x[3]);
}

// ---- system properties ----
static int system_property_get(const char* name, char* value) {
    if (value) value[0] = 0;
    return 0;
}
static uint64_t h_system_property_get(CPUState& cpu) {
    return (uint64_t)system_property_get((const char*)cpu.x[0], (char*)cpu.x[1]);
}

// ---- Register shims ----
void register_shims(bully::elf::Linker& linker) {
    linker.add_host_symbol("__android_log_print", h_android_log_print);
    linker.add_host_symbol("AAssetManager_fromJava", h_AAssetManager_fromJava);
    linker.add_host_symbol("AAssetManager_open", h_AAssetManager_open);
    linker.add_host_symbol("AAsset_read", h_AAsset_read);
    linker.add_host_symbol("AAsset_seek64", h_AAsset_seek64);
    linker.add_host_symbol("AAsset_getLength64", h_AAsset_getLength64);
    linker.add_host_symbol("AAsset_getRemainingLength64", h_AAsset_getRemainingLength64);
    linker.add_host_symbol("AAsset_close", h_AAsset_close);
    linker.add_host_symbol("ANativeWindow_fromSurface", h_ANativeWindow_fromSurface);
    linker.add_host_symbol("ANativeWindow_getWidth", h_ANativeWindow_getWidth);
    linker.add_host_symbol("ANativeWindow_getHeight", h_ANativeWindow_getHeight);
    linker.add_host_symbol("ANativeWindow_release", h_ANativeWindow_release);
    linker.add_host_symbol("ANativeWindow_setBuffersGeometry", h_ANativeWindow_setBuffersGeometry);
    linker.add_host_symbol("__system_property_get", h_system_property_get);
}

} // namespace bully::android
