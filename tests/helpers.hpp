// Copyright (c) 2025 Ian Torres <iantorres@outlook.com>.
// All rights reserved.

#ifndef TESTS_HELPERS_HPP
#define TESTS_HELPERS_HPP

#include <gtest/gtest.h>

inline void test_response_base_protocol_structure(std::shared_ptr<engine::response> response, const char *status,
                                                  const char *message, boost::uuids::uuid transaction_id) {
    ASSERT_TRUE(response->get_data().contains("transaction_id"));
    ASSERT_TRUE(response->get_data().at("transaction_id").is_string());
    ASSERT_EQ(response->get_data().at("transaction_id").as_string(), to_string(transaction_id));

    ASSERT_TRUE(response->get_data().contains("status"));
    ASSERT_TRUE(response->get_data().at("status").is_string());
    ASSERT_EQ(response->get_data().at("status").as_string(), status);
    ASSERT_TRUE(response->get_data().contains("message"));
    ASSERT_TRUE(response->get_data().at("message").is_string());
    ASSERT_EQ(response->get_data().at("message").as_string(), message);
    ASSERT_TRUE(response->get_data().contains("data"));
    ASSERT_TRUE(response->get_data().at("data").is_object());

    ASSERT_TRUE(response->get_data().contains("runtime"));
    ASSERT_TRUE(response->get_data().at("runtime").is_number());
    ASSERT_TRUE(response->get_data().at("runtime").as_int64() > 0);

    ASSERT_TRUE(response->get_data().contains("timestamp"));
    ASSERT_TRUE(response->get_data().at("timestamp").is_number());
    ASSERT_TRUE(response->get_data().at("timestamp").as_int64() > 0);
    ASSERT_TRUE(
        response->get_data().at("timestamp").as_int64() < std::chrono::system_clock::now().
        time_since_epoch().count());
}

#endif // TESTS_HELPERS_HPP
