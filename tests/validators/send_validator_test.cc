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
#include <boost/lexical_cast.hpp>

#include "../helpers.hpp"

using namespace engine;

TEST(validators_send_validator_test, on_payload_empty) {
    const auto _state = std::make_shared<state>();

    const auto _client = std::make_shared<client>(_state->get_id(), _state);

    const auto _transaction_id = boost::uuids::random_generator()();
    const boost::json::object _data = {
        {"action", "send"},
        {"transaction_id", to_string(_transaction_id)},
        {
            "params", {}
        }
    };

    const auto _response = kernel(_state, _data, on_client, _client->get_id());

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
              "params payload attribute must be present");
}

TEST(validators_send_validator_test, on_wrong_payload_primitive) {
    const auto _state = std::make_shared<state>();

    const auto _client = std::make_shared<client>(_state->get_id(), _state);

    const auto _transaction_id = boost::uuids::random_generator()();
    const boost::json::object _data = {
        {"action", "send"},
        {"transaction_id", to_string(_transaction_id)},
        {
            "params",
            {
                {"payload", 7}
            }
        }
    };

    const auto _response = kernel(_state, _data, on_client, _client->get_id());

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
              "params payload attribute must be object");
}
