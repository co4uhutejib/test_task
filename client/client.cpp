#include "../common/tcp_connection.h"
#include "../common/udp_listener.h"
#include "../common/udp_connection.h"

#include "../common/config.h"
#include "../common/logger.h"

#include <iostream>
#include <thread>


void usage();
void start_communication_tcp();
void start_communication_udp();


const char* server_url = "127.0.0.1";


int main(int argc, char** argv)
{
	if ((argc != 2)
		|| (config::arg_tcp != argv[1] && config::arg_udp != argv[1]))
	{
		usage();
		return 1;
	}
	const char* type = argv[1];

	// main connection
	Logger::log_debug() << "[clt] init connection: " << type << std::endl;
	net::TcpConnection main_connection;
	if (!main_connection.init_connection(server_url, config::server_port))
	{
		return 1;
	}

	if (!main_connection.write(type))
	{
		main_connection.close();
		return 1;
	}

	std::string msg;
	bool rc = main_connection.read(msg);
	if (!rc || msg != config::server_answer::ok)
	{
		Logger::log_error() << "[clt] bad server answer" << std::endl;
		return 1;
	}

	main_connection.close();

	if (config::arg_tcp == type) start_communication_tcp();
	else start_communication_udp();

	return 0;
}


// tcp
void start_communication_tcp()
{
	Logger::log_debug() << "[cmnt] start tcp communication" << std::endl;
	net::TcpConnection communication;

	if (!communication.init_connection(server_url, config::communication_port_tcp))
	{
		return;
	}

	while (true)
	{
		std::string input;
		std::getline(std::cin, input);
		if (std::cin.eof()) break;

		if (!communication.write(input))
		{
			Logger::log_error() << "[cmnt] can't send" << std::endl;
			return;
		}

		std::string echo_msg;
		if (!communication.read(echo_msg))
		{
			Logger::log_error() << "[cmnt] can't receive" << std::endl;
			return;
		}

		if (input == echo_msg) {
			Logger::log_debug() << "[cmnt] receive normal msg from server" << std::endl;
		} else {
			Logger::log_error() << "[cmnt] receive wrong msg from server. Input: \""
				<< input << "\" receive: \"" << echo_msg << "\"" << std::endl;
		}
	}
	Logger::log_debug() << "[cmnt] finish tcp communication" << std::endl;
}

// udp
void write_func(net::UdpListener& reader, net::UdpConnection& writer)
{
	while (true)
	{
		std::string input;
		std::getline(std::cin, input);
		if (std::cin.eof()) break;

		if (!writer.write(input)) break;
	}

	writer.close();
	reader.close();
}

void read_func(net::UdpListener& reader, net::UdpConnection& writer)
{
	while (true)
	{
		std::string msg;
		if (!reader.read(msg)) break;
		Logger::log_debug() << "[cmnt] receive msg: " << msg << std::endl;
		// ignore server answer
	}

	writer.close();
	reader.close();
}

void start_communication_udp()
{
	Logger::log_debug() << "[cmnt] start udp communication" << std::endl;

	net::UdpListener reader;
	net::UdpConnection writer;

	if (!reader.init_connection(config::communication_port_udp_client)
		|| !writer.init_connection(server_url, config::communication_port_udp_server))
	{
		return;
	}

	std::thread read_thread(read_func, std::ref(reader), std::ref(writer));
	write_func(reader, writer);
	read_thread.join();

	Logger::log_debug() << "[cmnt] finish udp communication" << std::endl;

	Logger::log_debug() << "[cmnt] send finish to server" << std::endl;
	net::TcpConnection finish_connection;
	if (!finish_connection.init_connection(server_url, config::server_port))
	{
		return;
	}

	finish_connection.write(config::server_cmd::stop_udp_session);
}

void usage()
{
	Logger::log_info() << "Usage:" << std::endl;
	Logger::log_info() << "\tclient connection_type" << std::endl;
	Logger::log_info() << "\tWhere connection_type is \"tcp\" or \"udp\"." << std::endl;
}
