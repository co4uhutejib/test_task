#include "udp_listener.h"

#include "../common/config.h"
#include "../common/logger.h"

#include <errno.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>


namespace net
{

constexpr int empty_handle = -1;


UdpListener::UdpListener() :
	_socket_fd(empty_handle)
{
}

UdpListener::UdpListener(UdpListener&& helper) :
	_socket_fd(helper._socket_fd), _sa(helper._sa)
{
	helper._socket_fd = empty_handle;
	memset(&helper._sa, 0, sizeof(helper._sa));
}

UdpListener::~UdpListener()
{
	close();
}

void UdpListener::close()
{
	if (_socket_fd >= 0)
	{
		shutdown(_socket_fd, SHUT_RDWR);
		::close(_socket_fd);
		_socket_fd = empty_handle;
	}
}

bool UdpListener::is_valid() const
{
	return _socket_fd >= 0;
}

bool UdpListener::init_connection(uint16_t port)
{
	assert(!is_valid());

	memset(&_sa, 0, sizeof(_sa));
	_sa.sin_family = AF_INET;
	_sa.sin_addr.s_addr = htonl(INADDR_ANY);
	_sa.sin_port = htons(port);

	_socket_fd = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if (_socket_fd == empty_handle)
	{
		Logger::log_error() << "[udp] cannot create socket" << std::endl;
		return false;
	}

	if (bind(_socket_fd, (struct sockaddr*)&_sa, sizeof(_sa)) == -1) {
		Logger::log_error() << "[udp] error bind failed" << std::endl;
		::close(_socket_fd);
		return false;
	}

	return true;
}

bool UdpListener::read(std::string& msg)
{
	char buf[config::buffser_size];
	socklen_t sa_len = sizeof(_sa);
	ssize_t recsize = recvfrom(_socket_fd, buf, sizeof(buf), 0, (struct sockaddr*)&_sa, &sa_len);

	if (recsize < 0) {
		Logger::log_error() << "[udp] error: " << strerror(errno) << std::endl;
		return false;
	} else if (recsize == 0) {
		return false;
	}

	msg = std::string(buf);
	return true;
}

}
