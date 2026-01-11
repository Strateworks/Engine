// Copyright (c) 2025 Ian Torres <iantorres@outlook.com>.
// All rights reserved.

#pragma once

#ifndef ENGINE_UTILS_HPP
#define ENGINE_UTILS_HPP

#include <boost/json/object.hpp>
#include <boost/uuid/uuid.hpp>
#include <string>

#include <engine/kernel_context.hpp>

namespace engine {
    /**
     * Forward Request
     */
    struct request;

    /**
     * Forward Subscription
     */
    class subscription;

    /**
     * Forward Client
     */
    class client;

    /**
     * Forward Session
     */
    class session;

    /**
     * Forward State
     */
    class state;

    /**
     * Next
     *
     * @param request
     * @param status
     * @param data
     */
    void next(const request &request, const char *status, const boost::json::object &data = {});

    /**
     * Mark As Invalid
     *
     * @param request
     * @param field
     * @param argument
     */
    void mark_as_invalid(const request &request, const char *field, const char *argument);

    /**
     * Get Param As String
     *
     * @param params
     * @param field
     * @return string
     */
    std::string get_param_as_string(const boost::json::object &params, const char *field);

    /**
     * Get Params
     *
     * @param request
     * @return object
     */
    const boost::json::object &get_params(const request &request);

    /**
     * Get Param As Object
     *
     * @param params
     * @param field
     * @return object
     */
    const boost::json::object &get_param_as_object(const boost::json::object &params, const char *field);

    /**
     * Get Params As Value
     *
     * @param request
     * @return value
     */
    const boost::json::value &get_params_as_value(const request &request);

    /**
     * Get Params As ID
     *
     * @param params
     * @param field
     * @return uuid
     */
    boost::uuids::uuid get_param_as_id(const boost::json::object &params, const char *field);

    /**
     * Get Params As Number
     *
     * @param params
     * @param field
     * @return size_t
     */
    std::size_t get_param_as_number(const boost::json::object &params, const char *field);

    /**
     * Get Params As Bool
     *
     * @param params
     * @param field
     * @return bool
     */
    bool get_param_as_bool(const boost::json::object &params, const char *field);

    /**
     * Make Broadcast Request Object
     *
     * @param request
     * @param client_id
     * @param payload
     * @return object
     */
    boost::json::object make_broadcast_request_object(const request &request,
                                                      const boost::uuids::uuid &client_id,
                                                      const boost::json::object &payload);

    /**
     * Make Join Request Object
     *
     * @param client_id
     * @return object
     */
    boost::json::object make_join_request_object(const boost::uuids::uuid &client_id);

    /**
     * Make Leave Request Object
     *
     * @param client_id
     * @return object
     */
    boost::json::object make_leave_request_object(const boost::uuids::uuid &client_id);

    /**
     * Make Publish Request Object
     *
     * @param request
     * @param client_id
     * @param channel
     * @param payload
     * @return object
     */
    boost::json::object make_publish_request_object(const request &request,
                                                    const boost::uuids::uuid &client_id,
                                                    const std::string &channel,
                                                    const boost::json::object &payload);

    /**
     * Make Subscribe Request Object
     *
     * @param request
     * @param client_id
     * @param channel
     * @return object
     */
    boost::json::object make_subscribe_request_object(const request &request,
                                                      const boost::uuids::uuid &client_id,
                                                      const std::string &channel);

    /**
     * Make Unsubscribe Request Object
     *
     * @param request
     * @param client_id
     * @param channel
     * @return object
     */
    boost::json::object make_unsubscribe_request_object(const request &request,
                                                        const boost::uuids::uuid &client_id,
                                                        const std::string &channel);

    /**
     * Get Status
     *
     * @param gate
     * @param on_true
     * @param on_false
     * @return char *
     */
    const char *get_status(bool gate, const char *on_true = "ok", const char *on_false = "no effect");

    /**
     * Kernel Context To String
     *
     * @param context
     * @return string
     */
    std::string kernel_context_to_string(kernel_context context);
}

#endif // ENGINE_UTILS_HPP
