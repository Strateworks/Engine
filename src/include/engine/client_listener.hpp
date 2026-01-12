// Copyright (c) 2025 — 2026 Ian Torres <iantorres@outlook.com>.
// All rights reserved.

#pragma once

#ifndef ENGINE_CLIENT_LISTENER_HPP
#define ENGINE_CLIENT_LISTENER_HPP

#include <memory>

#include <boost/asio/ip/tcp.hpp>
#include <boost/beast/core.hpp>

namespace engine {
    class state;

    class client_listener : public std::enable_shared_from_this<client_listener> {
        boost::asio::io_context &ioc_;
        boost::asio::ip::tcp::acceptor acceptor_;
        std::shared_ptr<state> state_;

    public:
        client_listener(boost::asio::io_context &ioc, const boost::asio::ip::tcp::endpoint &endpoint,
                        const std::shared_ptr<state> &state);

        void on_accept(const boost::beast::error_code &ec, boost::asio::ip::tcp::socket socket);

        void do_accept();

        void start();
    };
} // namespace engine

#endif  // ENGINE_CLIENT_LISTENER_HPP
