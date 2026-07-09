#include <jni.h>
#include <string>
#include <dlfcn.h>
#include <sys/mman.h>
#include <unistd.h>
#include <android/log.h>

#define LOG_TAG "AimBody"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

struct HitboxData {
    float head[3];
    float body[3];
    float arms[3];
    float legs[3];
    float headSize;
    float bodySize;
    float armSize;
    float legSize;
};

const HitboxData AIMBODY_DATA = {
    .head = {1.0f, 1.0f, 1.0f},
    .body = {1.0f, 1.0f, 1.0f},
    .arms = {1.0f, 1.0f, 1.0f},
    .legs = {1.0f, 1.0f, 1.0f},
    .headSize = 10.0f,
    .bodySize = 10.0f,
    .armSize = 10.0f,
    .legSize = 10.0f
};

void* findHitboxAddress() {
    void* base = dlopen("libil2cpp.so", RTLD_LAZY);
    if (!base) return nullptr;
    Dl_info info;
    if (dladdr(base, &info)) {
        uint8_t* start = (uint8_t*)info.dli_fbase;
        uint8_t* end = start + 0x1000000;
        for (uint8_t* p = start; p < end; p++) {
            if (memcmp(p, "\x00\x00\x80\x3F\x00\x00\x80\x3F", 8) == 0) {
                return (void*)p;
            }
        }
    }
    return nullptr;
}

void overrideHitbox() {
    void* addr = findHitboxAddress();
    if (!addr) return;
    size_t page_size = sysconf(_SC_PAGE_SIZE);
    void* page_start = (void*)((uintptr_t)addr & ~(page_size - 1));
    mprotect(page_start, page_size * 2, PROT_READ | PROT_WRITE | PROT_EXEC);
    memcpy(addr, &AIMBODY_DATA, sizeof(HitboxData));
    mprotect(page_start, page_size * 2, PROT_READ | PROT_EXEC);
    LOGI("AimBody activated!");
}

void antiBan() {
    system("logcat -c");
    system("dmesg -c > /dev/null 2>&1");
    system("echo 'system_server' > /proc/self/comm");
    system("rm -rf /data/local/tmp/*.log 2>/dev/null");
}

__attribute__((constructor))
void init() {
    LOGI("AimBody loaded!");
    sleep(2);
    antiBan();
    overrideHitbox();
    LOGI("AimBody activated successfully!");
}
