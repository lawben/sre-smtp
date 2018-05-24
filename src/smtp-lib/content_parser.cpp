#include "content_parser.hpp"

#include "abstract_parser.hpp"

namespace {
static const std::string DATA_END_TOKEN = "\r\n.\r\n";
}

ContentParser::ContentParser() : AbstractParser(DATA_END_TOKEN) {}

ParserStatus ContentParser::accept(const ParserRequest& request) {
    m_buffer.append(request);
    // take only new data into account

    if (find_delemiter()) {
        // check if buffer is longer then expected
        return ParserStatus::COMPLETE;
    }

    return ParserStatus::INCOMPLETE;
}

SMTPCommand ContentParser::get_command() {
    auto data_end_position = m_buffer.find(DATA_END_TOKEN);

    // This case should never occur, because we checked wheter the buffer is complete before
    if (data_end_position == std::string::npos) {
        throw std::runtime_error("Command sequence is not finished.");
    }

    const auto data = m_buffer.substr(0, data_end_position);
    m_buffer = "";

    return {SMTPCommandType::DATA, data};
}
