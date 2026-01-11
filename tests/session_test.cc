// Copyright (c) 2025 Ian Torres <iantorres@outlook.com>.
// All rights reserved.

#include <gtest/gtest.h>

#include <engine/session.hpp>
#include <engine/state.hpp>
#include <boost/uuid/random_generator.hpp>

TEST(session_test, can_be_created) {
    boost::asio::io_context _io_context;
    const auto _state = std::make_shared<engine::state>();
    const auto _session = std::make_shared<engine::session>(_state,
                                                          boost::asio::ip::tcp::socket{ _io_context });

    _state->add_session(_session);

    ASSERT_TRUE(!_session->get_id().is_nil());
    ASSERT_TRUE(&_session->get_socket() == &_session->get_socket());

    _state->remove_session(_session->get_id());
}
