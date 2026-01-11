// Copyright (c) 2025 Ian Torres <iantorres@outlook.com>.
// All rights reserved.

#include <gtest/gtest.h>

#include <engine/kernel.hpp>
#include <engine/kernel_context.hpp>

#include <engine/response.hpp>
#include <engine/session.hpp>
#include <engine/client.hpp>
#include <engine/state.hpp>
#include <engine/logger.hpp>

#include <boost/json/serialize.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "../helpers.hpp"

using namespace engine;

TEST(handlers_publish_handler_test, can_handle_publish_on_client) {
    const auto _state = std::make_shared<state>();

    const auto _client = std::make_shared<client>(_state->get_id(), _state);
    const auto _other = std::make_shared<client>(_state->get_id(), _state);

    _state->push_client(_client);
    _state->push_client(_other);
    _state->subscribe(_state->get_id(), _other->get_id(), "welcome");

    const auto _transaction_id = boost::uuids::random_generator()();
    const boost::json::object _data = {
        {"action", "publish"},
        {"transaction_id", to_string(_transaction_id)},
        {
            "params",
            {
                {"channel", "welcome"},
                {"payload", {{"message", "EHLO"}}}
            }
        }
    };

    const auto _response = kernel(_state, _data, on_client, _client->get_id());

    LOG_INFO("response processed={} failed={} data={}", _response->get_processed(), _response->get_failed(),
             serialize(_response->get_data()));

    ASSERT_TRUE(_response->get_processed());
    ASSERT_TRUE(!_response->get_failed());

    test_response_base_protocol_structure(_response, "success", "ok", _transaction_id);

    ASSERT_TRUE(_response->get_data().contains("data"));
    ASSERT_TRUE(_response->get_data().at("data").is_object());

    _state->remove_client(_client->get_id());
    _state->remove_client(_other->get_id());
}

TEST(handlers_publish_handler_test, can_handle_publish_on_session) {
    const auto _state = std::make_shared<state>();

    const auto _client = std::make_shared<client>(_state->get_id(), _state);
    const auto _other = std::make_shared<client>(_state->get_id(), _state);

    _state->push_client(_client);
    _state->push_client(_other);
    _state->subscribe(_state->get_id(), _other->get_id(), "welcome");

    const auto _transaction_id = boost::uuids::random_generator()();
    const boost::json::object _data = {
        {"action", "publish"},
        {"transaction_id", to_string(_transaction_id)},
        {
            "params",
            {
                {"client_id", to_string(_client->get_id())},
                {"channel", "welcome"},
                {"payload", {{"message", "EHLO"}}}
            }
        }
    };

    const auto _response = kernel(_state, _data, on_session, _state->get_id());

    LOG_INFO("response processed={} failed={} data={}", _response->get_processed(), _response->get_failed(),
             serialize(_response->get_data()));

    ASSERT_TRUE(_response->get_processed());
    ASSERT_TRUE(!_response->get_failed());

    test_response_base_protocol_structure(_response, "success", "ok", _transaction_id);

    ASSERT_TRUE(_response->get_data().contains("data"));
    ASSERT_TRUE(_response->get_data().at("data").is_object());

    _state->remove_client(_client->get_id());
    _state->remove_client(_other->get_id());
}
