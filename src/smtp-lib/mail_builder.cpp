#include "mail_builder.hpp"

Mail MailBuilder::build() { 
	if (is_mail_valid())
		return m_mail_under_construction;
    throw std::runtime_error("Mail building is incomplete.");
}

void MailBuilder::add(SMTPCommand command) {
    switch (command.type) {
        case SMTPCommandType::MAIL: {
            m_mail_under_construction.from = command.data;
            break;
        }
        case SMTPCommandType::RCPT: {
            m_mail_under_construction.to.emplace_back(command.data);
            break;
        }
        case SMTPCommandType::DATA: {
            m_mail_under_construction.data = command.data;
            break;
        }
        default: { throw std::runtime_error("SMTPCommandType is invalide for MailBuilder."); }
    }
}

bool MailBuilder::is_mail_valid() { 
	if (m_mail_under_construction.from.empty()) return false;
    if (m_mail_under_construction.to.empty()) return false;
	if (m_mail_under_construction.data.empty()) return false;
    return true;
}