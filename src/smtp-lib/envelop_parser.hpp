#pragma once

#include "abstract_parser.hpp"

class EnvelopParser : public AbstractParser {
  public:
    EnvelopParser();
    ~EnvelopParser() = default;
    ParserStatus accept(const ParserRequest& request) override;
    SMTPCommand get_command() override;

  private:
    SMTPCommand parse_buffer();
};