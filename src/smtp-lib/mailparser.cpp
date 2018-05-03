#include <stdexcept>
#include "mailparser.h"

MailParser::MailParser() = default;

MailParser::MailParser(const ParserState &state) : m_state(state) { }

bool MailParser::isTerminated() {
    return m_state.status == TERMINATED;
}

std::vector<Mail> MailParser::getMails() {
    return m_state.mails;
}

ParserResponse MailParser::accept(const ParserRequest& request) {
    switch (m_state.status) {
        case WAIT_INIT:
            return handleInit(request);
        case WAIT_HELO:
            return handleHelo(request);
        case WAIT_MAIL_FROM:
            return handleMailFrom(request);
        case WAIT_RCPT_FROM:
            return handleRcptFrom(request);
        case WAIT_DATA_BEGIN:
            return handleDataBegin(request);
        case WAIT_DATA_END:
            return handleData(request);
        case TERMINATED:
            throw std::invalid_argument("Connection already terminated");
        default:
            throw std::invalid_argument("Invalid status: " + m_state.status);
    }
}

ParserResponse MailParser::handleInit(const ParserRequest &request) {
    m_state.status = WAIT_HELO;
    return {220, "smtp.example.com ESMTP Postfix"};
}

ParserResponse MailParser::handleHelo(const ParserRequest &request) {
    // TODO: validate Helo message
    // C: HELO relay.example.com
    m_state.status = WAIT_MAIL_FROM;
    return {250, "smtp.example.com, Hello from me"};
}

ParserResponse MailParser::handleMailFrom(const ParserRequest &request) {
    // TODO: validate mail from
    // C: MAIL FROM:<bob@example.com>
    getCurrentMail().mailFrom = "bob@example.com";
    m_state.status = WAIT_RCPT_FROM;
    return {250, "ok"};
}

ParserResponse MailParser::handleRcptFrom(const ParserRequest &request) {
    // TODO: validate rcpt from
    // C: RCPT TO:<alice@example.com>
    getCurrentMail().rcptFrom = "alice@example.com";
    m_state.status = WAIT_DATA_BEGIN;
    return {250, "ok"};
}

ParserResponse MailParser::handleDataBegin(const ParserRequest &request) {
    // TODO: validate data begin (DATA or QUIT)
    if (request.message == "QUIT") {
        m_state.status = TERMINATED;
        return {221, "Bye"};
    }
    m_state.mails.push_back({});
    m_state.status = WAIT_DATA_END;
    return {354, "End data with <CR><LF>.<CR><LF>"};
}

ParserResponse MailParser::handleData(const ParserRequest &request) {
    // TODO: check for data termination
    if (request.message == ".") {
        m_state.status = WAIT_DATA_BEGIN;
        return {250, "ok: queued"};
    }
    getCurrentMail().content.push_back(request.message);
    return {250, "ok"}; // TODO: check response on data row
}

Mail MailParser::getCurrentMail() {
    if (m_state.mails.empty()) {
        throw std::runtime_error("Invalid state, no mails recieved");
    }
    return m_state.mails[m_state.mails.size() - 1];
}
