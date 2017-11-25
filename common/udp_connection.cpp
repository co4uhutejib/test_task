#include "udp_connection.h"

#include "../common/config.h"
#include "../common/logger.h"

#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <assert.h>


namespace net
{

constexpr int empty_handle = -1;


UdpConnection::UdpConnection() :
	_socket_fd(empty_handle)
{
}

UdpConnection::UdpConnection(UdpConnection&& helper) :
	_socket_fd(helper._socket_fd), _sa(helper._sa)
{
	helper._socket_fd = empty_handle;
	memset(&helper._sa, 0, sizeof(helper._sa));
}

UdpConnection::~UdpConnection()
{
	close();
}

void UdpConnection::close()
{
	if (_socket_fd >= 0)
	{
		shutdown(_socket_fd, SHUT_RDWR);
		::close(_socket_fd);
		_socket_fd = empty_handle;
	}
}

bool UdpConnection::is_valid() const
{
	return _socket_fd >= 0;
}


bool UdpConnection::init_connection(const char* url, uint16_t port)
{
	assert(!is_valid());

	memset(&_sa, 0, sizeof(_sa));
	_sa.sin_family = AF_INET;
	_sa.sin_addr.s_addr = inet_addr(url);
	_sa.sin_port = htons(port);

	_socket_fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (_socket_fd == empty_handle)
	{
		Logger::log_error() << "[udp] can't create socket: " << strerror(errno) << std::endl;
		return false;
	}

	return true;
}

bool UdpConnection::write(const std::string& msg)
{
	socklen_t fromlen = sizeof(_sa);
	ssize_t send_size = msg.length()+1;
	ssize_t bytes_sent = sendto(_socket_fd, msg.c_str(), send_size, 0, (struct sockaddr*)&_sa, fromlen);

	if (bytes_sent < 0)
	{
		Logger::log_error() << "[udp] can't send data: " << strerror(errno) << std::endl;
		return false;
	}

	return true;
}

}
