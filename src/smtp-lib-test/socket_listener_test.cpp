#include "catch/catch.hpp"

#include "smtp-lib/socket_listener.hpp"

#include "smtp-lib-test/helpers.hpp"

TEST_CASE("accept with no listener", "[socket_listener]") {

	uint16_t port = 5556;
	SocketListener socket(port);

	auto connection = socket.accept_connection();

	CHECK_FALSE(connection.is_valid());
}

TEST_CASE("accept listeners", "[socket_listener]") {

	uint16_t port = 5556;
	std::string host = "127.0.0.1";
	SocketListener socket(port);

	uint16_t sender_port_1 = 5557;
	uint16_t sender_port_2 = 5558;
	uint16_t sender_port_3 = 5559;

	auto sender_1 = RawSocket::new_socket(sender_port_1);
	auto sender_2 = RawSocket::new_socket(sender_port_2);
	auto sender_3 = RawSocket::new_socket(sender_port_3);
	
	sender_1.connect(std::string(host), port);
	sender_2.connect(std::string(host), port);
	sender_3.connect(std::string(host), port);

	wait_for_network_interaction();
	
	auto connection_1 = socket.accept_connection();
	auto connection_2 = socket.accept_connection();
	auto connection_3 = socket.accept_connection();
	auto connection_4 = socket.accept_connection();

	CHECK(connection_1.is_valid());
	CHECK(connection_2.is_valid());
	CHECK(connection_3.is_valid());

	CHECK_FALSE(connection_4.is_valid());
}


TEST_CASE("free port correctly", "[socket_listener]") {

	uint16_t listener_port = 5555;
	std::string listener_address = "127.0.0.1";
	SocketListener listener(listener_port);

	uint16_t client_port = 5556;
	std::string to_send = "220";
	std::vector<Connection> server_connections;

	{
		auto client = RawSocket::new_socket(client_port);
		client.connect(listener_address, listener_port);

		wait_for_network_interaction();

		server_connections.push_back(std::move(listener.accept_connection()));
		server_connections.back().write(to_send);

		wait_for_network_interaction();

		CHECK(check_return_code(client, to_send));

		server_connections.back().close(); // close client befor client, do lead to leave the clients port unfreed
		client.close();
	}
	wait_for_network_interaction();
	{
		auto client = RawSocket::new_socket(client_port);
		client.connect(listener_address, listener_port);

		wait_for_network_interaction();

		server_connections.push_back(std::move(listener.accept_connection()));
		server_connections.back().write(to_send);

		wait_for_network_interaction();

		CHECK(check_return_code(client, to_send));

		server_connections.back().close(); // close client befor client, do lead to leave the clients port unfreed
		client.close();
	}
}