// Copyright (c) 2025 — 2026 Ian Torres <iantorres@outlook.com>.
// All rights reserved.

#pragma once

#ifndef ENGINE_SUBSCRIPTION_HPP
#define ENGINE_SUBSCRIPTION_HPP

#include <boost/uuid/uuid.hpp>

namespace engine {
    /**
     * Subscription
     */
    struct subscription {
        /**
         * Session ID
         */
        boost::uuids::uuid session_id_;

        /**
         * Client ID
         */
        boost::uuids::uuid client_id_;

        /**
         * Channel
         */
        std::string channel_;
    };
} // namespace engine

#endif  // ENGINE_SUBSCRIPTION_HPP
