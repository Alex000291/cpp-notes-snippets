// 异步日志 — SPSC 队列将写入剥离到专用线程；union 存日志项；logf 变参 printf 风格
#ifndef LOGGING_H
#define LOGGING_H

#include <thread>
#include <atomic>
#include <cstdarg>
#include <cstdio>
#include <cstring>

// 依赖 SPSC 队列（此处内联简化版，仅存固定大小消息）
#include "lfqueue.h"

enum class LogLevel { Debug, Info, Warn, Error };

struct LogEntry {
    LogLevel level;
    char msg[256];
};

class AsyncLogger {
public:
    AsyncLogger() : running_(true), th_([this] { worker(); }) {}

    ~AsyncLogger() {
        running_.store(false);
        if (th_.joinable()) th_.join();
    }

    void logf(LogLevel level, const char* fmt, ...) {
        LogEntry e{};
        e.level = level;
        va_list ap;
        va_start(ap, fmt);
        int n = std::vsnprintf(e.msg, sizeof(e.msg), fmt, ap);
        va_end(ap);
        if (n >= (int)sizeof(e.msg)) e.msg[sizeof(e.msg) - 1] = '\0';
        queue_.push(e);
    }

private:
    void worker() {
        while (running_.load(std::memory_order_acquire)) {
            auto e = queue_.pop();
            if (e) {
                const char* tag = "?";
                switch (e->level) {
                    case LogLevel::Debug: tag = "D"; break;
                    case LogLevel::Info:  tag = "I"; break;
                    case LogLevel::Warn:  tag = "W"; break;
                    case LogLevel::Error: tag = "E"; break;
                }
                std::fprintf(stderr, "[%s] %s\n", tag, e->msg);
            }
        }
    }

    SPSCQueue<LogEntry> queue_{1024};
    std::atomic<bool> running_;
    std::thread th_;
};

#ifdef LOGGING_MAIN
int main() {
    AsyncLogger log;
    log.logf(LogLevel::Info, "start %d", 42);
    log.logf(LogLevel::Warn, "value=%s", "test");
    return 0;
}
#endif

#endif
