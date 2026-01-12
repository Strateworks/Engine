// Copyright (c) 2025 — 2026 Ian Torres <iantorres@outlook.com>.
// All rights reserved.

#pragma once

#ifndef ENGINE_CLIENT_HPP
#define ENGINE_CLIENT_HPP

#include <memory>
#include <boost/uuid/uuid.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/json/object.hpp>

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/asio/ssl/stream.hpp>

namespace engine {
    /**
     * Forward State
     */
    class state;

    /**
     * Client
     */
    class client : public std::enable_shared_from_this<client> {
        /**
         * State
         */
        std::shared_ptr<state> state_;

        /**
         * Client ID
         */
        boost::uuids::uuid id_;

        /**
         * Session ID
         */
        boost::uuids::uuid session_id_;

        /**
         * Is Local
         */
        bool is_local_;

    public:
        /**
         * Construct
         *
         * @param session_id
         * @param state
         * @param id
         */
        explicit client(boost::uuids::uuid session_id, const std::shared_ptr<state> &state,
                        boost::uuids::uuid id = boost::uuids::random_generator()());

        /**
         * Destructor
         */
        ~client();

        /**
         * Get ID
         *
         * @return uuid
         */
        boost::uuids::uuid get_id() const;

        /**
         * Get Session ID
         *
         * @return uuid
         */
        boost::uuids::uuid get_session_id() const;

        /**
         * Get Socket
         *
         * @return tcp::socket
         */
        std::optional<boost::beast::websocket::stream<boost::asio::ssl::stream<boost::beast::tcp_stream>>> &get_socket();

        /**
         * Get Is Local
         *
         * @return
         */
        bool get_is_local() const;

        /**
         * Run
         */
        void run();

        /**
         * Send
         *
         * @param data
         */
        void send(std::shared_ptr<std::string const> const &data);

        /**
         * Set Socket
         *
         * @param socket
         */
        void set_socket(boost::asio::ip::tcp::socket &&socket);

    private:
        /**
         * Socket
         */
        std::optional<boost::beast::websocket::stream<boost::asio::ssl::stream<boost::beast::tcp_stream>>> socket_;

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
         * @param run_at
         * @param ec
         */
        void on_accept(long run_at, const boost::beast::error_code &ec);

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
         * On Handshake
         *
         * @param ec
         */
        void on_handshake(const boost::beast::error_code &ec);

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
    };
} // namespace engine

#endif  // ENGINE_CLIENT_HPP
