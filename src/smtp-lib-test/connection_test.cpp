#include "catch/catch.hpp"

#include <vector>

#include "smtp-lib/socket_listener.hpp"

#include "smtp-lib-test/helpers.hpp"

TEST_CASE("connection test", "[unit][connection]") {
    uint16_t port = 5555;
    SocketListener listener(port);
    std::string host("127.0.0.1");

    uint16_t client_port = 5556;

    std::string to_send = "Want to communicate?";
    std::string to_respond = "250 OK";

    SECTION("bidirectional communicaton") {
        auto client = RawSocket::new_socket(client_port);

        client.connect(host, port);

        wait_for_network_interaction();

        auto connection = listener.accept_connection();

        {
            client.write(to_send);
            wait_for_network_interaction();
            const auto bytes = connection.read();

            CHECK(to_send == std::string(bytes.begin(), bytes.end()));
        }
        {
            connection.write(to_respond);
            wait_for_network_interaction();
            const auto bytes = client.read(1024);

            CHECK(to_respond == std::string(bytes.begin(), bytes.end()));
        }
    }

    SECTION("free resources", "[unit][connection]") {
        std::vector<Connection> connections;

        {
            auto client = RawSocket::new_socket(client_port);
            client.connect(host, port);

            wait_for_network_interaction();

            auto connection = listener.accept_connection();
            connections.push_back(std::move(connection));
            connections.back().write(to_send);
            wait_for_network_interaction();
            const auto bytes = client.read(1024);

            CHECK(to_send == std::string(bytes.begin(), bytes.end()));

            connections.back().close();
        }
        {
            auto client = RawSocket::new_socket(client_port);
            client.connect(host, port);

            wait_for_network_interaction();

            auto connection = listener.accept_connection();
            connections.push_back(std::move(connection));
            connections.back().write(to_send);
            wait_for_network_interaction();
            const auto bytes = client.read(1024);

            CHECK(to_send == std::string(bytes.begin(), bytes.end()));

            connections.back().close();
        }
    }
}