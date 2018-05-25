#pragma once

#include "mail.hpp"

#include "smtp_utils.hpp"

class MailBuilder {
  public:
    Mail build();
    void add(SMTPCommand command);

	void reset();

  private:
    Mail m_mail_under_construction;

	bool is_mail_valid();

	void set_from(const std::string& from);
	void append_to(const std::string& to);
	void set_data(const std::string& data);
};
