// Copyright (c) 2025 Ian Torres <iantorres@outlook.com>.
// All rights reserved.

#include <engine/handlers/ping_handler.hpp>

#include <engine/request.hpp>

#include <engine/utils.hpp>
#include <engine/logger.hpp>
#include <boost/uuid/uuid_io.hpp>

namespace engine::handlers {
    void ping_handler(const request &request) {
        LOG_INFO("state_id=[{}] action=[ping] context=[{}] status=[{}]", to_string(request.state_->get_id()),
                 kernel_context_to_string(request.context_), "pong");

        next(request, "pong");
    }
}
