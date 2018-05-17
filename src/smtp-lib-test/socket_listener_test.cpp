#include "catch/catch.hpp"

#include <thread>
#include <chrono>

#include "smtp-lib/socket.hpp"

TEST_CASE("accept with no listener", "[socket_listener]") {

	uint16_t port = 5556;
	Socket socket(port);
	{
		auto connection = socket.accept_connection();
		CHECK_FALSE(connection->is_valid());
	}
}

TEST_CASE("accept listeners", "[socket_listener]") {

	uint16_t port = 5556;
	Socket socket(port);

	{
		auto sender = std::make_unique<RawSocket>(RawSocket::new_socket());
		uint16_t sender_port = 5555;
		CHECK(sender->bind(sender_port));
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		CHECK(sender->connect(std::string("127.0.0.1"), port));
	}
	{
		auto sender = std::make_unique<RawSocket>(RawSocket::new_socket());
		uint16_t sender_port = 5555;
		CHECK(sender->bind(sender_port));
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		CHECK(sender->connect(std::string("127.0.0.1"), port));
	}
	{
		auto sender = std::make_unique<RawSocket>(RawSocket::new_socket());
		uint16_t sender_port = 5555;
		CHECK(sender->bind(sender_port));
		std::this_thread::sleep_for(std::chrono::milliseconds(100));
		CHECK(sender->connect(std::string("127.0.0.1"), port));
	}

}