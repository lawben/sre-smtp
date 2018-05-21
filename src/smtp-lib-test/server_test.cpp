#include "catch/catch.hpp"

#include "smtp-lib/raw_socket.hpp"
#include "smtp-lib/smtp_server.hpp"

#include "smtp-lib-test/helpers.hpp"

TEST_CASE("use smtp server", "[unit][server]") {

	uint16_t server_port = 5555;
	std::string server_address = "127.0.0.1";
	uint16_t client_1_port = 5556;
	uint16_t client_2_port = 5557;
	SMTPServer server(server_port);
	auto server_thread = std::thread(&SMTPServer::run, &server);

	SECTION("welcome clients") {
		auto client_1 = RawSocket::new_socket(client_1_port);
		auto client_2 = RawSocket::new_socket(client_2_port);

		wait_for_network_interaction();

		client_1.connect(server_address, server_port);
		client_2.connect(server_address, server_port);

		wait_for_network_interaction();

		CHECK(server.is_running());

		CHECK(client_1.is_valid());
		CHECK(client_2.is_valid());

		CHECK(check_return_code(client_1, "220"));
		CHECK(check_return_code(client_2, "220"));
	}

	server.stop();
	wait_for_network_interaction();

	server_thread.join();
	clean_up_sockets();
}