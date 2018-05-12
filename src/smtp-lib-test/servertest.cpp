#include "catch/catch.hpp"

#include "smtp-lib/smtp_server.hpp"

TEST_CASE("Open, run and close server", "[SMTPServer]") {
    SMTPServer server(8080);

	REQUIRE(server.is_running() == false);
	server.run();
	REQUIRE(server.is_running());
	server.stop();
	REQUIRE(server.is_running() == false);
}
