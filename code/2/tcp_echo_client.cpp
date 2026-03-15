// 简易 TCP Echo 客户端
// 连接服务器，发送用户输入，打印服务器回显
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>

int main(int argc, char* argv[]) {
    const char* host = (argc > 1) ? argv[1] : "127.0.0.1";
    uint16_t port = (argc > 2) ? static_cast<uint16_t>(atoi(argv[2])) : 9090;

    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd < 0) { perror("socket"); return 1; }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if (inet_pton(AF_INET, host, &addr.sin_addr) <= 0) {
        fprintf(stderr, "invalid address: %s\n", host);
        close(fd);
        return 1;
    }

    if (connect(fd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("connect");
        close(fd);
        return 1;
    }
    printf("connected to %s:%u\n", host, port);

    char line[4096];
    char buf[4096];
    while (fgets(line, sizeof(line), stdin)) {
        size_t len = strlen(line);
        if (write(fd, line, len) < 0) { perror("write"); break; }

        ssize_t n = read(fd, buf, sizeof(buf) - 1);
        if (n <= 0) { printf("server closed\n"); break; }
        buf[n] = '\0';
        printf("echo: %s", buf);
    }

    close(fd);
    return 0;
}
