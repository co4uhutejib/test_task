#pragma once

#include <inttypes.h>
#include <string>


namespace net
{

	class TcpConnection
	{
	public:
		TcpConnection();
		TcpConnection(TcpConnection&& helper);
		TcpConnection(const TcpConnection&) = delete;
		TcpConnection& operator=(const TcpConnection&) = delete;
		~TcpConnection();
		void close();

		bool is_valid() const;

		bool init_connection(const char* url, uint16_t port);

		bool read(std::string& msg);
		bool write(const std::string& msg);

	protected:
		int _connect_fd;
		bool _connected;
	};

}
