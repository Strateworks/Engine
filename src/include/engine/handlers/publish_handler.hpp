// Copyright (c) 2025 Ian Torres <iantorres@outlook.com>.
// All rights reserved.

#pragma once

#ifndef ENGINE_HANDLERS_PUBLISH_HANDLER_HPP
#define ENGINE_HANDLERS_PUBLISH_HANDLER_HPP

namespace engine {
    /**
     * Forward Request
     */
    struct request;

    namespace handlers {
        /**
         * Publish Handler
         *
         * @param request
         */
        void publish_handler(const request &request);
    }
} // namespace engine

#endif  // ENGINE_HANDLERS_PUBLISH_HANDLER_HPP
