#include "scenario_runner.hpp"

#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>

ScenarioRunner::~ScenarioRunner() {
    m_server.stop();
    m_server_thread.join();
}

int ScenarioRunner::run_test_scenario(const std::string& scenario_name) {
    std::ifstream scenario;
    scenario.open("../src/smtp-lib-test/scenarios/" + scenario_name + ".test", std::ios::binary);

    m_client.connect(m_server_address, m_server_port);

    bool passed = true;

    std::string line;
    int current_line_number = 1;

    while (std::getline(scenario, line, '\n')) {
        std::string type = line.substr(0, 3);
        line.erase(0, 3);
        clean_string(line);

        if (type == "C: ") {
            send_to_server(line);
        } else if (type == "S: ") {
            if (!check_server_response(line)) {
                return current_line_number;
            }
        }
        ++current_line_number;
    }

    scenario.close();
    return 0;
}

bool ScenarioRunner::check_server_response(const std::string& prefix) {
    wait_for_network_interaction();

    const auto bytes = m_client.read(1024);
    std::string response(bytes.begin(), bytes.end());
    return response.find(prefix) == 0;
}

void ScenarioRunner::send_to_server(const std::string& message) { m_client.write(message); }

void ScenarioRunner::wait_for_network_interaction() { std::this_thread::sleep_for(std::chrono::milliseconds(100)); }

void ScenarioRunner::clean_string(std::string& line) {
    auto i = line.size() - 1;
    if (line.find('\r', line.size()-1) != std::string::npos) {
        line.erase(line.size() - 1);
    }
    std::string search = "\\r\\n";
    std::string replace = "\r\n";
    size_t pos = 0;
    while ((pos = line.find(search, pos)) != std::string::npos) {
        line.replace(pos, search.length(), replace);
        pos += replace.length();
    }
}