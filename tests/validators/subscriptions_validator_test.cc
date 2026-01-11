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

TEST(validators_subscriptions_validator_test, can_handle_empty_params_channel_on_subscriptions) {
    const auto _state = std::make_shared<engine::state>();

    const auto _local_client = std::make_shared<engine::client>(_state->get_id(), _state);

    for (const auto _action: {"subscribe", "unsubscribe"}) {
        const auto _transaction_id = boost::uuids::random_generator()();
        const boost::json::object _data = {
            {"action", _action}, {"transaction_id", to_string(_transaction_id)}, {
                "params", {
                    {"session_id", to_string(_state->get_id())},
                    {"client_id", to_string(_local_client->get_id())},
                }
            }
        };

        const auto _response = kernel(_state, _data, on_session, _state->get_id());

        LOG_INFO("response processed={} failed={} data={}", _response->get_processed(), _response->get_failed(),
                 serialize(_response->get_data()));

        ASSERT_TRUE(_response->get_processed());
        ASSERT_TRUE(_response->get_failed());

        test_response_base_protocol_structure(_response, "failed", "unprocessable entity", _transaction_id);

        ASSERT_TRUE(_response->get_data().contains("data"));
        ASSERT_TRUE(_response->get_data().at("data").is_object());
        ASSERT_TRUE(_response->get_data().at("data").as_object().contains("params"));
        ASSERT_TRUE(_response->get_data().at("data").as_object().at("params").is_string());
        ASSERT_EQ(_response->get_data().at("data").as_object().at("params").as_string(),
                  "params channel attribute must be present");
    }
}

TEST(validators_subscriptions_validator_test, can_handle_wrong_params_channel_primivite_on_subscriptions) {
    const auto _state = std::make_shared<engine::state>();

    const auto _local_client = std::make_shared<engine::client>(_state->get_id(), _state);

    for (const auto _action: {"subscribe", "unsubscribe"}) {
        const auto _transaction_id = boost::uuids::random_generator()();
        const boost::json::object _data = {
            {"action", _action},
            {"transaction_id", to_string(_transaction_id)},
            {
                "params", {
                    {"session_id", to_string(_state->get_id())},
                    {"client_id", to_string(_local_client->get_id())},
                    {"channel", 7},
                }
            }
        };

        const auto _response = kernel(_state, _data, on_session, _state->get_id());

        LOG_INFO("response processed={} failed={} data={}", _response->get_processed(), _response->get_failed(),
                 serialize(_response->get_data()));

        ASSERT_TRUE(_response->get_processed());
        ASSERT_TRUE(_response->get_failed());

        test_response_base_protocol_structure(_response, "failed", "unprocessable entity", _transaction_id);

        ASSERT_TRUE(_response->get_data().contains("data"));
        ASSERT_TRUE(_response->get_data().at("data").is_object());
        ASSERT_TRUE(_response->get_data().at("data").as_object().contains("params"));
        ASSERT_TRUE(_response->get_data().at("data").as_object().at("params").is_string());
        ASSERT_EQ(_response->get_data().at("data").as_object().at("params").as_string(),
                  "params channel attribute must be string");
    }
}
