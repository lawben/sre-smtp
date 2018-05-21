#pragma once

#include <thread>

#include "utils.hpp"
#include "mail_receiver.hpp"
#include "socket_listener.hpp"

class SMTPServer{
  public:
    SMTPServer(uint16_t port);
    void run();
    bool is_running();
    void stop();

  private:
    SocketListener m_socket_listener;
    bool m_is_running;
    bool m_stop_requested;
    std::vector<MailReceiver> m_mail_receivers;
    std::vector<std::thread> m_workers;
    
	void accept_connections();
	Connection accept_connection();

	void add_new_mail_receiver_for(Connection connection);
	void start_worker_for_last_mail_receiver();

	void stop_mail_receivers();
	void request_stop_on_mail_receivers();
	void join_worker_threads();
};
