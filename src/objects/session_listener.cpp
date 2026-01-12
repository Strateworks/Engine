// Copyright (c) 2025 — 2026 Ian Torres <iantorres@outlook.com>.
// All rights reserved.

#include <engine/session_listener.hpp>

#include <boost/asio/strand.hpp>

#include <engine/logger.hpp>
#include <engine/session.hpp>
#include <engine/state.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace engine {
    session_listener::session_listener(boost::asio::io_context &ioc, const boost::asio::ip::tcp::endpoint &endpoint,
                                       const std::shared_ptr<state> &state) : ioc_(ioc), acceptor_(make_strand(ioc)),
                                                                              state_(state) {
        boost::beast::error_code _ec;

        acceptor_.open(endpoint.protocol(), _ec);
        if (_ec) {
            LOG_INFO("listener failed on open: {}", _ec.what());
            return;
        }

        acceptor_.set_option(boost::asio::socket_base::reuse_address(true), _ec);
        if (_ec) {
            LOG_INFO("listener failed on set option reuse address: {}", _ec.what());
            return;
        }

        acceptor_.bind(endpoint, _ec);
        if (_ec) {
            LOG_INFO("listener failed on bind: {}", _ec.what());
            return;
        }

        acceptor_.listen(boost::asio::socket_base::max_listen_connections, _ec);
        if (_ec) {
            LOG_INFO("listener failed on listen: {}", _ec.what());
            return;
        }

        state_->get_config()->sessions_port_.store(acceptor_.local_endpoint().port(), std::memory_order_release);
        LOG_INFO("state_id=[{}] sessions is listening on [{}]", to_string(state_->get_id()),
                 state_->get_config()->sessions_port_.load(std::memory_order_acquire));
    }

    void session_listener::on_accept(const boost::beast::error_code &ec, boost::asio::ip::tcp::socket socket) {
        if (ec) {
            LOG_INFO("listener failed on accept: {}", ec.what());
        } else {
            const auto _session = std::make_shared<session>(state_, std::move(socket), local);
            state_->add_session(_session);
            _session->run();
        }

        do_accept();
    }

    void session_listener::do_accept() {
        acceptor_.async_accept(
            make_strand(ioc_),
            boost::beast::bind_front_handler(
                &session_listener::on_accept,
                shared_from_this()));
    }

    void session_listener::start() {
        do_accept();
    }
} // namespace engine
