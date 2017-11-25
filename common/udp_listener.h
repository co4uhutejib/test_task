#pragma once

#include <inttypes.h>
#include <string>
#include <netinet/in.h>


namespace net
{

	class UdpListener
	{
	public:
		UdpListener();
		UdpListener(UdpListener&& helper);
		UdpListener(const UdpListener&) = delete;
		UdpListener& operator=(const UdpListener&) = delete;
		~UdpListener();
		void close();

		bool is_valid() const;

		bool init_connection(uint16_t port);

		bool read(std::string& msg);

	protected:
		int _socket_fd;
		struct sockaddr_in _sa;
	};

}
