// Copyright (c) 2025 — 2026 Ian Torres <iantorres@outlook.com>.
// All rights reserved.

#include <gtest/gtest.h>

#include <engine/server.hpp>
#include <engine/logger.hpp>
#include <engine/state.hpp>

class server_test : public testing::Test {
    std::unique_ptr<std::jthread> thread_a_;
    std::unique_ptr<std::jthread> thread_b_;
    std::unique_ptr<std::jthread> thread_c_;

protected:
    std::shared_ptr<engine::server> server_a_ = std::make_shared<engine::server>();
    std::shared_ptr<engine::server> server_b_ = std::make_shared<engine::server>();
    std::shared_ptr<engine::server> server_c_ = std::make_shared<engine::server>();

    void SetUp() override {
        const auto &_server_a_config = server_a_->get_config();
        _server_a_config->sessions_port_.store(0, std::memory_order_release);
        _server_a_config->clients_port_.store(0, std::memory_order_release);
        _server_a_config->repl_enabled = false;
        _server_a_config->threads_ = 1;

        thread_a_ = std::make_unique<std::jthread>([this]() {

            LOG_INFO("starting server A");
            server_a_->start();
            LOG_INFO("server A stopped");
        });

        LOG_INFO("waiting for server A ready");
        while (_server_a_config->clients_port_.load(std::memory_order_acquire) == 0 || _server_a_config->sessions_port_.load(std::memory_order_acquire) == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        const auto &_server_b_config = server_b_->get_config();
        _server_b_config->sessions_port_.store(0, std::memory_order_release);
        _server_b_config->clients_port_.store(0, std::memory_order_release);
        _server_b_config->is_node_ = true;
        _server_b_config->repl_enabled = false;
        _server_b_config->threads_ = 1;
        _server_b_config->remote_clients_port_.store(
            server_a_->get_config()->clients_port_.load(std::memory_order_acquire), std::memory_order_release);
        _server_b_config->remote_sessions_port_.store(
            server_a_->get_config()->sessions_port_.load(std::memory_order_acquire),
            std::memory_order_release);

        thread_b_ = std::make_unique<std::jthread>([this]() {
            LOG_INFO("starting server B");
            server_b_->start();
            LOG_INFO("server B stopped");
        });

        LOG_INFO("waiting for server B ready");
        while (_server_b_config->clients_port_.load(std::memory_order_acquire) == 0 || _server_b_config->sessions_port_.load(std::memory_order_acquire) == 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }


        const auto &_server_c_config = server_c_->get_config();
        _server_c_config->sessions_port_.store(0, std::memory_order_release);
        _server_c_config->clients_port_.store(0, std::memory_order_release);
        _server_c_config->is_node_ = true;
        _server_c_config->threads_ = 1;
        _server_c_config->repl_enabled = false;

        _server_c_config->remote_clients_port_.store(
            server_a_->get_config()->clients_port_.load(std::memory_order_acquire), std::memory_order_release);
        _server_c_config->remote_sessions_port_.store(
            server_a_->get_config()->sessions_port_.load(std::memory_order_acquire),
            std::memory_order_release);

        thread_c_ = std::make_unique<std::jthread>([this]() {
            LOG_INFO("starting server C");
            server_c_->start();
            LOG_INFO("server C stopped");
        });

        while (_server_c_config->clients_port_.load(std::memory_order_acquire) == 0 || _server_c_config->sessions_port_.load(std::memory_order_acquire) == 0 || !_server_c_config->registered_.load(std::memory_order_acquire) || server_c_->get_state()->get_sessions().size() != 2) {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    void TearDown() override {
        LOG_INFO("server A stop ...");
        server_a_->stop();

        LOG_INFO("server B stop ...");
        server_b_->stop();

        LOG_INFO("server C stop ...");
        server_c_->stop();

        while (!server_a_->get_state()->get_ioc().stopped() || !server_b_->get_state()->get_ioc().stopped() || !server_c_->get_state()->get_ioc().stopped()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        server_a_.reset();
        server_b_.reset();
        server_c_.reset();
    }
};
