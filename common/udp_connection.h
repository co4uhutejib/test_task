#pragma once

#include <inttypes.h>
#include <string>
#include <netinet/in.h>


namespace net
{

	class UdpConnection
	{
	public:
		UdpConnection();
		UdpConnection(UdpConnection&& helper);
		UdpConnection(const UdpConnection&) = delete;
		UdpConnection& operator=(const UdpConnection&) = delete;
		~UdpConnection();
		void close();

		bool is_valid() const;

		bool init_connection(const char* url, uint16_t port);

		bool write(const std::string& msg);

	protected:
		int _socket_fd;
		struct sockaddr_in _sa;
	};

}
