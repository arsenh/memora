#pragma once

#include <boost/asio.hpp>
#include <array>

using namespace boost::asio::ip;

namespace memora
{
    class Session : public std::enable_shared_from_this<Session>
    {
        static constexpr std::size_t MAX_PAYLOAD_LENGTH = 1024;
    public:
        explicit Session(tcp::socket socket);

        void start();
        void stop();

    private:
        void do_read();
        void do_write(std::size_t length);

    private:
        tcp::socket socket_;
        std::array<char, MAX_PAYLOAD_LENGTH> buffer_;
    };
}