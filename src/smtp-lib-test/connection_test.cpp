#include "catch/catch.hpp"

#include <thread>
#include <chrono>

#include "smtp-lib/socket_listener.hpp"

#include "smtp-lib-test/helpers.hpp"

TEST_CASE("bidirectional communicaton", "[connection]") {

	uint16_t port = 5556;
	SocketListener listener(port);

	auto client = std::make_unique<RawSocket>(RawSocket::new_socket());
	uint16_t client_port = 5555;
	CHECK(client->bind(client_port));

	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	client->connect(std::string("127.0.0.1"), port);
	
	wait_for_network_interaction();

	auto connection = listener.accept_connection();
	connection->is_valid();

	{
		std::string to_send = "Want to communicate?";
		client->write(to_send);

		wait_for_network_interaction();

		auto bytes = connection->read();
		std::string message(bytes.begin(), bytes.end());
		CHECK(to_send == message);
	}
	{
		std::string to_respond = "250 OK";
		connection->write(to_respond);

		wait_for_network_interaction();

		auto bytes = client->read(1024);
		std::string message(bytes.begin(), bytes.end());
		CHECK(to_respond == message);
	}
}