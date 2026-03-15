// 线程创建 + CPU 亲和性 — 变参完美转发；pthread 绑定核；atomic 启动屏障
#ifndef THREADING_H
#define THREADING_H

#include <thread>
#include <atomic>
#include <utility>
#include <functional>

#ifdef __linux__
#include <pthread.h>
#include <sched.h>
#endif

// 带 CPU 亲和性的线程：绑定到 core_id，并用 ready 做启动同步
template<typename F, typename... Args>
std::thread start_thread_on_cpu(int core_id, std::atomic<bool>& ready, F&& f, Args&&... args) {
    std::thread t([core_id, &ready, f = std::forward<F>(f), ... args = std::forward<Args>(args)]() mutable {
#ifdef __linux__
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(core_id, &cpuset);
        if (pthread_setaffinity_np(pthread_self(), sizeof(cpuset), &cpuset) != 0) { /* 绑定失败 */ }
#endif
        ready.store(true, std::memory_order_release);
        std::invoke(std::move(f), std::move(args)...);
    });
    while (!ready.load(std::memory_order_acquire)) {}
    return t;
}

#ifdef THREADING_MAIN
#include <cstdio>
int main() {
    std::atomic<bool> ready{false};
    std::thread t = start_thread_on_cpu(0, ready, [] { printf("running on core 0\n"); });
    t.join();
    return 0;
}
#endif

#endif
