// Copyright (c) 2025 Ian Torres <iantorres@outlook.com>.
// All rights reserved.

#pragma once

#ifndef ENGINE_HANDLERS_UNIMPLEMENTED_HPP
#define ENGINE_HANDLERS_UNIMPLEMENTED_HPP

namespace engine {
    /**
     * Forward Request
     */
    struct request;

    namespace handlers {
        /**
         * Unimplemented Handler
         *
         * @param request
         */
        void unimplemented_handler(const request &request);
    }
} // namespace engine

#endif  // ENGINE_HANDLERS_UNIMPLEMENTED_HPP
