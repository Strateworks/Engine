// Copyright (c) 2025 — 2026 Ian Torres <iantorres@outlook.com>.
// All rights reserved.

#include <engine/handlers/join_handler.hpp>

#include <engine/state.hpp>
#include <engine/request.hpp>

#include <engine/validators/id_validator.hpp>

#include <engine/utils.hpp>
#include <engine/logger.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace engine::handlers {
    void join_handler(const request &request) {
        auto &_state = request.state_;

        switch (request.context_) {
            case on_client: {
                LOG_INFO("state_id=[{}] action=[join] context=[{}] client_id=[{}] status=[{}]",
                         to_string(_state->get_id()), kernel_context_to_string(request.context_),
                         to_string(request.entity_id_), "no effect");

                next(request, "no effect");
                break;
            }
            case on_session: {
                if (const auto &_params = get_params(request);
                    validators::id_validator(request, _params, "client_id")) {
                    auto _client_id = get_param_as_id(_params, "client_id");
                    const auto _inserted = _state->add_client(
                        std::make_shared<client>(request.entity_id_, _state, _client_id));
                    const auto _status = get_status(_inserted);

                    LOG_INFO("state_id=[{}] action=[join] context=[{}] session_id=[{}] client_id=[{}] status=[{}]",
                             to_string(_state->get_id()), kernel_context_to_string(request.context_),
                             to_string(request.entity_id_), to_string(_client_id), _status);
                    next(request, _status);
                }
                break;
            }
        }
    }
}
