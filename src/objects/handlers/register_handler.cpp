// Copyright (c) 2025 Ian Torres <iantorres@outlook.com>.
// All rights reserved.

#include <engine/logger.hpp>
#include <engine/handlers/register_handler.hpp>

#include <engine/state.hpp>
#include <engine/request.hpp>
#include <engine/session.hpp>

#include <engine/validators/register_validator.hpp>

#include <engine/utils.hpp>

#include <boost/uuid/uuid_io.hpp>

namespace engine::handlers {
    void register_handler(const request &request) {
        auto &_state = request.state_;

        switch (request.context_) {
            case on_client: {
                next(request, "no effect");
                break;
            }
            case on_session: {
                if (validators::register_validator(request)) {
                    const auto &_params = get_params(request);
                    const auto _sessions_port = get_param_as_number(_params, "sessions_port");
                    const auto _clients_port = get_param_as_number(_params, "clients_port");
                    const auto _registered = get_param_as_bool(_params, "registered");

                    if (const auto _session = _state->get_session(request.entity_id_); _session.has_value()) {
                        const auto &_instance = _session.value();
                        _instance->set_clients_port(_clients_port);
                        _instance->set_sessions_port(_sessions_port);
                        _instance->mark_as_registered();

                        LOG_INFO(
                            "state_id=[{}] action=[register] context=[{}] session_id=[{}] sessions_port=[{}] clients_port=[{}] registered=[{}] status=[ok]",
                            to_string(request.state_->get_id()), kernel_context_to_string(request.context_),
                            to_string(request.entity_id_), _sessions_port, _clients_port, _registered);

                        _state->sync(_instance, _registered);

                        next(request, "ok");
                    } else {
                        next(request, "no effect");

                        LOG_INFO("state_id=[{}] action=[register] context=[{}] status=[no effect]",
                                 to_string(request.state_->get_id()), kernel_context_to_string(request.context_));
                    }
                }
                break;
            }
        }
    }
}
