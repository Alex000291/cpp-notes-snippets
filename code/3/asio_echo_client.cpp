// Boost.Asio 异步 TCP Echo 客户端
#include <boost/asio.hpp>
#include <cstdio>
#include <cstring>
#include <string>

using boost::asio::ip::tcp;

class Client {
public:
    Client(boost::asio::io_context& io,
           const std::string& host, uint16_t port)
        : socket_(io), resolver_(io)
    {
        auto endpoints = resolver_.resolve(host, std::to_string(port));
        do_connect(endpoints);
    }

private:
    void do_connect(const tcp::resolver::results_type& endpoints) {
        boost::asio::async_connect(socket_, endpoints,
            [this](boost::system::error_code ec, const tcp::endpoint& ep) {
                if (!ec) {
                    printf("connected to %s:%u\n",
                           ep.address().to_string().c_str(), ep.port());
                    do_read_stdin();
                } else {
                    printf("connect failed: %s\n", ec.message().c_str());
                }
            });
    }

    void do_read_stdin() {
        printf("> ");
        fflush(stdout);
        // 从 stdin 异步读取（用 posix stream descriptor）
        // 这里简化为同步读一行，然后异步发送
        if (!fgets(send_buf_, sizeof(send_buf_), stdin)) {
            printf("EOF, closing\n");
            socket_.close();
            return;
        }
        send_len_ = std::strlen(send_buf_);
        do_write();
    }

    void do_write() {
        boost::asio::async_write(socket_,
            boost::asio::buffer(send_buf_, send_len_),
            [this](boost::system::error_code ec, std::size_t) {
                if (!ec) {
                    do_read_reply();
                } else {
                    printf("write error: %s\n", ec.message().c_str());
                }
            });
    }

    void do_read_reply() {
        socket_.async_read_some(boost::asio::buffer(recv_buf_),
            [this](boost::system::error_code ec, std::size_t len) {
                if (!ec) {
                    printf("echo: %.*s", (int)len, recv_buf_);
                    do_read_stdin();
                } else {
                    printf("read error: %s\n", ec.message().c_str());
                }
            });
    }

    tcp::socket socket_;
    tcp::resolver resolver_;
    char send_buf_[4096];
    std::size_t send_len_ = 0;
    char recv_buf_[4096];
};

int main(int argc, char* argv[]) {
    const char* host = "127.0.0.1";
    uint16_t port = 9091;
    if (argc >= 2) host = argv[1];
    if (argc >= 3) port = static_cast<uint16_t>(std::atoi(argv[2]));

    printf("connecting to %s:%u ...\n", host, port);
    boost::asio::io_context io;
    Client client(io, host, port);
    io.run();
    return 0;
}
