#include <cassert>
#include <stdexcept>

#include <mail_parser.hpp>


namespace
{
  static const std::string END_TOKEN = "\r\n";

  static const std::string HELO_TOKEN = "HELO";
  static const std::string EHLO_TOKEN = "EHLO";
  static const std::string FROM_TOKEN = "MAIL FROM:";
  static const std::string TO_TOKEN = "RCPT TO:";
  static const std::string DATA_BEGIN_TOKEN = "DATA";
  static const std::string DATA_END_TOKEN = "\r\n.\r\n";
  static const std::string QUIT_TOKEN = "QUIT";
}

ParserResponse MailParser::accept(const ParserRequest& request) {
  m_buffer.insert(m_buffer.end(), request.message.cbegin(), request.message.cend());

  // TODO: what happens if we get multiple commands in one buffer, e.g. HELO and MAIL FROM:? Is that even allowed?
  // TODO: what happens if we get commands out of order?
  switch (m_state) {
    case ParserStatus::CLIENT_INITIATION:
    {
      auto helo_info = parse_buffer(HELO_TOKEN);
      auto ehlo_info = parse_buffer(EHLO_TOKEN);
      return handle_client_initiation(helo_info, ehlo_info);
    }
    case ParserStatus::FROM_FIELD:
    {
      auto info = parse_buffer(FROM_TOKEN);
      return handle_from_field(info);
    }
    case ParserStatus::TO_FIELD:
    {
      auto info = parse_buffer(TO_TOKEN);
      return handle_to_field(info);
    }
    case ParserStatus::TO_FIELD_OR_DATA_BEGIN:
    {
      auto to_info = parse_buffer(TO_TOKEN);
      auto data_info = parse_buffer(DATA_BEGIN_TOKEN);
      return handle_to_and_data(to_info, data_info);
    }
    case ParserStatus::DATA_CONTENT_OR_END:
    {
      auto info = parse_buffer(DATA_END_TOKEN, false);
      return handle_data_end(info);
    }
    case ParserStatus::QUIT:
    {
      auto info = parse_buffer(QUIT_TOKEN);
      return handle_quit(info);
    }
    case ParserStatus::FINISHED:
    {
      return ParserResponse{};
    }
    default:
    {
      throw std::runtime_error("Not implemented.");
    }
  }
}

bool MailParser::has_finished() const
{
  return m_state == ParserStatus::FINISHED;
}

MailParser::BufferInformation MailParser::parse_buffer(const std::string & token, bool at_start)
{
  auto pos_begin = find_token(token);
  auto pos_end = find_token(END_TOKEN);

  if (pos_end == std::string::npos)
  {
    // TODO: Completely valid, we just need more data
    // TODO: Check that this line does not become too long, I don't know if the RFC says anything about the max length
    return { BufferStatus::INCOMPLETE, 0 };
  }

  if (pos_begin == std::string::npos)
  {
    return { BufferStatus::INVALID, 0 };
  }

  // There should be nothing in front of our token
  if (at_start && pos_begin != 0)
  {
    return { BufferStatus::INVALID, 0 };
  }

  return { BufferStatus::COMPLETE, pos_end };
}

ParserResponse MailParser::handle_client_initiation(BufferInformation helo_info, BufferInformation ehlo_info)
{
  if (ehlo_info.status == BufferStatus::INVALID && ehlo_info.status == BufferStatus::INCOMPLETE)
  {
    // TODO: implement EHLO
    throw std::runtime_error("EHLO currently not implemented!");
  }

  if (helo_info.status == BufferStatus::INVALID)
  {
    // TODO: implement error case
    throw std::runtime_error("Error case is currently not implemented!");
  }

  if (helo_info.status == BufferStatus::INCOMPLETE)
  {
    return ParserResponse{};
  }

  m_buffer.erase(m_buffer.begin(), m_buffer.begin() + helo_info.end + END_TOKEN.length());
  m_state = ParserStatus::FROM_FIELD;

  // TODO: check which is the appropriate response
  return ParserResponse{ 123, "OK" };
}

ParserResponse MailParser::handle_from_field(BufferInformation info)
{
  if (info.status == BufferStatus::INVALID)
  {
    // TODO: implement error case
    throw std::runtime_error("Error case is currently not implemented!");
  }

  if (info.status == BufferStatus::INCOMPLETE)
  {
    return ParserResponse{};
  }

  m_mail.to.emplace_back(m_buffer.begin() + FROM_TOKEN.length(), m_buffer.begin() + info.end);

  m_buffer.erase(m_buffer.begin(), m_buffer.begin() + info.end + END_TOKEN.length());
  m_state = ParserStatus::TO_FIELD;

  return ParserResponse{ 250, "OK" };
}

ParserResponse MailParser::handle_to_field(BufferInformation info)
{
  if (info.status == BufferStatus::INVALID)
  {
    // TODO: implement error case
    throw std::runtime_error("Error case is currently not implemented!");
  }

  if (info.status == BufferStatus::INCOMPLETE)
  {
    return ParserResponse{};
  }

  m_mail.to.emplace_back(m_buffer.begin() + TO_TOKEN.length(), m_buffer.begin() + info.end);

  m_buffer.erase(m_buffer.begin(), m_buffer.begin() + info.end + END_TOKEN.length());
  m_state = ParserStatus::TO_FIELD_OR_DATA_BEGIN;

  return ParserResponse{ 250, "OK" };
}

ParserResponse MailParser::handle_to_and_data(BufferInformation to_info, BufferInformation data_info)
{
  if (to_info.status == BufferStatus::INVALID && data_info.status == BufferStatus::INVALID)
  {
    // TODO: implement error case
    throw std::runtime_error("Error case is currently not implemented!");
  }

  if (to_info.status == BufferStatus::INCOMPLETE && data_info.status == BufferStatus::INCOMPLETE)
  {
    return ParserResponse{};
  }

  if (to_info.status == BufferStatus::COMPLETE)
  {
    return handle_to_field(to_info);
  }

  if (data_info.status == BufferStatus::COMPLETE)
  {
    return handle_data_begin(data_info);
  }

  throw std::runtime_error("This should never be reached! Implementation is broken if it does.");
}

ParserResponse MailParser::handle_data_begin(BufferInformation info)
{
  m_buffer.erase(m_buffer.begin(), m_buffer.begin() + info.end + END_TOKEN.length());
  m_state = ParserStatus::DATA_CONTENT_OR_END;

  return ParserResponse{ 354, "End data with <CR><LF>.<CR><LF>" };
}

ParserResponse MailParser::handle_data_end(BufferInformation info)
{
  if (info.status == BufferStatus::INCOMPLETE || info.status == BufferStatus::INVALID)
  {
    return ParserResponse{};
  }

  auto end_pos = find_token(DATA_END_TOKEN);
  m_mail.content = std::string(m_buffer.begin(), m_buffer.begin() + end_pos);
  m_buffer.erase(m_buffer.begin(), m_buffer.begin() + end_pos + DATA_END_TOKEN.length());

  m_state = ParserStatus::QUIT;
  return ParserResponse{ 250, "OK" };
}

ParserResponse MailParser::handle_quit(BufferInformation info)
{
  if (info.status == BufferStatus::INVALID)
  {
    // TODO: implement error case
    throw std::runtime_error("Error case is currently not implemented!");
  }

  if (info.status == BufferStatus::INCOMPLETE)
  {
    return ParserResponse{};
  }

  m_buffer.resize(0);
  m_state = ParserStatus::FINISHED;

  return ParserResponse{ 221, "OK" };
}

size_t MailParser::find_token(const std::string& token)
{
  std::string buffer_as_string(m_buffer.cbegin(), m_buffer.cend());
  return buffer_as_string.find(token);
}
