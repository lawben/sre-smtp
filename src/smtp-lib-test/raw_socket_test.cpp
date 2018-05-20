#include "catch/catch.hpp"

#include "smtp-lib/connection.hpp"

#include "smtp-lib-test/helpers.hpp"

TEST_CASE("open socket", "[raw_socket]") {

	auto socket = RawSocket::new_socket();

	CHECK(socket.is_valid());
}

TEST_CASE("bind socket", "[raw_socket]") {

	auto socket = RawSocket::new_socket();
	uint16_t port = 5555;
	CHECK(socket.bind(port));
	CHECK(socket.is_valid());

	uint16_t other_port = 5556;
	CHECK_THROWS(socket.bind(other_port));
}

TEST_CASE("open with bind socket", "[raw_socket]") {

	uint16_t port = 5555;
	auto socket = RawSocket::new_socket(port);
	CHECK(socket.is_valid());
	CHECK_THROWS(RawSocket::new_socket(port));
}

TEST_CASE("close socket", "[raw_socket]") {

	uint16_t port = 5555;
	auto socket = RawSocket::new_socket(port);
	CHECK(socket.is_valid());
	socket.close();

	auto next_socket = RawSocket::new_socket(port);
	CHECK(socket.is_valid());
}

TEST_CASE("accept with no listender", "[raw_socket]") {

	uint16_t port = 5555;
	auto listener = RawSocket::new_socket(port);
	
	CHECK(listener.listen(5));

	auto receiver = listener.accept();
	CHECK_FALSE(receiver.is_valid());
}

TEST_CASE("accept listender", "[raw_socket]") {

	uint16_t port = 5555;
	auto listener = RawSocket::new_socket(port);
	listener.listen(5);

	uint16_t sender_port = 5556;
	auto sender = RawSocket::new_socket(sender_port);
	sender.connect(std::string("127.0.0.1"), port);

	wait_for_network_interaction();
	
	auto receiver = listener.accept();

	CHECK(receiver.is_valid());
}

TEST_CASE("reuse port", "[raw_socket]") {

	uint16_t port = 5555;
	auto listener = RawSocket::new_socket(port);
	listener.listen(5);
	
	uint16_t sender_port = 5556;
	{
		auto sender = RawSocket::new_socket(sender_port);
		sender.connect(std::string("127.0.0.1"), port);

		wait_for_network_interaction();
		auto receiver = listener.accept();

		CHECK(receiver.is_valid());
	}
	{
		auto sender = RawSocket::new_socket(sender_port);
		sender.connect(std::string("127.0.0.1"), port);

		wait_for_network_interaction();
		auto receiver = listener.accept();

		CHECK(receiver.is_valid());
	}
}

TEST_CASE("write and receive", "[raw_socket]") {

	uint16_t port = 5555;
	auto socket = RawSocket::new_socket(port);
	socket.listen(5);

	uint16_t sender_port = 5556;
	auto sender = RawSocket::new_socket(sender_port);
	sender.connect(std::string("127.0.0.1"), port);

	wait_for_network_interaction();

	auto receiver = socket.accept();
	CHECK(receiver.is_valid());

	std::string to_send = "test";
	CHECK(sender.write(to_send));

	wait_for_network_interaction();

	auto bytes = receiver.read(4);
	CHECK(to_send == std::string(bytes.begin(), bytes.end()));
}