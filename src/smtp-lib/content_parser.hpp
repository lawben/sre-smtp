#pragma once

#include "abstract_parser.hpp"

class ContentParser : public AbstractParser {
  public:
    ContentParser();
    ~ContentParser() = default;

    ParserStatus accept(const ParserRequest& request) override;
    SMTPCommand get_command() override;
};