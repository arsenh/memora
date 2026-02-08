#include <memora/server/server.hpp>
#include <print>

namespace memora
{
    Server::Server(boost::asio::io_context& io_context, const unsigned short port)
        : acceptor_(io_context, tcp::endpoint(tcp::v4(), port))
    {
    }

    void Server::run()
    {
        std::println("Memora server started on port: {}", acceptor_.local_endpoint().port());
        do_accept();
    }

    void Server::do_accept()
    {
        acceptor_.async_accept([this](boost::system::error_code ec, tcp::socket socket)
        {
            if (!ec) {
                std::println("New connection from: {}", socket.remote_endpoint().address().to_string());
            }
            do_accept();
        });
    }
} // namespace memora
