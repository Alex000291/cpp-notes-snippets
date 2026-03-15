// 简易 Linux TCP Echo 服务器
// 单线程，accept → read → write 回显 → close
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdio>
#include <cstring>
int main() {

    constexpr uint16_t PORT = 9090;
    constexpr int BACKLOG = 8;
    char buf[4096];

    // 1. 创建 socket
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd < 0) { perror("socket"); return 1; }

    int opt = 1;
    setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // 2. 绑定地址
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(PORT);
    if (bind(listen_fd, (sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind"); close(listen_fd); return 1;
    }

    // 3. 监听
    if (listen(listen_fd, BACKLOG) < 0) {
        perror("listen"); close(listen_fd); return 1;
    }
    printf("listening on 0.0.0.0:%u ...\n", PORT);

    // 4. 主循环：accept → echo → close
    while (true) {
        sockaddr_in cli{};
        socklen_t cli_len = sizeof(cli);
        int conn_fd = accept(listen_fd, (sockaddr*)&cli, &cli_len);
        if (conn_fd < 0) {
            perror("accept");
            continue;
        }
        printf("client connected: %s:%u\n",
               inet_ntoa(cli.sin_addr), ntohs(cli.sin_port));

        // 回显循环
        ssize_t n;
        while ((n = read(conn_fd, buf, sizeof(buf))) > 0) {
            ssize_t sent = 0;
            while (sent < n) {
                ssize_t w = write(conn_fd, buf + sent, n - sent);
                if (w <= 0) break;
                sent += w;
            }
            if (sent < n) break;
        }
        printf("client disconnected\n");
        close(conn_fd);
    }

    close(listen_fd);
    printf("server stopped\n");
    return 0;
}
