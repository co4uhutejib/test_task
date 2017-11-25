#include "tcp_connection.h"

#include "../common/config.h"
#include "../common/logger.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


namespace net
{

constexpr int empty_handle = -1;

TcpConnection::TcpConnection() :
	_connect_fd(empty_handle), _connected(false)
{
}

TcpConnection::TcpConnection(TcpConnection&& helper) :
	_connect_fd(helper._connect_fd), _connected(helper._connected)
{
	helper._connect_fd = empty_handle;
	helper._connected = false;
}

TcpConnection::~TcpConnection()
{
	close();
}

void TcpConnection::close()
{
	if (_connect_fd >= 0)
	{
		if (_connected)
		{
			shutdown(_connect_fd, SHUT_RDWR);
			_connected = false;
		}
		::close(_connect_fd);
		_connect_fd = empty_handle;
	}
}

bool TcpConnection::is_valid() const
{
	return _connect_fd >= 0 && _connected;
}

bool TcpConnection::init_connection(const char* url, uint16_t port)
{
	_connect_fd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (_connect_fd == empty_handle)
	{
		Logger::log_error() << "[tcp] cannot create socket: " << strerror(errno) << std::endl;
		return false;
	}

	struct sockaddr_in sa;
	memset(&sa, 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_port = htons(port);


	int rc = inet_pton(AF_INET, url, &sa.sin_addr);
	if (rc != 1)
	{
		Logger::log_error() << "[tcp] cant resolve url" << std::endl;
		close();
		return false;
	}

	if (connect(_connect_fd, (struct sockaddr*)&sa, sizeof(sa)) == -1) {
		Logger::log_error() << "[tcp] connect failed" << std::endl;
		close();
		return false;
	}

	return true;
}

bool TcpConnection::read(std::string& msg)
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

bool TcpConnection::write(const std::string& msg)
{
	ssize_t send_size = msg.length()+1;
	if (send(_connect_fd, msg.c_str(), send_size, 0) != send_size)
	{
		Logger::log_error() << "[tcp] send failed " << strerror(errno) << std::endl;
		return false;
	}

	return true;
}

}
