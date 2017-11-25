#include "logger.h"

#include <iostream>


std::ostream& Logger::log_info()
{
	return std::cout;
}

std::ostream& Logger::log_debug()
{
	static std::ostream bit_bucket(0);
	return bit_bucket;
	//return std::cout;
}

std::ostream& Logger::log_error()
{
	return std::cerr;
}
