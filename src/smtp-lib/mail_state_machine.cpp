#include "mail_state_machine.hpp"

#include <map>

static const std::multimap<SMTPState, SMTPCommandType> accepted_commands{
    {SMTPState::CLIENT_INIT, SMTPCommandType::HELO},
    {SMTPState::MAIL_FROM, SMTPCommandType::MAIL},
    {SMTPState::RCPT_TO, SMTPCommandType::RCPT},
    {SMTPState::RCPT_TO_OR_DATA_BEGIN, SMTPCommandType::RCPT},
    {SMTPState::RCPT_TO_OR_DATA_BEGIN, SMTPCommandType::DATA_BEGIN},
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

SMTPResponse MailStateMachine::accept(const SMTPCommand& command) {
    const auto valid_command = is_valid_command(command);

    if (!valid_command) {
        return create_invalid_response();
    }

    handle_command(command);
    m_state = advanced_state(command);

    return create_valid_response(command);
}

bool MailStateMachine::is_valid_command(const SMTPCommand& command) {
    const auto commands = accepted_commands.equal_range(m_state);
    for (auto it = commands.first; it != commands.second; ++it) {
        if (it->second == command.type) {
            return true;
        }
    }

    return false;
}

void MailStateMachine::handle_command(const SMTPCommand& command) {
    // TODO: maybe create mail here?
    (void)command;
}

SMTPState MailStateMachine::advanced_state(const SMTPCommand& command) {
    // There is probably a better way to do this, since this is quite error prone
    // Or we need really good tests
    switch (m_state) {
        case SMTPState::CLIENT_INIT:
            return SMTPState::MAIL_FROM;
        case SMTPState::MAIL_FROM:
            return SMTPState::RCPT_TO;
        case SMTPState::RCPT_TO:
            return SMTPState::RCPT_TO_OR_DATA_BEGIN;
        case SMTPState::RCPT_TO_OR_DATA_BEGIN:
            if (command.type == SMTPCommandType::DATA_BEGIN) {
                return SMTPState::DATA_CONTENT;
            }
            return SMTPState::RCPT_TO_OR_DATA_BEGIN;
        case SMTPState::DATA_CONTENT:
            return SMTPState::QUIT;
        case SMTPState::QUIT:
            return SMTPState::FINISHED;
        default:
            throw std::runtime_error("Case not implemented.");
    }
}

SMTPResponse MailStateMachine::create_valid_response(const SMTPCommand& command) {
    switch (command.type) {
        case SMTPCommandType::DATA_BEGIN:
            return {354, "End Data with <CR><LF>.<CR><LF>"};
        case SMTPCommandType::QUIT:
            return {221, "Bye!"};
        default:
            return {250, "OK"};
    }
}

SMTPResponse MailStateMachine::create_invalid_response() { return {500, "Invalid command!"}; }
