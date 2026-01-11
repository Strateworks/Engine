// Copyright (c) 2025 Ian Torres <iantorres@outlook.com>.
// All rights reserved.

#include <engine/validators/id_validator.hpp>

#include <engine/request.hpp>
#include <engine/validator.hpp>

#include <engine/utils.hpp>
#include <fmt/format.h>

namespace engine::validators {
    bool id_validator(const request &request, const boost::json::object &params, const char * attribute) {
        if (!params.contains(attribute)) {
            mark_as_invalid(request, "params", fmt::format("params {} attribute must be present", attribute).data());
            return false;
        }

        const boost::json::value &_value = params.at(attribute);
        if (!_value.is_string()) {
            mark_as_invalid(request, "params", fmt::format("params {} attribute must be string", attribute).data());
            return false;
        }

        if (!validator::is_uuid(_value.as_string().c_str())) {
            mark_as_invalid(request, "params", fmt::format("params {} attribute must be uuid", attribute).data());
            return false;
        }

        return true;
    }
}
