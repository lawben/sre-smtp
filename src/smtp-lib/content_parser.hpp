#pragma once

#include "abstract_parser.hpp"

class ContentParser : public AbstractParser {
  public:
    ~ContentParser() = default;
    std::vector<SMTPCommand> accept(const ParserRequest& request) override;
    SMTPCommand parse_buffer();

  private:
    bool find_delemiter();
};