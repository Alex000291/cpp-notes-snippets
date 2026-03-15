// Socket 性能优化集合 — TCP_NODELAY / O_NONBLOCK / SO_TIMESTAMP；组播 TTL 与订阅
#ifndef SOCKETS_H
#define SOCKETS_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <unistd.h>
#include <cerrno>
#include <cstring>

namespace sockets {

inline int set_tcp_nodelay(int fd, bool on = true) {
    int v = on ? 1 : 0;
    return setsockopt(fd, IPPROTO_TCP, TCP_NODELAY, &v, sizeof(v));
}

inline int set_nonblocking(int fd, bool on = true) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0) return -1;
    if (on) flags |= O_NONBLOCK;
    else    flags &= ~O_NONBLOCK;
    return fcntl(fd, F_SETFL, flags);
}

inline int set_so_timestamp(int fd, bool on = true) {
    int v = on ? 1 : 0;
    return setsockopt(fd, SOL_SOCKET, SO_TIMESTAMP, &v, sizeof(v));
}

// 组播 TTL
inline int set_multicast_ttl(int fd, int ttl) {
    return setsockopt(fd, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl));
}

// 加入组播组
inline int join_multicast_group(int fd, const char* group_ip, const char* local_if = nullptr) {
    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = inet_addr(group_ip);
    mreq.imr_interface.s_addr = local_if ? inet_addr(local_if) : INADDR_ANY;
    return setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));
}

inline int leave_multicast_group(int fd, const char* group_ip, const char* local_if = nullptr) {
    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = inet_addr(group_ip);
    mreq.imr_interface.s_addr = local_if ? inet_addr(local_if) : INADDR_ANY;
    return setsockopt(fd, IPPROTO_IP, IP_DROP_MEMBERSHIP, &mreq, sizeof(mreq));
}

} // namespace sockets

#ifdef SOCKETS_MAIN
#include <cstdio>
int main() {
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) { perror("socket"); return 1; }
    if (sockets::set_tcp_nodelay(fd) < 0) perror("TCP_NODELAY");
    if (sockets::set_nonblocking(fd) < 0) perror("O_NONBLOCK");
    if (sockets::set_so_timestamp(fd) < 0) perror("SO_TIMESTAMP");
    close(fd);
    printf("socket options demo done\n");
    return 0;
}
#endif

#endif
