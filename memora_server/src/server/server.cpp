#include <memora/server/server.hpp>
#include <print>
#include <memory>

#include "memora/server/session.hpp"

namespace memora
{
    Server::Server(boost::asio::io_context& io_context, const unsigned short port)
        :   io_context_(io_context),
            acceptor_(io_context, tcp::endpoint(tcp::v4(), port)),
            signals_(io_context, SIGINT, SIGTERM),
            sessions_{}
    {
    }

    void Server::run()
    {
        std::println("Memora server started on port: {}", acceptor_.local_endpoint().port());
        setup_signal_handling();
        do_accept();
    }

    void Server::do_accept()
    {
        auto self = shared_from_this();
        acceptor_.async_accept([self](boost::system::error_code ec, tcp::socket socket)
        {
            if (!ec) {
                std::println("New connection from: {}", socket.remote_endpoint().address().to_string());
                std::make_shared<Session>(std::move(socket))->start();
            }
            self->do_accept();
        });
    }

    void Server::setup_signal_handling()
    {
        auto self = shared_from_this();
        signals_.async_wait([self](boost::system::error_code ec, int signal)
        {
            if (! ec)
            {
                std::println("Signal {} received, shutting down...", signal);
                self->stop();
            }
        });
    }

    void Server::stop()
    {
        std::println("Shutting down...");
        boost::system::error_code ec;
        if (acceptor_.close(ec)) std::println("Error closing acceptor: {}", ec.message());
        for (auto& session : sessions_) {
            session->stop();
        }
        sessions_.clear();
        io_context_.stop();
        std::println("Server stopped.");
    }
} // namespace memora
