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

#include "helpers.hpp"

using namespace engine;

TEST(kernel_test, on_action_empty) {
    boost::asio::io_context _io_context;

    const auto _state = std::make_shared<state>();

    const auto _transaction_id = boost::uuids::random_generator()();
    const boost::json::object _data = {
        {"transaction_id", to_string(_transaction_id)},
    };

    const auto _response = kernel(_state, _data, on_session, _state->get_id());

    LOG_INFO("response processed={} failed={} data={}", _response->get_processed(), _response->get_failed(),
             serialize(_response->get_data()));

    ASSERT_TRUE(_response->get_processed());
    ASSERT_TRUE(_response->get_failed());

    test_response_base_protocol_structure(_response, "failed", "unprocessable entity", _transaction_id);

    ASSERT_TRUE(_response->get_data().contains("data"));
    ASSERT_TRUE(_response->get_data().at("data").is_object());
    ASSERT_TRUE(_response->get_data().at("data").as_object().contains("action"));
    ASSERT_TRUE(_response->get_data().at("data").as_object().at("action").is_string());
    ASSERT_EQ(_response->get_data().at("data").as_object().at("action").as_string(),
              "action attribute must be present");
}

TEST(kernel_test, on_wrong_action_primitive) {
    boost::asio::io_context _io_context;

    const auto _state = std::make_shared<state>();

    const auto _transaction_id = boost::uuids::random_generator()();
    const boost::json::object _data = {
        {"transaction_id", to_string(_transaction_id)}, {"action", 7},
    };

    const auto _response = kernel(_state, _data, on_session, _state->get_id());

    LOG_INFO("response processed={} failed={} data={}", _response->get_processed(), _response->get_failed(),
             serialize(_response->get_data()));

    ASSERT_TRUE(_response->get_processed());
    ASSERT_TRUE(_response->get_failed());

    test_response_base_protocol_structure(_response, "failed", "unprocessable entity", _transaction_id);

    ASSERT_TRUE(_response->get_data().contains("data"));
    ASSERT_TRUE(_response->get_data().at("data").is_object());
    ASSERT_TRUE(_response->get_data().at("data").as_object().contains("action"));
    ASSERT_TRUE(_response->get_data().at("data").as_object().at("action").is_string());
    ASSERT_EQ(_response->get_data().at("data").as_object().at("action").as_string(), "action attribute must be string");
}

TEST(kernel_test, on_transaction_id_empty) {
    boost::asio::io_context _io_context;

    const auto _state = std::make_shared<state>();

    const boost::json::object _data = {
        {"action", "something"},
    };

    const auto _response = kernel(_state, _data, on_session, _state->get_id());

    LOG_INFO("response processed={} failed={} data={}", _response->get_processed(), _response->get_failed(),
             serialize(_response->get_data()));

    ASSERT_TRUE(_response->get_processed());
    ASSERT_TRUE(_response->get_failed());
    ASSERT_TRUE(_response->get_data().contains("status"));
    ASSERT_TRUE(_response->get_data().at("status").is_string());
    ASSERT_EQ(_response->get_data().at("status").as_string(), "failed");
    ASSERT_TRUE(_response->get_data().contains("message"));
    ASSERT_TRUE(_response->get_data().at("message").is_string());
    ASSERT_EQ(_response->get_data().at("message").as_string(), "unprocessable entity");
    ASSERT_TRUE(_response->get_data().contains("data"));
    ASSERT_TRUE(_response->get_data().at("data").is_object());
    ASSERT_TRUE(_response->get_data().at("data").as_object().contains("transaction_id"));
    ASSERT_TRUE(_response->get_data().at("data").as_object().at("transaction_id").is_string());
    ASSERT_EQ(_response->get_data().at("data").as_object().at("transaction_id").as_string(),
              "transaction_id attribute must be present");

    ASSERT_TRUE(_response->get_data().contains("runtime"));
    ASSERT_TRUE(_response->get_data().at("runtime").is_number());
    ASSERT_TRUE(_response->get_data().at("runtime").as_int64() > 0);

    ASSERT_TRUE(_response->get_data().contains("timestamp"));
    ASSERT_TRUE(_response->get_data().at("timestamp").is_number());
    ASSERT_TRUE(_response->get_data().at("timestamp").as_int64() > 0);
    ASSERT_TRUE(
        _response->get_data().at("timestamp").as_int64() < std::chrono::system_clock::now().
        time_since_epoch().count());

    ASSERT_TRUE(_response->get_data().contains("transaction_id"));
    ASSERT_TRUE(_response->get_data().at("transaction_id").is_null());
}

TEST(kernel_test, on_wrong_transaction_id_primitive) {
    boost::asio::io_context _io_context;

    const auto _state = std::make_shared<state>();

    const boost::json::object _data = {{"action", "something"},{"transaction_id", 7}};

    const auto _response = kernel(_state, _data, on_session, _state->get_id());

    LOG_INFO("response processed={} failed={} data={}", _response->get_processed(), _response->get_failed(),
             serialize(_response->get_data()));

    ASSERT_TRUE(_response->get_processed());
    ASSERT_TRUE(_response->get_failed());
    ASSERT_TRUE(_response->get_data().contains("status"));
    ASSERT_TRUE(_response->get_data().at("status").is_string());
    ASSERT_EQ(_response->get_data().at("status").as_string(), "failed");
    ASSERT_TRUE(_response->get_data().contains("message"));
    ASSERT_TRUE(_response->get_data().at("message").is_string());
    ASSERT_EQ(_response->get_data().at("message").as_string(), "unprocessable entity");
    ASSERT_TRUE(_response->get_data().contains("data"));
    ASSERT_TRUE(_response->get_data().at("data").is_object());
    ASSERT_TRUE(_response->get_data().at("data").as_object().contains("transaction_id"));
    ASSERT_TRUE(_response->get_data().at("data").as_object().at("transaction_id").is_string());
    ASSERT_EQ(_response->get_data().at("data").as_object().at("transaction_id").as_string(),
              "transaction_id attribute must be string");

    ASSERT_TRUE(_response->get_data().contains("runtime"));
    ASSERT_TRUE(_response->get_data().at("runtime").is_number());
    ASSERT_TRUE(_response->get_data().at("runtime").as_int64() > 0);

    ASSERT_TRUE(_response->get_data().contains("timestamp"));
    ASSERT_TRUE(_response->get_data().at("timestamp").is_number());
    ASSERT_TRUE(_response->get_data().at("timestamp").as_int64() > 0);
    ASSERT_TRUE(
        _response->get_data().at("timestamp").as_int64() < std::chrono::system_clock::now().
        time_since_epoch().count());

    ASSERT_TRUE(_response->get_data().contains("transaction_id"));
    ASSERT_TRUE(_response->get_data().at("transaction_id").is_null());
}


TEST(kernel_test, on_non_uuid_transaction_id) {
    boost::asio::io_context _io_context;

    const auto _state = std::make_shared<state>();

    const boost::json::object _data = {
        {"action", "something"}, {"transaction_id", "7"},
    };

    const auto _response = kernel(_state, _data, on_session, _state->get_id());

    LOG_INFO("response processed={} failed={} data={}", _response->get_processed(), _response->get_failed(),
             serialize(_response->get_data()));

    ASSERT_TRUE(_response->get_processed());
    ASSERT_TRUE(_response->get_failed());
    ASSERT_TRUE(_response->get_data().contains("status"));
    ASSERT_TRUE(_response->get_data().at("status").is_string());
    ASSERT_EQ(_response->get_data().at("status").as_string(), "failed");
    ASSERT_TRUE(_response->get_data().contains("message"));
    ASSERT_TRUE(_response->get_data().at("message").is_string());
    ASSERT_EQ(_response->get_data().at("message").as_string(), "unprocessable entity");
    ASSERT_TRUE(_response->get_data().contains("data"));
    ASSERT_TRUE(_response->get_data().at("data").is_object());
    ASSERT_TRUE(_response->get_data().at("data").as_object().contains("transaction_id"));
    ASSERT_TRUE(_response->get_data().at("data").as_object().at("transaction_id").is_string());
    ASSERT_EQ(_response->get_data().at("data").as_object().at("transaction_id").as_string(),
              "transaction_id attribute must be uuid");

    ASSERT_TRUE(_response->get_data().contains("runtime"));
    ASSERT_TRUE(_response->get_data().at("runtime").is_number());
    ASSERT_TRUE(_response->get_data().at("runtime").as_int64() > 0);

    ASSERT_TRUE(_response->get_data().contains("timestamp"));
    ASSERT_TRUE(_response->get_data().at("timestamp").is_number());
    ASSERT_TRUE(_response->get_data().at("timestamp").as_int64() > 0);
    ASSERT_TRUE(
        _response->get_data().at("timestamp").as_int64() < std::chrono::system_clock::now().
        time_since_epoch().count());

    ASSERT_TRUE(_response->get_data().contains("transaction_id"));
    ASSERT_TRUE(_response->get_data().at("transaction_id").is_null());
}

TEST(kernel_test, on_action_non_implemented) {
    boost::asio::io_context _io_context;

    const auto _state = std::make_shared<state>();

    const auto _transaction_id = boost::uuids::random_generator()();
    const boost::json::object _data = {
        {"action", "invalid"}, {"transaction_id", to_string(_transaction_id)},
    };

    const auto _response = kernel(_state, _data, on_session, _state->get_id());

    LOG_INFO("response processed={} failed={} data={}", _response->get_processed(), _response->get_failed(),
             serialize(_response->get_data()));

    ASSERT_TRUE(_response->get_processed());
    ASSERT_TRUE(_response->get_failed());
    ASSERT_TRUE(_response->get_data().contains("status"));
    ASSERT_TRUE(_response->get_data().at("status").is_string());
    ASSERT_EQ(_response->get_data().at("status").as_string(), "failed");
    ASSERT_TRUE(_response->get_data().contains("message"));
    ASSERT_TRUE(_response->get_data().at("message").is_string());
    ASSERT_EQ(_response->get_data().at("message").as_string(), "unprocessable entity");
    ASSERT_TRUE(_response->get_data().contains("data"));
    ASSERT_TRUE(_response->get_data().at("data").is_object());
    ASSERT_TRUE(_response->get_data().at("data").as_object().contains("action"));
    ASSERT_TRUE(_response->get_data().at("data").as_object().at("action").is_string());
    ASSERT_EQ(_response->get_data().at("data").as_object().at("action").as_string(),
              "action attribute isn't implemented");

    ASSERT_TRUE(_response->get_data().contains("runtime"));
    ASSERT_TRUE(_response->get_data().at("runtime").is_number());
    ASSERT_TRUE(_response->get_data().at("runtime").as_int64() > 0);

    ASSERT_TRUE(_response->get_data().contains("timestamp"));
    ASSERT_TRUE(_response->get_data().at("timestamp").is_number());
    ASSERT_TRUE(_response->get_data().at("timestamp").as_int64() > 0);
    ASSERT_TRUE(
        _response->get_data().at("timestamp").as_int64() < std::chrono::system_clock::now().
        time_since_epoch().count());

    ASSERT_TRUE(_response->get_data().contains("transaction_id"));
    ASSERT_TRUE(_response->get_data().at("transaction_id").is_string());
    ASSERT_EQ(_response->get_data().at("transaction_id").as_string(), to_string(_transaction_id));
}

TEST(kernel_test, on_ack) {
    boost::asio::io_context _io_context;

    const auto _state = std::make_shared<state>();

    const auto _transaction_id = boost::uuids::random_generator()();
    const boost::json::object _data = {
        {"action", "ack"}, {"transaction_id", to_string(_transaction_id)},
    };

    const auto _response = kernel(_state, _data, on_session, _state->get_id());

    LOG_INFO("response processed={} failed={} data={}", _response->get_processed(), _response->get_failed(),
             serialize(_response->get_data()));

    ASSERT_TRUE(_response->get_processed());
    ASSERT_TRUE(_response->is_ack());
}
