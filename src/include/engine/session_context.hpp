// Copyright (c) 2025 Ian Torres <iantorres@outlook.com>.
// All rights reserved.

#pragma once

#ifndef ENGINE_SESSION_MODE_HPP
#define ENGINE_SESSION_MODE_HPP

#include <memory>
#include <boost/asio/ip/tcp.hpp>
#include <boost/json/object.hpp>
#include <boost/uuid/uuid.hpp>

#include <boost/beast/core.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/uuid/random_generator.hpp>

namespace engine {
    enum session_context {
      local,
      remote,
    };
} // namespace engine

#endif  // ENGINE_SESSION_MODE_HPP
