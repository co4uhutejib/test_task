#pragma once

#include <ostream>


class Logger
{
public:
	static std::ostream& log_info();
	static std::ostream& log_debug();
	static std::ostream& log_error();
};
