// Copyright (c) 2025 — 2026 Ian Torres <iantorres@outlook.com>.
// All rights reserved.

#pragma once

#ifndef ENGINE_VALIDATORS_SESSION_VALIDATOR_HPP
#define ENGINE_VALIDATORS_SESSION_VALIDATOR_HPP

namespace engine {
    /**
     * Forward Request
     */
    struct request;

    namespace validators {
        /**
         * Session Validator
         *
         * @param request
         * @return bool
         */
        bool session_validator(const request &request);
    }
} // namespace engine

#endif  // ENGINE_VALIDATORS_SESSION_VALIDATOR_HPP
