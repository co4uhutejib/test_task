#pragma once

#include <inttypes.h>
#include <string>


namespace net
{

	class TcpListener
	{
	public:
		TcpListener();
		TcpListener(TcpListener&& helper);
		TcpListener(const TcpListener&) = delete;
		TcpListener& operator=(const TcpListener&) = delete;
		~TcpListener();
		void close();

		bool is_valid() const;

		bool init_connection(uint16_t port);

		bool accept_connection();
		void close_connection();

		bool read(std::string& msg);
		bool write(const std::string& msg);

		std::string get_remote_url();

	protected:
		int _socket_fd;
		int _connect_fd;
	};

}
