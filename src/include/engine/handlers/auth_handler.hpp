// Copyright (c) 2025 Ian Torres <iantorres@outlook.com>.
// All rights reserved.

#pragma once

#ifndef ENGINE_HANDLERS_AUTH_HANDLER_HPP
#define ENGINE_HANDLERS_AUTH_HANDLER_HPP

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
         * Auth Handler
         *
         * @param request
         */
        void auth_handler(const request &request);
    }
} // namespace engine

#endif  // ENGINE_HANDLERS_AUTH_HANDLER_HPP
