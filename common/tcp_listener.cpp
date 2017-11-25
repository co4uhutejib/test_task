#include "tcp_listener.h"

#include "../common/config.h"
#include "../common/logger.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#include <memory>


namespace net
{

constexpr int empty_handle = -1;


TcpListener::TcpListener() :
	_socket_fd(empty_handle), _connect_fd(empty_handle)
{
}

TcpListener::TcpListener(TcpListener&& helper) :
	_socket_fd(helper._socket_fd), _connect_fd(helper._connect_fd)
{
	helper._socket_fd = empty_handle;
	helper._connect_fd = empty_handle;
}

TcpListener::~TcpListener()
{
	close();
}

void TcpListener::close()
{
	if (_connect_fd >= 0)
	{
		close_connection();
	}
	if (_socket_fd >= 0)
	{
		::close(_socket_fd);
		_socket_fd = empty_handle;
	}
}

bool TcpListener::is_valid() const
{
	return _socket_fd >= 0;
}

bool TcpListener::init_connection(uint16_t port)
{
	assert(!is_valid());

	_socket_fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (_socket_fd == empty_handle)
	{
		Logger::log_error() << "[tcp] cannot create socket" << std::endl;
		return false;
	}

	struct sockaddr_in sa;
	memset(&sa, 0, sizeof(sa));

	sa.sin_family = AF_INET;
	sa.sin_port = htons(port);
	sa.sin_addr.s_addr = htonl(INADDR_ANY);

	int opt_val = 1;
	setsockopt(_socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt_val, sizeof(opt_val));

	if (bind(_socket_fd, (struct sockaddr*)&sa, sizeof(sa)) == -1)
	{
		Logger::log_error() << "[tcp] bind failed " << strerror(errno) << std::endl;
		close();
		return false;
	}

	if (listen(_socket_fd, 10) == -1)
	{
		Logger::log_error() << "[tcp] listen failed " << strerror(errno) << std::endl;
		close();
		return false;
	}

	return true;
}

bool TcpListener::accept_connection()
{
	assert(is_valid());
	assert(_connect_fd == empty_handle);

//	struct sockaddr_in client_addr;
//	memset(&client_addr, 0, sizeof(client_addr));
//	socklen_t client_addr_len = sizeof(client_addr);
//	_connect_fd = accept(_socket_fd, (struct sockaddr*)&client_addr, &client_addr_len);
	_connect_fd = accept(_socket_fd, NULL, NULL);

	if (0 > _connect_fd)
	{
		Logger::log_error() << "[tcp] accept failed " << strerror(errno) << std::endl;
		return false;
	}

	Logger::log_debug() << "[tcp] connection accpeted" << std::endl;

	return true;
}

void TcpListener::close_connection()
{
	assert(is_valid());
	assert(_connect_fd >= 0);

	Logger::log_debug() << "[tcp] close connection" << std::endl;

	int rc = shutdown(_connect_fd, SHUT_RDWR);
	::close(_connect_fd);
	_connect_fd = empty_handle;

	if (rc == -1)
	{
		close();
	}
}

bool TcpListener::read(std::string& msg)
{
	char buf[config::buffser_size];
	ssize_t recv_size = recv(_connect_fd, buf, sizeof(buf), 0);
	if (recv_size < 0) {
		Logger::log_error() << "[tcp] receive failed" << std::endl;
		return false;
	} else if (recv_size == 0) {
		Logger::log_debug() << "[tcp] connection closed" << std::endl;
		return false;
	}

	msg = std::string(buf);
	return true;
}

bool TcpListener::write(const std::string& msg)
{
	ssize_t send_size = msg.length()+1;
	if (send(_connect_fd, msg.c_str(), send_size, 0) != send_size)
	{
		Logger::log_error() << "[tcp] send failed " << strerror(errno) << std::endl;
		return false;
	}

	return true;
}

std::string TcpListener::get_remote_url()
{
	assert(is_valid());
	assert(_connect_fd >= 0);

	socklen_t len;
	struct sockaddr_storage addr;
	char ipstr[std::max(INET6_ADDRSTRLEN,INET_ADDRSTRLEN)];
//	int port;

	len = sizeof(addr);
	getpeername(_connect_fd, (struct sockaddr*)&addr, &len);

	// deal with both IPv4 and IPv6:
	if (addr.ss_family == AF_INET) {
		struct sockaddr_in* s = (struct sockaddr_in*)&addr;
//		port = ntohs(s->sin_port);
		inet_ntop(AF_INET, &s->sin_addr, ipstr, sizeof(ipstr));
	} else { // AF_INET6
		struct sockaddr_in6* s = (struct sockaddr_in6*)&addr;
//		port = ntohs(s->sin6_port);
		inet_ntop(AF_INET6, &s->sin6_addr, ipstr, sizeof(ipstr));
	}

	return std::string(ipstr);
}

}
