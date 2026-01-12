// Copyright (c) 2025 — 2026 Ian Torres <iantorres@outlook.com>.
// All rights reserved.

#pragma once

#ifndef ENGINE_VALIDATORS_PUBLISH_VALIDATOR_HPP
#define ENGINE_VALIDATORS_PUBLISH_VALIDATOR_HPP

namespace engine {
    /**
     * Forward Request
     */
    struct request;

    namespace validators {
        /**
         * Publish Validator
         *
         * @param request
         * @return bool
         */
        bool publish_validator(const request &request);
    }
} // namespace engine

#endif  // ENGINE_VALIDATORS_PUBLISH_VALIDATOR_HPP
