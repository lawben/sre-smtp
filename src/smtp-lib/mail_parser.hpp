#pragma once

#include <algorithm>
#include <map>
#include <vector>

#include "smtp_utils.hpp"

using Bytes = std::vector<char>;

struct ParserRequest {
  protected:
    std::string message;

  public:
    explicit ParserRequest(const Bytes& bytes) : message(bytes.begin(), bytes.end()) {}
    explicit ParserRequest(const std::string& bytes) : message(bytes) {}

    bool empty() { return message.empty(); }

    operator std::string() const { return message; }
};

enum class ParserStatus { COMPLETE, INCOMPLETE, TOO_LONG };

class MailParser {
  public:
    static MailParser get_content_parser() { return MailParser(DATA_END_TOKEN, content_tokens); }
    static MailParser get_envelop_parser() { return MailParser(NEWLINE_TOKEN, envelop_tokens); }

    MailParser(const std::string& delimiter, const std::map<std::string, SMTPCommandType>& string_to_tokentype)
        : m_delemiter(delimiter),
          m_string_to_tokentype(string_to_tokentype),
          m_command({SMTPCommandType::INVALID, ""}){};

    ParserStatus accept(const ParserRequest& request);
    SMTPCommand get_command();

  protected:
    std::string m_delemiter;
    std::string m_buffer;
    std::map<std::string, SMTPCommandType> m_string_to_tokentype;
    SMTPCommand m_command;

    ParserStatus parser_status();

    void extract_command_type();
    void extract_data();
};