#include <stdexcept>
#include "mailparser.h"

MailParser::MailParser() = default;

MailParser::MailParser(const State &state) : m_state(state) { }

bool MailParser::isTerminated() {
    return m_state.status == TERMINATED;
}

Response MailParser::accept(const Request& request) {
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

Response MailParser::handleInit(const Request &request) {
    m_state.status = WAIT_HELO;
    return {220, "smtp.example.com ESMTP Postfix"};
}

Response MailParser::handleHelo(const Request &request) {
    // TODO: validate Helo message
    // C: HELO relay.example.com
    m_state.status = WAIT_MAIL_FROM;
    return {250, "smtp.example.com, Hello from me"};
}

Response MailParser::handleMailFrom(const Request &request) {
    // TODO: validate mail from
    // C: MAIL FROM:<bob@example.com>
    m_state.mailFrom = "bob@example.com";
    m_state.status = WAIT_RCPT_FROM;
    return {250, "ok"};
}

Response MailParser::handleRcptFrom(const Request &request) {
    // TODO: validate rcpt from
    // C: RCPT TO:<alice@example.com>
    m_state.rcptFrom = "alice@example.com";
    m_state.status = WAIT_DATA_BEGIN;
    return {250, "ok"};
}

Response MailParser::handleDataBegin(const Request &request) {
    if (request.message == "QUIT") {
        m_state.status = TERMINATED;
        return {221, "Bye"};
    }
    m_state.status = WAIT_DATA_END;
    return {354, "End data with <CR><LF>.<CR><LF>"};
}

Response MailParser::handleData(const Request &request) {
    // TODO: check for data termination
    if (request.message == ".") {
        m_state.status = WAIT_DATA_BEGIN;
        return {250, "ok: queued"};
    }
    m_state.content.push_back(request.message);
    return {250, "ok"}; // TODO: check response on data row
}
