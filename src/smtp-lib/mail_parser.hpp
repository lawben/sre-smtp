#pragma once

#include <string>
#include <vector>
#include "raw_socket.hpp"
#include "utils.hpp"

struct Mail {
    std::string from;
    std::vector<std::string> to;
    std::string content;
};

struct ParserRequest {
    std::string message;

    explicit ParserRequest(const Bytes& bytes) : message(bytes.begin(), bytes.end()) {}
};

struct ParserResponse {
    int code;
    std::string message;
    bool is_error = false;

    Bytes to_bytes() const {
        const std::string code_string = std::to_string(code) + " ";  // for format '123 msg'
        Bytes bytes{code_string.begin(), code_string.end()};
        std::copy(message.begin(), message.end(), std::back_inserter(bytes));
        return bytes;
    }
};

class MailParser : public NonCopyable {
  public:
    MailParser() = default;

    ParserResponse accept(const ParserRequest& message);
    bool has_finished() const;

  private:
    enum class ParserStatus {
        CLIENT_INITIATION,
        FROM_FIELD,
        TO_FIELD,
        TO_FIELD_OR_DATA_BEGIN,
        DATA_CONTENT_OR_END,
        QUIT,
        FINISHED
    };

    enum class BufferStatus { COMPLETE, INCOMPLETE, INVALID };

    struct BufferInformation {
        BufferStatus status;
        size_t end;
    };

    BufferInformation parse_buffer(const std::string& token, bool at_start = true);

    ParserResponse handle_client_initiation(BufferInformation helo_info, BufferInformation ehlo_info);
    ParserResponse handle_from_field(BufferInformation info);
    ParserResponse handle_to_field(BufferInformation info);
    ParserResponse handle_to_and_data(BufferInformation toInfo, BufferInformation data_info);
    ParserResponse handle_data_begin(BufferInformation info);
    ParserResponse handle_data_end(BufferInformation info);
    ParserResponse handle_quit(BufferInformation info);

    size_t find_token(const std::string& token);

    ParserStatus m_state = ParserStatus::CLIENT_INITIATION;
    Bytes m_buffer;
    Mail m_mail;
};
