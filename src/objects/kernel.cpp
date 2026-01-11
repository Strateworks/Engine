// Copyright (c) 2025 Ian Torres <iantorres@outlook.com>.
// All rights reserved.

#include <fmt/format.h>

#include <engine/kernel.hpp>
#include <engine/request.hpp>
#include <engine/response.hpp>
#include <engine/session.hpp>
#include <engine/state.hpp>
#include <engine/logger.hpp>
#include <engine/validator.hpp>

#include <engine/handlers/ping_handler.hpp>
#include <engine/handlers/register_handler.hpp>
#include <engine/handlers/session_handler.hpp>

#include <engine/handlers/join_handler.hpp>
#include <engine/handlers/leave_handler.hpp>

#include <engine/handlers/subscribe_handler.hpp>

#include <engine/handlers/unsubscribe_handler.hpp>

#include <engine/handlers/is_subscribed_handler.hpp>

#include <engine/handlers/broadcast_handler.hpp>
#include <engine/handlers/publish_handler.hpp>
#include <engine/handlers/send_handler.hpp>

#include <engine/handlers/unimplemented_handler.hpp>

#include <engine/utils.hpp>

#include <boost/json/serialize.hpp>
#include <boost/core/ignore_unused.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace engine {
    std::shared_ptr<response> kernel(const std::shared_ptr<state> &state,
                                     const boost::json::object &data,
                                     const kernel_context context,
                                     const boost::uuids::uuid entity_id) {
        boost::ignore_unused(state);

        const auto _timestamp = std::chrono::system_clock::now().time_since_epoch().count();

        auto _response = std::make_shared<response>();
        if (const validator _validator(data); _validator.get_passed()) {
            const auto _request = request{
                .transaction_id_ = get_param_as_id(data, "transaction_id"),
                .response_ = _response,
                .entity_id_ = entity_id,
                .context_ = context,
                .state_ = state,
                .data_ = data,
                .timestamp_ = _timestamp,
            };

            if (const std::string _action{data.at("action").as_string()}; _action == "ping") {
                handlers::ping_handler(_request);
            } else if (_action == "send") {
                handlers::send_handler(_request);
            } else if (_action == "register") {
                handlers::register_handler(_request);
            } else if (_action == "session") {
                handlers::session_handler(_request);
            } else if (_action == "ack") {
                _response->mark_as_ack();
            } else if (_action == "subscribe") {
                handlers::subscribe_handler(_request);
            } else if (_action == "is_subscribed") {
                handlers::is_subscribed_handler(_request);
            } else if (_action == "unsubscribe") {
                handlers::unsubscribe_handler(_request);
            } else if (_action == "broadcast") {
                handlers::broadcast_handler(_request);
            } else if (_action == "publish") {
                handlers::publish_handler(_request);
            } else if (_action == "join") {
                handlers::join_handler(_request);
            } else if (_action == "leave") {
                handlers::leave_handler(_request);
            } else {
                handlers::unimplemented_handler(_request);
            }
        } else {
            if (data.contains("transaction_id") && data.at("transaction_id").is_string() && validator::is_uuid(
                    data.at("transaction_id").as_string().c_str())) {
                _response->mark_as_failed(
                    get_param_as_id(data, "transaction_id"),
                    "unprocessable entity", _timestamp, _validator.get_bag());
            } else {
                _response->mark_as_failed(boost::uuids::uuid{}, "unprocessable entity", _timestamp,
                                          _validator.get_bag());
            }
        }
        _response->mark_as_processed();

        return _response;
    }
} // namespace engine
