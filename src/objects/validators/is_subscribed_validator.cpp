// Copyright (c) 2025 Ian Torres <iantorres@outlook.com>.
// All rights reserved.

#include <engine/validators/is_subscribed_validator.hpp>

#include <engine/request.hpp>
#include <engine/validator.hpp>

#include <engine/utils.hpp>

namespace engine::validators {
    bool is_subscribed_validator(const request &request) {
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

        return true;
    }
}
