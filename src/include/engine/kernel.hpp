// Copyright (c) 2025 Ian Torres <iantorres@outlook.com>.
// All rights reserved.

#pragma once

#ifndef ENGINE_KERNEL_HPP
#define ENGINE_KERNEL_HPP

#include <engine/kernel_context.hpp>

#include <boost/json/object.hpp>
#include <boost/uuid/uuid.hpp>
#include <memory>

namespace engine {
    /**
     * Forward State
     */
    class state;

    /**
     * Forward Session
     */
    class session;

    /**
     * Forward Client
     */
    class client;

    /**
     * Forward Response
     */
    class response;

    /**
     * Kernel
     *
     * @param state
     * @param data
     * @param context
     * @param entity_id
     * @return shared_ptr<response>
     */
    std::shared_ptr<response> kernel(const std::shared_ptr<state> &state,
                                     const boost::json::object &data, kernel_context context, boost::uuids::uuid entity_id);
} // namespace engine

#endif  // ENGINE_KERNEL_HPP
