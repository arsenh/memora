#ifndef MEMORA_SERVER_SESSION_HPP
#define MEMORA_SERVER_SESSION_HPP

#include <boost/asio.hpp>
#include <array>

using namespace boost::asio::ip;

class Session : public std::enable_shared_from_this<Session>
{
public:
    explicit Session(tcp::socket socket);

    void start();

private:
    void do_read();
    void do_write(std::size_t length);

private:
    tcp::socket socket_;
    std::array<char, 1024> buffer_;
};


#endif //MEMORA_SERVER_SESSION_HPP