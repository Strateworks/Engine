// Copyright (c) 2025 Ian Torres <iantorres@outlook.com>.
// All rights reserved.

#pragma once

#ifndef ENGINE_SERVER_HPP
#define ENGINE_SERVER_HPP

#include <engine/config.hpp>

#include <memory>
#include <thread>
#include <vector>
#include <boost/asio/ip/tcp.hpp>

#include <boost/program_options/variables_map.hpp>

#include "repl.hpp"

namespace engine {
    /**
     * Forward State
     */
    class state;

    /**
     * Forward Session Listener
     */
    class session_listener;

    /**
     * Forward Client Listener
     */
    class client_listener;

    class server : public std::enable_shared_from_this<server> {
        /**
         * State
         */
        std::shared_ptr<state> state_;

        /**
         * Session Listener
         */
        std::shared_ptr<session_listener> session_listener_;

        /**
         * Client Listener
         */
        std::shared_ptr<client_listener> client_listener_;

        /**
         * Vector Of Threads
         */
        std::vector<std::jthread> vector_of_threads_;

        /**
         * REPL
         */
        std::unique_ptr<repl> repl_;
    public:
        /**
         * Constructor
         *
         * @param configuration
         */
        explicit server(const std::shared_ptr<config> &configuration = std::make_shared<config>());

        void start_session_listener();

        void start_client_listener();

        /**
         * Start
         */
        void start();

        /**
         * Get Config
         *
         * @return
         */
        std::shared_ptr<config> get_config() const;

        /**
         * Get State
         *
         * @return
         */
        std::shared_ptr<state> get_state();

        /**
         * Configure
         *
         * @param vm
         */
        void configure(const boost::program_options::variables_map &vm) const;

        /**
         * Stop
         */
        void stop() const;

    private:
        /**
         * Resolve
         *
         * @param host
         * @param port
         * @return
         */
        boost::asio::ip::basic_resolver_results<boost::asio::ip::tcp> resolve(const std::string &host, unsigned short port) const;

        /**
         * Connect To Remote
         */
        void connect_to_remote();

        /**
         * Run In Threads
         */
        void run_in_threads();
    };
} // namespace engine

#endif  // ENGINE_SERVER_HPP
