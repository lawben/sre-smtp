#include "catch/catch.hpp"

#include <thread>
#include <chrono>

#include "smtp-lib/socket_listener.hpp"

TEST_CASE("accept with no listener", "[socket_listener]") {

	uint16_t port = 5556;
	SocketListener socket(port);
	{
		auto connection = socket.accept_connection();
		CHECK_FALSE(connection->is_valid());
	}
}

TEST_CASE("accept listeners", "[socket_listener]") {

	uint16_t port = 5556;
	std::string host = "127.0.0.1";
	SocketListener socket(port);

	uint16_t sender_port = 5555;

	{
		auto sender = std::make_unique<RawSocket>(RawSocket::new_socket());
		CHECK(sender->bind(sender_port));
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		CHECK(sender->connect(std::string(host), port));

		auto connection = socket.accept_connection();
		CHECK(connection->is_valid());
	}
	{
		auto sender = std::make_unique<RawSocket>(RawSocket::new_socket());
		CHECK(sender->bind(sender_port));
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		CHECK(sender->connect(std::string(host), port));
		
		auto connection = socket.accept_connection();
		CHECK(connection->is_valid());
	}
	{
		auto sender = std::make_unique<RawSocket>(RawSocket::new_socket());
		CHECK(sender->bind(sender_port));
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		CHECK(sender->connect(std::string(host), port));

		auto connection = socket.accept_connection();
		CHECK(connection->is_valid());
	}

}