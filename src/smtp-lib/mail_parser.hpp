#pragma once

#include <string>
#include <vector>
#include "utils.hpp"
#include <raw_socket.hpp>
#include <smtp_helper.hpp>


struct ParserRequest {
    std::string message;

    explicit ParserRequest(const Bytes& bytes) : message(bytes.begin(), bytes.end()) {}
    explicit ParserRequest(const std::string& bytes) : message(bytes) {}
};

class MailParser : public NonCopyable {
  public:
    MailParser() = default;

    std::vector<SMTPCommand> accept(const ParserRequest& message, SimplifiedSMTPState state);

  private:
    enum class BufferStatus { COMPLETE, INCOMPLETE };

    struct BufferLine {
        BufferStatus status;
        std::string line;
    };

    BufferStatus get_buffer_status(SimplifiedSMTPState state);
    SMTPCommand parse_buffer(SimplifiedSMTPState state);
    SMTPCommand parse_envelope_buffer();
    SMTPCommand parse_content_buffer();

    std::string m_buffer;
};
