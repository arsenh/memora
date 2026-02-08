#include <memora/server/session.hpp>

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
                // TODO: close client socket
            }

        });
}

void Session::do_write(const std::size_t length)
{
    auto self = shared_from_this();
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
                // TODO: close client socket
            }
        });
}
