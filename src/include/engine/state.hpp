// Copyright (c) 2025 — 2026 Ian Torres <iantorres@outlook.com>.
// All rights reserved.

#pragma once

#ifndef ENGINE_STATE_HPP
#define ENGINE_STATE_HPP

#include <engine/config.hpp>
#include <engine/subscriptions.hpp>
#include <engine/clients.hpp>

#include <boost/uuid/uuid.hpp>
#include <chrono>
#include <map>
#include <memory>
#include <vector>
#include <shared_mutex>

#include <boost/json/object.hpp>
#include <boost/asio/io_context.hpp>
#include <boost/asio/ssl/context.hpp>

namespace engine {
    /**
     * Forward Session
     */
    class session;

    /**
     * Forward Request
     */
    struct request;

    /**
     * Instance
     */
    class state : public std::enable_shared_from_this<state> {
        /**
         * IO Context
         */
        boost::asio::io_context ioc_;

        /**
         * Session Listener SSL Context
         */
        boost::asio::ssl::context session_listener_ssl_context_;

        /**
         * Session SSL Context
         */
        boost::asio::ssl::context session_ssl_context_;

        /**
         * Client Listener SSL Context
         */
        boost::asio::ssl::context client_listener_ssl_context_;

        /**
         * Client SSL Context
         */
        boost::asio::ssl::context client_ssl_context_;

        /**
         * Config
         */
        std::shared_ptr<config> config_;

    public:
        /**
         * Constructor
         */
        explicit state(const std::shared_ptr<config> &configuration = std::make_shared<config>());

        /**
         * Destructor
         */
        ~state();

        /**
         * Get ID
         *
         * @return uuid
         */
        boost::uuids::uuid get_id() const;

        /**
         * Get Created At
         *
         * @return system_clock::time_point
         */
        std::chrono::system_clock::time_point get_created_at() const;

        /**
         * Get Sessions
         *
         * @return vector<shared_ptr<session>>
         */
        std::vector<std::shared_ptr<session> > get_sessions() const;

        /**
         * Get Clients
         *
         * @return vector<uuid>
         */
        std::vector<std::shared_ptr<client> > get_clients() const;

        /**
         * Get Subscriptions
         *
         * @return vector<subscription>
         */
        std::vector<subscription> get_subscriptions() const;

        /**
         * Get Client Exists
         *
         * @param client_id
         * @return bool
         */
        bool get_client_exists(boost::uuids::uuid client_id) const;

        /**
         * Get Session
         *
         * @param id uuid
         * @return optional<std::shared_ptr<session>>
         */
        std::optional<std::shared_ptr<session> > get_session(
            boost::uuids::uuid id) const;

        /**
         * Get Client
         *
         * @param id uuid
         * @return optional<client>
         */
        std::optional<std::shared_ptr<client> > get_client(
            boost::uuids::uuid id) const;

        /**
         * Add Session
         *
         * @param session shared_ptr<session>
         * @return bool
         */
        bool add_session(std::shared_ptr<session> session);

        /**
         * Remove Session
         *
         * @param id uuid
         */
        bool remove_session(boost::uuids::uuid id);


        /**
         * Add Client
         *
         * @param client
         * @return
         */
        bool add_client(const std::shared_ptr<client> &client);

        /**
         * Remove Client
         *
         * @param client_id
         * @return bool
         */
        bool remove_client(boost::uuids::uuid client_id);

        /**
         * Subscribe
         *
         * @param session_id
         * @param client_id
         * @param channel
         * @return bool
         */
        bool subscribe(const boost::uuids::uuid &session_id, const boost::uuids::uuid &client_id,
                       const std::string &channel);

        /**
         * Unsubscribe
         *
         * @param session_id
         * @param client_id
         * @param channel
         * @return bool
         */
        bool unsubscribe(const boost::uuids::uuid &session_id, const boost::uuids::uuid &client_id,
                         const std::string &channel);

        /**
         * Is Subscribed
         *
         * @param client_id
         * @param channel
         * @return bool
         */
        bool is_subscribed(const boost::uuids::uuid &client_id,
                           const std::string &channel);

        /**
         * Broadcast To Sessions
         *
         * @param request
         * @param client_id
         * @param data
         *
         * @return size_t
         */
        std::size_t broadcast_to_sessions(const request &request,
                                          boost::uuids::uuid client_id, const boost::json::object &data) const;

        /**
         * Broadcast To Clients
         *
         * @param request
         * @param session_id
         * @param client_id
         * @param data
         *
         * @return size_t
         */
        std::size_t broadcast_to_clients(const request &request, boost::uuids::uuid session_id,
                                         boost::uuids::uuid client_id, const boost::json::object &data) const;


        /**
         * Send To Subscribed Sessions
         *
         * @param data
         * @param channel
         * @return
         */
        std::size_t send_to_subscribed_sessions(const boost::json::object &data, const std::string &channel) const;

        /**
         * Publish To Sessions
         *
         * @param request
         * @param client_id
         * @param channel
         * @param data
         *
         * @return size_t
         */
        std::size_t publish_to_sessions(const request &request,
                                        boost::uuids::uuid client_id, const std::string &channel,
                                        const boost::json::object &data) const;

        /**
         * Publish To Clients
         *
         * @param request
         * @param session_id
         * @param client_id
         * @param channel
         * @param data
         *
         * @return size_t
         */
        std::size_t publish_to_clients(const request &request, boost::uuids::uuid session_id,
                                       boost::uuids::uuid client_id, const std::string &channel,
                                       const boost::json::object &data) const;


        /**
         * Join To Sessions
         *
         * @param client_id
         *
         * @return size_t
         */
        std::size_t join_to_sessions(boost::uuids::uuid client_id) const;

        /**
         * Leave To Sessions
         *
         * @param client_id
         *
         * @return size_t
         */
        std::size_t leave_to_sessions(boost::uuids::uuid client_id) const;

        /**
         * Subscribe To Sessions
         *
         * @param request
         * @param client_id
         * @param channel
         *
         * @return size_t
         */
        std::size_t subscribe_to_sessions(const request &request,
                                          boost::uuids::uuid client_id, const std::string &channel) const;

        /**
         *  Push Client
         *
         * @param client
         * @return bool
         */
        bool push_client(const std::shared_ptr<client> &client);

        /**
         * Sync
         *
         * @param session
         * @param registered
         */
        void sync(const std::shared_ptr<session> &session, bool registered);

        /**
         * Get IO Context
         *
         * @return
         */
        boost::asio::io_context &get_ioc();

        /**
         * Unsubscribe To Sessions
         *
         * @param request
         * @param client_id
         * @param channel
         * @return size_t
         */
        std::size_t unsubscribe_to_sessions(const request &request,
                                            boost::uuids::uuid client_id, const std::string &channel) const;

        /**
         * Remove State Of Session
         *
         * @param id
         */
        void remove_state_of_session(boost::uuids::uuid id);

        /**
         * Send To Session
         *
         * @param session_id
         * @param from_client_id
         * @param to_client_id
         * @param payload
         */
        void send_to_session(boost::uuids::uuid session_id, boost::uuids::uuid from_client_id,
                             boost::uuids::uuid to_client_id, const boost::json::object &payload) const;

        /**
         * Get Config
         *
         * @return shared<config>
         */
        std::shared_ptr<config> get_config();

        /**
         * Get Session Listener SSL Context
         *
         * @return
         */
        boost::asio::ssl::context & get_session_listener_ssl_context();

        /**
         * Get Session SSL Context
         *
         * @return
         */
        boost::asio::ssl::context & get_session_ssl_context();

        /**
         * Get Client Listener SSL Context
         *
         * @return
         */
        boost::asio::ssl::context & get_client_listener_ssl_context();

        /**
         * Get Client SSL Context
         *
         * @return
         */
        boost::asio::ssl::context & get_client_ssl_context();

    private:
        /**
         * Send To Sessions
         *
         * @param data
         * @return
         */
        std::size_t send_to_sessions(const boost::json::object &data) const;

        /**
         * Send To Clients
         *
         * @param data
         * @param session_id Sesión que recibió la solicitud del Cliente
         * @param client_id Cliente que solicitó transmitir
         * @return
         */
        std::size_t send_to_others_clients(const std::shared_ptr<boost::json::object> &data,
                                           boost::uuids::uuid session_id,
                                           boost::uuids::uuid client_id) const;

        /**
         * ID
         */
        boost::uuids::uuid id_;

        /**
         * Created At
         */
        std::chrono::system_clock::time_point created_at_;

        /**
         * Sessions
         */
        std::map<boost::uuids::uuid, std::shared_ptr<session> > sessions_;

        /**
         * Sessions Shared Mutex
         */
        mutable std::shared_mutex sessions_mutex_;

        /**
         * Clients
         */
        clients clients_;

        /**
         * Clients Shared Mutex
         */
        mutable std::shared_mutex clients_mutex_;

        /**
         * Subscriptions
         */
        subscriptions subscriptions_;

        /**
         * Sessions Shared Mutex
         */
        mutable std::shared_mutex subscriptions_mutex_;
    };
} // namespace engine

#endif  // ENGINE_STATE_HPP
