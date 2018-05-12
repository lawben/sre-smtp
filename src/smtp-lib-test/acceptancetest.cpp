#include "catch/catch.hpp"

#include "smtp-lib/smtp_server.hpp"
#include "smtp-lib/raw_socket.hpp"
#include "smtp-lib/connection.hpp"

bool string_starts_with(std::string& s, std::string& prefix) {
	return s.find(prefix) == 0;
}

TEST_CASE("Send valid mails", "[Message Receiving]") {
	uint16_t in_port = 5556;
	uint16_t out_port = 5557;
	auto socket = std::make_unique<RawSocket>(RawSocket::new_socket());
	
	SMTPServer server(in_port);
	server.run();
	
	socket->bind(out_port);
	socket->connect(std::string("127.0.0.1"), in_port);
	auto connection = std::make_unique<Connection>(std::move(socket));
	
	{
		const auto bytes = connection->read();
		std::string result(bytes.begin(), bytes.end());
		REQUIRE(string_starts_with(result, std::string("220")));
	}

	std::string helo_message("HELO 127.0.0.1");
	connection->write(Bytes(helo_message.begin(), helo_message.end()));

	{
		const auto bytes = connection->read();
		std::string result(bytes.begin(), bytes.end());
		REQUIRE(string_starts_with(result, std::string("250")));
	}

	server.stop();
}

TEST_CASE("Send invalid mails", "[Message Receiving]") {

	REQUIRE(false);
}
