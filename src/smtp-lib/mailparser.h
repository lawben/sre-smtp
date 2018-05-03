#pragma once

#include <vector>
#include "string"

enum ParserStatus {
    WAIT_INIT,
    WAIT_HELO,
    WAIT_MAIL_FROM,
    WAIT_RCPT_FROM,
    WAIT_DATA_BEGIN,
    WAIT_DATA_END,
    TERMINATED,
};

struct Mail {
    std::string mailFrom;
    std::string rcptFrom;
    std::vector<std::string> content;
};

struct ParserState {
    ParserStatus status;
    std::vector<Mail> mails;
};

struct ParserRequest {
    std::string message;
};

struct ParserResponse {
    int code;
    std::string message;
};

class MailParser {
public:
    MailParser();
    MailParser(const ParserState& state);
    ParserResponse accept(const ParserRequest& message);
    bool isTerminated();
    std::vector<Mail> getMails();
private:
    ParserState m_state;
    ParserResponse handleInit(const ParserRequest& request);
    ParserResponse handleHelo(const ParserRequest& request);
    ParserResponse handleMailFrom(const ParserRequest& request);
    ParserResponse handleRcptFrom(const ParserRequest& request);
    ParserResponse handleDataBegin(const ParserRequest& request);
    ParserResponse handleData(const ParserRequest& request);
    Mail getCurrentMail();
};
