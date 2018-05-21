 #include "catch/catch.hpp"

#include "smtp-lib/raw_socket.hpp"
#include "smtp-lib/smtp_server.hpp"

#include "smtp-lib-test/helpers.hpp"

TEST_CASE("acceptance test", "[acceptance][server]") {
    uint16_t server_port = 5555;
	std::string server_address = "127.0.0.1";
    SMTPServer server(server_port);
	auto server_thread = std::thread(&SMTPServer::run, &server);

	SECTION("handle valid mail") {
		uint16_t client_port = 5556;
		auto client = RawSocket::new_socket(client_port);
		client.connect(server_address, server_port);

		wait_for_network_interaction();

		CHECK(check_return_code(client, "220"));

		client.write("HELO 127.0.0.1\r\n");
		CHECK(check_return_code(client, "250"));

		client.write("MAIL FROM:<Martin@JA.NE>\r\n");
		CHECK(check_return_code(client, "250"));

		client.write("RCPT TO:<Lawrenc@WTF.GBR>\r\n");
		CHECK(check_return_code(client, "250"));

		client.write("RCPT TO:<Fabi@LALA.JAJ>\r\n");
		CHECK(check_return_code(client, "250"));

		client.write("RCPT TO:<Jan@OCH.NO>\r\n");
		CHECK(check_return_code(client, "250"));

		client.write("DATA\r\n");
		CHECK(check_return_code(client, "354"));

		client.write("\r\n");
		client.write("Blah blah blah...\r\n");
		client.write("...etc. etc. etc.\r\n");
		client.write(".\r\n");
		CHECK(check_return_code(client, "250"));

		client.write("QUIT\r\n");
		CHECK(check_return_code(client, "221"));
	}
	
	server.stop();
	server_thread.join();

	clean_up_sockets();
}