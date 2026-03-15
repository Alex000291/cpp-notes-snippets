// 无锁 SPSC 队列 — 单生产者单消费者，零锁
// 预分配固定大小 buffer，运行期无堆分配；环形缓冲 + 取模循环读写
#ifndef LFQUEUE_H
#define LFQUEUE_H

#include <atomic>
#include <vector>
#include <optional>

template<typename T>
class SPSCQueue {
public:
    explicit SPSCQueue(size_t capacity)
        : cap_(capacity), buf_(capacity), write_idx_(0), read_idx_(0) {}

    // 仅生产者调用
    bool push(T item) {
        size_t w = write_idx_.load(std::memory_order_relaxed);
        size_t next = (w + 1) % cap_;
        if (next == read_idx_.load(std::memory_order_acquire))
            return false;  // 满
        buf_[w] = std::move(item);
        write_idx_.store(next, std::memory_order_release);
        return true;
    }

    // 仅消费者调用
    std::optional<T> pop() {
        size_t r = read_idx_.load(std::memory_order_relaxed);
        if (r == write_idx_.load(std::memory_order_acquire))
            return std::nullopt;  // 空
        T item = std::move(buf_[r]);
        read_idx_.store((r + 1) % cap_, std::memory_order_release);
        return item;
    }

    bool empty() const {
        return read_idx_.load(std::memory_order_acquire) ==
               write_idx_.load(std::memory_order_acquire);
    }

private:
    const size_t cap_;
    std::vector<T> buf_;
    std::atomic<size_t> write_idx_;
    std::atomic<size_t> read_idx_;
};

#ifdef LFQUEUE_MAIN
#include <thread>
#include <cstdio>
int main() {
    SPSCQueue<int> q(64);
    std::thread producer([&] {
        for (int i = 0; i < 100; ++i)
            while (!q.push(i)) {}
    });
    std::thread consumer([&] {
        int n = 0;
        while (n < 100) {
            auto v = q.pop();
            if (v) { printf("%d\n", *v); ++n; }
        }
    });
    producer.join();
    consumer.join();
    return 0;
}
#endif

#endif
