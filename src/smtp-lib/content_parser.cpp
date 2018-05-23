#include "content_parser.hpp"

std::vector<SMTPCommand> ContentParser::accept(const ParserRequest& request) {
    m_buffer.append(request.message);

    std::vector<SMTPCommand> responses;
    if (find_delemiter()) {
        const auto command = parse_buffer();
        responses.push_back(command);
    }

    return responses;
}

SMTPCommand ContentParser::parse_buffer() {
    auto data_end_position = m_buffer.find(DATA_END_TOKEN);

    // This case should never occur, because we checked wheter the buffer is complete before
    if (data_end_position == std::string::npos) {
        throw std::runtime_error("Implementation is broken!");
    }

    const auto data = m_buffer.substr(0, data_end_position);
    m_buffer.erase(0, data_end_position + DATA_END_TOKEN.length());

    return {SMTPCommandType::DATA, data};
}

bool ContentParser::find_delemiter() { return m_buffer.find(DATA_END_TOKEN) != std::string::npos; }
