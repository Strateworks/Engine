// Copyright (c) 2025 — 2026 Ian Torres <iantorres@outlook.com>.
// All rights reserved.

#pragma once

#ifndef ENGINE_HANDLERS_LEAVE_HANDLER_HPP
#define ENGINE_HANDLERS_LEAVE_HANDLER_HPP

namespace engine {
    /**
     * Forward Request
     */
    struct request;

    namespace handlers {
        /**
         * Leave Handler
         *
         * @param request
         */
        void leave_handler(const request &request);
    }
} // namespace engine

#endif  // ENGINE_HANDLERS_LEAVE_HANDLER_HPP
