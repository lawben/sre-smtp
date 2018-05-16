#include "mail_state_machine.hpp"

#include <map>


static const std::multimap<SMTPState, SMTPCommandType> accepted_commands{
    {SMTPState::CLIENT_INIT, SMTPCommandType::HELO},
    {SMTPState::MAIL_FROM, SMTPCommandType::MAIL},
    {SMTPState::RCPT_TO, SMTPCommandType::RCPT},
    {SMTPState::RCPT_TO_OR_DATA_BEGIN, SMTPCommandType::RCPT},
    {SMTPState::RCPT_TO_OR_DATA_BEGIN, SMTPCommandType::DATA_BEGIN},
    {SMTPState::DATA_CONTENT, SMTPCommandType::DATA},
    {SMTPState::QUIT, SMTPCommandType::QUIT}
};

MailStateMachine::MailStateMachine()
    : m_state(SMTPState::CLIENT_INIT)
{

}

SMTPState MailStateMachine::current_state() const {
    return m_state;
}

SimplifiedSMTPState MailStateMachine::current_simplified_state() const {
    if (m_state == SMTPState::DATA_CONTENT) {
        return SimplifiedSMTPState::CONTENT;
    }
    return SimplifiedSMTPState::ENVELOPE;
}

SMTPResponse MailStateMachine::accept(const SMTPCommand& command) {
    bool valid_command = false;

    auto commands = accepted_commands.equal_range(m_state);
    for (auto it = commands.first; it != commands.second; ++it) {
        if (it->second == command.type) {
            valid_command = true;
            break;
        }
    }

    if (!valid_command) {
        return create_invalid_response();
    }

    handle_command(command);
    advance_state(command);

    return create_valid_response(command);
}

void MailStateMachine::handle_command(const SMTPCommand& command) {
    // TODO: maybe create mail here?
    (void) command;
}

void MailStateMachine::advance_state(const SMTPCommand& command) {
    // There is probably a better way to do this, since this is quite error prone
    // Or we need really good tests
    switch (m_state) {
        case SMTPState::CLIENT_INIT:
            m_state = SMTPState::MAIL_FROM;
            break;
        case SMTPState::MAIL_FROM:
            m_state = SMTPState::RCPT_TO;
            break;
        case SMTPState::RCPT_TO:
            m_state = SMTPState::RCPT_TO_OR_DATA_BEGIN;
            break;
        case SMTPState::RCPT_TO_OR_DATA_BEGIN:
            if (command.type == SMTPCommandType::DATA_BEGIN) {
                m_state = SMTPState::DATA_CONTENT;
            }
            break;
        case SMTPState::DATA_CONTENT:
            m_state = SMTPState::QUIT;
            break;
        case SMTPState::QUIT:
            m_state = SMTPState::FINISHED;
            break;
        default:
            throw std::runtime_error("Case not implemented.");
    }
}

SMTPResponse MailStateMachine::create_valid_response(const SMTPCommand& command) {
    switch (command.type) {
        case SMTPCommandType::DATA_BEGIN:
            return {354, "End Data with <CR><LF>.<CR><LF>\r\n"};
        case SMTPCommandType::QUIT:
            return {221, "Bye!\r\n"};
        default:
            return {250, "OK\r\n"};
    }    
}

SMTPResponse MailStateMachine::create_invalid_response() {
    return {500, "Invalid command!\r\n"};
}
