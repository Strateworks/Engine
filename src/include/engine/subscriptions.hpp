// Copyright (c) 2025 — 2026 Ian Torres <iantorres@outlook.com>.
// All rights reserved.

#pragma once

#ifndef ENGINE_SUBSCRIPTIONS_HPP
#define ENGINE_SUBSCRIPTIONS_HPP

#include <engine/subscription.hpp>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/member.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/composite_key.hpp>

#include <boost/uuid/uuid.hpp>

namespace engine {
    /**
     * Subscriptions By Session
     */
    struct subscriptions_by_session {
    };

    /**
     * Subscriptions By Client
     */
    struct subscriptions_by_client {
    };

    /**
     * Subscriptions By Channel
     */
    struct subscriptions_by_channel {
    };

    /**
     * Subscriptions By Client Channel
     */
    struct subscriptions_by_client_channel {};

    /**
     * Subscriptions By Session, Client and Channel
     */
    struct subscriptions_by_session_client_channel {
    };

    /**
     * Subscriptions
     */
    using subscriptions = boost::multi_index::multi_index_container<
        subscription,
        boost::multi_index::indexed_by<
            boost::multi_index::ordered_non_unique<
                boost::multi_index::tag<subscriptions_by_session>,
                boost::multi_index::member<subscription, boost::uuids::uuid, &subscription::session_id_>
            >,
            boost::multi_index::ordered_non_unique<
                boost::multi_index::tag<subscriptions_by_client>,
                boost::multi_index::member<subscription, boost::uuids::uuid, &subscription::client_id_>
            >,
            boost::multi_index::ordered_non_unique<
                boost::multi_index::tag<subscriptions_by_channel>,
                boost::multi_index::member<subscription, std::string, &subscription::channel_>
            >,
            boost::multi_index::ordered_non_unique<
                boost::multi_index::tag<subscriptions_by_client_channel>,
                boost::multi_index::composite_key<
                    subscription,
                    boost::multi_index::member<subscription, boost::uuids::uuid, &subscription::client_id_>,
                    boost::multi_index::member<subscription, std::string, &subscription::channel_>
                >
            >,
            boost::multi_index::ordered_unique<
                boost::multi_index::tag<subscriptions_by_session_client_channel>,
                boost::multi_index::composite_key<
                    subscription,
                    boost::multi_index::member<subscription, boost::uuids::uuid, &subscription::session_id_>,
                    boost::multi_index::member<subscription, boost::uuids::uuid, &subscription::client_id_>,
                    boost::multi_index::member<subscription, std::string, &subscription::channel_>
                >
            >
        >
    >;
} // namespace engine

#endif  // ENGINE_SUBSCRIPTIONS_HPP
