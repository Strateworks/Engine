// Copyright (c) 2025 Ian Torres <iantorres@outlook.com>.
// All rights reserved.

#include <engine/server.hpp>

#include <engine/state.hpp>
#include <engine/session.hpp>

#include <engine/logger.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <engine/session_listener.hpp>
#include <engine/client_listener.hpp>
#include <engine/repl.hpp>
#include <boost/asio/strand.hpp>

namespace engine {
    server::server(const std::shared_ptr<config> &configuration) : state_(std::make_shared<state>(configuration)) {
    }

    void server::start_session_listener() {
        const auto &_config = state_->get_config();
        auto const _address = boost::asio::ip::make_address(_config->address_);

        session_listener_ = std::make_shared<session_listener>(state_->get_ioc(),
                                                       boost::asio::ip::tcp::endpoint{
                                                           _address, _config->sessions_port_.load(std::memory_order_acquire)
                                                       },
                                                       state_);

        session_listener_->start();
    }

    void server::start_client_listener() {
        const auto &_config = state_->get_config();
        auto const _address = boost::asio::ip::make_address(_config->address_);

        client_listener_ = std::make_shared<client_listener>(state_->get_ioc(),
                                                             boost::asio::ip::tcp::endpoint{
                                                                 _address, _config->clients_port_.load(std::memory_order_acquire)
                                                             },
                                                             state_);

        client_listener_->start();
    }

    void server::connect_to_remote() {
        auto const &_config = state_->get_config();

        auto const _results = resolve(_config->remote_address_, _config->remote_sessions_port_.load(std::memory_order_acquire));

        const auto _remote_session = std::make_shared<session>(
            state_, boost::asio::ip::tcp::socket{make_strand(state_->get_ioc())}, remote);
        auto &_socket = _remote_session->get_socket();
        auto &_lowest_socket = _socket.next_layer().lowest_layer();

        while (!_lowest_socket.is_open()) {
            try {
                boost::asio::connect(_lowest_socket, _results);
            } catch (std::exception &e) {
                LOG_INFO("Connection refused ... retrying : {}", e.what());
                boost::system::error_code _ec;
                _lowest_socket.close(_ec);
                std::this_thread::sleep_for(std::chrono::seconds(3));
            }
        }
        _remote_session->set_sessions_port(_config->remote_sessions_port_.load(std::memory_order_acquire));
        _remote_session->set_clients_port(_config->remote_clients_port_.load(std::memory_order_acquire));
        _remote_session->run();

        state_->add_session(_remote_session);
    }

    void server::run_in_threads() {
        auto const &_config = state_->get_config();
        vector_of_threads_.reserve(_config->threads_ - 1);
        for (auto i = _config->threads_ - 1; i > 0; --i)
            vector_of_threads_.emplace_back(
                [_state = this->state_->shared_from_this()]() {
                    _state->get_ioc().run();
                });
        state_->get_ioc().run();
    }

    boost::asio::ip::basic_resolver_results<boost::asio::ip::tcp> server::resolve(const std::string & host, const unsigned short port) const {
        boost::asio::ip::tcp::resolver _resolver{make_strand(state_->get_ioc())};

        return _resolver.resolve(host, std::to_string(port));
    }

    void server::start() {
        auto const &_config = state_->get_config();
        auto const _address = boost::asio::ip::make_address(_config->address_);

        LOG_INFO("state_id=[{}] action=[running] sessions_port=[{}] clients_port=[{}]", to_string(state_->get_id()),
                 _config->sessions_port_.load(std::memory_order_acquire), _config->clients_port_.load(std::memory_order_acquire));

        if (_config->is_node_) {
            LOG_INFO("state_id=[{}] action=[waiting for remote] remote_address=[{}] remote_sessions_port=[{}]", to_string(state_->get_id()),
                     _config->remote_address_, _config->remote_sessions_port_.load(std::memory_order_acquire));

            connect_to_remote();
        }

        start_session_listener();

        start_client_listener();

        if (_config->repl_enabled) {
            repl_ = std::make_unique<repl>(state_);
        }


        run_in_threads();
    }

    std::shared_ptr<config> server::get_config() const {
        return state_->get_config();
    }

    std::shared_ptr<state> server::get_state() {
        return state_;
    }

    void server::configure(const boost::program_options::variables_map &vm) const {
        auto const &_config = state_->get_config();
        _config->address_ = vm["address"].as<std::string>();
        _config->threads_ = vm["threads"].as<unsigned short>();
        _config->is_node_ = vm["is_node"].as<bool>();
        _config->sessions_port_ = vm["sessions_port"].as<unsigned short>();
        _config->clients_port_ = vm["clients_port"].as<unsigned short>();
        _config->remote_address_ = vm["remote_address"].as<std::string>();
        _config->remote_sessions_port_ = vm["remote_sessions_port"].as<unsigned short>();
        _config->remote_clients_port_ = vm["remote_clients_port"].as<unsigned short>();
        _config->ssl_client_listener_password_ = vm["ssl_client_listener_password"].as<std::string>();
        _config->ssl_session_listener_password_ = vm["ssl_session_listener_password"].as<std::string>();
        _config->ssl_session_password_ = vm["ssl_session_password"].as<std::string>();
        _config->ssl_client_password_ = vm["ssl_client_password"].as<std::string>();
    }

    void server::stop() const {
        state_->get_ioc().stop();
    }
} // namespace engine
