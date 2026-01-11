// Copyright (c) 2025 Ian Torres <iantorres@outlook.com>.
// All rights reserved.

#pragma once

#ifndef ENGINE_VALIDATOR_HPP
#define ENGINE_VALIDATOR_HPP

#include <boost/json/object.hpp>
#include <map>

namespace engine {
    /**
     * Validator
     */
    class validator {
        /**
         * Passed
         */
        bool passed_ = false;

        /**
         * Bag
         */
        std::map<std::string, std::string> bag_;

    public:
        /**
         * Constructor
         *
         * @param data
         */
        explicit validator(boost::json::object data);

        /**
         * Get Passed
         *
         * @return bool
         */
        [[nodiscard]] bool get_passed() const;

        /**
         * Get Bag
         *
         * @return map<string, string>
         */
        [[nodiscard]] std::map<std::string, std::string> get_bag() const;

        /**
         * Is UUID
         *
         * @param uuid
         * @return
         */
        static bool is_uuid(const char *uuid);
    };
} // namespace engine

#endif  // ENGINE_VALIDATOR_HPP
