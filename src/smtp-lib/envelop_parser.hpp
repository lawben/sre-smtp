#pragma once

#include "abstract_parser.hpp"

class EnvelopParser : public AbstractParser {
  public:
    ~EnvelopParser() = default;
    std::vector<SMTPCommand> accept(const ParserRequest& request) override;
    SMTPCommand parse_buffer();

  private:
    bool find_delemiter();
};