#pragma once

#include <vector>
#include "string"

enum Status {
    WAIT_INIT,
    WAIT_HELO,
    WAIT_MAIL_FROM,
    WAIT_RCPT_FROM,
    WAIT_DATA_BEGIN,
    WAIT_DATA_END,
    TERMINATED,
};

struct State {
    Status status;
    std::string mailFrom;
    std::string rcptFrom;
    std::vector<std::string> content;
};

struct Request {
    std::string message;
};

struct Response {
    int code;
    std::string message;
};

class MailParser {
public:
    MailParser();
    MailParser(const State& state);
    Response accept(const Request& message);
    bool isTerminated();
private:
    State m_state;
    Response handleInit(const Request& request);
    Response handleHelo(const Request& request);
    Response handleMailFrom(const Request& request);
    Response handleRcptFrom(const Request& request);
    Response handleDataBegin(const Request& request);
    Response handleData(const Request& request);
};
