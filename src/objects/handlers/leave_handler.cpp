// Copyright (c) 2025 — 2026 Ian Torres <iantorres@outlook.com>.
// All rights reserved.

#include <engine/handlers/leave_handler.hpp>

#include <engine/validators/id_validator.hpp>

#include <engine/state.hpp>
#include <engine/request.hpp>

#include <engine/utils.hpp>
#include <engine/logger.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace engine::handlers {
    void leave_handler(const request &request) {
        auto &_state = request.state_;

        switch (request.context_) {
            case on_client: {
                LOG_INFO("state_id=[{}] action=[leave] context=[{}] client_id=[{}] status=[{}]",
                         to_string(_state->get_id()), kernel_context_to_string(request.context_),
                         to_string(request.entity_id_), "no effect");

                next(request, "no effect");
                break;
            }
            case on_session: {
                if (const auto &_params = get_params(request);
                    validators::id_validator(request, _params, "client_id")) {
                    const auto _client_id = get_param_as_id(_params, "client_id");
                    const auto _removed = _state->remove_client(_client_id);
                    const auto _status = get_status(_removed);

                    LOG_INFO("state_id=[{}] action=[leave] context=[{}] session_id=[{}] client_id=[{}] status=[{}]",
                             to_string(_state->get_id()), kernel_context_to_string(request.context_),
                             to_string(request.entity_id_), to_string(_client_id), _status);
                    next(request, _status);
                }
                break;
            }
        }
    }
}
