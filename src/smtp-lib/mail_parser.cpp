#include <cassert>
#include <stdexcept>
#include <map>

#include "mail_parser.hpp"

namespace {
static const std::string CRLF_TOKEN = "\r\n";
static const std::string DATA_END_TOKEN = "\r\n.\r\n";
static const std::map<std::string, SMTPCommandType> string_to_token{
    {"HELO ", SMTPCommandType::HELO}, {"MAIL FROM:", SMTPCommandType::MAIL}, {"RCPT TO:", SMTPCommandType::RCPT},
    {"DATA", SMTPCommandType::DATA_BEGIN}, {"QUIT", SMTPCommandType::QUIT}};
}  // namespace

std::vector<SMTPCommand> MailParser::accept(const ParserRequest& request, SimplifiedSMTPState state) {
    m_buffer.append(request.message);
    std::vector<SMTPCommand> responses;

    while (get_buffer_status(state) == BufferStatus::COMPLETE) {
        const auto command = parse_buffer(state);
        responses.push_back(command);
    }

    return responses;
}

MailParser::BufferStatus MailParser::get_buffer_status(SimplifiedSMTPState state) {
    switch (state) { 
        case SimplifiedSMTPState::ENVELOPE:
            return m_buffer.find(CRLF_TOKEN) == std::string::npos ? BufferStatus::INCOMPLETE : BufferStatus::COMPLETE;
        case SimplifiedSMTPState::CONTENT:
            return m_buffer.find(DATA_END_TOKEN) == std::string::npos ? BufferStatus::INCOMPLETE : BufferStatus::COMPLETE;
        default:
            throw std::runtime_error("Case not implemented.");
    }
}

SMTPCommand MailParser::parse_buffer(SimplifiedSMTPState state) {
    switch (state) {
        case SimplifiedSMTPState::ENVELOPE:
            return parse_envelope_buffer();
        case SimplifiedSMTPState::CONTENT:
            return parse_content_buffer();
        default:
            throw std::runtime_error("Case not implemented.");
    }
}

SMTPCommand MailParser::parse_envelope_buffer() {
    std::pair<std::string, SMTPCommandType> token{"", SMTPCommandType::INVALID};
    auto token_position = std::string::npos;

    // This only works as long as no token is a prefix of another token
    for (const auto& conversion : string_to_token) {
        token_position = m_buffer.find(conversion.first);
        if (token_position != std::string::npos) {
            token = conversion;
            break;
        }
    }

    // The token ALWAYS has to be the first thing we find
    if (token_position != 0) {
        token_position = 0;
        token = {"", SMTPCommandType::INVALID};
    }

    // TODO: check if the data we carry is data we want / if its valid
    const auto end = m_buffer.find(CRLF_TOKEN);
    const auto data = m_buffer.substr(token_position + token.first.length(), end - token.first.length());
    m_buffer.erase(0, end + CRLF_TOKEN.length());

    return {token.second, data};
}

SMTPCommand MailParser::parse_content_buffer() {
    auto data_end_position = m_buffer.find(DATA_END_TOKEN);

    // This case should never occur, because we checked wheter the buffer is complete before
    if (data_end_position == std::string::npos) {
        throw std::runtime_error("Implementation is broken!");
    }

    const auto data = m_buffer.substr(0, data_end_position);
    m_buffer.erase(0, data_end_position + DATA_END_TOKEN.length());

    return {SMTPCommandType::DATA, data};
}
