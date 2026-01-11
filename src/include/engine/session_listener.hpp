// Copyright (c) 2025 Ian Torres <iantorres@outlook.com>.
// All rights reserved.

#pragma once

#ifndef ENGINE_SESSION_LISTENER_HPP
#define ENGINE_SESSION_LISTENER_HPP

#include <memory>

#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>

namespace engine {
    /**
     * Forward State
     */
    class state;

    /**
     * Session Listener
     */
    class session_listener : public std::enable_shared_from_this<session_listener> {
        /**
         * IO Context
         */
        boost::asio::io_context &ioc_;

        /**
         * Acceptor
         */
        boost::asio::ip::tcp::acceptor acceptor_;

        /**
         * State
         */
        std::shared_ptr<state> state_;

    public:
        /**
         * Constructor
         *
         * @param ioc
         * @param endpoint
         * @param state
         */
        session_listener(boost::asio::io_context &ioc, const boost::asio::ip::tcp::endpoint &endpoint,
                         const std::shared_ptr<state> &state);

        /**
         * On Accept
         *
         * @param ec
         * @param socket
         */
        void on_accept(const boost::beast::error_code &ec, boost::asio::ip::tcp::socket socket);

        /**
         * Do Accept
         */
        void do_accept();

        /**
         * Start
         */
        void start();
    };
} // namespace engine

#endif  // ENGINE_SESSION_LISTENER_HPP
