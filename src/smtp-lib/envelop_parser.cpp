#include "envelop_parser.hpp"

#include <algorithm>

std::vector<SMTPCommand> EnvelopParser::accept(const ParserRequest& request) {
    m_buffer.append(request.message);
	
	std::vector<SMTPCommand> responses;
    if (find_delemiter()) {
        const auto command = parse_buffer();
        responses.push_back(command);
    }

	return responses;
}

SMTPCommand EnvelopParser::parse_buffer() {
    SMTPCommand command;
    command.type = SMTPCommandType::INVALID;
    command.data = "";

    for (const auto& conversion : string_to_token) {
        std::string prefix = m_buffer.substr(0, conversion.first.size());
        std::transform(prefix.begin(), prefix.end(), prefix.begin(), ::tolower);

        if (conversion.first == prefix) {
            command.type = conversion.second;
            command.data = conversion.first;
            break;
        }
    }

    const auto end = m_buffer.find(NEWLINE_TOKEN);
    const auto data = m_buffer.substr(command.data.length(), end - command.data.length());
    m_buffer.erase(0, end + NEWLINE_TOKEN.length());
    command.data = data;
    return command;
}

bool EnvelopParser::find_delemiter() { return m_buffer.find(NEWLINE_TOKEN) != std::string::npos; }
