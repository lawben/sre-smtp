#include <stdexcept>
#include "mail_parser.hpp"

MailParser::MailParser() : MailParser(ParserState{}) {}
MailParser::MailParser(const ParserState& state) : m_state(state) {}

bool MailParser::is_terminated() const {
  return m_state.status == TERMINATED;
}

const std::vector<Mail>& MailParser::get_mails() const {
  return m_state.mails;
}

ParserResponse MailParser::accept(const ParserRequest& request) {
  switch (m_state.status) {
    case WAIT_INIT:
      return handle_init(request);
    case WAIT_HELO:
      return handle_helo(request);
    case WAIT_MAIL_FROM:
      return handle_mail_from(request);
    case WAIT_RCPT_FROM:
      return handle_rcpt_from(request);
    case WAIT_DATA_BEGIN:
      return handle_data_begin(request);
    case WAIT_DATA_END:
      return handle_data(request);
    case TERMINATED:
      throw std::invalid_argument("Connection already terminated");
    default:
      throw std::invalid_argument("Invalid status: " + std::to_string(m_state.status));
    }
}

ParserResponse MailParser::handle_init(const ParserRequest& /*request*/) {
  m_state.status = WAIT_HELO;
  return {220, "smtp.example.com ESMTP Postfix"};
}

ParserResponse MailParser::handle_helo(const ParserRequest& /*request*/) {
  // TODO: validate Helo message
  // C: HELO relay.example.com
  m_state.status = WAIT_MAIL_FROM;
  return {250, "smtp.example.com, Hello from me"};
}

ParserResponse MailParser::handle_mail_from(const ParserRequest& /*request*/) {
  // TODO: validate mail from
  // C: MAIL FROM:<bob@example.com>
  get_current_mail()->mail_from = "bob@example.com";
  m_state.status = WAIT_RCPT_FROM;
  return {250, "ok"};
}

ParserResponse MailParser::handle_rcpt_from(const ParserRequest& /*request*/) {
  // TODO: validate rcpt from
  // C: RCPT TO:<alice@example.com>
  get_current_mail()->rcpt_from = "alice@example.com";
  m_state.status = WAIT_DATA_BEGIN;
  return {250, "ok"};
}

ParserResponse MailParser::handle_data_begin(const ParserRequest& request) {
  // TODO: validate data begin (DATA or QUIT)
  if (request.message == "QUIT") {
    m_state.status = TERMINATED;
    return {221, "Bye"};
  }
  m_state.mails.push_back({});
  m_state.status = WAIT_DATA_END;
  return {354, "End data with <CR><LF>.<CR><LF>"};
}

ParserResponse MailParser::handle_data(const ParserRequest& request) {
  // TODO: check for data termination
  if (request.message == ".") {
    m_state.status = WAIT_DATA_BEGIN;
    return {250, "ok: queued"};
  }
  get_current_mail()->content.push_back(request.message);
  return {250, "ok"}; // TODO: check response on data row
}

Mail* MailParser::get_current_mail() {
  if (m_state.mails.empty()) {
    throw std::runtime_error("Invalid state, no mails received");
  }
  return &(m_state.mails.back());
}
