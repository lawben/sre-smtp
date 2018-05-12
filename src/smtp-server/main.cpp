#include <iostream>

#include "smtp-lib/smtp_server.hpp"

int main(int argc, char* argv[]) {
    uint16_t port = 5555;
    if (argc > 1) {
        port = static_cast<uint16_t>(std::atoi(argv[1]));
    }

    try {
		auto server = SMTPServer(port);
		server.run();
		while (server.is_running())
		{

		}

    } catch (const std::exception& exception) {
        std::cout << exception.what() << std::endl;
        return 1;
    }
}
