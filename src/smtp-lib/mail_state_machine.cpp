#include "mail_state_machine.hpp"

#include <map>

static const std::multimap<SMTPState, SMTPCommandType> accepted_commands{
    {SMTPState::CLIENT_INIT, SMTPCommandType::HELO},
    {SMTPState::CLIENT_INIT, SMTPCommandType::HELO},
    {SMTPState::CLIENT_INIT, SMTPCommandType::RSET},
    {SMTPState::CLIENT_INIT, SMTPCommandType::QUIT},
    {SMTPState::MAIL_FROM, SMTPCommandType::MAIL},
    {SMTPState::MAIL_FROM, SMTPCommandType::HELO},
    {SMTPState::MAIL_FROM, SMTPCommandType::RSET},
    {SMTPState::MAIL_FROM, SMTPCommandType::QUIT},
    {SMTPState::RCPT_TO, SMTPCommandType::RCPT},
    {SMTPState::RCPT_TO, SMTPCommandType::HELO},
    {SMTPState::RCPT_TO, SMTPCommandType::RSET},
    {SMTPState::RCPT_TO, SMTPCommandType::QUIT},
    {SMTPState::RCPT_TO_OR_DATA_BEGIN, SMTPCommandType::RCPT},
    {SMTPState::RCPT_TO_OR_DATA_BEGIN, SMTPCommandType::DATA_BEGIN},
    {SMTPState::RCPT_TO_OR_DATA_BEGIN, SMTPCommandType::HELO},
    {SMTPState::RCPT_TO_OR_DATA_BEGIN, SMTPCommandType::RSET},
    {SMTPState::RCPT_TO_OR_DATA_BEGIN, SMTPCommandType::QUIT},
    {SMTPState::DATA_CONTENT, SMTPCommandType::DATA},
    {SMTPState::QUIT, SMTPCommandType::QUIT}};

MailStateMachine::MailStateMachine() : m_state(SMTPState::CLIENT_INIT) {}

SMTPState MailStateMachine::current_state() const { return m_state; }

SimplifiedSMTPState MailStateMachine::current_simplified_state() const {
    if (m_state == SMTPState::DATA_CONTENT) {
        return SimplifiedSMTPState::CONTENT;
    }
    return SimplifiedSMTPState::ENVELOPE;
}

bool MailStateMachine::accept(const SMTPCommandType& type) {
    if (is_valid_command(type)) {
        advanced_state(type);
        return true;
    } else {
        return false;
	}
}

bool MailStateMachine::is_valid_command(const SMTPCommandType& type) {
    const auto commands = accepted_commands.equal_range(m_state);
    for (auto it = commands.first; it != commands.second; ++it) {
        if (it->second == type) {
            return true;
        }
    }

    return false;
}

void MailStateMachine::advanced_state(const SMTPCommandType& type) {
    switch (type) {
        case SMTPCommandType::HELO:
            m_state = SMTPState::MAIL_FROM;
            break;
        case SMTPCommandType::MAIL:
            m_state = SMTPState::RCPT_TO;
            break;
        case SMTPCommandType::RCPT:
            m_state = SMTPState::RCPT_TO_OR_DATA_BEGIN;
            break;
        case SMTPCommandType::DATA_BEGIN:
            m_state = SMTPState::DATA_CONTENT;
            break;
        case SMTPCommandType::DATA:
            m_state = SMTPState::MAIL_FROM;
            break;
        case SMTPCommandType::RSET:
            m_state = SMTPState::MAIL_FROM;
            break;
        case SMTPCommandType::QUIT:
            m_state = SMTPState::FINISHED;
            break;
        default:
            throw std::runtime_error("Case not implemented.");
    }
}

SMTPResponse MailStateMachine::create_valid_response(const SMTPCommandType& type) {
    switch (type) {
        case SMTPCommandType::DATA_BEGIN:
            return {354, "End Data with <CR><LF>.<CR><LF>"};
        case SMTPCommandType::QUIT:
            return {221, "Bye!"};
        default:
            return {250, "OK"};
    }
}

SMTPResponse MailStateMachine::create_invalid_response() { return {500, "Invalid command!"}; }
