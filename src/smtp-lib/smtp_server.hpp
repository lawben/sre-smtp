#pragma once

#include <thread>

#include "utils.hpp"

class SMTPServer : public NonCopyable {
  public:
	  SMTPServer(uint16_t port);
	  void run();
	  bool is_running();
	  void stop();

  private:
	  void static accept_connections(uint16_t port);

	  uint16_t m_port;
	  bool m_is_running;
	  std::thread m_receiverThread;
};
