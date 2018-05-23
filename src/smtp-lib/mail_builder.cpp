#include "mail_builder.hpp"

Mail MailBuilder::build() {
    if (is_mail_valid()) {
        auto mail = std::move(m_mail_under_construction);
        reset();
        return mail;
    }
    throw std::runtime_error("Mail building is incomplete.");
}

void MailBuilder::add(SMTPCommand command) {
    switch (command.type) {
        case SMTPCommandType::MAIL: {
            set_from(command.data);
            break;
        }
        case SMTPCommandType::RCPT: {
            append_to(command.data);
            break;
        }
        case SMTPCommandType::DATA: {
            set_data(command.data);
            break;
        }
        default: 
			break;
    }
}

bool MailBuilder::is_mail_valid() {
    if (m_mail_under_construction.from.empty()) return false;
    if (m_mail_under_construction.to.empty()) return false;
    if (m_mail_under_construction.data.empty()) return false;
    return true;
}

void MailBuilder::reset() { m_mail_under_construction = Mail(); }

void MailBuilder::reset(SMTPCommandType type) { 
    switch (type) {
        case SMTPCommandType::MAIL: {
            m_mail_under_construction.from = "";
            break;
        }
        case SMTPCommandType::RCPT: {
            m_mail_under_construction.to.clear();
            break;
        }
        case SMTPCommandType::DATA: {
            m_mail_under_construction.data = "";
            break;
        }
        default: { throw std::runtime_error("SMTPCommandType is invalide for MailBuilder."); }
    }
}

void MailBuilder::set_from(const std::string& from) {
    if (!m_mail_under_construction.from.empty()) throw std::runtime_error("From already defined.");
    m_mail_under_construction.from = from;
}

void MailBuilder::append_to(const std::string& to) { m_mail_under_construction.to.emplace_back(to); }

void MailBuilder::set_data(const std::string& data) {
    if (!m_mail_under_construction.data.empty()) throw std::runtime_error("Body already defined.");
    m_mail_under_construction.data = data;
}