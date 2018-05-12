#pragma once

#include <atomic>
#include <thread>

#include "utils.hpp"

class SMTPServer : public NonCopyable {
  public:
	  SMTPServer(uint16_t port);
	  void run();
	  bool is_running();
	  void stop();

  private:
	  void accept_connections();

	  uint16_t m_port;
	  std::atomic<bool> m_stop_requested;
	  bool m_is_running;
	  std::thread m_receiverThread;
};
