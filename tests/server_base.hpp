// Copyright (c) 2025 Ian Torres <iantorres@outlook.com>.
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
        thread_a_ = std::make_unique<std::jthread>([this]() {
            const auto &_config = server_a_->get_config();
            _config->sessions_port_.store(0, std::memory_order_release);
            _config->clients_port_.store(0, std::memory_order_release);
            _config->repl_enabled = false;
            _config->threads_ = 4;

            LOG_INFO("starting server A");
            server_a_->start();
            LOG_INFO("server A stopped");
        });

        LOG_INFO("waiting for server A ready");
        std::this_thread::sleep_for(std::chrono::seconds(3));

        thread_b_ = std::make_unique<std::jthread>([this]() {
            const auto &_config = server_b_->get_config();
            _config->sessions_port_.store(0, std::memory_order_release);
            _config->clients_port_.store(0, std::memory_order_release);
            _config->is_node_ = true;
            _config->threads_ = 4;
            _config->repl_enabled = false;

            while (server_a_->get_config()->sessions_port_.load(std::memory_order_acquire) == 0 ||
                    server_a_->get_config()->clients_port_.load(std::memory_order_acquire) == 0) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
                LOG_INFO("waiting for server A ready ...");
            }

            _config->remote_clients_port_.store(
                server_a_->get_config()->clients_port_.load(std::memory_order_acquire), std::memory_order_release);
            _config->remote_sessions_port_.store(
                server_a_->get_config()->sessions_port_.load(std::memory_order_acquire),
                std::memory_order_release);

            LOG_INFO("starting server B");
            server_b_->start();
            LOG_INFO("server B stopped");
        });

        LOG_INFO("waiting for server B ready");
        std::this_thread::sleep_for(std::chrono::seconds(5));

        thread_c_ = std::make_unique<std::jthread>([this]() {
            const auto &_config = server_c_->get_config();
            _config->sessions_port_.store(0, std::memory_order_release);
            _config->clients_port_.store(0, std::memory_order_release);
            _config->is_node_ = true;
            _config->threads_ = 4;
            _config->repl_enabled = false;

            while (server_a_->get_config()->sessions_port_.load(std::memory_order_acquire) == 0 ||
                    server_a_->get_config()->clients_port_.load(std::memory_order_acquire) == 0) {
                std::this_thread::sleep_for(std::chrono::seconds(1));
                LOG_INFO("waiting for Server A ready ...");
            }

            _config->remote_clients_port_.store(
                server_a_->get_config()->clients_port_.load(std::memory_order_acquire), std::memory_order_release);
            _config->remote_sessions_port_.store(
                server_a_->get_config()->sessions_port_.load(std::memory_order_acquire),
                std::memory_order_release);

            LOG_INFO("starting server C");
            server_c_->start();
            LOG_INFO("server C stopped");
        });

        std::this_thread::sleep_for(std::chrono::seconds(5));

        while (server_a_->get_state()->get_sessions().size() != 2 || !server_b_->get_config()->registered_.load(std::memory_order_acquire) || !server_c_->get_config()->registered_.load(std::memory_order_acquire)) {
            LOG_INFO("waiting 1 second for all servers ready ...");
            std::this_thread::sleep_for(std::chrono::seconds(1));
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
            LOG_INFO("waiting for io stop ...");
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
        server_a_.reset();
        server_b_.reset();
        server_c_.reset();
    }
};
