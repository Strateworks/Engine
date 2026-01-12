// Copyright (c) 2025 — 2026 Ian Torres <iantorres@outlook.com>.
// All rights reserved.

#include <gtest/gtest.h>

#include <engine/session.hpp>
#include <engine/state.hpp>

#include <boost/uuid/random_generator.hpp>

TEST(state_test, can_be_created) {
    const auto _state = std::make_shared<engine::state>();
    ASSERT_TRUE(!boost::uuids::random_generator()().is_nil());
    ASSERT_TRUE(!_state->get_id().is_nil());
    ASSERT_TRUE(boost::uuids::random_generator()() != _state->get_id());
    ASSERT_TRUE(std::chrono::system_clock::now() > _state->get_created_at());
}

TEST(state_test, can_contains_sessions) {
    const auto _state = std::make_shared<engine::state>();

    boost::asio::io_context _io_context;
    boost::asio::ip::tcp::socket _socket(_io_context);
    const auto _session = std::make_shared<engine::session>(_state, boost::asio::ip::tcp::socket{ _io_context });
    ASSERT_EQ(_state->get_session(_session->get_id()), std::nullopt);
    ASSERT_EQ(_state->get_sessions().size(), 0);
    _state->add_session(_session);
    ASSERT_EQ(_state->get_sessions().size(), 1);
    ASSERT_TRUE(_state->get_session(_session->get_id()).has_value());
    ASSERT_EQ(_state->get_session(_session->get_id()).value()->get_id(), _session->get_id());
    _state->remove_session(_session->get_id());
    ASSERT_EQ(_state->get_sessions().size(), 0);
    ASSERT_EQ(_state->get_session(_session->get_id()), std::nullopt);
}
