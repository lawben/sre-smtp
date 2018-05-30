#pragma once

#include <map>
#include <string>

enum SMTPCommandType { HELO, MAIL, RCPT, DATA_BEGIN, DATA, QUIT, RSET, INVALID };

struct SMTPCommand {
    SMTPCommandType type;
    std::string data;
};

enum class SMTPState { CLIENT_INIT, MAIL_FROM, RCPT_TO, RCPT_TO_OR_DATA_BEGIN, DATA_CONTENT, QUIT, FINISHED };

enum class SimplifiedSMTPState { ENVELOPE, CONTENT };

static const std::string NEWLINE_TOKEN = "\r\n";
static const std::string DATA_END_TOKEN = "\r\n.\r\n";

static const std::map<std::string, SMTPCommandType> envelop_tokens{{"helo ", SMTPCommandType::HELO},
                                                                   {"mail from:", SMTPCommandType::MAIL},
                                                                   {"rcpt to:", SMTPCommandType::RCPT},
                                                                   {"data", SMTPCommandType::DATA_BEGIN},
                                                                   {"quit", SMTPCommandType::QUIT}};
static const std::map<std::string, SMTPCommandType> content_tokens{{"", SMTPCommandType::DATA}};

class SMTPResponse {
  public:
    SMTPResponse(uint16_t c, const std::string& s) : code(c), string(s) {}

    uint16_t code;
    std::string string;

    std::string get_message() const { return std::to_string(code) + " " + string + NEWLINE_TOKEN; }
};
