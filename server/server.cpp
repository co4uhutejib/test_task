#include "../common/tcp_listener.h"
#include "../common/udp_listener.h"
#include "../common/udp_connection.h"
#include "udp_session.h"

#include "../common/config.h"
#include "../common/logger.h"

#include <deque>
#include <thread>
#include <mutex>
#include <functional>
#include <utility>


void release_main_connection(net::TcpListener& main_listener);
void tcp_communication(net::TcpListener& main_listener);
void udp_communication(net::TcpListener& main_listener);
void stop_udp_communication();

void parse_line(const std::string&);

static std::unique_ptr<UdpSession> last_udp_session;

int main()
{
	net::TcpListener main_listener;

	if (!main_listener.init_connection(config::server_port))
	{
		Logger::log_error() << "[srv] Fail init main connection" << std::endl;
		return 1;
	}

	while (main_listener.is_valid())
	{
		Logger::log_debug() << "[srv] start new session/wait cmd" << std::endl;
		if (!main_listener.accept_connection())
		{
			Logger::log_error() << "[srv] Fail accept main connection" << std::endl;
			return 1;
		}

		std::string msg;
		if (!main_listener.read(msg))
		{
			main_listener.close_connection();
			continue;
		}

		if (msg == config::arg_tcp)
		{
			stop_udp_communication();
			tcp_communication(main_listener);
		}
		else if (msg == config::arg_udp)
		{
			stop_udp_communication();
			udp_communication(main_listener);
		}
		else if (msg == config::server_cmd::stop_udp_session)
		{
			stop_udp_communication();
			main_listener.close_connection();
		}
		else
		{
			Logger::log_error() << "[srv] unknown communication type: " << msg << std::endl;
			main_listener.close_connection();
		}
	}

	main_listener.close();

	Logger::log_debug() << "[srv] finish" << std::endl;

	return 0;
}


void release_main_connection(net::TcpListener& main_listener)
{
	main_listener.write(config::server_answer::ok);
	main_listener.close_connection();
}

// tcp
void tcp_communication(net::TcpListener& main_listener)
{
	Logger::log_debug() << "[cmnt] start tcp communication" << std::endl;

	// comunication
	net::TcpListener communication;

	if (!communication.init_connection(config::communication_port_tcp))
	{
		main_listener.close_connection();
		return;
	}

	release_main_connection(main_listener);

	if (!communication.accept_connection())
	{
		return;
	}

	while (communication.is_valid())
	{
		Logger::log_debug() << "[cmnt] next echo iter" << std::endl;

		std::string msg;
		if (!communication.read(msg)) break;
		Logger::log_debug() << "[cmnt] receive msg: " << msg << std::endl;
		parse_line(msg);
		if (!communication.write(msg)) break;
	}

	Logger::log_debug() << "[cmnt] finish tcp communication" << std::endl;
}

// udp
void udp_communication(net::TcpListener& main_listener)
{
	Logger::log_debug() << "[cmnt] start udp communication" << std::endl;

	net::UdpListener reader;
	net::UdpConnection writer;

	std::string remote = main_listener.get_remote_url();
	if (!reader.init_connection(config::communication_port_udp_server)
		|| !writer.init_connection(remote.c_str(),config::communication_port_udp_client))
	{
		main_listener.close_connection();
		return;
	}

	release_main_connection(main_listener);

	// todo remove
	for (int test_i = 0; test_i < 0; ++test_i)
	{
		std::string test_msg;
		reader.read(test_msg);
		Logger::log_debug() << "test test!!! " << test_msg << std::endl;
	}
	//

	last_udp_session.reset( new UdpSession(std::move(reader), std::move(writer)) );
	last_udp_session->start();

	Logger::log_debug() << "[cmnt] move to background udp communication" << std::endl;
}

void stop_udp_communication()
{
	if (!last_udp_session) return;

	last_udp_session->stop();
	last_udp_session.reset();
}
