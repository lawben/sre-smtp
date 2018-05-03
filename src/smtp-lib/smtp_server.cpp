#include <thread>

#include "smtp_server.hpp"
#include "socket.hpp"

void SMTPServer::run() {
  // This can throw if socket bind/listen fails, but the executable should take care of this
  Socket socket{};

  // This is the server's main loop to accept connections
  while (true) {
    // This blocks until a new connection comes in.
    auto smtp_handler = socket.accept_connection();

    // Start the the handler for a new SMTP request in a separate thread
    auto handle_client = [](SMTPHandler handler) { handler.run(); };
    std::thread thread{handle_client, std::move(smtp_handler)};
    thread.detach();
  }
}
