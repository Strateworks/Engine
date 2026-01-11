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

TEST(validators_session_validator_test, on_sessions_port_empty) {
    const auto _state = std::make_shared<state>();

    const auto _transaction_id = boost::uuids::random_generator()();
    const boost::json::object _data = {
        {"action", "session"},
        {"transaction_id", to_string(_transaction_id)},
        {
            "params", {
                {"host", "127.0.0.1"},
                {"clients_port", 9000},
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
              "params sessions_port attribute must be present");
}

TEST(validators_session_validator_test, on_wrong_sessions_port_primitive) {
    const auto _state = std::make_shared<state>();

    const auto _transaction_id = boost::uuids::random_generator()();
    const boost::json::object _data = {
        {"action", "session"},
        {"transaction_id", to_string(_transaction_id)},
        {
            "params", {
                {"host", "127.0.0.1"},
                {"sessions_port", "EHLO"},
                {"clients_port", 9000},
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
              "params sessions_port attribute must be number");
}

TEST(validators_session_validator_test, on_clients_port_empty) {
    const auto _state = std::make_shared<state>();

    const auto _client = std::make_shared<client>(_state->get_id(), _state);

    const auto _transaction_id = boost::uuids::random_generator()();
    const boost::json::object _data = {
        {"action", "session"},
        {"transaction_id", to_string(_transaction_id)},
        {
            "params", {

                {"host", "127.0.0.1"},
                {"sessions_port", 10000},
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
              "params clients_port attribute must be present");
}

TEST(validators_session_validator_test, on_wrong_clients_port_primitive) {
    const auto _state = std::make_shared<state>();

    const auto _transaction_id = boost::uuids::random_generator()();
    const boost::json::object _data = {
        {"action", "session"},
        {"transaction_id", to_string(_transaction_id)},
        {
            "params", {
                {"host", "127.0.0.1"},
                {"sessions_port", 10000},
                {"clients_port", "EHLO"},
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
              "params clients_port attribute must be number");
}


TEST(validators_session_validator_test, on_host_empty) {
    const auto _state = std::make_shared<state>();

    const auto _client = std::make_shared<client>(_state->get_id(), _state);

    const auto _transaction_id = boost::uuids::random_generator()();
    const boost::json::object _data = {
        {"action", "session"},
        {"transaction_id", to_string(_transaction_id)},
        {
            "params", {
                {"sessions_port", 10000},
                {"clients_port", 9000},
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
              "params host attribute must be present");
}

TEST(validators_session_validator_test, on_wrong_host_primitive) {
    const auto _state = std::make_shared<state>();

    const auto _client = std::make_shared<client>(_state->get_id(), _state);

    const auto _transaction_id = boost::uuids::random_generator()();
    const boost::json::object _data = {
        {"action", "session"},
        {"transaction_id", to_string(_transaction_id)},
        {
            "params", {
                {"host", 100},
                {"sessions_port", 10000},
                {"clients_port", 9000},
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
              "params host attribute must be string");
}
