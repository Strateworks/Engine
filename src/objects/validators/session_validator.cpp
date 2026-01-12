// Copyright (c) 2025 — 2026 Ian Torres <iantorres@outlook.com>.
// All rights reserved.

#include <engine/validators/register_validator.hpp>

#include <engine/validators/id_validator.hpp>

#include <engine/request.hpp>
#include <engine/validator.hpp>

#include <engine/utils.hpp>

namespace engine::validators {
    bool session_validator(const request &request) {
        const boost::json::value &_params = get_params_as_value(request);
        const boost::json::object &_params_object = _params.as_object();
        if (!_params_object.contains("host")) {
            mark_as_invalid(request, "params", "params host attribute must be present");
            return false;
        }

        if (const boost::json::value &_host = _params_object.at("host"); !_host.is_string()) {
            mark_as_invalid(request, "params", "params host attribute must be string");
            return false;
        }

        if (!_params_object.contains("sessions_port")) {
            mark_as_invalid(request, "params", "params sessions_port attribute must be present");
            return false;
        }

        if (const boost::json::value &_sessions_port = _params_object.at("sessions_port"); !_sessions_port.
            is_number()) {
            mark_as_invalid(request, "params", "params sessions_port attribute must be number");
            return false;
        }

        if (!_params_object.contains("clients_port")) {
            mark_as_invalid(request, "params", "params clients_port attribute must be present");
            return false;
        }

        if (const boost::json::value &_clients_port = _params_object.at("clients_port"); !_clients_port.is_number()) {
            mark_as_invalid(request, "params", "params clients_port attribute must be number");
            return false;
        }

        return true;
    }
}
