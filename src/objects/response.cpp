// Copyright (c) 2025 Ian Torres <iantorres@outlook.com>.
// All rights reserved.

#include <engine/response.hpp>
#include <map>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace engine {
    bool response::get_failed() const {
        return failed_.load(std::memory_order_acquire);
    }

    bool response::get_processed() const {
        return processed_.load(std::memory_order_acquire);
    }

    bool response::is_ack() const {
        return is_ack_.load(std::memory_order_acquire);
    }

    void response::mark_as_ack() {
        is_ack_.store(true, std::memory_order_release);
    }

    boost::json::object response::get_data() const { return data_; }

    void response::mark_as_failed(const boost::uuids::uuid transaction_id, const char *error, long timestamp,
                                  const std::map<std::string, std::string> &bag) {
        failed_.store(true, std::memory_order_release);
        const auto _current_timestamp = std::chrono::system_clock::now().time_since_epoch().count();

        const auto _runtime = _current_timestamp - timestamp;

        boost::json::object _data;
        _data.reserve(bag.size());
        for (auto &[_key, _entry]: bag) {
            _data.insert_or_assign(_key, _entry);
        }

        if (!transaction_id.is_nil()) {
            data_ = {
                {"transaction_id", to_string(transaction_id)},
                {"action", "ack"},
                {"status", "failed"},
                {"message", error},
                {"data", _data},
                {"timestamp", timestamp},
                {"runtime", _runtime}
            };
        } else {
            data_ = {
                {"transaction_id", nullptr},
                {"action", "ack"},
                {"status", "failed"},
                {"message", error},
                {"data", _data},
                {"timestamp", timestamp},
                {"runtime", _runtime}
            };
        }
    }

    void response::mark_as_processed() {
        processed_.store(true, std::memory_order_release);
    }


    void response::set_data(const boost::uuids::uuid transaction_id, const char *message, long timestamp,
                            const boost::json::object &data) {
        const auto _current_timestamp = std::chrono::system_clock::now().time_since_epoch().count();

        const auto _runtime = _current_timestamp - timestamp;

        data_ = {
            {"transaction_id", to_string(transaction_id)},
            {"action", "ack"},
            {"status", "success"},
            {"message", message},
            {"timestamp", timestamp},
            {"runtime", _runtime},
            {"data", data}
        };
    }
} // namespace engine
