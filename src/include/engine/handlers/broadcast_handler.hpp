// Copyright (c) 2025 — 2026 Ian Torres <iantorres@outlook.com>.
// All rights reserved.

#pragma once

#ifndef ENGINE_HANDLERS_BROADCAST_HANDLER_HPP
#define ENGINE_HANDLERS_BROADCAST_HANDLER_HPP

namespace engine {
    /**
     * Forward Request
     */
    struct request;

    /**
     * Forward Response
     */
    class response;

    /**
     * Forward Session
     */
    class session;

    /**
     * Forward State
     */
    class state;

    namespace handlers {
        /**
         * Broadcast Handler
         *
         * @param request
         */
        void broadcast_handler(const request &request);
    }
} // namespace engine

#endif  // ENGINE_HANDLERS_BROADCAST_HANDLER_HPP
