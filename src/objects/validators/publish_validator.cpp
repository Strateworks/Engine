// Copyright (c) 2025 Ian Torres <iantorres@outlook.com>.
// All rights reserved.

#include <engine/validators/publish_validator.hpp>

#include <engine/validators/id_validator.hpp>

#include <engine/request.hpp>

#include <engine/utils.hpp>

namespace engine::validators {
    bool publish_validator(const request &request) {
        const boost::json::value &_params = get_params_as_value(request);
        const boost::json::object &_params_object = _params.as_object();
        if (!_params_object.contains("channel")) {
            mark_as_invalid(request, "params", "params channel attribute must be present");
            return false;
        }

        if (const boost::json::value &_channel = _params_object.at("channel"); !_channel.is_string()) {
            mark_as_invalid(request, "params", "params channel attribute must be string");
            return false;
        }

        if (!_params_object.contains("payload")) {
            mark_as_invalid(request, "params", "params payload attribute must be present");
            return false;
        }

        if (const boost::json::value &_payload = _params_object.at("payload"); !_payload.is_object()) {
            mark_as_invalid(request, "params", "params payload attribute must be object");
            return false;
        }

        if (request.context_ == on_session) {
            return id_validator(request, _params_object, "client_id");
        }

        return true;
    }
}
