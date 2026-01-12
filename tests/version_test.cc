// Copyright (c) 2025 — 2026 Ian Torres <iantorres@outlook.com>.
// All rights reserved.

#include <gtest/gtest.h>

#include <engine/version.hpp>

TEST(version_test, contains_version) {
    ASSERT_EQ(engine::version::get_major(), 1);
    ASSERT_EQ(engine::version::get_minor(), 0);
    ASSERT_EQ(engine::version::get_patch(), 0);
}
