#include "../common/logger.h"

#include <vector>
#include <algorithm>
#include <regex>
#include <vector>
#include <string>


void parse_line(const std::string& line)
{
	std::vector<int> numbers;
	int min = -1;
	int max = -1;
	int sum = 0;

	std::regex digit_regex("\\b(\\d)\\b");
	auto digit_begin = std::sregex_iterator(line.begin(), line.end(), digit_regex);
	auto digit_end = std::sregex_iterator();

	for (auto it = digit_begin; it != digit_end; ++it)
	{
		std::smatch match = *it;
		std::string match_str = match.str();

		int value = std::stoi(match_str);

		if (min >= 0)
		{
			if (min > value) min = value;
			if (max < value) max = value;
		}
		else
		{
			min = max = value;
		}
		numbers.push_back(value);
		sum += value;
	}


	if (min < 0)
	{
		Logger::log_info() << "Not found numbers in [0; 9]" << std::endl;
		return;
	}

	Logger::log_info() << "Sum: " << sum << std::endl;

	std::sort(numbers.begin(), numbers.end(), std::greater<int>());
	Logger::log_info() << "Numbers:" << std::endl;
	auto it = numbers.begin();
	Logger::log_info() << *it++;
	for (; it != numbers.end(); ++it)
	{
		Logger::log_info() << ' ' <<  *it;
	}
	Logger::log_info() << std::endl;

	Logger::log_info() << "Max: " << max << " Min: " << min << std::endl;
}

