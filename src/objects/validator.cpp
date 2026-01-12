// Copyright (c) 2025 — 2026 Ian Torres <iantorres@outlook.com>.
// All rights reserved.

#include <engine/validator.hpp>

#include <boost/uuid/string_generator.hpp>
#include <boost/optional.hpp>

#include <engine/utils.hpp>

namespace engine {
    validator::validator(boost::json::object data) {
        if (!data.contains("action")) {
            bag_.insert_or_assign("action", "action attribute must be present");
            passed_ = false;
            return;
        }

        if (!data.at("action").is_string()) {
            bag_.insert_or_assign("action", "action attribute must be string");
            passed_ = false;
            return;
        }

        if (!data.contains("transaction_id")) {
            bag_.insert_or_assign("transaction_id", "transaction_id attribute must be present");
            passed_ = false;
            return;
        }

        auto _transaction_id_value = data.at("transaction_id");
        if (!_transaction_id_value.is_string()) {
            bag_.insert_or_assign("transaction_id", "transaction_id attribute must be string");
            passed_ = false;
            return;
        }

        const auto _transaction_id = _transaction_id_value.as_string();
        if (!is_uuid(_transaction_id.c_str())) {
            bag_.insert_or_assign("transaction_id", "transaction_id attribute must be uuid");
            passed_ = false;
            return;
        }

        passed_ = true;
    }

    bool validator::get_passed() const { return passed_; }

    std::map<std::string, std::string> validator::get_bag() const { return bag_; }

    bool validator::is_uuid(const char *uuid) {
        try {
            constexpr boost::uuids::string_generator _generator;
            const boost::optional<boost::uuids::uuid> _uuid = _generator(uuid);
            return true;
        } catch (...) {
            return false;
        }
    }
} // namespace engine
