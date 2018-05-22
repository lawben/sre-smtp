#include <algorithm>
#include <cassert>
#include <map>
#include <stdexcept>

#include "mail_parser.hpp"

namespace {
static const std::string DATA_END_TOKEN = "\r\n.\r\n";
static const std::map<std::string, SMTPCommandType> string_to_token{{"helo ", SMTPCommandType::HELO},
                                                                    {"mail from:", SMTPCommandType::MAIL},
                                                                    {"rcpt to:", SMTPCommandType::RCPT},
                                                                    {"data", SMTPCommandType::DATA_BEGIN},
                                                                    {"quit", SMTPCommandType::QUIT}};
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
    size_t token_position = std::string::npos;
    switch (state) {
        case SimplifiedSMTPState::ENVELOPE:
            token_position = m_buffer.find(NEWLINE_TOKEN);
            break;
        case SimplifiedSMTPState::CONTENT:
            token_position = m_buffer.find(DATA_END_TOKEN);
            break;
        default:
            throw std::runtime_error("Case not implemented.");
    }
    return token_position == std::string::npos ? BufferStatus::INCOMPLETE : BufferStatus::COMPLETE;
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

    // This only works as long as no token is a prefix of another token
    // Check if the string representation of one token is present in the buffer
    for (const auto& conversion : string_to_token) {
        // Only consider this token if we both found it and its at the start
        // This prevents cases like: MAIL FROM:<HELO@test.com>
        std::string command = m_buffer.substr(0, conversion.first.size());
        std::transform(command.begin(), command.end(), command.begin(), ::tolower);

		if (conversion.first == command)
		{
            token = conversion;
			break;
		}
    }

    // TODO: check if the data we carry is data we want / if its valid
    const auto end = m_buffer.find(NEWLINE_TOKEN);
    const auto data = m_buffer.substr(token.first.length(), end - token.first.length());
    m_buffer.erase(0, end + NEWLINE_TOKEN.length());

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
