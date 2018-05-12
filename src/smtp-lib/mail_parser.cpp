#include <cassert>
#include <stdexcept>

#include <mail_parser.hpp>

namespace {
static const std::string CRLF_TOKEN = "\r\n";
static const std::string DATA_BEGIN_TOKEN = "DATA";
static const std::string DATA_END_TOKEN = "\r\n.\r\n";
static const std::string HELO_TOKEN = "HELO";
static const std::string EHLO_TOKEN = "EHLO";
static const std::string FROM_TOKEN = "MAIL FROM:";
static const std::string TO_TOKEN = "RCPT TO:";
static const std::string QUIT_TOKEN = "QUIT";
}  // namespace

std::vector<ParserResponse> MailParser::accept(const ParserRequest& request) {
    m_buffer.append(request.message);
    std::vector<ParserResponse> responses;
    BufferInformation bufferInformation = parse_buffer(m_buffer);
    while (bufferInformation.status != BufferStatus::INCOMPLETE) {
        responses.push_back({bufferInformation.line});
        bufferInformation = parse_buffer(m_buffer);
    }
    return responses;
}

MailParser::BufferInformation MailParser::parse_buffer(std::string& buffer) {
    if (m_is_data_state) {
        return parse_data_buffer(buffer);
    }
    return parse_line_buffer(buffer);
}

MailParser::BufferInformation MailParser::parse_data_buffer(std::string &buffer) {
    if (buffer.size() < DATA_END_TOKEN.size() ||
            buffer.compare(buffer.size() - DATA_END_TOKEN.size(), buffer.size(), DATA_END_TOKEN) != 0) {
        return {BufferStatus::INCOMPLETE, ""};
    }
    std::string data_body = buffer.substr(0, buffer.size() - DATA_END_TOKEN.size());
    buffer.erase();
    m_is_data_state = false;
    return {BufferStatus::COMPLETE, data_body};
}

MailParser::BufferInformation MailParser::parse_line_buffer(std::string &buffer) {
    size_t crlf_index = buffer.find(CRLF_TOKEN);
    if (crlf_index == std::string::npos) {
        return {BufferStatus::INCOMPLETE, ""};
    }
    std::string line = buffer.substr(0, crlf_index);
    buffer.erase(0, crlf_index + CRLF_TOKEN.length());
    if (line == DATA_BEGIN_TOKEN) {
        m_is_data_state = true;
    }
    return {BufferStatus::COMPLETE, line};
}
