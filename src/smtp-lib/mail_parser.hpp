#pragma once

#include <string>
#include <vector>
#include "raw_socket.hpp"
#include "utils.hpp"

struct ParserRequest {
    std::string message;

    explicit ParserRequest(const Bytes& bytes) : message(bytes.begin(), bytes.end()) {}
    explicit ParserRequest(const std::string& bytes) : message(bytes) {}
};

struct ParserResponse {
    std::string message;
};

class MailParser : public NonCopyable {
  public:
    MailParser() = default;

    std::vector<ParserResponse> accept(const ParserRequest& message);

  private:
    enum class BufferStatus { COMPLETE, INCOMPLETE };

    struct BufferInformation {
        BufferStatus status;
        std::string line;
    };

    BufferInformation parse_buffer(std::string& buffer);
    BufferInformation parse_data_buffer(std::string& buffer);
    BufferInformation parse_line_buffer(std::string& buffer);

    std::string m_buffer;
    bool m_is_data_state = false;
};
