// Edge-triggered Epoll TCP 服务器 — EPOLLET；分离 rx/tx/dx 列表；非阻塞 accept 循环
#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <sys/epoll.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <vector>
#include <list>
#include <cerrno>
#include <cstring>
#include <cstdio>

#include "sockets.h"

class EpollTcpServer {
public:
    static constexpr int MAX_EVENTS = 64;
    static constexpr int LISTEN_BACKLOG = 128;

    EpollTcpServer(const char* bind_ip, uint16_t port) : listen_fd_(-1), epoll_fd_(-1) {
        listen_fd_ = socket(AF_INET, SOCK_STREAM, 0);
        if (listen_fd_ < 0) return;
        int opt = 1;
        setsockopt(listen_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        inet_pton(AF_INET, bind_ip, &addr.sin_addr);
        if (bind(listen_fd_, (sockaddr*)&addr, sizeof(addr)) < 0) {
            close(listen_fd_);
            listen_fd_ = -1;
            return;
        }
        if (listen(listen_fd_, LISTEN_BACKLOG) < 0) {
            close(listen_fd_);
            listen_fd_ = -1;
            return;
        }
        sockets::set_nonblocking(listen_fd_, true);

        epoll_fd_ = epoll_create1(EPOLL_CLOEXEC);
        if (epoll_fd_ < 0) {
            close(listen_fd_);
            listen_fd_ = -1;
            return;
        }
        add_epoll(listen_fd_, EPOLLIN | EPOLLET);
    }

    ~EpollTcpServer() {
        if (epoll_fd_ >= 0) close(epoll_fd_);
        for (int fd : rx_fds_) close(fd);
        for (int fd : tx_fds_) close(fd);
        for (int fd : dx_fds_) close(fd);
        if (listen_fd_ >= 0) close(listen_fd_);
    }

    void run() {
        epoll_event events[MAX_EVENTS];
        while (true) {
            int n = epoll_wait(epoll_fd_, events, MAX_EVENTS, -1);
            if (n < 0) { if (errno == EINTR) continue; perror("epoll_wait"); break; }
            for (int i = 0; i < n; ++i) {
                int fd = events[i].data.fd;
                uint32_t ev = events[i].events;
                if (fd == listen_fd_) {
                    accept_all();
                } else if (ev & (EPOLLIN | EPOLLERR | EPOLLHUP)) {
                    rx_fds_.push_back(fd);
                } else if (ev & EPOLLOUT) {
                    tx_fds_.push_back(fd);
                }
            }
            process_rx();
            process_tx();
            process_dx();
        }
    }

private:
    void add_epoll(int fd, uint32_t flags) {
        epoll_event e{};
        e.events = flags;
        e.data.fd = fd;
        epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, fd, &e);
    }

    void mod_epoll(int fd, uint32_t flags) {
        epoll_event e{};
        e.events = flags;
        e.data.fd = fd;
        epoll_ctl(epoll_fd_, EPOLL_CTL_MOD, fd, &e);
    }

    void accept_all() {
        for (;;) {
            int client = accept(listen_fd_, nullptr, nullptr);
            if (client < 0) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) break;
                perror("accept");
                break;
            }
            sockets::set_nonblocking(client, true);
            add_epoll(client, EPOLLIN | EPOLLET | EPOLLRDHUP);
            rx_fds_.push_back(client);
        }
    }

    void process_rx() {
        for (int fd : rx_fds_) {
            char buf[4096];
            ssize_t n = recv(fd, buf, sizeof(buf), 0);
            if (n <= 0) {
                dx_fds_.push_back(fd);
                continue;
            }
            // 业务：这里可入队待回复
            (void)buf;
        }
        rx_fds_.clear();
    }

    void process_tx() {
        for (int fd : tx_fds_) {
            // 若有待发数据则 send；发完后可 mod 去掉 EPOLLOUT
        }
        tx_fds_.clear();
    }

    void process_dx() {
        for (int fd : dx_fds_) {
            epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, fd, nullptr);
            close(fd);
        }
        dx_fds_.clear();
    }

    int listen_fd_;
    int epoll_fd_;
    std::vector<int> rx_fds_;
    std::vector<int> tx_fds_;
    std::list<int> dx_fds_;
};

#ifdef TCP_SERVER_MAIN
int main() {
    EpollTcpServer srv("0.0.0.0", 8888);
    srv.run();
    return 0;
}
#endif

#endif
