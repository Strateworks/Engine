// Copyright (c) 2025 Ian Torres <iantorres@outlook.com>.
// All rights reserved.

#pragma once

#ifndef ENGINE_RESPONSE_HPP
#define ENGINE_RESPONSE_HPP

#include <atomic>
#include <boost/json/object.hpp>
#include <boost/uuid/uuid.hpp>
#include <map>
#include <memory>

namespace engine {
    /**
     * Response
     */
    class response : public std::enable_shared_from_this<response> {
        /**
         * Failed
         */
        std::atomic<bool> failed_ = false;

        /**
         * Processed
         */
        std::atomic<bool> processed_ = false;

        /**
         * Is Ack
         */
        std::atomic<bool> is_ack_ = false;

        /**
         * Data
         */
        boost::json::object data_;

    public:
        /**
         * Get Failed
         *
         * @return bool
         */
        bool get_failed() const;

        /**
         * Get Processed
         *
         * @return bool
         */
        bool get_processed() const;

        /**
         * Is Ack
         *
         * @return bool
         */
        bool is_ack() const;

        /**
         * Get Data
         *
         * @return json::object
         */
        boost::json::object get_data() const;

        /**
         * Mark As Failed
         *
         * @param transaction_id
         * @param error
         * @param timestamp
         * @param bag
         */
        void mark_as_failed(boost::uuids::uuid transaction_id, const char *error, long timestamp,
                            const std::map<std::string, std::string> &bag);

        /**
         * Mark As Processed
         */
        void mark_as_processed();

        /**
         * Mark As Ack
         */
        void mark_as_ack();

        /**
         * Set Data
         *
         * @param transaction_id
         * @param message
         * @param timestamp
         * @param data
         */
        void set_data(boost::uuids::uuid transaction_id, const char *message, long timestamp = 0,
                      const boost::json::object &data = {});
    };
} // namespace engine

#endif  // ENGINE_RESPONSE_HPP
