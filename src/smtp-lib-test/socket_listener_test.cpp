#include "catch/catch.hpp"

#include <thread>
#include <chrono>

#include "smtp-lib/socket.hpp"

TEST_CASE("accept listener", "[socket_listener]") {

	uint16_t port = 5556;
	Socket socket(port);
	{
		auto connection = socket.accept_connection();
		CHECK_FALSE(connection->is_valid());
	}

	auto sender = std::make_unique<RawSocket>(RawSocket::new_socket());
	uint16_t sender_port = 5555;
	CHECK(sender->bind(sender_port));

	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	CHECK(sender->connect(std::string("127.0.0.1"), port));
	{
		auto receiver_connection = socket.accept_connection();
		CHECK(receiver_connection->is_valid());

		std::string to_send = "test";

		CHECK(sender->write(to_send));

		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		auto bytes = receiver_connection->read();
		std::string message(bytes.begin(), bytes.end());
		CHECK(to_send == message);
	}
}