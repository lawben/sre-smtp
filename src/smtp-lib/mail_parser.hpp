#pragma once

#include <map>
#include <string>
#include <vector>
#include "raw_socket.hpp"
#include "utils.hpp"

enum SMTPCommandType {
    HELO,
    MAIL,
    RCPT,
    DATA,
    QUIT,
    RSET,

    DATA_BODY  // Custom type for handling the mail data body
};

struct SMTPCommand {
    SMTPCommandType type;
    std::string data;
};

struct SMTPCommandProps {
    std::string identifier;
    bool has_data;
};

struct ParserRequest {
    std::string message;

    explicit ParserRequest(const Bytes& bytes) : message(bytes.begin(), bytes.end()) {}
    explicit ParserRequest(const std::string& bytes) : message(bytes) {}
};

class MailParser {
  public:
    MailParser() = default;

    std::vector<SMTPCommand> accept(const ParserRequest& message);

  private:
    enum class BufferStatus { COMPLETE, INCOMPLETE };

    struct BufferLine {
        BufferStatus status;
        std::string line;
    };

    BufferLine parse_buffer(std::string& buffer);
    BufferLine parse_data_buffer(std::string& buffer);
    BufferLine parse_line_buffer(std::string& buffer);
    SMTPCommand line_to_command(std::string& line);
    std::vector<std::string> tokenize(const std::string& input);

    std::string m_buffer;
    bool m_is_data_state = false;
};
