#include <memora/server/server.hpp>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <cerrno>
#include <cassert>

#include <iostream>

void print_client_host(int clientfd)
{
	char ip[INET_ADDRSTRLEN];

	sockaddr_in client_addr{};	
	socklen_t len = sizeof(client_addr);
	if (const int rv = getpeername(clientfd, reinterpret_cast<sockaddr *>(&client_addr), &len); rv < 0)
	{
		std::cerr << "getpeername failed: " << strerror(errno) << std::endl;
	}

	inet_ntop(AF_INET, &client_addr.sin_addr, ip, sizeof(ip));
	const int port = ntohs(client_addr.sin_port);
	std::cout << "Client host: " << ip << ":" << port << std::endl;
}


void do_something(int clientfd)
{
	char buf[1024];
	char rbuf[64] = {};
	ssize_t n = read(clientfd, rbuf, sizeof(rbuf) - 1);
	if (n < 0) {
		std::cerr << "read failed: " << strerror(errno) << std::endl;
		return;
	}
	printf("client says: %s\n", rbuf);
	char wbuf[] = "world";
	write(clientfd, wbuf, strlen(wbuf));
}

int32_t read_full(int fd, char *buf, size_t n) {
	while (n > 0)
	{
		ssize_t rv = read(fd, buf, n);
		if (rv <= 0) {
			return -1; // error, or unexpected EOF
		}
		assert((size_t)rv <= n);
		n -= (size_t)rv;
		buf += rv;
	}
	return 0;
}

int32_t write_all(int fd, const char *buf, size_t n) {
	while (n > 0)
	{
		ssize_t rv = write(fd, buf, n);
		if (rv <= 0) {
			return -1; // error
		}
		assert((size_t)rv <= n);
		n -= (size_t)rv;
		buf += rv;
	}
	return 0;
}

constexpr size_t k_max_msg = 4096;

int32_t one_request(int connfd)
{
	// 4 bytes header
	char rbuf[4 + k_max_msg];
	errno = 0;
	int32_t err = read_full(connfd, rbuf, 4);
	if (err) {
		printf(errno == 0 ? "EOF" : "read() error");
		return err;
	}
	uint32_t len = 0;
	memcpy(&len, rbuf, 4); // assume little endian
	if (len > k_max_msg) {
		printf("too long");
		return -1;
	}
	// request body
	err = read_full(connfd, &rbuf[4], len);
	if (err) {
		printf("read() error");
		return err;
	}

	// do something
	printf("client says: %.*s\n", len, &rbuf[4]);

	// reply using the same protocol
	constexpr char reply[] = "world";
	char wbuf[4 + sizeof(reply)];
	len = static_cast<uint32_t>(strlen(reply));
	memcpy(wbuf, &len, 4);
	memcpy(&wbuf[4], reply, len);
	return write_all(connfd, wbuf, 4 + len);
}


[[noreturn]] auto main() -> int
{
	memora::Server server;
	server.run();

	// AF_INET for ipv4, SOCK_STREAM for TCP
	const int fd = socket(AF_INET, SOCK_STREAM, 0);

	constexpr int yes{1};
	setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

	sockaddr_in addr{};
	addr.sin_family = AF_INET;
	addr.sin_port = htons(8080);
	addr.sin_addr.s_addr = htonl(0);
	if(const int rv = bind(fd, reinterpret_cast<const sockaddr*>(&addr), sizeof(addr)); rv < 0)
	{
		std::cerr << "bind failed: " << strerror(errno) << std::endl;
	}

	if(const int rv = listen(fd, SOMAXCONN); rv < 0)
	{
		std::cerr << "listen failed: " << strerror(errno) << std::endl;
	}

	sockaddr_in local{};
	socklen_t len = sizeof(local);

	if(getsockname(fd, reinterpret_cast<sockaddr *>(&local), &len) == -1) {
		perror("getsockname");
	}

	char ip[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &local.sin_addr, ip, sizeof(ip));

	int port = ntohs(local.sin_port);

	printf("Local IP: %s, port: %d\n", ip, port);

	while(true)
	{
		sockaddr_in client_addr = {};
		socklen_t addrlen = sizeof(client_addr);
		const int clientfd = accept(fd, reinterpret_cast<sockaddr*>(&client_addr), &addrlen);
		if(clientfd < 0)
		{
			std::cerr << "accept failed: " << strerror(errno) << std::endl;
			continue;
		}
		print_client_host(clientfd);
		while(true)
		{
			if (const int err = one_request(clientfd))
			{
				break;
			}
		}
		close(clientfd);
	}
}
