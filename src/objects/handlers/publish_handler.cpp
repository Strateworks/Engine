// Copyright (c) 2025 — 2026 Ian Torres <iantorres@outlook.com>.
// All rights reserved.

#include <engine/handlers/publish_handler.hpp>

#include <engine/state.hpp>
#include <engine/session.hpp>
#include <engine/request.hpp>
#include <engine/kernel_context.hpp>

#include <engine/logger.hpp>

#include <engine/validators/publish_validator.hpp>

#include <engine/utils.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace engine::handlers {
    void publish_handler(const request &request) {
        auto &_state = request.state_;

        if (validators::publish_validator(request)) {
            const auto &_params = get_params(request);
            const auto _channel = get_param_as_string(_params, "channel");
            const auto &_payload = get_param_as_object(_params, "payload");
            std::size_t _count = 0;
            switch (request.context_) {
                case on_client: {
                    _count = _state->publish_to_clients(
                        request,
                        _state->get_id(),
                        request.entity_id_,
                        _channel,
                        _payload
                    );

                    const auto _ = request.state_->publish_to_sessions(
                        request,
                        request.entity_id_,
                        _channel,
                        _payload
                    );
                    boost::ignore_unused(_);


                    LOG_INFO(
                        "state_id=[{}] action=[publish] context=[{}] client_id=[{}] channel=[{}] count=[{}] size=[{}]",
                        to_string(request.state_->get_id()), kernel_context_to_string(request.context_),
                        to_string(request.entity_id_), _channel, _count, _payload.size());
                    break;
                }
                case on_session: {
                    const auto &_client_id = get_param_as_id(_params, "client_id");
                    _count = _state->publish_to_clients(
                        request,
                        _state->get_id(),
                        _client_id,
                        _channel,
                        _payload
                    );

                    LOG_INFO(
                        "state_id=[{}] action=[publish] context=[{}] session_id=[{}] client_id=[{}] channel=[{}] count=[{}] size=[{}]",
                        to_string(request.state_->get_id()), kernel_context_to_string(request.context_),
                        to_string(request.entity_id_), to_string(_client_id), _channel, _count, _payload.size());

                    break;
                }
            }

            const auto _status = get_status(_count > 0);

            next(request, _status, {
                     {"count", _count}
                 });
        }
    }
}
