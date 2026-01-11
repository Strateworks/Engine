// Copyright (c) 2025 Ian Torres <iantorres@outlook.com>.
// All rights reserved.

#include <gtest/gtest.h>

#include "server_base.hpp"
#include <engine/logger.hpp>
#include <boost/json/serialize.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/asio/strand.hpp>
#include <boost/asio/ssl/host_name_verification.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/json/parse.hpp>

TEST_F(server_test, servers_are_registered) {
    // Server isn't registered as isn't node
    ASSERT_FALSE(server_a_->get_config()->registered_);
    ASSERT_TRUE(server_b_->get_config()->registered_);
    ASSERT_TRUE(server_c_->get_config()->registered_);
    ASSERT_TRUE(server_a_->get_state()->get_sessions().size() == 2);
    ASSERT_TRUE(server_b_->get_state()->get_sessions().size() == 2);
    ASSERT_TRUE(server_c_->get_state()->get_sessions().size() == 2);
}

TEST_F(server_test, servers_accept_clients) {

    for (auto &_server : { server_a_, server_b_, server_c_ }) {
        boost::asio::io_context _ioc;
        boost::asio::ip::tcp::resolver _resolver{make_strand(_ioc)};
        boost::beast::websocket::stream<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>> _client{make_strand(_ioc), _server->get_state()->get_client_ssl_context()};

        auto const _results = _resolver.resolve("localhost", std::to_string(_server->get_config()->clients_port_.load(std::memory_order_acquire)));
        boost::asio::connect(boost::beast::get_lowest_layer(_client), _results);

        SSL_set_tlsext_host_name(_client.next_layer().native_handle(), "localhost");
        _client.next_layer().set_verify_callback(boost::asio::ssl::host_name_verification("localhost"));

        const auto _host = fmt::format("localhost:{}", std::to_string(_server->get_config()->clients_port_.load(std::memory_order_acquire)));

        _client.next_layer().handshake(boost::asio::ssl::stream_base::client);
        _client.handshake(_host, "/");

        boost::beast::flat_buffer _accepted_buffer;
        _client.read(_accepted_buffer);
        auto _accepted_message = boost::beast::buffers_to_string(_accepted_buffer.data());

        auto _accepted_object = boost::json::parse(_accepted_message);

        ASSERT_TRUE(_accepted_object.is_object());
        ASSERT_TRUE(_accepted_object.as_object().contains("action"));
        ASSERT_TRUE(_accepted_object.as_object().at("action").is_string());
        ASSERT_EQ(_accepted_object.as_object().at("action").as_string(), "welcome");
        ASSERT_TRUE(_accepted_object.as_object().contains("transaction_id"));
        ASSERT_TRUE(_accepted_object.as_object().at("transaction_id").is_string());
        ASSERT_TRUE(_accepted_object.as_object().contains("status"));
        ASSERT_TRUE(_accepted_object.as_object().at("status").is_string());
        ASSERT_EQ(_accepted_object.as_object().at("status").as_string(), "success");
        ASSERT_TRUE(_accepted_object.as_object().contains("data"));
        ASSERT_TRUE(_accepted_object.as_object().at("data").is_object());
        ASSERT_TRUE(_accepted_object.as_object().at("data").as_object().contains("client_id"));
        ASSERT_TRUE(_accepted_object.as_object().at("data").as_object().at("client_id").is_string());

        ASSERT_TRUE(_server->get_state()->get_clients().size() == 1);

        boost::system::error_code ec;
        _client.close(boost::beast::websocket::close_code::normal, ec);

        std::this_thread::sleep_for(std::chrono::seconds(3));

        ASSERT_TRUE(_server->get_state()->get_clients().size() == 0);
    }
}

TEST_F(server_test, server_can_handle_subscribe) {
    boost::asio::io_context _ioc;
    boost::asio::ip::tcp::resolver _resolver{make_strand(_ioc)};
    boost::beast::websocket::stream<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>> _client{make_strand(_ioc), server_a_->get_state()->get_client_ssl_context()};

    auto const _results = _resolver.resolve("localhost", std::to_string(server_a_->get_config()->clients_port_.load(std::memory_order_acquire)));
    boost::asio::connect(boost::beast::get_lowest_layer(_client), _results);

    SSL_set_tlsext_host_name(_client.next_layer().native_handle(), "localhost");
    _client.next_layer().set_verify_callback(boost::asio::ssl::host_name_verification("localhost"));

    const auto _host = fmt::format("localhost:{}", std::to_string(server_a_->get_config()->clients_port_.load(std::memory_order_acquire)));
    _client.next_layer().handshake(boost::asio::ssl::stream_base::client);
    _client.handshake(_host, "/");

    boost::beast::flat_buffer _accepted_buffer;
    _client.read(_accepted_buffer);
    ASSERT_TRUE(server_a_->get_state()->get_clients().size() == 1);

    _client.write(boost::asio::buffer(std::string(serialize(boost::json::object{
        {"transaction_id", to_string(boost::uuids::random_generator()())},
        {"action", "subscribe"},
        {"params", {{"channel", "welcome"}}},
    }))));

    boost::beast::flat_buffer _buffer;
    _client.read(_buffer);


    auto _subscribe_message = boost::beast::buffers_to_string(_accepted_buffer.data());
    auto _subscribe_object = boost::json::parse(_subscribe_message);

    ASSERT_TRUE(_subscribe_object.is_object());
    ASSERT_TRUE(_subscribe_object.as_object().contains("action"));
    ASSERT_TRUE(_subscribe_object.as_object().at("action").is_string());
    ASSERT_EQ(_subscribe_object.as_object().at("action").as_string(), "welcome");
    ASSERT_TRUE(_subscribe_object.as_object().contains("transaction_id"));
    ASSERT_TRUE(_subscribe_object.as_object().at("transaction_id").is_string());
    ASSERT_TRUE(_subscribe_object.as_object().contains("status"));
    ASSERT_TRUE(_subscribe_object.as_object().at("status").is_string());
    ASSERT_EQ(_subscribe_object.as_object().at("status").as_string(), "success");
    ASSERT_TRUE(_subscribe_object.as_object().contains("data"));
    ASSERT_TRUE(_subscribe_object.as_object().at("data").is_object());

    boost::system::error_code ec;
    _client.close(boost::beast::websocket::close_code::normal, ec);
}

TEST_F(server_test, assert_server_can_handle_publish) {
    boost::asio::io_context _ioc;
    boost::asio::ip::tcp::resolver _resolver{make_strand(_ioc)};
    boost::beast::websocket::stream<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>> _client_a{make_strand(_ioc), server_a_->get_state()->get_client_ssl_context()};
    boost::beast::websocket::stream<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>> _client_b{make_strand(_ioc), server_a_->get_state()->get_client_ssl_context()};
    boost::beast::websocket::stream<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>> _client_c{make_strand(_ioc), server_a_->get_state()->get_client_ssl_context()};
    boost::beast::websocket::stream<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>> _client_d{make_strand(_ioc), server_a_->get_state()->get_client_ssl_context()};

    {
        auto const _results = _resolver.resolve("localhost", std::to_string(server_b_->get_config()->clients_port_.load(std::memory_order_acquire)));

        boost::asio::connect(boost::beast::get_lowest_layer(_client_a), _results);
        SSL_set_tlsext_host_name(_client_a.next_layer().native_handle(), "localhost");
        _client_a.next_layer().set_verify_callback(boost::asio::ssl::host_name_verification("localhost"));
        _client_a.next_layer().handshake(boost::asio::ssl::stream_base::client);

        boost::asio::connect(boost::beast::get_lowest_layer(_client_b), _results);
        SSL_set_tlsext_host_name(_client_b.next_layer().native_handle(), "localhost");
        _client_b.next_layer().set_verify_callback(boost::asio::ssl::host_name_verification("localhost"));
        _client_b.next_layer().handshake(boost::asio::ssl::stream_base::client);
    }

    {
        auto const _results = _resolver.resolve("localhost", std::to_string(server_c_->get_config()->clients_port_.load(std::memory_order_acquire)));

        boost::asio::connect(boost::beast::get_lowest_layer(_client_c), _results);
        SSL_set_tlsext_host_name(_client_c.next_layer().native_handle(), "localhost");
        _client_c.next_layer().set_verify_callback(boost::asio::ssl::host_name_verification("localhost"));
        _client_c.next_layer().handshake(boost::asio::ssl::stream_base::client);

        boost::asio::connect(boost::beast::get_lowest_layer(_client_d), _results);
        SSL_set_tlsext_host_name(_client_d.next_layer().native_handle(), "localhost");
        _client_d.next_layer().set_verify_callback(boost::asio::ssl::host_name_verification("localhost"));
        _client_d.next_layer().handshake(boost::asio::ssl::stream_base::client);
    }

    const auto _server_b_host = fmt::format("localhost:{}", std::to_string(server_b_->get_config()->clients_port_.load(std::memory_order_acquire)));
    _client_a.handshake(_server_b_host, "/");
    _client_b.handshake(_server_b_host, "/");

    const auto _server_c_host = fmt::format("localhost:{}", std::to_string(server_c_->get_config()->clients_port_.load(std::memory_order_acquire)));
    _client_c.handshake(_server_c_host, "/");
    _client_d.handshake(_server_c_host, "/");

    for (const auto _client : { &_client_a, &_client_b, &_client_c, &_client_d }) {
        boost::beast::flat_buffer _buffer;
        _client->read(_buffer);
        LOG_INFO("receiving client welcome ...");
    }

    _client_a.write(boost::asio::buffer(std::string(serialize(boost::json::object{
        {"transaction_id", to_string(boost::uuids::random_generator()())},
        {"action", "subscribe"},
        {"params", {{"channel", "welcome"}}},
    }))));

    {
        boost::beast::flat_buffer _buffer;
        _client_a.read(_buffer);
        LOG_INFO("client A should receive subscribe ACK ... {}", boost::beast::buffers_to_string(_buffer.data()));
        _buffer.clear();
    }

    _client_c.write(boost::asio::buffer(std::string(serialize(boost::json::object{
    {"transaction_id", to_string(boost::uuids::random_generator()())},
    {"action", "subscribe"},
    {"params", {{"channel", "welcome"}}},
    }))));

    {
        boost::beast::flat_buffer _buffer;
        _client_c.read(_buffer);
        LOG_INFO("client C should receive subscribe ACK ... {}", boost::beast::buffers_to_string(_buffer.data()));
        _buffer.clear();
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));

    _client_b.write(boost::asio::buffer(std::string(serialize(boost::json::object{
        {"transaction_id", to_string(boost::uuids::random_generator()())},
        {"action", "publish"},
        {"params", {{"channel", "welcome"}, {"payload", {{"message", "EHLO"}}}}},
    }))));

    {
        boost::beast::flat_buffer _buffer;
        _client_b.read(_buffer);
        LOG_INFO("client B should receive publish ACK ... {}", boost::beast::buffers_to_string(_buffer.data()));
        _buffer.clear();
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));

    {
        boost::beast::flat_buffer _buffer;
        _client_a.read(_buffer);
        LOG_INFO("client A should receive publish MESSAGE ... {}", boost::beast::buffers_to_string(_buffer.data()));

        auto _publish_message = boost::beast::buffers_to_string(_buffer.data());
        auto _publish_object = boost::json::parse(_publish_message);

        ASSERT_TRUE(_publish_object.is_object());
        ASSERT_TRUE(_publish_object.as_object().contains("action"));
        ASSERT_TRUE(_publish_object.as_object().at("action").is_string());
        ASSERT_EQ(_publish_object.as_object().at("action").as_string(), "publish");
        ASSERT_TRUE(_publish_object.as_object().contains("transaction_id"));
        ASSERT_TRUE(_publish_object.as_object().at("transaction_id").is_string());
        ASSERT_TRUE(_publish_object.as_object().contains("params"));
        ASSERT_TRUE(_publish_object.as_object().at("params").is_object());
        ASSERT_TRUE(_publish_object.as_object().at("params").as_object().contains("channel"));
        ASSERT_TRUE(_publish_object.as_object().at("params").as_object().at("channel").is_string());
        ASSERT_EQ(_publish_object.as_object().at("params").as_object().at("channel").as_string(), "welcome");
        ASSERT_TRUE(_publish_object.as_object().at("params").as_object().contains("payload"));
        ASSERT_TRUE(_publish_object.as_object().at("params").as_object().at("payload").is_object());
        ASSERT_TRUE(_publish_object.as_object().at("params").as_object().at("payload").as_object().contains("message"));
        ASSERT_TRUE(_publish_object.as_object().at("params").as_object().at("payload").as_object().at("message").is_string());
        ASSERT_EQ(_publish_object.as_object().at("params").as_object().at("payload").as_object().at("message").as_string(), "EHLO");
        _buffer.clear();
    }


    {
        boost::beast::flat_buffer _buffer;
        _client_c.read(_buffer);
        LOG_INFO("client C should receive publish MESSAGE ... {}", boost::beast::buffers_to_string(_buffer.data()));

        auto _publish_message = boost::beast::buffers_to_string(_buffer.data());
        auto _publish_object = boost::json::parse(_publish_message);

        ASSERT_TRUE(_publish_object.is_object());
        ASSERT_TRUE(_publish_object.as_object().contains("action"));
        ASSERT_TRUE(_publish_object.as_object().at("action").is_string());
        ASSERT_EQ(_publish_object.as_object().at("action").as_string(), "publish");
        ASSERT_TRUE(_publish_object.as_object().contains("transaction_id"));
        ASSERT_TRUE(_publish_object.as_object().at("transaction_id").is_string());
        ASSERT_TRUE(_publish_object.as_object().contains("params"));
        ASSERT_TRUE(_publish_object.as_object().at("params").is_object());
        ASSERT_TRUE(_publish_object.as_object().at("params").as_object().contains("channel"));
        ASSERT_TRUE(_publish_object.as_object().at("params").as_object().at("channel").is_string());
        ASSERT_EQ(_publish_object.as_object().at("params").as_object().at("channel").as_string(), "welcome");
        ASSERT_TRUE(_publish_object.as_object().at("params").as_object().contains("payload"));
        ASSERT_TRUE(_publish_object.as_object().at("params").as_object().at("payload").is_object());
        ASSERT_TRUE(_publish_object.as_object().at("params").as_object().at("payload").as_object().contains("message"));
        ASSERT_TRUE(_publish_object.as_object().at("params").as_object().at("payload").as_object().at("message").is_string());
        ASSERT_EQ(_publish_object.as_object().at("params").as_object().at("payload").as_object().at("message").as_string(), "EHLO");
    }

    boost::system::error_code ec;
    _client_a.close(boost::beast::websocket::close_code::normal, ec);
    _client_b.close(boost::beast::websocket::close_code::normal, ec);
    _client_c.close(boost::beast::websocket::close_code::normal, ec);
    _client_d.close(boost::beast::websocket::close_code::normal, ec);
}

TEST_F(server_test, assert_server_can_handle_broadcast) {
    boost::asio::io_context _ioc;
    boost::asio::ip::tcp::resolver _resolver{make_strand(_ioc)};
    boost::beast::websocket::stream<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>> _client_a{make_strand(_ioc), server_a_->get_state()->get_client_ssl_context()};
    boost::beast::websocket::stream<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>> _client_b{make_strand(_ioc), server_a_->get_state()->get_client_ssl_context()};
    boost::beast::websocket::stream<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>> _client_c{make_strand(_ioc), server_a_->get_state()->get_client_ssl_context()};
    boost::beast::websocket::stream<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>> _client_d{make_strand(_ioc), server_a_->get_state()->get_client_ssl_context()};

    {
        auto const _results = _resolver.resolve("localhost", std::to_string(server_b_->get_config()->clients_port_.load(std::memory_order_acquire)));
        boost::asio::connect(boost::beast::get_lowest_layer(_client_a), _results);
        SSL_set_tlsext_host_name(_client_a.next_layer().native_handle(), "localhost");
        _client_a.next_layer().set_verify_callback(boost::asio::ssl::host_name_verification("localhost"));
        _client_a.next_layer().handshake(boost::asio::ssl::stream_base::client);

        boost::asio::connect(boost::beast::get_lowest_layer(_client_b), _results);
        SSL_set_tlsext_host_name(_client_b.next_layer().native_handle(), "localhost");
        _client_b.next_layer().set_verify_callback(boost::asio::ssl::host_name_verification("localhost"));
        _client_b.next_layer().handshake(boost::asio::ssl::stream_base::client);

    }

    {
        auto const _results = _resolver.resolve("localhost", std::to_string(server_c_->get_config()->clients_port_.load(std::memory_order_acquire)));

        boost::asio::connect(boost::beast::get_lowest_layer(_client_c), _results);
        SSL_set_tlsext_host_name(_client_c.next_layer().native_handle(), "localhost");
        _client_c.next_layer().set_verify_callback(boost::asio::ssl::host_name_verification("localhost"));
        _client_c.next_layer().handshake(boost::asio::ssl::stream_base::client);

        boost::asio::connect(boost::beast::get_lowest_layer(_client_d), _results);
        SSL_set_tlsext_host_name(_client_d.next_layer().native_handle(), "localhost");
        _client_d.next_layer().set_verify_callback(boost::asio::ssl::host_name_verification("localhost"));
        _client_d.next_layer().handshake(boost::asio::ssl::stream_base::client);
    }

    const auto _server_b_host = fmt::format("localhost:{}", std::to_string(server_b_->get_config()->clients_port_.load(std::memory_order_acquire)));
    _client_a.handshake(_server_b_host, "/");
    _client_b.handshake(_server_b_host, "/");

    const auto _server_c_host = fmt::format("localhost:{}", std::to_string(server_c_->get_config()->clients_port_.load(std::memory_order_acquire)));
    _client_c.handshake(_server_c_host, "/");
    _client_d.handshake(_server_c_host, "/");

    for (const auto _client : { &_client_a, &_client_b, &_client_c, &_client_d }) {
        boost::beast::flat_buffer _buffer;
        _client->read(_buffer);
        LOG_INFO("receiving client welcome ...");
    }

    _client_a.write(boost::asio::buffer(std::string(serialize(boost::json::object{
        {"transaction_id", to_string(boost::uuids::random_generator()())},
        {"action", "broadcast"},
        {"params", {{"payload", {{"message", "EHLO"}}}}},
    }))));

    {
        boost::beast::flat_buffer _buffer;
        _client_a.read(_buffer);
        LOG_INFO("client A should receive broadcast ACK ... {}", boost::beast::buffers_to_string(_buffer.data()));
        _buffer.clear();
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));

    for (const auto _client : { &_client_b, &_client_c, &_client_d })
    {
        boost::beast::flat_buffer _buffer;
        _client->read(_buffer);
        LOG_INFO("client should receive broadcast MESSAGE ... {}", boost::beast::buffers_to_string(_buffer.data()));

        auto _publish_message = boost::beast::buffers_to_string(_buffer.data());
        auto _publish_object = boost::json::parse(_publish_message);

        ASSERT_TRUE(_publish_object.is_object());
        ASSERT_TRUE(_publish_object.as_object().contains("action"));
        ASSERT_TRUE(_publish_object.as_object().at("action").is_string());
        ASSERT_EQ(_publish_object.as_object().at("action").as_string(), "broadcast");
        ASSERT_TRUE(_publish_object.as_object().contains("transaction_id"));
        ASSERT_TRUE(_publish_object.as_object().at("transaction_id").is_string());
        ASSERT_TRUE(_publish_object.as_object().contains("params"));
        ASSERT_TRUE(_publish_object.as_object().at("params").is_object());
        ASSERT_TRUE(_publish_object.as_object().at("params").as_object().contains("payload"));
        ASSERT_TRUE(_publish_object.as_object().at("params").as_object().at("payload").is_object());
        ASSERT_TRUE(_publish_object.as_object().at("params").as_object().at("payload").as_object().contains("message"));
        ASSERT_TRUE(_publish_object.as_object().at("params").as_object().at("payload").as_object().at("message").is_string());
        ASSERT_EQ(_publish_object.as_object().at("params").as_object().at("payload").as_object().at("message").as_string(), "EHLO");
        _buffer.clear();
    }

    boost::system::error_code ec;
    _client_a.close(boost::beast::websocket::close_code::normal, ec);
    _client_b.close(boost::beast::websocket::close_code::normal, ec);
    _client_c.close(boost::beast::websocket::close_code::normal, ec);
    _client_d.close(boost::beast::websocket::close_code::normal, ec);
}

TEST_F(server_test, assert_server_can_handle_send) {
    boost::asio::io_context _ioc;
    boost::asio::ip::tcp::resolver _resolver{make_strand(_ioc)};
    boost::beast::websocket::stream<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>> _client_a{make_strand(_ioc), server_a_->get_state()->get_client_ssl_context()};
    boost::beast::websocket::stream<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>> _client_b{make_strand(_ioc), server_a_->get_state()->get_client_ssl_context()};

    {
        auto const _results = _resolver.resolve("localhost", std::to_string(server_b_->get_config()->clients_port_.load(std::memory_order_acquire)));
        boost::asio::connect(boost::beast::get_lowest_layer(_client_a), _results);
        SSL_set_tlsext_host_name(_client_a.next_layer().native_handle(), "localhost");
        _client_a.next_layer().set_verify_callback(boost::asio::ssl::host_name_verification("localhost"));
        _client_a.next_layer().handshake(boost::asio::ssl::stream_base::client);
    }

    {
        auto const _results = _resolver.resolve("localhost", std::to_string(server_c_->get_config()->clients_port_.load(std::memory_order_acquire)));
        boost::asio::connect(boost::beast::get_lowest_layer(_client_b), _results);
        SSL_set_tlsext_host_name(_client_b.next_layer().native_handle(), "localhost");
        _client_b.next_layer().set_verify_callback(boost::asio::ssl::host_name_verification("localhost"));
        _client_b.next_layer().handshake(boost::asio::ssl::stream_base::client);
    }

    const auto _server_b_host = fmt::format("localhost:{}", std::to_string(server_b_->get_config()->clients_port_.load(std::memory_order_acquire)));
    _client_a.handshake(_server_b_host, "/");

    const auto _server_c_host = fmt::format("localhost:{}", std::to_string(server_c_->get_config()->clients_port_.load(std::memory_order_acquire)));
    _client_b.handshake(_server_c_host, "/");

    std::this_thread::sleep_for(std::chrono::seconds(1));

    boost::beast::flat_buffer _client_a_accepted_buffer;
    _client_a.read(_client_a_accepted_buffer);
    auto _accepted_a_message = boost::beast::buffers_to_string(_client_a_accepted_buffer.data());
    auto _accepted_a_object = boost::json::parse(_accepted_a_message);

    auto _client_a_id = _accepted_a_object.as_object().at("data").at("client_id").as_string();

    boost::beast::flat_buffer _client_b_accepted_buffer;
    _client_b.read(_client_b_accepted_buffer);
    auto _accepted_b_message = boost::beast::buffers_to_string(_client_b_accepted_buffer.data());
    auto _accepted_b_object = boost::json::parse(_accepted_b_message);

    auto _client_b_id = _accepted_b_object.as_object().at("data").at("client_id").as_string();

    _client_a.write(boost::asio::buffer(std::string(serialize(boost::json::object{
        {"transaction_id", to_string(boost::uuids::random_generator()())},
        {"action", "send"},
        {"params", {
            {"to_client_id", _client_b_id},
            {"payload", {{"message", "EHLO"}}}}},
    }))));

    {
        boost::beast::flat_buffer _buffer;
        _client_a.read(_buffer);
        LOG_INFO("client A should receive send ACK ... {}", boost::beast::buffers_to_string(_buffer.data()));
        _buffer.clear();
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));

    boost::beast::flat_buffer _buffer;
    _client_b.read(_buffer);
    LOG_INFO("client should receive send MESSAGE ... {}", boost::beast::buffers_to_string(_buffer.data()));

    auto _send_message = boost::beast::buffers_to_string(_buffer.data());
    auto _send_object = boost::json::parse(_send_message);

    ASSERT_TRUE(_send_object.is_object());
    ASSERT_TRUE(_send_object.as_object().contains("action"));
    ASSERT_TRUE(_send_object.as_object().at("action").is_string());
    ASSERT_EQ(_send_object.as_object().at("action").as_string(), "send");
    ASSERT_TRUE(_send_object.as_object().contains("transaction_id"));
    ASSERT_TRUE(_send_object.as_object().at("transaction_id").is_string());
    ASSERT_TRUE(_send_object.as_object().contains("params"));
    ASSERT_TRUE(_send_object.as_object().at("params").is_object());
    ASSERT_TRUE(_send_object.as_object().at("params").as_object().contains("payload"));
    ASSERT_TRUE(_send_object.as_object().at("params").as_object().at("payload").is_object());
    ASSERT_TRUE(_send_object.as_object().at("params").as_object().at("payload").as_object().contains("message"));
    ASSERT_TRUE(_send_object.as_object().at("params").as_object().at("payload").as_object().at("message").is_string());
    ASSERT_EQ(_send_object.as_object().at("params").as_object().at("payload").as_object().at("message").as_string(), "EHLO");


    boost::system::error_code ec;
    _client_a.close(boost::beast::websocket::close_code::normal, ec);
    _client_b.close(boost::beast::websocket::close_code::normal, ec);
}

TEST_F(server_test, assert_server_can_handle_sync) {
    boost::asio::io_context _ioc;
    boost::asio::ip::tcp::resolver _resolver{make_strand(_ioc)};
    boost::beast::websocket::stream<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>> _client_a{make_strand(_ioc), server_a_->get_state()->get_client_ssl_context()};
    boost::beast::websocket::stream<boost::asio::ssl::stream<boost::asio::ip::tcp::socket>> _client_b{make_strand(_ioc), server_a_->get_state()->get_client_ssl_context()};

    {
        auto const _results = _resolver.resolve("localhost", std::to_string(server_b_->get_config()->clients_port_.load(std::memory_order_acquire)));
        boost::asio::connect(boost::beast::get_lowest_layer(_client_a), _results);
        SSL_set_tlsext_host_name(_client_a.next_layer().native_handle(), "localhost");
        _client_a.next_layer().set_verify_callback(boost::asio::ssl::host_name_verification("localhost"));
        _client_a.next_layer().handshake(boost::asio::ssl::stream_base::client);
    }

    {
        auto const _results = _resolver.resolve("localhost", std::to_string(server_c_->get_config()->clients_port_.load(std::memory_order_acquire)));
        boost::asio::connect(boost::beast::get_lowest_layer(_client_b), _results);
        SSL_set_tlsext_host_name(_client_b.next_layer().native_handle(), "localhost");
        _client_b.next_layer().set_verify_callback(boost::asio::ssl::host_name_verification("localhost"));
        _client_b.next_layer().handshake(boost::asio::ssl::stream_base::client);
    }

    const auto _server_b_host = fmt::format("localhost:{}", std::to_string(server_b_->get_config()->clients_port_.load(std::memory_order_acquire)));
    _client_a.handshake(_server_b_host, "/");

    const auto _server_c_host = fmt::format("localhost:{}", std::to_string(server_c_->get_config()->clients_port_.load(std::memory_order_acquire)));
    _client_b.handshake(_server_c_host, "/");

    _client_a.write(boost::asio::buffer(std::string(serialize(boost::json::object{
        {"transaction_id", to_string(boost::uuids::random_generator()())},
        {"action", "subscribe"},
        {"params", {{"channel", "welcome"}}},
    }))));

    {
        boost::beast::flat_buffer _buffer;
        _client_a.read(_buffer);
        LOG_INFO("client A should receive subscribe ACK ... {}", boost::beast::buffers_to_string(_buffer.data()));
        _buffer.clear();
    }

    auto _server_e = std::make_shared<engine::server>();

    auto _thread_e = std::make_unique<std::jthread>([_server_e, this]() {
            const auto &_config = _server_e->get_config();
            _config->sessions_port_.store(0, std::memory_order_release);
            _config->clients_port_.store(0, std::memory_order_release);
            _config->is_node_ = true;
            _config->threads_ = 4;
            _config->repl_enabled = false;

            _config->remote_clients_port_.store(
                server_a_->get_config()->clients_port_.load(std::memory_order_acquire), std::memory_order_release);
            _config->remote_sessions_port_.store(
                server_a_->get_config()->sessions_port_.load(std::memory_order_acquire),
                std::memory_order_release);

            LOG_INFO("starting server E");
            _server_e->start();
            LOG_INFO("server E stopped");
        });

    std::this_thread::sleep_for(std::chrono::seconds(5));

    ASSERT_EQ(_server_e->get_state()->get_subscriptions().size(), 1);
    ASSERT_EQ(_server_e->get_state()->get_clients().size(), 2);

    _server_e->stop();
}
