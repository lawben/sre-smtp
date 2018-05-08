#pragma once

#include <string>
#include <vector>
#include "utils.hpp"
#include "raw_socket.hpp"

enum ParserStatus {
  WAIT_INIT,
  WAIT_HELO,
  WAIT_MAIL_FROM,
  WAIT_RCPT_FROM,
  WAIT_DATA_BEGIN,
  WAIT_DATA_END,
  TERMINATED,
};

struct Mail {
  std::string mail_from;
  std::string rcpt_from;
  std::vector<std::string> content;
};

struct ParserState {
  ParserStatus status = ParserStatus::WAIT_INIT;
  std::vector<Mail> mails;
};

struct ParserRequest {
  std::string message;

  explicit ParserRequest(const Bytes& bytes) : message(bytes.begin(), bytes.end()) {}
};

struct ParserResponse {
  int code;
  std::string message;
  bool is_error = false;

  Bytes to_bytes() const {
    const std::string code_string = std::to_string(code) + " ";  // for format '123 msg'
    Bytes bytes{code_string.begin(), code_string.end()};
    std::copy(message.begin(), message.end(), std::back_inserter(bytes));
    return bytes;
  }
};

class MailParser : public NonCopyable {
public:
  MailParser();
  explicit MailParser(const ParserState& state);

  ParserResponse accept(const ParserRequest& message);
  bool is_terminated() const ;
  const std::vector<Mail>& get_mails() const;

private:
  ParserResponse handle_init(const ParserRequest& request);
  ParserResponse handle_helo(const ParserRequest& request);
  ParserResponse handle_mail_from(const ParserRequest& request);
  ParserResponse handle_rcpt_from(const ParserRequest& request);
  ParserResponse handle_data_begin(const ParserRequest& request);
  ParserResponse handle_data(const ParserRequest& request);
  Mail* get_current_mail();

  ParserState m_state;
};
