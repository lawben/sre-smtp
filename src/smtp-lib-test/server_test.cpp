#include "catch/catch.hpp"

#include "smtp-lib/smtp_server.hpp"
#include "smtp-lib/raw_socket.hpp"
#include "smtp-lib/connection.hpp"

#include "smtp-lib-test/helpers.hpp"

TEST_CASE("run server", "[smtp_server]") {
    
	SMTPServer server(8080);

    CHECK_FALSE(server.is_running());
    
	auto server_thread = std::thread(&SMTPServer::run, &server);
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
	
	CHECK(server.is_running());

    server.stop();
    
	CHECK_FALSE(server.is_running());

	server_thread.join();
}

TEST_CASE("Stop server while client is connected", "[smtp_server]") {

	uint16_t server_port = 5555;
	std::string server_address = "127.0.0.1";
	SMTPServer server(server_port);
	auto server_thread = std::thread(&SMTPServer::run, &server);

	uint16_t client_port = 5556;
	auto client = RawSocket::new_socket(client_port);

	wait_for_network_interaction();

	client.connect(server_address, server_port);
	auto connection = std::make_unique<Connection>(std::make_unique<RawSocket>(std::move(client)));

	wait_for_network_interaction();

	CHECK(server.is_running());
	CHECK(connection->is_valid());
	CHECK(check_return_code(connection, "220"));

	server.stop();

	CHECK_FALSE(server.is_running());

	server_thread.join();
}