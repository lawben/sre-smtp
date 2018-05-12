#include "catch/catch.hpp"

#include "smtp-lib/smtp_server.hpp"
#include "smtp-lib/raw_socket.hpp"
#include "smtp-lib/connection.hpp"

bool check_return_code(const std::unique_ptr<Connection>& connection, std::string prefix) {
	const auto bytes = connection->read();
	std::string result(bytes.begin(), bytes.end());
	return result.find(prefix) == 0;
}

TEST_CASE("Send valid mails", "[Message Receiving]") {
	uint16_t in_port = 5560;
	auto socket = std::make_unique<RawSocket>(RawSocket::new_socket());
	SMTPServer server(in_port);
	server.run();
	
	uint16_t out_port = 5561;
	socket->bind(out_port);
	std::string host = "127.0.0.1";
	socket->connect(host, in_port);
	auto connection = std::make_unique<Connection>(std::move(socket));
	
	REQUIRE(check_return_code(connection, "220"));

	connection->write("HELO 127.0.0.1\r\n");
	REQUIRE(check_return_code(connection, "250"));
	
	connection->write("MAIL FROM:<Martin@JA.NE>\r\n");
	REQUIRE(check_return_code(connection, "250"));

	connection->write("RCPT TO:<Lawrenc@WTF.GBR>\r\n");
	REQUIRE(check_return_code(connection, "250"));

	connection->write("RCPT TO:<Fabi@LALA.JAJ>\r\n");
	REQUIRE(check_return_code(connection, "250"));

	connection->write("RCPT TO:<Jan@OCH.NO>\r\n");
	REQUIRE(check_return_code(connection, "250"));

	connection->write("DATA\r\n");
	REQUIRE(check_return_code(connection, "354"));

	connection->write("\r\n");
	connection->write("Blah blah blah...\r\n");
	connection->write("...etc. etc. etc.\r\n");
	connection->write(".\r\n");
	REQUIRE(check_return_code(connection, "250"));

	connection->write("QUIT\r\n");
	REQUIRE(check_return_code(connection, "221"));

	server.stop();
}

TEST_CASE("Send invalid mails", "[Message Receiving]") {

	REQUIRE(false);
}

