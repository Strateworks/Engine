// Copyright (c) 2025 — 2026 Ian Torres <iantorres@outlook.com>.
// All rights reserved.

#pragma once

#ifndef ENGINE_CONFIG_HPP
#define ENGINE_CONFIG_HPP

#include <string>
#include <atomic>

namespace engine {
    struct config {
        /**
         * Address
         */
        std::string address_ = "0.0.0.0";

        /**
         * Sessions Port
         */
        std::atomic<unsigned short> sessions_port_ = 11000;

        /**
         * Clients Port
         */
        std::atomic<unsigned short> clients_port_ = 12000;

        /**
         * Is Node
         */
        bool is_node_ = false;

        /**
         * Remote Address
         */
        std::string remote_address_ = "127.0.0.1";

        /**
         * Remote Sessions Port
         */
        std::atomic<unsigned short> remote_sessions_port_ = 9000;

        /**
         * Remote Clients Port
         */
        std::atomic<unsigned short> remote_clients_port_ = 10000;

        /**
         * Threads
         */
        unsigned short threads_ = 1;

        /**
         * Registered
         */
        std::atomic<bool> registered_ = false;

        /**
         * REPL Enabled
         */
        bool repl_enabled = true;

        /**
         * SSL CA Certificate
         */
        std::string ssl_ca_certificate_ = "certificates/ca.crt";

        /**
         * SSL Session Listener Chain Certificate
         */
        std::string ssl_session_listener_chain_certificate_ = "certificates/session_listener.crt";

        /**
         * SSL Session Listener Private Key
         */
        std::string ssl_session_listener_private_key_ = "certificates/session_listener.key";

        /**
         * SSL Session Chain Certificate
         */
        std::string ssl_session_chain_certificate_ = "certificates/session.crt";

        /**
         * SSL Session Private Key
         */
        std::string ssl_session_private_key_ = "certificates/session.key";

        /**
         * SSL Client Listener Chain Certificate
         */
        std::string ssl_client_listener_chain_certificate_ = "certificates/client_listener.crt";

        /**
         * SSL Client Listener Private Key
         */
        std::string ssl_client_listener_private_key_ = "certificates/client_listener.key";

        /**
         * SSL Client Chain Certificate
         */
        std::string ssl_client_chain_certificate_ = "certificates/client.crt";

        /**
         * SSL Client Private Key
         */
        std::string ssl_client_private_key_ = "certificates/client.key";

        /**
         * SSL DH Params
         */
        std::string ssl_dh_params_ = "certificates/dhparams.pem";

        /**
         * SSL Session Password
         */
        std::string ssl_session_password_ = "5ec35a12";

        /**
         * SSL Session Listener Password
         */
        std::string ssl_session_listener_password_ = "610191e8";

        /**
         * SSL Client Password
         */
        std::string ssl_client_password_ = "d96ab300";

        /**
         * SSL Client Listener Password
         */
        std::string ssl_client_listener_password_ = "36e422f3";
    };
} // namespace engine

#endif  // ENGINE_CONFIG_HPP
