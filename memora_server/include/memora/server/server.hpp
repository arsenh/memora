#pragma once

#include "memora/server/session.hpp"
#include <boost/asio.hpp>
#include <set>


using boost::asio::ip::tcp;

namespace memora
{
    class Server : public std::enable_shared_from_this<Server>
    {
    public:
        Server(boost::asio::io_context& io_context, unsigned short port);

        void run();

        void do_accept();

    private:
        void setup_signal_handling();
        void stop();

    private:
        boost::asio::io_context& io_context_;
        tcp::acceptor acceptor_;
        boost::asio::signal_set signals_;
        std::set<std::shared_ptr<Session>> sessions_;
    };
}

