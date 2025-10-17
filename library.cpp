#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/mman.h>
#include <sys/ptrace.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "Log.h"
#include "driver.h"

#define PAGE_SIZE 4096
static Driver *driver; // KMA当前版本 420


static uintptr_t get_module_base(const char *module_name)
{
    FILE *file = fopen("/proc/self/maps", "r");
    if (!file)
    {
        return 0;
    }
    uintptr_t base = 0;
    char line[256];
    while (fgets(line, sizeof(line), file))
    {
        if (strstr(line, module_name) && !strstr(line, "r-xp"))
        {
            sscanf(line, "%lx", &base);
            break;
        }
    }
    fclose(file);
    return base;
}

/**
 * @brief 修改selinux为强制模式
 * @param file_path 文件路径
 * @return int 修改成功返回1，无需修改返回0，失败返回-1
 */
static inline int set_selinux_enforce(const char *file_path)
{
    static int fd = -1;
    char buf;

    // 如果文件描述符无效，重新打开
    if (fd == -1)
    {
        fd = open(file_path, O_RDWR);
        if (fd == -1)
        {
            return -1;
        }
    }

    // 每次读取前重置文件指针
    if (lseek(fd, 0, SEEK_SET) == -1)
    {
        close(fd);
        fd = -1;
        return -1;
    }

    // 读取单个字符
    if (read(fd, &buf, 1) != 1)
    {
        close(fd);
        fd = -1;
        return -1;
    }

    // 如果不是'0'，直接返回
    if (buf != '0')
    {
        return 0;
    }

    // 回退文件指针到开头
    if (lseek(fd, 0, SEEK_SET) == -1)
    {
        close(fd);
        fd = -1;
        return -1;
    }

    // 写入'1'
    if (write(fd, "1", 1) != 1)
    {
        close(fd);
        fd = -1;
        return -1;
    }

    // 确保数据写入磁盘
    if (fsync(fd) == -1)
    {
        // fsync失败不算严重错误，可以继续
    }

    return 1;
}

static inline void vm_readv(int pid, uintptr_t address, void *buffer, size_t size)
{
    if (!address || !size || !pid || !buffer)
    {
        return;
    }
    //TODO
    if (!driver)
    {
        driver = new Driver();
    }

    if (driver->initpid(pid))
    {
        // 恢复selinux
        set_selinux_enforce("/sys/fs/selinux/enforce");
        if (size <= PAGE_SIZE)
            // 硬件读
            driver->read_safe(address, buffer, size);

            // 内核读
            //driver->read(address, buffer, size);
    }
}

long my_syscall(long number, long arg1, long arg2, long arg3, long arg4, long arg5, long arg6)
{
    if (number == 270)
    {
        int pid = arg1;
        auto *local = (iovec *)arg2;
        long local_count = arg3;
        auto *remote = (iovec *)arg4;
        long remote_count = arg5;

        long result = 0;
        //LOGD("my_syscall: pid=%d, local_count=%ld, remote_count=%ld", pid, local_count, remote_count);

        for (long i = 0; i < local_count; ++i)
        {
            vm_readv(pid, (uintptr_t)remote[i].iov_base, (void *)local[i].iov_base, local[i].iov_len);
            result += local[i].iov_len;
        }

        return result;
    }
    return syscall(number, arg1, arg2, arg3, arg4, arg5, arg6);
}

long my_ptrace(int request, pid_t pid, void *addr, void *data) { return -1; }

__attribute__((constructor)) static void init()
{
    auto base = get_module_base("lib5.so");
    //LOGD("Module base address: %lx", base);
    for (long i = 0;; ++i)
    {
        auto addr = base + i * sizeof(uintptr_t);
        if (*(uintptr_t *)addr == (uintptr_t)syscall)
        {
            //LOGD("Found syscall at offset %p syscall %p", addr, *(uintptr_t*)addr);
            //对齐
            mprotect((void *)(addr & ~(PAGE_SIZE - 1)), PAGE_SIZE, PROT_READ | PROT_WRITE | PROT_EXEC);
            *(uintptr_t *)(addr) = (uintptr_t)my_syscall;
            break;
        }
    }

    for (long i = 0;; ++i)
    {
        auto addr = base + i * sizeof(uintptr_t);
        if (*(uintptr_t *)addr == (uintptr_t)ptrace)
        {
            //LOGD("Found ptrace at offset %p ptrace %p", addr, *(uintptr_t*)addr);
            //对齐
            mprotect((void *)(addr & ~(PAGE_SIZE - 1)), PAGE_SIZE, PROT_READ | PROT_WRITE | PROT_EXEC);
            *(uintptr_t *)(addr) = (uintptr_t)my_ptrace;
            break;
        }
    }
}