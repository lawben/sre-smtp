#include "catch/catch.hpp"

#include "smtp-lib/socket_listener.hpp"

#include "smtp-lib-test/helpers.hpp"

TEST_CASE("bidirectional communicaton", "[connection]") {

	uint16_t port = 5556;
	SocketListener listener(port);

	uint16_t client_port = 5555;
	auto client = RawSocket::new_socket(client_port);

	client.connect(std::string("127.0.0.1"), port);
	
	wait_for_network_interaction();

	auto connection = listener.accept_connection();

	{
		std::string to_send = "Want to communicate?";

		client.write(to_send);
		wait_for_network_interaction();
		auto bytes = connection->read();

		CHECK(to_send == std::string(bytes.begin(), bytes.end()));
	}
	{
		std::string to_respond = "250 OK";

		connection->write(to_respond);
		wait_for_network_interaction();
		auto bytes = client.read(1024);

		CHECK(to_respond == std::string(bytes.begin(), bytes.end()));
	}
}