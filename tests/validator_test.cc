// Copyright (c) 2025 Ian Torres <iantorres@outlook.com>.
// All rights reserved.

#include <gtest/gtest.h>

#include <engine/validator.hpp>

TEST(validator_test, validates_uuid) {
    ASSERT_FALSE(engine::validator::is_uuid(""));
    ASSERT_FALSE(engine::validator::is_uuid("7"));
    ASSERT_FALSE(engine::validator::is_uuid("12345678-1234-1234-1234-1234567890"));
    ASSERT_FALSE(engine::validator::is_uuid("12345678-1234-1234-1234-1234567890123"));
    ASSERT_FALSE(engine::validator::is_uuid("12345678-1234-1234-1234-1234567890ZZ"));
    ASSERT_FALSE(engine::validator::is_uuid("g2345678-1234-1234-1234-123456789012"));
    ASSERT_FALSE(engine::validator::is_uuid("12345678-1234-1234-1234-12345678901G"));
    ASSERT_FALSE(engine::validator::is_uuid("12345678-1234-1234-1234-12345"));
    ASSERT_FALSE(engine::validator::is_uuid("12345678-1234-1234-1234-12345678901-"));
    ASSERT_FALSE(engine::validator::is_uuid("12345678-1234-1234-1234_123456789012"));
    ASSERT_FALSE(engine::validator::is_uuid("12345678-1234-1234-1234-12345678901Z"));
    ASSERT_TRUE(engine::validator::is_uuid("12345678123412341234123456789012"));
    ASSERT_TRUE(engine::validator::is_uuid("1a880b64-759e-4978-81c0-de7b90feedde"));
}
