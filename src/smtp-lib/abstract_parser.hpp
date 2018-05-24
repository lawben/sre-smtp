#pragma once

#include <vector>

#include "smtp_utils.hpp"

using Bytes = std::vector<char>;

struct ParserRequest {
  protected:
    std::string message;

  public:
    explicit ParserRequest(const Bytes& bytes) : message(bytes.begin(), bytes.end()) {}
    explicit ParserRequest(const std::string& bytes) : message(bytes) {}

    operator std::string() const { return message; }
};

enum class ParserStatus { COMPLETE, INCOMPLETE, TO_LONG };

class AbstractParser {
  public:
    AbstractParser(const std::string& delemiter) : m_delemiter(delemiter){};
    virtual ~AbstractParser() = default;

    virtual ParserStatus accept(const ParserRequest& message) = 0;
    virtual SMTPCommand get_command() = 0;

  protected:
    const std::string m_delemiter;
    std::string m_buffer;

    bool find_delemiter() { return m_buffer.find(m_delemiter) != std::string::npos; }
};