#include "catch/catch.hpp"

#include <thread>
#include <chrono>

#include "smtp-lib/connection.hpp"

TEST_CASE("open socket", "[raw_socket]") {

	auto socket = std::make_unique<RawSocket>(RawSocket::new_socket());

	CHECK(socket->is_valid());
}

TEST_CASE("bind socket", "[raw_socket]") {

	auto socket = std::make_unique<RawSocket>(RawSocket::new_socket());
	uint16_t port = 5555;
	CHECK(socket->bind(port));
	CHECK(socket->is_valid());
}

TEST_CASE("bind socket twice", "[raw_socket]") {

	auto socket = std::make_unique<RawSocket>(RawSocket::new_socket());
	uint16_t port = 5555;
	uint16_t other_port = 5556;
	CHECK(socket->bind(port));
	CHECK(socket->is_valid());
	CHECK_THROWS(socket->bind(other_port));
}

TEST_CASE("accept with no listender", "[raw_socket]") {

	auto server = std::make_unique<RawSocket>(RawSocket::new_socket());
	uint16_t server_port = 5556;
	CHECK(server->bind(server_port));
	CHECK(server->listen(5));

	auto receiver = std::make_unique<RawSocket>(server->accept());
	CHECK_FALSE(receiver->is_valid());
}

TEST_CASE("accept listender", "[raw_socket]") {

	auto server = std::make_unique<RawSocket>(RawSocket::new_socket());
	uint16_t server_port = 5556;
	CHECK(server->bind(server_port));
	CHECK(server->listen(5));

	auto sender = std::make_unique<RawSocket>(RawSocket::new_socket());
	uint16_t sender_port = 5555;
	CHECK(sender->bind(sender_port));

	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	CHECK(sender->connect(std::string("127.0.0.1"), server_port));

	auto receiver = std::make_unique<RawSocket>(server->accept());
	CHECK(receiver->is_valid());
}

TEST_CASE("write and receive", "[raw_socket]") {

	auto server = std::make_unique<RawSocket>(RawSocket::new_socket());
	uint16_t server_port = 5556;
	CHECK(server->bind(server_port));
	CHECK(server->listen(5));

	auto sender = std::make_unique<RawSocket>(RawSocket::new_socket());
	uint16_t sender_port = 5555;
	CHECK(sender->bind(sender_port));

	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	CHECK(sender->connect(std::string("127.0.0.1"), server_port));

	auto receiver = std::make_unique<RawSocket>(server->accept());
	CHECK(receiver->is_valid());

	std::string to_send = "test";
	CHECK(sender->write(to_send));

	std::this_thread::sleep_for(std::chrono::milliseconds(100));

	auto bytes = receiver->read(4);
	std::string message(bytes.begin(), bytes.end());
	CHECK(to_send == message);
}