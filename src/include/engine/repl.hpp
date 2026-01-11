// Copyright (c) 2025 Ian Torres <iantorres@outlook.com>.
// All rights reserved.

#pragma once

#ifndef ENGINE_REPL_HPP
#define ENGINE_REPL_HPP

#include <memory>
#include <boost/asio/io_context.hpp>
#include <boost/asio/posix/stream_descriptor.hpp>

namespace engine {
    /**
     * Forward State
     */
    class state;

    /**
     * REPL
     */
    class repl {
    public:
        /**
         * Constructor
         *
         * @param state
         */
        explicit repl(const std::shared_ptr<state> &state);

    private:
        /**
         * Start
         */
        void start();

        /**
         * Read
         *
         * @param error
         * @param bytes_transferred
         */
        void read(const boost::system::error_code &error, std::size_t bytes_transferred);

        /**
         * Write
         *
         * @param error
         */
        void write(const boost::system::error_code &error);

        /**
         * Input
         */
        boost::asio::posix::stream_descriptor input_;

        /**
         * Output
         */
        boost::asio::posix::stream_descriptor output_;

        /**
         * Buffer
         */
        std::string buffer_;

        /**
         * State
         */
        std::shared_ptr<state> state_;
    };
} // namespace engine

#endif  // ENGINE_REPL_HPP
