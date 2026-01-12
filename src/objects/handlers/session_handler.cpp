// Copyright (c) 2025 — 2026 Ian Torres <iantorres@outlook.com>.
// All rights reserved.

#include <engine/logger.hpp>
#include <engine/handlers/session_handler.hpp>

#include <engine/state.hpp>
#include <engine/request.hpp>
#include <engine/session.hpp>

#include <engine/validators/session_validator.hpp>

#include <engine/utils.hpp>
#include <boost/asio/strand.hpp>

#include <boost/uuid/uuid_io.hpp>

namespace engine::handlers {
    void session_handler(const request &request) {
        auto &_state = request.state_;

        switch (request.context_) {
            case on_client: {
                next(request, "no effect");
                break;
            }
            case on_session: {
                if (validators::session_validator(request)) {
                    const auto &_params = get_params(request);
                    const auto _host = get_param_as_string(_params, "host");
                    const auto _sessions_port = get_param_as_number(_params, "sessions_port");
                    const auto _clients_port = get_param_as_number(_params, "clients_port");


                    bool _found = false;
                    for (const auto &_session: _state->get_sessions()) {
                        if (_session->get_host() == _host && _session->get_sessions_port() == _sessions_port && _session
                            ->get_clients_port() == _clients_port) {
                            _found = true;
                        }
                    }

                    if (!_found) {
                        boost::asio::ip::tcp::resolver _resolver{make_strand(_state->get_ioc())};
                        auto const _results = _resolver.resolve(_host, std::to_string(_sessions_port));
                        const auto _remote_session = std::make_shared<session>(
                            _state, boost::asio::ip::tcp::socket{make_strand(_state->get_ioc())}, remote);
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
                        _remote_session->set_clients_port(_clients_port);
                        _remote_session->set_sessions_port(_sessions_port);
                        _remote_session->run();
                        _state->add_session(_remote_session);
                        LOG_INFO(
                            "state_id=[{}] action=[session] context=[{}] session_id=[{}] host=[{}] sessions_port=[{}] clients_port=[{}] status=[ok]",
                            to_string(request.state_->get_id()), kernel_context_to_string(request.context_),
                            to_string(_remote_session->get_id()), _host, _sessions_port, _clients_port);

                        next(request, "ok");
                    } else {
                        LOG_INFO("state_id=[{}] action=[session] context=[{}] status=[no effect]",
                                 to_string(request.state_->get_id()), kernel_context_to_string(request.context_));

                        next(request, "no effect");
                    }
                }
                break;
            }
        }
    }
}
