// Copyright (c) 2025 Ian Torres <iantorres@outlook.com>.
// All rights reserved.

#pragma once

#ifndef ENGINE_SESSION_HPP
#define ENGINE_SESSION_HPP

#include <engine/session_context.hpp>

#include <memory>
#include <boost/asio/ip/tcp.hpp>
#include <boost/json/object.hpp>
#include <boost/uuid/uuid.hpp>

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/ssl/ssl_stream.hpp>
#include <boost/uuid/random_generator.hpp>

namespace engine {
    /**
     * Forward State
     */
    class state;

    /**
     * Session
     */
    class session : public std::enable_shared_from_this<session> {
        /**
         * Sessions port
         */
        unsigned short sessions_port_ = 0;

        /**
         * Clients port
         */
        unsigned short clients_port_ = 0;

        /**
         * Host
         */
        std::string host_;

        /**
         * Registered
         */
        std::atomic<bool> registered_ = false;

        /**
         * Context
         */
        session_context context_;

    public:
        /**
         * Constructor
         *
         * @param state
         * @param socket
         * @param context
         * @param id
         */
        session(const std::shared_ptr<state> &state, boost::asio::ip::tcp::socket &&socket, session_context context = remote, boost::uuids::uuid id = boost::uuids::random_generator()());

        /**
         * Destructor
         */
        ~session();

        /**
         * Get ID
         *
         * @return uuid
         */
        boost::uuids::uuid get_id() const;

        /**
         * Get Socket
         *
         * @return tcp::socket
         */
        boost::beast::websocket::stream<boost::asio::ssl::stream<boost::beast::tcp_stream>> &get_socket();

        /**
         * Send
         *
         * @param data
         */
        void send(std::shared_ptr<std::string const> const &data);

        /**
         * Run
         */
        void run();

        /**
         * Get Sessions Port
         *
         * @return
         */
        unsigned short get_sessions_port() const;

        /**
         * Set Sessions Port
         *
         * @param port
         */
        void set_sessions_port(unsigned short port);

        /**
         * Get Clients Port
         *
         * @return
         */
        unsigned short get_clients_port() const;

        /**
         * Set Clients Port
         *
         * @param port
         */
        void set_clients_port(unsigned short port);

        /**
         * Get Host
         *
         * @return
         */
        std::string get_host() const;

        /**
         * Set Host
         *
         * @param host
         */
        void set_host(const std::string &host);

        /**
         * Mark As Registered
         */
        void mark_as_registered();

        /**
         * Get Registered
         *
         * @return
         */
        bool get_registered() const;
    private:
        /**
         * State
         */
        std::shared_ptr<state> state_;

        /**
         * ID
         */
        boost::uuids::uuid id_;

        /**
         * Socket
         */
        boost::beast::websocket::stream<boost::asio::ssl::stream<boost::beast::tcp_stream>> socket_;

        /**
         * Buffer
         */
        boost::beast::flat_buffer buffer_;

        /**
         * Queue
         */
        std::vector<std::shared_ptr<std::string const> > queue_;

        /**
         * TLS Shutdown Started
         */
        std::atomic<bool> tls_shutdown_started_{false};

        /**
         * On Run
         */
        void on_run();

        /**
         * On Accept
         *
         * @param ec
         */
        void on_accept(const boost::beast::error_code &ec);

        /**
         * On Client Handshake
         *
         * @param ec
         */
        void on_client_handshake(const boost::beast::error_code &ec);

        /**
         * On Server Handshake
         *
         * @param ec
         */
        void on_server_handshake(const boost::beast::error_code &ec);

        /**
         * Do Read
         */
        void do_read();

        /**
         * On Read
         *
         * @param ec
         * @param bytes_transferred
         */
        void on_read(const boost::system::error_code &ec, std::size_t bytes_transferred);

        /**
         * On Send
         *
         * @param data
         */
        void on_send(std::shared_ptr<std::string const> const &data);

        /**
         * On Write
         *
         * @param ec
         * @param bytes_transferred
         */
        void on_write(const boost::beast::error_code &ec, std::size_t bytes_transferred);

        /**
         * Do TLS Shutdown
         */
        void do_tls_shutdown();

        /**
         * On TLS Shutdown
         *
         * @param ec
         */
        void on_tls_shutdown(const boost::system::error_code& ec);

        /**
         * On TLS Shutdown Complete
         *
         * @param ec
         */
        void on_tls_shutdown_complete(const boost::system::error_code& ec);
    };
} // namespace engine

#endif  // ENGINE_SESSION_HPP
