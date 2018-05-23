#pragma once

#include <string>
#include <vector>

class MailBuilder;

class Mail {
  friend MailBuilder;

  protected:
    Mail(std::string from, std::vector<std::string> to, std::string data) :
	from(from),
	to(to),
	data(data){};

  public:
    const std::string from;
    const std::vector<std::string> to;

	const std::string data;
};