#include "log/log_targer.h"
#include "net/net_address.h"
#include "text/json.hpp"
namespace sq
{
	void stdout_target::log(const char* log)
	{
		m_lock.lock();
		printf("%s", log);
		m_lock.unlock();
	}
	void stdout_target::flush() {
		m_lock.lock();
		fflush(stdout);
		m_lock.unlock();
	}
	//===================================
	 void file_target::log(const char* log)
	{
		m_lock.lock();
		fwrite(log, strlen(log), 1, m_file);
		m_lock.unlock();
	}
	void file_target::flush() {
		m_lock.lock();
		if (m_file)
			fflush(m_file);
		m_lock.unlock();
	}
//	====================================
	async_file_target::~async_file_target() {

		m_stop = true;
		m_thread.join();

		if (m_file) {
			fclose(m_file);
		}
		delete m_msg_queue;
	}
	void async_file_target::log(const char*log)
	{
		m_lock.lock();

		string* q = *m_msg_queue->back();
		while (q == nullptr) {
			std::this_thread::yield();
			q = *m_msg_queue->back();
		}
		if (q) {
			*q = log;
			m_msg_queue->push();

		}
		m_lock.unlock();
	}
	void async_file_target::loop() {
		while (true)
		{
			if (m_msg_queue) {

				string* q = *m_msg_queue->front();
				while (q) {
					fwrite(q->c_str(), q->size(), 1, m_file);
					m_msg_queue->pop();
					q = *m_msg_queue->front();
				}
			}
			else {
				std::this_thread::sleep_for(std::chrono::microseconds(10));
			}
			if (m_stop) {
				string* q = *m_msg_queue->front();
				while (q) {
					fwrite(q->c_str(), q->size(), 1, m_file);
					m_msg_queue->pop();
					q = *m_msg_queue->front();
				}
				break;
			}
		}

	}
	void async_file_target::flush() {
		m_lock.lock();
		if (m_file)
			fflush(m_file);
		m_lock.unlock();
	}
	//==============================
#if 0
	udp_target::udp_target(const char*ip)
	{
		net_address addr;
		addr.parse_ipv4(ip);

		m_udp.set_dest_info(addr.m_ip.c_str(), addr.m_port);

	}
#if 0
	void udp_target::log(const char* log)
	{
		nlohmann::json js;
		js["tid"] = 50;
		js["time"] = 0;
		js["owner"] = 0;
		js["type"] = 52;
		js["info"] = log;
		js["maxVol"] = 0;
		string ss=js.dump();
		m_udp.send_to_dest(ss.c_str(),ss.length());
	}
#endif
#endif
}