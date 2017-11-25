#pragma once

#include "../common/udp_listener.h"
#include "../common/udp_connection.h"

#include <deque>
#include <thread>
#include <mutex>
#include <condition_variable>


class UdpSession
{
public:
	UdpSession(net::UdpListener&& reader, net::UdpConnection&& writer) :
		_reader(std::move(reader)), _writer(std::move(writer)) {}
	UdpSession(UdpSession&& helper) = delete;
	UdpSession(const UdpSession&) = delete;
	UdpSession& operator=(const UdpSession&) = delete;
	~UdpSession() {}

	void start();
	void stop();

	void write_func();
	void read_func();

private:
	bool _active;

	std::deque<std::string> _msg_queue;
	std::mutex _msg_queue_mutex;
	std::condition_variable _msg_queue_notifier;

	net::UdpListener _reader;
	net::UdpConnection _writer;

	std::thread _writer_thread;
	std::thread _reader_thread;
};
