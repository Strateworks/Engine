// Copyright (c) 2025 Ian Torres <iantorres@outlook.com>.
// All rights reserved.

#pragma once

#ifndef ENGINE_HANDLERS_SEND_HANDLER_HPP
#define ENGINE_HANDLERS_SEND_HANDLER_HPP

namespace engine {
    /**
     * Forward Request
     */
    struct request;

    namespace handlers {
        /**
         * Send Handler
         *
         * @param request
         */
        void send_handler(const request &request);
    }
} // namespace engine

#endif  // ENGINE_HANDLERS_SEND_HANDLER_HPP
