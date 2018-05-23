#pragma once

#include <vector>
#include <map>

#include "smtp_utils.hpp"

using Bytes = std::vector<char>;

struct ParserRequest {
    std::string message;

    explicit ParserRequest(const Bytes& bytes) : message(bytes.begin(), bytes.end()) {}
    explicit ParserRequest(const std::string& bytes) : message(bytes) {}
};

namespace {
static const std::string DATA_END_TOKEN = "\r\n.\r\n";
static const std::map<std::string, SMTPCommandType> string_to_token{{"helo ", SMTPCommandType::HELO},
                                                                    {"mail from:", SMTPCommandType::MAIL},
                                                                    {"rcpt to:", SMTPCommandType::RCPT},
                                                                    {"data", SMTPCommandType::DATA_BEGIN},
                                                                    {"quit", SMTPCommandType::QUIT}};
}  // namespace

class AbstractParser {
  public:
    virtual ~AbstractParser() = default;

    virtual std::vector<SMTPCommand> accept(const ParserRequest& message) = 0;

  protected:
    enum class BufferStatus { COMPLETE, INCOMPLETE };
    std::string delemiter;
    std::string m_buffer;
};