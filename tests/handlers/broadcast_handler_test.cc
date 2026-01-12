// Copyright (c) 2025 — 2026 Ian Torres <iantorres@outlook.com>.
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

TEST(handlers_broadcast_handler_test, can_handle_broadcast_on_client_context) {
    boost::asio::io_context _io_context;

    const auto _state = std::make_shared<state>();

    const auto _remote_session = std::make_shared<session>(_state, boost::asio::ip::tcp::socket { _io_context }, remote);

    const auto _local_client = std::make_shared<client>(_state->get_id(), _state);
    const auto _other_client = std::make_shared<client>(_state->get_id(), _state);

    _state->add_session(_remote_session);
    _state->add_client(_local_client);
    _state->add_client(_other_client);

    const auto _transaction_id = boost::uuids::random_generator()();
    const boost::json::object _data = {
        {"action", "broadcast"},
        {"transaction_id", to_string(_transaction_id)},
        {"params", {{"payload", {{"message", "EHLO"}}}}}
    };

    const auto _response = kernel(_state, _data, on_client, _local_client->get_id());

    LOG_INFO("response processed={} failed={} data={}", _response->get_processed(), _response->get_failed(),
             serialize(_response->get_data()));

    ASSERT_TRUE(_response->get_processed());
    ASSERT_TRUE(!_response->get_failed());

    test_response_base_protocol_structure(_response, "success", "ok", _transaction_id);

    ASSERT_TRUE(_response->get_data().at("data").as_object().contains("count"));
    ASSERT_TRUE(_response->get_data().at("data").as_object().at("count").is_number());
    ASSERT_TRUE(_response->get_data().at("data").as_object().at("count").as_uint64() > 0);

    _state->remove_session(_remote_session->get_id());
    _state->remove_client(_local_client->get_id());
    _state->remove_client(_other_client->get_id());
}

TEST(handlers_broadcast_handler_test, can_handle_broadcast_on_session_context) {
    boost::asio::io_context _io_context;

    const auto _state = std::make_shared<state>();

    const auto _remote_session = std::make_shared<session>(_state, boost::asio::ip::tcp::socket { _io_context }, remote);
    const auto _local_client = std::make_shared<client>(_state->get_id(), _state);
    const auto _remote_client = std::make_shared<client>(_remote_session->get_id(), _state);

    _state->add_session(_remote_session);
    _state->add_client(_local_client);
    _state->add_client(_remote_client);

    const auto _transaction_id = boost::uuids::random_generator()();
    const boost::json::object _data = {
        {"action", "broadcast"},
        {"transaction_id", to_string(_transaction_id)},
        {"params", {
            {"client_id", to_string(_remote_client->get_id())},
            {"payload", {{"message", "EHLO"}}}}
        }
    };

    const auto _response = kernel(_state, _data, on_session, _state->get_id());

    LOG_INFO("response processed={} failed={} data={}", _response->get_processed(), _response->get_failed(),
             serialize(_response->get_data()));

    ASSERT_TRUE(_response->get_processed());
    ASSERT_TRUE(!_response->get_failed());

    test_response_base_protocol_structure(_response, "success", "ok", _transaction_id);

    ASSERT_TRUE(_response->get_data().at("data").as_object().contains("count"));
    ASSERT_TRUE(_response->get_data().at("data").as_object().at("count").is_number());
    ASSERT_TRUE(_response->get_data().at("data").as_object().at("count").as_uint64() > 0);

    _state->remove_session(_remote_session->get_id());
    _state->remove_client(_local_client->get_id());
    _state->remove_client(_remote_client->get_id());
}