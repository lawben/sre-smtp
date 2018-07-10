#include "mail_store.hpp"

#include <ctime>

#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

namespace {
    void fix_time_value(std::string& time_value) {
        if (time_value.length() == 1) {
            time_value.insert(time_value.begin(), '0');
        }
    }

    std::string time_string() {
        const auto elapsed_time = std::time(nullptr);
        const auto utc_time = std::gmtime(&elapsed_time);

        const auto year = std::to_string(utc_time->tm_year + 1900);
        auto month = std::to_string(utc_time->tm_mon + 1);
        auto day = std::to_string(utc_time->tm_mday);
        auto hour = std::to_string(utc_time->tm_hour);
        auto minute = std::to_string(utc_time->tm_min);
        auto second = std::to_string(utc_time->tm_sec);

        fix_time_value(month);
        fix_time_value(day);
        fix_time_value(hour);
        fix_time_value(minute);
        fix_time_value(second);

        return std::string{year + '-' + month + '-' + day + 'T' + hour + ':' + minute + ':' + second + ".00+00:00"};
    }
}

bool MailStore::store_mail(const Mail& mail) {
    nlohmann::json json;
    json["received"] = time_string();
    json["received_from"] = "sending.server";
    json["received_by"] = "receiving.server";
    json["mail_from"] = mail.from;
    json["rcpt_to"] = mail.to;
    json["data"] = mail.data;

    for (auto i{0}; i < 5; ++i) {
        auto response =
            cpr::Post(cpr::Url{"http://localhost:5000/mailstore"}, cpr::Header{{"Content-Type", "application/json"}},
                      cpr::Body{json.dump(4)}, cpr::Timeout{5000});

        if (response.error.code == cpr::ErrorCode::OK) {
            return true;
        }
    }

    return false;
}