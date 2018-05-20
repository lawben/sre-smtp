#include "catch/catch.hpp"

#include <vector>

#include "smtp-lib/socket_listener.hpp"

#include "smtp-lib-test/helpers.hpp"

TEST_CASE("bidirectional communicaton", "[connection]") {

	uint16_t port = 5555;
	SocketListener listener(port);

	uint16_t client_port = 5556;
	auto client = RawSocket::new_socket(client_port);

	client.connect(std::string("127.0.0.1"), port);
	
	wait_for_network_interaction();

	auto connection = listener.accept_connection();

	{
		std::string to_send = "Want to communicate?";

		client.write(to_send);
		wait_for_network_interaction();
		auto bytes = connection.read();

		CHECK(to_send == std::string(bytes.begin(), bytes.end()));
	}
	{
		std::string to_respond = "250 OK";

		connection.write(to_respond);
		wait_for_network_interaction();
		auto bytes = client.read(1024);

		CHECK(to_respond == std::string(bytes.begin(), bytes.end()));
	}
}

TEST_CASE("free resources", "[connection]") {

	uint16_t listener_port = 5555;
	std::string listener_address = "127.0.0.1";
	SocketListener listener(listener_port);

	uint16_t client_port = 5556;
	std::string to_send = "Hello Client!";

	std::vector<Connection> connections;

	{
		auto client = RawSocket::new_socket(client_port);
		client.connect(listener_address, listener_port);

		wait_for_network_interaction();

		connections.push_back(std::move(listener.accept_connection()));
		connections.back().write(to_send);
		wait_for_network_interaction();
		auto bytes = client.read(1024);

		CHECK(to_send == std::string(bytes.begin(), bytes.end()));

		connections.back().close();
	}
	{
		auto client = RawSocket::new_socket(client_port);
		client.connect(listener_address, listener_port);

		wait_for_network_interaction();

		connections.push_back(std::move(listener.accept_connection()));
		connections.back().write(to_send);
		wait_for_network_interaction();
		auto bytes = client.read(1024);

		CHECK(to_send == std::string(bytes.begin(), bytes.end()));

		connections.back().close();
	}
}