#include <cassert>
#include <stdexcept>

#include <mail_parser.hpp>

namespace {
static const std::string CRLF_TOKEN = "\r\n";
static const std::string DATA_END_TOKEN = "\r\n.\r\n";
static const std::map<SMTPCommandType, SMTPCommandProps> command_to_props {
        {HELO, {"HELO", true}},
        {MAIL, {"MAIL", true}},
        {RCPT, {"RCPT", true}},
        {DATA, {"DATA", false}},
        {QUIT, {"QUIT", false}},
        {RSET, {"RSET", false}},
};
}  // namespace

std::vector<SMTPCommand> MailParser::accept(const ParserRequest& request) {
    m_buffer.append(request.message);
    std::vector<SMTPCommand> responses;
    BufferLine next_line = parse_buffer(m_buffer);
    while (next_line.status != BufferStatus::INCOMPLETE) {
        SMTPCommand command;
        if (m_is_data_state) {
            command = {DATA_BODY, next_line.line};
            m_is_data_state = false;
        } else {
            command = line_to_command(next_line.line);
            if (command.type == SMTPCommandType::DATA) {
                m_is_data_state = true;
            }
        }
        responses.push_back(command);
        next_line = parse_buffer(m_buffer);
    }
    return responses;
}

MailParser::BufferLine MailParser::parse_buffer(std::string& buffer) {
    if (m_is_data_state) {
        return parse_data_buffer(buffer);
    }
    return parse_line_buffer(buffer);
}

MailParser::BufferLine MailParser::parse_data_buffer(std::string& buffer) {
    if (buffer.size() < DATA_END_TOKEN.size() ||
            buffer.compare(buffer.size() - DATA_END_TOKEN.size(), buffer.size(), DATA_END_TOKEN) != 0) {
        return {BufferStatus::INCOMPLETE, ""};
    }
    std::string data_body = buffer.substr(0, buffer.size() - DATA_END_TOKEN.size());
    buffer.erase();
    return {BufferStatus::COMPLETE, data_body};
}

MailParser::BufferLine MailParser::parse_line_buffer(std::string& buffer) {
    size_t crlf_index = buffer.find(CRLF_TOKEN);
    if (crlf_index == std::string::npos) {
        return {BufferStatus::INCOMPLETE, ""};
    }
    std::string line = buffer.substr(0, crlf_index);
    buffer.erase(0, crlf_index + CRLF_TOKEN.length());
    return {BufferStatus::COMPLETE, line};
}

SMTPCommand MailParser::line_to_command(std::string& line) {
    std::vector<std::string> tokens = tokenize(line);
    if (tokens.empty()) {
        throw std::runtime_error("Invalid command string: " + line);
    }
    std::string command_id = tokens[0];
    for (auto elem : command_to_props) {
        SMTPCommandType type = elem.first;
        SMTPCommandProps props = elem.second;
        if (props.identifier != command_id) {
            continue;
        }
        std::string body;
        if (props.has_data) {
            for (size_t i = 1; i < tokens.size(); i++) {
                body += tokens[i];
            }
        }
        return {type, body};
    }
    throw std::runtime_error("Invalid command identifier: " + command_id);
}

std::vector<std::string> MailParser::tokenize(const std::string& input) {
    std::istringstream iss(input);
    std::vector<std::string> tokens;
    std::copy(std::istream_iterator<std::string>(iss),
         std::istream_iterator<std::string>(),
         back_inserter(tokens));
    return tokens;
}
