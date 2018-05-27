#pragma once

#include <fstream>
#include <string>

#include "smtp-lib/smtp_server.hpp"

class ScenarioRunner {
  public:
    /** Loads the given scenario and runs it.
	* return values: -1 error load file
	*				 0 successful
	*				 >0 fail, line number failed at 
	*/
    int run_test_scenario(const std::string& scenario_name);

    explicit ScenarioRunner(uint16_t server_port = 5555, uint16_t client_port = 5556)
        : m_server_port(server_port),
          m_client_port(client_port),
          m_server_address("127.0.0.1"),
          m_server(server_port),
          m_client(RawSocket::new_socket(m_client_port)) {
        m_server_thread = std::thread(&SMTPServer::run, &m_server);
    }

    ~ScenarioRunner();

  protected:
    uint16_t m_server_port;
    uint16_t m_client_port;
    std::string m_server_address;
    SMTPServer m_server;
    RawSocket m_client;
    std::thread m_server_thread;
    std::ifstream m_scenario;

    bool initialize_scenario(const std::string& scenario_name);
    int run_scenario();
    void shutdown_scenario();

    void clean_up_command(std::string& line);

    bool check_server_response(const std::string& prefix);
    void send_to_server(const std::string& message);
    void wait_for_network_interaction();
};