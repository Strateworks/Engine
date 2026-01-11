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

TEST(handlers_send_handler_test, can_handle_local_send_on_client) {
    const auto _state = std::make_shared<state>();

    const auto _client = std::make_shared<client>(_state->get_id(), _state);
    const auto _other = std::make_shared<client>(_state->get_id(), _state);

    _state->push_client(_client);
    _state->push_client(_other);

    const auto _transaction_id = boost::uuids::random_generator()();
    const boost::json::object _data = {
        {"action", "send"},
        {"transaction_id", to_string(_transaction_id)},
        {
            "params",
            {
                {"to_client_id", to_string(_other->get_id())},
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

TEST(handlers_send_handler_test, can_handle_remote_send_on_client) {
    const auto _state = std::make_shared<state>();

    const auto _session = std::make_shared<session>(_state, boost::asio::ip::tcp::socket{_state->get_ioc()}, remote);

    const auto _client = std::make_shared<client>(_state->get_id(), _state);
    const auto _other = std::make_shared<client>(_session->get_id(), _state);

    _state->push_client(_client);
    _state->push_client(_other);

    const auto _transaction_id = boost::uuids::random_generator()();
    const boost::json::object _data = {
        {"action", "send"},
        {"transaction_id", to_string(_transaction_id)},
        {
            "params",
            {
                {"to_client_id", to_string(_other->get_id())},
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

TEST(handlers_send_handler_test, can_handle_no_effect_on_client) {
    const auto _state = std::make_shared<state>();

    const auto _session = std::make_shared<session>(_state, boost::asio::ip::tcp::socket{_state->get_ioc()}, remote);

    const auto _client = std::make_shared<client>(_state->get_id(), _state);

    _state->push_client(_client);

    const auto _transaction_id = boost::uuids::random_generator()();
    const boost::json::object _data = {
        {"action", "send"},
        {"transaction_id", to_string(_transaction_id)},
        {
            "params",
            {
                {"to_client_id", to_string(boost::uuids::random_generator()())},
                {"payload", {{"message", "EHLO"}}}
            }
        }
    };

    const auto _response = kernel(_state, _data, on_client, _client->get_id());

    LOG_INFO("response processed={} failed={} data={}", _response->get_processed(), _response->get_failed(),
             serialize(_response->get_data()));

    ASSERT_TRUE(_response->get_processed());
    ASSERT_TRUE(!_response->get_failed());

    test_response_base_protocol_structure(_response, "success", "no effect", _transaction_id);

    ASSERT_TRUE(_response->get_data().contains("data"));
    ASSERT_TRUE(_response->get_data().at("data").is_object());

    _state->remove_client(_client->get_id());
}

TEST(handlers_send_handler_test, can_handle_send_on_session) {
    const auto _state = std::make_shared<state>();

    const auto _session = std::make_shared<session>(_state, boost::asio::ip::tcp::socket{_state->get_ioc()}, remote);
    const auto _client = std::make_shared<client>(_state->get_id(), _state);
    const auto _other = std::make_shared<client>(_session->get_id(), _state);

    _state->push_client(_client);
    _state->push_client(_other);
    _state->add_session(_session);

    const auto _transaction_id = boost::uuids::random_generator()();
    const boost::json::object _data = {
        {"action", "send"},
        {"transaction_id", to_string(_transaction_id)},
        {
            "params",
            {
                {"from_client_id", to_string(_other->get_id())},
                {"to_client_id", to_string(_client->get_id())},
                {"payload", {{"message", "EHLO"}}}
            }
        }
    };

    const auto _response = kernel(_state, _data, on_session, _session->get_id());

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

TEST(handlers_send_handler_test, can_handle_send_no_effect_on_session) {
    const auto _state = std::make_shared<state>();

    const auto _session = std::make_shared<session>(_state, boost::asio::ip::tcp::socket{_state->get_ioc()}, remote);
    const auto _client = std::make_shared<client>(_state->get_id(), _state);
    const auto _other = std::make_shared<client>(_session->get_id(), _state);

    _state->push_client(_client);
    _state->push_client(_other);
    _state->add_session(_session);

    const auto _transaction_id = boost::uuids::random_generator()();
    const boost::json::object _data = {
        {"action", "send"},
        {"transaction_id", to_string(_transaction_id)},
        {
            "params",
            {
                {"from_client_id", to_string(_client->get_id())},
                {"to_client_id", to_string(_other->get_id())},
                {"payload", {{"message", "EHLO"}}}
            }
        }
    };

    const auto _response = kernel(_state, _data, on_session, _session->get_id());

    LOG_INFO("response processed={} failed={} data={}", _response->get_processed(), _response->get_failed(),
             serialize(_response->get_data()));

    ASSERT_TRUE(_response->get_processed());
    ASSERT_TRUE(!_response->get_failed());

    test_response_base_protocol_structure(_response, "success", "no effect", _transaction_id);

    ASSERT_TRUE(_response->get_data().contains("data"));
    ASSERT_TRUE(_response->get_data().at("data").is_object());

    _state->remove_client(_client->get_id());
}

TEST(handlers_send_handler_test, can_handle_send_no_effect_on_session_when_missing_client) {
    const auto _state = std::make_shared<state>();

    const auto _session = std::make_shared<session>(_state, boost::asio::ip::tcp::socket{_state->get_ioc()}, remote);
    const auto _client = std::make_shared<client>(_state->get_id(), _state);
    const auto _other = std::make_shared<client>(_session->get_id(), _state);

    _state->push_client(_client);
    _state->add_session(_session);

    const auto _transaction_id = boost::uuids::random_generator()();
    const boost::json::object _data = {
        {"action", "send"},
        {"transaction_id", to_string(_transaction_id)},
        {
            "params",
            {
                {"from_client_id", to_string(_client->get_id())},
                {"to_client_id", to_string(_other->get_id())},
                {"payload", {{"message", "EHLO"}}}
            }
        }
    };

    const auto _response = kernel(_state, _data, on_session, _session->get_id());

    LOG_INFO("response processed={} failed={} data={}", _response->get_processed(), _response->get_failed(),
             serialize(_response->get_data()));

    ASSERT_TRUE(_response->get_processed());
    ASSERT_TRUE(!_response->get_failed());

    test_response_base_protocol_structure(_response, "success", "no effect", _transaction_id);

    ASSERT_TRUE(_response->get_data().contains("data"));
    ASSERT_TRUE(_response->get_data().at("data").is_object());

    _state->remove_client(_client->get_id());
}
