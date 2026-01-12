// Copyright (c) 2025 — 2026 Ian Torres <iantorres@outlook.com>.
// All rights reserved.

#include <engine/handlers/unimplemented_handler.hpp>

#include <engine/utils.hpp>

namespace engine::handlers {
    void unimplemented_handler(const request &request) {
        mark_as_invalid(request, "action", "action attribute isn't implemented");
    }
}
