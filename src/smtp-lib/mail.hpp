#pragma once

#include <string>
#include <vector>

class MailBuilder;

class Mail {
  friend MailBuilder;

  protected:
    Mail(){};
    Mail(std::string from, std::vector<std::string> to, std::string data) :
	from(from),
	to(to),
	data(data){};

  public:
    std::string from;
    std::vector<std::string> to;

	std::string data;
};