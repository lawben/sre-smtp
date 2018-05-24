#include "mail_parser.hpp"

ParserStatus MailParser::accept(const ParserRequest& request) {
    m_buffer.append(request);
    // if no type, search for identifier

    return parser_status();
};

SMTPCommand MailParser::get_command() {
    if (m_command.type == SMTPCommandType::INVALID) extract_command_type();
    extract_command_type();
    extract_data();
    return std::move(m_command);
};

ParserStatus MailParser::parser_status() {
    const auto pos = m_buffer.find(m_delemiter);

    if (pos == std::string::npos) return ParserStatus::INCOMPLETE;
    if (pos + m_delemiter.size() == m_buffer.size()) return ParserStatus::COMPLETE;
    return ParserStatus::TO_LONG;
}

void MailParser::extract_command_type() {
    for (const auto& conversion : m_string_to_tokentype) {
        std::string prefix = m_buffer.substr(0, conversion.first.size());
        std::transform(prefix.begin(), prefix.end(), prefix.begin(), ::tolower);

        if (conversion.first == prefix) {
            m_command.type = conversion.second;
            m_buffer.erase(0, conversion.first.size());
            break;
        }
    }
}

void MailParser::extract_data() {
    const auto end = m_buffer.find(m_delemiter);
    m_buffer.erase(end);
    m_command.data = m_buffer;
    m_buffer.erase(0);
}