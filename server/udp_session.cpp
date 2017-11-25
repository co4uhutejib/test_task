#include "udp_session.h"

#include "../common/tcp_listener.h"
#include "../common/udp_listener.h"
#include "../common/udp_connection.h"

#include "../common/config.h"
#include "../common/logger.h"

#include <deque>
#include <thread>
#include <mutex>
#include <functional>
#include <utility>


void parse_line(const std::string&);


void UdpSession::start()
{
	_active = true;

	_writer_thread = std::thread(&UdpSession::write_func, this);
	_reader_thread = std::thread(&UdpSession::read_func, this);
}

void UdpSession::stop()
{
	_active = false;

	_writer.close();
	_reader.close();

	_msg_queue_notifier.notify_one();

	if (_writer_thread.joinable()) _writer_thread.join();
	if (_reader_thread.joinable()) _reader_thread.join();
}

void UdpSession::write_func()
{
	Logger::log_debug() << "[udp_ssn] start write thread" << std::endl;
	while (_active)
	{
		std::unique_lock<std::mutex> lock(_msg_queue_mutex);
		_msg_queue_notifier.wait(lock,
			[this]{ return !_active || !_msg_queue.empty(); }
		);
		if (!_active) break;
		std::string msg(_msg_queue.front());
		_msg_queue.pop_front();
		lock.unlock();

		parse_line(msg);
		if (!_writer.write(msg)) break;
	}

	_writer.close();
	_reader.close();
	Logger::log_debug() << "[udp_ssn] finish write thread" << std::endl;
}

void UdpSession::read_func()
{
	Logger::log_debug() << "[udp_ssn] start read thread" << std::endl;
	while (_active)
	{
		std::string msg;
		if (!_reader.read(msg)) break;
		Logger::log_debug() << "[udp_ssn] receive msg: " << msg << std::endl;

		std::lock_guard<std::mutex> lock(_msg_queue_mutex);
		_msg_queue.push_back(msg);
		_msg_queue_notifier.notify_one();
	}

	_writer.close();
	_reader.close();
	Logger::log_debug() << "[udp_ssn] finish read thread" << std::endl;
}
