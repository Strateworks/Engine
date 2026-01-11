// Copyright (c) 2025 Ian Torres <iantorres@outlook.com>.
// All rights reserved.

#pragma once

#ifndef ENGINE_VALIDATORS_IS_SUBSCRIBED_VALIDATOR_HPP
#define ENGINE_VALIDATORS_IS_SUBSCRIBED_VALIDATOR_HPP

namespace engine {
    /**
     * Forward Request
     */
    struct request;

    namespace validators {
        /**
         * Is Subscribed Validator
         *
         * @param request
         * @return bool
         */
        bool is_subscribed_validator(const request &request);
    }
} // namespace engine

#endif  // ENGINE_VALIDATORS_IS_SUBSCRIBED_VALIDATOR_HPP
