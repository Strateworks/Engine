// Copyright (c) 2025 Ian Torres <iantorres@outlook.com>.
// All rights reserved.

#pragma once

#ifndef ENGINE_REQUEST_HPP
#define ENGINE_REQUEST_HPP

#include <boost/uuid/uuid.hpp>
#include <boost/json/object.hpp>
#include <memory>

#include <engine/response.hpp>
#include <engine/state.hpp>

#include <engine/kernel_context.hpp>

namespace engine {
    struct request {
        const boost::uuids::uuid transaction_id_;
        std::shared_ptr<engine::response> &response_;
        const boost::uuids::uuid entity_id_;
        const kernel_context context_;
        const std::shared_ptr<engine::state> &state_;
        const boost::json::object &data_;
        long timestamp_;
        bool is_local_;
    };
} // namespace engine

#endif  // ENGINE_REQUEST_HPP
