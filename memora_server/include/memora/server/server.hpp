#pragma once

#include <boost/asio.hpp>

using boost::asio::ip::tcp;

namespace memora {

class Server {
public:
    Server(boost::asio::io_context& io_context, unsigned short port);

    void run();

    void do_accept();

private:
    tcp::acceptor acceptor_;
};

} // namespace memora

