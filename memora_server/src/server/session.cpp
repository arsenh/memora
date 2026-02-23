#include <memora/server/session.hpp>

namespace memora
{
    Session::Session(tcp::socket socket)
    : socket_(std::move(socket)), buffer_{}
    {

    }

    void Session::start()
    {
        do_read();
    }

    void Session::do_read()
    {
        auto self = shared_from_this();
        socket_.async_read_some(boost::asio::buffer(buffer_),
            [self](const boost::system::error_code& ec, std::size_t length)
            {
                if (!ec)
                {
                    std::println("Read {} bytes", length);
                    self->do_write(length);
                }
                else
                {
                    std::println("Read error: {}", ec.message());
                    self->stop();
                }
            });
    }

    void Session::do_write(const std::size_t length)
    {
        auto self = shared_from_this();

        std::transform(buffer_.begin(), buffer_.begin() + length, buffer_.begin(), [](const char& c) { return std::toupper(c); });

        boost::asio::async_write(
            socket_,
            boost::asio::buffer(buffer_, length),
            [self](const boost::system::error_code& ec, std::size_t /*length*/) {
                if (!ec)
                {
                    self->do_read();
                }
                else
                {
                    std::println("Write error: {}", ec.message());
                    self->stop();
                }
            });
    }

    void Session::stop()
    {
        std::println("Closing socket...");
        boost::system::error_code ec;
        socket_.shutdown(tcp::socket::shutdown_both, ec);
        if (ec && ec != boost::asio::error::not_connected) {
            std::println("Error shutting down socket: {}", ec.message());
        }
        if (socket_.close(ec)) std::println("Error closing socket: {}", ec.message());
    }
}