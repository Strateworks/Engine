// Copyright (c) 2025 — 2026 Ian Torres <iantorres@outlook.com>.
// All rights reserved.

#include <engine/validators/register_validator.hpp>

#include <engine/validators/id_validator.hpp>

#include <engine/request.hpp>
#include <engine/validator.hpp>

#include <engine/utils.hpp>

namespace engine::validators {
    bool register_validator(const request &request) {
        const boost::json::value &_params = get_params_as_value(request);
        const boost::json::object &_params_object = _params.as_object();
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

        if (!_params_object.contains("registered")) {
            mark_as_invalid(request, "params", "params registered attribute must be present");
            return false;
        }

        if (const boost::json::value &_registered = _params_object.at("registered"); !_registered.is_bool()) {
            mark_as_invalid(request, "params", "params registered attribute must be boolean");
            return false;
        }

        return true;
    }
}
