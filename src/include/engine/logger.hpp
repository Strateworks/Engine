// Copyright (c) 2025 Ian Torres <iantorres@outlook.com>.
// All rights reserved.

#pragma once

#ifndef ENGINE_LOGGER_HPP
#define ENGINE_LOGGER_HPP

#include <spdlog/spdlog.h>

#ifdef DEBUG_ENABLED
#define LOG_INFO(...) spdlog::info(__VA_ARGS__)
#else
#define LOG_INFO(...) ((void)0)
#endif

#endif // ENGINE_LOGGER_HPP
