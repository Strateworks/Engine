// Copyright (c) 2025 — 2026 Ian Torres <iantorres@outlook.com>.
// All rights reserved.

#pragma once

#ifndef ENGINE_VALIDATORS_ID_VALIDATOR_HPP
#define ENGINE_VALIDATORS_ID_VALIDATOR_HPP

#include <boost/json/object.hpp>

namespace engine {
    /**
     * Forward Request
     */
    struct request;

    namespace validators {
        /**
         * ID Validator
         *
         * @param request
         * @param params
         * @param attribute
         * @return bool
         */
        bool id_validator(const request &request, const boost::json::object &params, const char * attribute);
    }
} // namespace engine

#endif  // ENGINE_VALIDATORS_ID_VALIDATOR_HPP
