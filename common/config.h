#pragma once

#include <inttypes.h>
#include <string>

namespace config
{

	constexpr uint16_t server_port = 63100;

	constexpr uint16_t communication_port_tcp = 63101;

	constexpr uint16_t communication_port_udp_server = 63101;
	constexpr uint16_t communication_port_udp_client = 63102;

	constexpr size_t buffser_size = 64*1024;

	const std::string arg_tcp("tcp");
	const std::string arg_udp("udp");

	namespace server_cmd
	{
		static const std::string stop_udp_session("stop_udp");
	}

	namespace server_answer
	{
		static const std::string ok("ok");
	}

}
