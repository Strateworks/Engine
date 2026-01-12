// Copyright (c) 2025 — 2026 Ian Torres <iantorres@outlook.com>.
// All rights reserved.

#include <engine/utils.hpp>

#include <engine/request.hpp>
#include <engine/response.hpp>
#include <engine/session.hpp>

#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/uuid.hpp>

namespace engine {
    void next(const request &request, const char *message, const boost::json::object &data) {
        request.response_->set_data(
            request.transaction_id_,
            message,
            request.timestamp_,
            data);
    }

    void mark_as_invalid(const request &request, const char *field, const char *argument) {
        request.response_->mark_as_failed(request.transaction_id_, "unprocessable entity", request.timestamp_,
                                          {{field, argument}});
    }

    std::string get_param_as_string(const boost::json::object &params, const char *field) {
        return std::string{params.at(field).as_string()};
    }

    std::size_t get_param_as_number(const boost::json::object &params, const char *field) {
        return params.at(field).as_int64();
    }

    bool get_param_as_bool(const boost::json::object &params, const char *field) {
        return params.at(field).as_bool();
    }

    const boost::json::object &get_params(const request &request) {
        return request.data_.at("params").as_object();
    }

    const boost::json::object &get_param_as_object(const boost::json::object &params, const char *field) {
        return params.at(field).as_object();
    }

    const boost::json::value &get_params_as_value(const request &request) {
        return request.data_.at("params");
    }

    boost::uuids::uuid get_param_as_id(const boost::json::object &params, const char *field) {
        return boost::lexical_cast<boost::uuids::uuid>(std::string{params.at(field).as_string()});
    }

    boost::json::object make_broadcast_request_object(const request &request,
                                                      const boost::uuids::uuid &client_id,
                                                      const boost::json::object &payload) {
        return {
            {"transaction_id", to_string(request.transaction_id_)},
            {"action", "broadcast"},
            {
                "params", {
                    {"client_id", to_string(client_id)},
                    {"payload", payload},
                }
            }
        };
    }

    boost::json::object make_publish_request_object(const request &request, const boost::uuids::uuid &client_id,
                                                    const std::string &channel, const boost::json::object &payload) {
        return {
            {"transaction_id", to_string(request.transaction_id_)},
            {"action", "publish"},
            {
                "params", {
                    {"client_id", to_string(client_id)},
                    {"channel", channel},
                    {"payload", payload},
                }
            }
        };
    }

    boost::json::object make_join_request_object(const boost::uuids::uuid &client_id) {
        return {
            {"transaction_id", to_string(boost::uuids::random_generator()())},
            {"action", "join"},
            {
                "params", {
                    {"client_id", to_string(client_id)},
                }
            }
        };
    }

    boost::json::object make_leave_request_object(const boost::uuids::uuid &client_id) {
        return {
            {"transaction_id", to_string(boost::uuids::random_generator()())},
            {"action", "leave"},
            {
                "params", {
                    {"client_id", to_string(client_id)},
                }
            }
        };
    }

    boost::json::object make_subscribe_request_object(const request &request, const boost::uuids::uuid &client_id,
                                                      const std::string &channel) {
        return {
            {"transaction_id", to_string(request.transaction_id_)},
            {"action", "subscribe"},
            {
                "params", {
                    {"client_id", to_string(client_id)},
                    {"channel", channel},
                }
            }
        };
    }

    boost::json::object make_unsubscribe_request_object(const request &request, const boost::uuids::uuid &client_id,
                                                        const std::string &channel) {
        return {
            {"transaction_id", to_string(request.transaction_id_)},
            {"action", "unsubscribe"},
            {
                "params", {
                    {"client_id", to_string(client_id)},
                    {"channel", channel},
                }
            }
        };
    }

    const char *get_status(const bool gate, const char *on_true, const char *on_false) {
        return gate
                   ? on_true
                   : on_false;
    }

    std::string kernel_context_to_string(const kernel_context context) {
        return context == on_session ? "on_session" : "on_client";
    }
} // namespace engine
