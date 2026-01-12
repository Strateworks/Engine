// Copyright (c) 2025 — 2026 Ian Torres <iantorres@outlook.com>.
// All rights reserved.

#pragma once

#ifndef ENGINE_CLIENTS_HPP
#define ENGINE_CLIENTS_HPP

#include <engine/client.hpp>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/mem_fun.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/composite_key.hpp>

#include <boost/uuid/uuid.hpp>

namespace engine {
    /**
     * Clients By Session
     */
    struct clients_by_session {
    };

    /**
     * Clients By Client
     */
    struct clients_by_client {
    };

    /**
     * Clients By Client and Session
     */
    struct clients_by_client_session {
    };

    using namespace boost::multi_index;

    /**
     * Clients
     */
    using clients = multi_index_container<
        std::shared_ptr<client>,
        indexed_by<
            ordered_non_unique<
                tag<clients_by_session>,
                const_mem_fun<client, boost::uuids::uuid, &client::get_session_id>
            >,
            ordered_unique<
                tag<clients_by_client>,
                const_mem_fun<client, boost::uuids::uuid, &client::get_id>
            >,
            ordered_unique<
                tag<clients_by_client_session>,
                composite_key<
                    std::shared_ptr<client>,
                    const_mem_fun<client, boost::uuids::uuid, &client::get_id>,
                    const_mem_fun<client, boost::uuids::uuid, &client::get_session_id>
                >
            >
        >
    >;
} // namespace engine

#endif  // ENGINE_CLIENTS_HPP