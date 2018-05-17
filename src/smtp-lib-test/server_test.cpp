#include "catch/catch.hpp"

#include "smtp-lib/smtp_server.hpp"
#include "smtp-lib/raw_socket.hpp"
#include "smtp-lib/connection.hpp"

bool check_return_code_new(const std::unique_ptr<Connection>& connection, std::string prefix) {
	std::vector<char> bytes;
	while(bytes.empty())
		bytes = connection->read();
	std::string result(bytes.begin(), bytes.end());
	return result.find(prefix) == 0;
}

TEST_CASE("Open, run and close server", "[SMTPServer]") {
    
	SMTPServer server(8080);

    REQUIRE(server.is_running() == false);
    
	auto server_thread = std::thread(&SMTPServer::run, &server);
	
	std::this_thread::sleep_for(std::chrono::milliseconds(10));
	REQUIRE(server.is_running());

    server.stop();
    
	REQUIRE(server.is_running() == false);

	server_thread.join();
}

TEST_CASE("Stop server while client is connected", "[SMTPServer]") {

	uint16_t in_port = 5572;
	auto socket = std::make_unique<RawSocket>(RawSocket::new_socket());
	SMTPServer server(in_port);
	auto server_thread = std::thread(&SMTPServer::run, &server);

	uint16_t out_port = 5573;
	socket->bind(out_port);
	std::string host = "127.0.0.1";
	socket->connect(host, in_port);
	auto connection = std::make_unique<Connection>(std::move(socket));

	REQUIRE(server.is_running());

	CHECK(connection->is_valid());
	CHECK(check_return_code_new(connection, "220"));

	server.stop();

	REQUIRE(server.is_running() == false);

	server_thread.join();
}