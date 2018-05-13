#include <fstream>
#include <random>
#include <string>

#if __has_include(<filesystem>)
#include <filesystem>
namespace fs = std::filesystem;
#else
#include <experimental/filesystem>
#include <iostream>
namespace fs = std::experimental::filesystem;
#endif

#include "mail_persister.hpp"

namespace {
static const fs::path MAIL_FOLDER = "mails";

std::string random_string(const size_t length) {
    auto randchar = []() -> char {
        const char charset[] =
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz";
        const size_t max_index = (sizeof(charset) - 1);
        return charset[rand() % max_index];
    };
    std::string str(length, 0);
    std::generate_n(str.begin(), length, randchar);
    return str;
}
}  // namespace

bool MailPersister::store_mail(const Mail& mail) {
    fs::create_directory(MAIL_FOLDER);
    const auto str = random_string(6);
    std::cout << str << std::endl;
    fs::path mail_path = MAIL_FOLDER / mail.from / "-" / str;
    std::ofstream mail_file{mail_path};
    mail_file << "FROM: " << mail.from << std::endl;

    for (const auto& recipient : mail.to) {
        mail_file << "TO: " << recipient << std::endl;
    }

    mail_file << "CONTENT:\n" << mail.content;

    mail_file.flush();
    mail_file.close();
    return true;
}
