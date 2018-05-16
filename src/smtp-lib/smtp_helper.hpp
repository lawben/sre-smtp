#pragma once

#include <string>

enum SMTPCommandType { HELO, MAIL, RCPT, DATA_BEGIN, DATA, QUIT, RSET, INVALID };

struct SMTPCommand {
    SMTPCommandType type;
    std::string data;
};

enum class SMTPState { CLIENT_INIT, MAIL_FROM, RCPT_TO, RCPT_TO_OR_DATA_BEGIN, DATA_CONTENT, QUIT, FINISHED };

enum class SimplifiedSMTPState { ENVELOPE, CONTENT };

struct SMTPResponse {
    uint16_t code;
    std::string string;
};
