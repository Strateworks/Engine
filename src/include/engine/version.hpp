// Copyright (c) 2025 — 2026 Ian Torres <iantorres@outlook.com>.
// All rights reserved.

#pragma once

#ifndef ENGINE_VERSION_HPP
#define ENGINE_VERSION_HPP

namespace engine::version {
    /**
     * Get major
     *
     * @return int
     */
    unsigned int get_major();

    /**
     * Get minor
     *
     * @return int
     */
    unsigned int get_minor();

    /**
     * Get patch
     *
     * @return int
     */
    unsigned int get_patch();
} // namespace engine::version

#endif  // ENGINE_VERSION_HPP
