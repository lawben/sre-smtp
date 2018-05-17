#pragma once

#include <atomic>
#include <thread>

#include "utils.hpp"
#include "mail_receiver.hpp"

class SMTPServer{
  public:
    SMTPServer(uint16_t port);
    void run();
    bool is_running();
    void stop();

  private:
    void accept_connections();

	void wait_incoming_connection();
	void start_worker_for_last_mail_receiver();

	void stop_mail_receivers();
	void request_stop_on_mail_receivers();
	void join_worker_threads();

    uint16_t m_port;
    bool m_is_running;
    std::atomic<bool> m_stop_requested;
	std::vector<MailReceiver> m_mail_receivers;
	std::vector<std::thread> m_workers;
};
