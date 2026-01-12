// Copyright (c) 2025 — 2026 Ian Torres <iantorres@outlook.com>.
// All rights reserved.

#include <engine/client_listener.hpp>

#include <boost/asio/strand.hpp>

#include <engine/logger.hpp>
#include <engine/client.hpp>
#include <engine/state.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace engine {
    client_listener::client_listener(boost::asio::io_context &ioc, const boost::asio::ip::tcp::endpoint &endpoint,
                                     const std::shared_ptr<state> &state) : ioc_(ioc), acceptor_(make_strand(ioc)), state_(state) {
        boost::beast::error_code ec;

        acceptor_.open(endpoint.protocol(), ec);
        if (ec) {
            LOG_INFO("listener failed on open: {}", ec.what());
            return;
        }

        acceptor_.set_option(boost::asio::socket_base::reuse_address(true), ec);
        if (ec) {
            LOG_INFO("listener failed on set option reuse address: {}", ec.what());
            return;
        }

        acceptor_.bind(endpoint, ec);
        if (ec) {
            LOG_INFO("listener failed on bind: {}", ec.what());
            return;
        }

        acceptor_.listen(boost::asio::socket_base::max_listen_connections, ec);
        if (ec) {
            LOG_INFO("listener failed on listen: {}", ec.what());
            return;
        }

        state_->get_config()->clients_port_.store(acceptor_.local_endpoint().port(), std::memory_order_release);
        LOG_INFO("state_id=[{}] clients is listening on [{}]", to_string(state_->get_id()), state_->get_config()->clients_port_.load(std::memory_order_acquire));
    }

    void client_listener::on_accept(const boost::beast::error_code &ec, boost::asio::ip::tcp::socket socket) {
        if (ec) {
            LOG_INFO("listener failed on accept: {}", ec.what());
        } else {
            const auto _client = std::make_shared<client>(state_->get_id(), state_);
            _client->set_socket(std::move(socket));
            state_->add_client(_client);
            const auto _ = state_->join_to_sessions(_client->get_id());
            boost::ignore_unused(_);
            _client->run();
        }

        do_accept();
    }

    void client_listener::do_accept() {
        acceptor_.async_accept(
            make_strand(ioc_),
            boost::beast::bind_front_handler(
                &client_listener::on_accept,
                shared_from_this()));
    }

    void client_listener::start() {
        do_accept();
    }
} // namespace engine
