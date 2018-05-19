#include "catch/catch.hpp"

#include "smtp-lib/connection.hpp"
#include "smtp-lib/raw_socket.hpp"
#include "smtp-lib/smtp_server.hpp"

#include "smtp-lib-test/helpers.hpp"

TEST_CASE("send valid mails", "[acceptance_test]") {
    uint16_t server_port = 5555;
	std::string server_address = "127.0.0.1";
    SMTPServer server(server_port);
	auto server_thread = std::thread(&SMTPServer::run, &server);

    uint16_t client_port = 5556;
	auto client = RawSocket::new_socket(client_port);
	client.connect(server_address, server_port);
    auto connection = std::make_unique<Connection>(std::make_unique<RawSocket>(std::move(client)));

	wait_for_network_interaction();

    CHECK(check_return_code(connection, "220"));

    connection->write("HELO 127.0.0.1\r\n");
    CHECK(check_return_code(connection, "250"));

    connection->write("MAIL FROM:<Martin@JA.NE>\r\n");
    CHECK(check_return_code(connection, "250"));

    connection->write("RCPT TO:<Lawrenc@WTF.GBR>\r\n");
    CHECK(check_return_code(connection, "250"));

    connection->write("RCPT TO:<Fabi@LALA.JAJ>\r\n");
    CHECK(check_return_code(connection, "250"));

    connection->write("RCPT TO:<Jan@OCH.NO>\r\n");
    CHECK(check_return_code(connection, "250"));

    connection->write("DATA\r\n");
    CHECK(check_return_code(connection, "354"));

    connection->write("\r\n");
    connection->write("Blah blah blah...\r\n");
    connection->write("...etc. etc. etc.\r\n");
    connection->write(".\r\n");
    CHECK(check_return_code(connection, "250"));

    connection->write("QUIT\r\n");
    CHECK(check_return_code(connection, "221"));

    server.stop();

	server_thread.join();
}

TEST_CASE("Send invalid mails", "[Message Receiving]") { REQUIRE(false); }
