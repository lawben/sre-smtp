#include <cassert>
#include <stdexcept>

#include <mail_parser.hpp>

namespace {
static const std::string CRLF_TOKEN = "\r\n";
static const std::string HELO_TOKEN = "HELO";
static const std::string EHLO_TOKEN = "EHLO";
static const std::string FROM_TOKEN = "MAIL FROM:";
static const std::string TO_TOKEN = "RCPT TO:";
static const std::string DATA_BEGIN_TOKEN = "DATA";
static const std::string DATA_END_TOKEN = "\r\n.\r\n";
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
    size_t clrf_index = buffer.find(CRLF_TOKEN);
    if (clrf_index == std::string::npos) {
        return {BufferStatus::INCOMPLETE, ""};
    }
    size_t line_length = clrf_index + CRLF_TOKEN.length();
    std::string line = buffer.substr(0, line_length);
    buffer.erase(0, line_length);
    return {BufferStatus::COMPLETE, line};
}
