#include "envelop_parser.hpp"

#include <algorithm>
#include <map>

namespace {
static const std::map<std::string, SMTPCommandType> string_to_token{{"helo ", SMTPCommandType::HELO},
                                                                    {"mail from:", SMTPCommandType::MAIL},
                                                                    {"rcpt to:", SMTPCommandType::RCPT},
                                                                    {"data", SMTPCommandType::DATA_BEGIN},
                                                                    {"quit", SMTPCommandType::QUIT}};
}  // namespace

EnvelopParser::EnvelopParser() : AbstractParser(NEWLINE_TOKEN) {}

ParserStatus EnvelopParser::accept(const ParserRequest& request) {
    m_buffer.append(request);
	// if no type, search for identifier

    if (find_delemiter()) {
        // check if buffer is longer then expected
        return ParserStatus::COMPLETE;
    }

    return ParserStatus::INCOMPLETE;
}

SMTPCommand EnvelopParser::get_command() { return parse_buffer(); }

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

    const auto end = m_buffer.find(m_delemiter);
    command.data = m_buffer.substr(command.data.length(), end - command.data.length());
    m_buffer = "";
    return command;
}

