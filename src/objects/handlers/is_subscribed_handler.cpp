// Copyright (c) 2025 Ian Torres <iantorres@outlook.com>.
// All rights reserved.

#include <engine/handlers/is_subscribed_handler.hpp>

#include <engine/state.hpp>
#include <engine/request.hpp>
#include <engine/kernel_context.hpp>

#include <engine/validators/is_subscribed_validator.hpp>

#include <engine/utils.hpp>
#include <engine/logger.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace engine::handlers {
    void is_subscribed_handler(const request &request) {
        const auto &_state = request.state_;
        if (validators::is_subscribed_validator(request)) {
            const auto &_params = get_params(request);
            const auto _channel = get_param_as_string(_params, "channel");

            switch (request.context_) {
                case on_client: {
                    const auto _success = request.state_->is_subscribed(request.entity_id_, _channel);

                    const auto _status = get_status(_success);

                    LOG_INFO("state_id=[{}] action=[is_subscribed] context=[{}] client_id=[{}] status=[{}]",
                             to_string(_state->get_id()), kernel_context_to_string(request.context_),
                             to_string(request.entity_id_), _status);

                    next(request, _status);
                    break;
                }
                case on_session: {
                    next(request, "no effect");

                    LOG_INFO("state_id=[{}] action=[is_subscribed] context=[{}] session_id=[{}] status=[{}]",
                             to_string(_state->get_id()), kernel_context_to_string(request.context_),
                             to_string(request.entity_id_), "no effect");

                    break;
                }
            }
        }
    }
}
