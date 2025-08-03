#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/mman.h>

#include "Log.h"
#define  PAGE_SIZE 4096

static uintptr_t get_module_base(const char* module_name) {
    FILE* file = fopen("/proc/self/maps", "r");
    if (!file) {
        return 0;
    }
    uintptr_t base = 0;
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, module_name) && !strstr(line, "r-xp")) {
            sscanf(line, "%lx", &base);
            break;
        }
    }
    fclose(file);
    return base;
}

static bool vm_readv(int pid, uintptr_t address, void* buffer, size_t size) {
    if (!address || !size || !pid || !buffer) {
        return false;
    }
    //TODO
}

long my_syscall(long number, long arg1, long arg2, long arg3, long arg4, long arg5, long arg6) {
    if (number == 270) {
        int pid = arg1;
        auto* local = (iovec*)arg2;
        long local_count = arg3;
        auto* remote = (iovec*)arg4;
        long remote_count = arg5;

        long result = syscall(number, arg1, arg2, arg3, arg4, arg5, arg6);
        if (result <= 0) {
            LOGD("my_syscall: pid=%d, local_count=%ld, remote_count=%ld", pid, local_count, remote_count);
            result = 0;
            for (long i = 0; i < local_count; ++i) {
                vm_readv(pid, (uintptr_t)remote[i].iov_base, (void*)local[i].iov_base, local[i].iov_len);
                result += local[i].iov_len;
            }
        }
        return result;
    }
    return syscall(number, arg1, arg2, arg3, arg4, arg5, arg6);
}

__attribute__((constructor)) static void init() {
    auto base = get_module_base("lib5.so");
    LOGD("Module base address: %lx", base);
    for (long i = 0; ; ++i) {
        auto addr = base + i * sizeof(uintptr_t);
        if (*(uintptr_t*)addr == (uintptr_t)syscall) {
            LOGD("Found syscall at offset %p syscall %p", addr, *(uintptr_t*)addr);
            //对齐
            mprotect((void*)(addr & ~(PAGE_SIZE - 1)), PAGE_SIZE, PROT_READ | PROT_WRITE | PROT_EXEC);
            *(uintptr_t*)(addr) = (uintptr_t)my_syscall;
            break;
        }
    }
}
