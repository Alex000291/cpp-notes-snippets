// Boost.Asio 异步 TCP Echo 服务器
#include <boost/asio.hpp>
#include <cstdio>
#include <memory>

using boost::asio::ip::tcp;

class Session : public std::enable_shared_from_this<Session> {
public:
    explicit Session(tcp::socket socket) : socket_(std::move(socket)) {}
    void start() { do_read(); }

private:
    void do_read() {
        auto self = shared_from_this();
        socket_.async_read_some(boost::asio::buffer(buf_),
            [this, self](boost::system::error_code ec, std::size_t len) {
                if (!ec) {
                    printf("recv %zu bytes\n", len);
                    do_write(len);
                }
            });
    }

    void do_write(std::size_t len) {
        auto self = shared_from_this();
        boost::asio::async_write(socket_, boost::asio::buffer(buf_, len),
            [this, self](boost::system::error_code ec, std::size_t) {
                if (!ec) do_read();
            });
    }

    tcp::socket socket_;
    char buf_[4096];
};

class Server {
public:
    Server(boost::asio::io_context& io, uint16_t port)
        : acceptor_(io, tcp::endpoint(tcp::v4(), port)) {
        printf("listening on 0.0.0.0:%u ...\n", port);
        do_accept();
    }

private:
    void do_accept() {
        acceptor_.async_accept([this](boost::system::error_code ec, tcp::socket sock) {
            if (!ec) {
                printf("client connected\n");
                std::make_shared<Session>(std::move(sock))->start();
            }
            do_accept();
        });
    }

    tcp::acceptor acceptor_;
};

int main() {
    boost::asio::io_context io;
    Server server(io, 9091);
    io.run();
    return 0;
}
