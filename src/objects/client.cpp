// Copyright (c) 2025 Ian Torres <iantorres@outlook.com>.
// All rights reserved.

#include <engine/client.hpp>

#include <engine/logger.hpp>
#include <engine/state.hpp>
#include <engine/kernel.hpp>
#include <engine/response.hpp>
#include <boost/asio/ssl.hpp>
#include <boost/beast/websocket/ssl.hpp>

#include <boost/core/ignore_unused.hpp>

#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/json/parse.hpp>
#include <boost/json/serialize.hpp>

namespace engine {
    client::client(const boost::uuids::uuid session_id,
                   const std::shared_ptr<state> &state, const boost::uuids::uuid id) : state_(state),
        id_(id),
        session_id_(session_id),
        is_local_(state->get_id() == session_id) {
        LOG_INFO("state_id=[{}] action=[client_allocated] session_id=[{}] client_id=[{}]", to_string(state_->get_id()),
                 to_string(session_id), to_string(id_));
    }

    client::~client() {
        LOG_INFO("state_id=[{}] action=[client_released] session_id=[{}] client_id=[{}]", to_string(state_->get_id()),
                 to_string(get_session_id()), to_string(id_));
    }

    boost::uuids::uuid client::get_id() const { return id_; }

    boost::uuids::uuid client::get_session_id() const {
        return session_id_;
    }

    bool client::get_is_local() const {
        return is_local_;
    }

    void client::run() {
        if (socket_.has_value()) {
            auto &_socket = socket_.value();
            get_lowest_layer(_socket).expires_after(std::chrono::seconds(30));

            _socket.next_layer().async_handshake(
                    boost::asio::ssl::stream_base::server,
                    boost::beast::bind_front_handler(
                        &client::on_handshake,
                        shared_from_this()));
        }
    }

    void client::send(std::shared_ptr<std::string const> const &data) {
        boost::ignore_unused(data);

        if (socket_.has_value()) {
            if (auto &_socket = socket_.value(); _socket.is_open()) {
                post(_socket.next_layer().get_executor(),
                     boost::beast::bind_front_handler(&client::on_send, shared_from_this(), data));
            }
        }
    }

    void client::set_socket(boost::asio::ip::tcp::socket &&socket) {
        socket_.emplace(std::move(socket), state_->get_client_listener_ssl_context());
    }

    void client::on_accept(long run_at, const boost::beast::error_code &ec) {
        if (ec) {
            state_->remove_client(id_);
            const auto _ = state_->leave_to_sessions(get_id());
            boost::ignore_unused(_);
            return;
        }

        auto _now = std::chrono::system_clock::now().time_since_epoch().count();
        const boost::json::object _welcome = {
            {"transaction_id", to_string(boost::uuids::random_generator()())},
            {"action", "welcome"},
            {"status", "success"},
            {"message", "accepted"},
            {"timestamp", _now},
            {"runtime", _now - run_at},
            {"data", {{"client_id", to_string(get_id())}}},
        };
        send(std::make_shared<std::string const>(serialize(_welcome)));

        do_read();
    }

    void client::do_read() {
        auto &_socket = socket_.value();
        _socket.async_read(buffer_, boost::beast::bind_front_handler(&client::on_read, shared_from_this()));
    }

    void client::on_read(const boost::system::error_code &ec, std::size_t bytes_transferred) {
        boost::ignore_unused(bytes_transferred);

        if (ec == boost::beast::websocket::error::closed) {
            state_->remove_client(id_);
            const auto _ = state_->leave_to_sessions(get_id());
            boost::ignore_unused(_);
            return;
        }

        if (ec) {
            state_->remove_client(id_);
            const auto _ = state_->leave_to_sessions(get_id());
            boost::ignore_unused(_);
            return;
        }

        const auto _read_at = std::chrono::system_clock::now().time_since_epoch().count();
        auto _stream = boost::beast::buffers_to_string(buffer_.data());

        boost::system::error_code _parse_ec;

        if (auto _data = boost::json::parse(_stream, _parse_ec); !_parse_ec && _data.is_object()) {
            const auto _response = kernel(state_, _data.as_object(), on_client, get_id());
            send(std::make_shared<std::string const>(serialize(_response->get_data())));
        } else {
            auto _now = std::chrono::system_clock::now().time_since_epoch().count();
            const boost::json::object _response = {
                {"transaction_id", nullptr},
                {"status", "failed"},
                {"message", "unprocessable entity"},
                {
                    "data", {
                        {"body", "body must be json object"},
                    }
                },
                {"timestamp", _now},
                {"runtime", _now - _read_at},
            };
            send(std::make_shared<std::string const>(serialize(_response)));
        }

        buffer_.consume(buffer_.size());

        do_read();
    }

    void client::on_send(std::shared_ptr<std::string const> const &data) {
        queue_.push_back(data);

        if (queue_.size() > 1)
            return;

        const auto _message = *queue_.begin();

        LOG_INFO("state_id=[{}] action=[write] session_id=[{}] client_id=[{}] data=[{}]", to_string(state_->get_id()),
                         to_string(get_session_id()), to_string(id_), *_message);

        if (socket_.has_value()) {
            if (auto &_socket = socket_.value(); _socket.is_open()) {
                _socket.async_write(boost::asio::buffer(*queue_.front()),
                                    boost::beast::bind_front_handler(&client::on_write, shared_from_this()));
            }
        }
    }

    void client::on_write(const boost::beast::error_code &ec, std::size_t bytes_transferred) {
        if (ec)
            return;

        queue_.erase(queue_.begin());

        if (!queue_.empty() && socket_.has_value()) {
            if (auto &_socket = socket_.value(); _socket.is_open()) {
                _socket.async_write(
                    boost::asio::buffer(*queue_.front()),
                    boost::beast::bind_front_handler(
                        &client::on_write,
                        shared_from_this()));
            }
        }
    }

    void client::on_handshake(const boost::beast::error_code &ec) {
        if (ec) {
            state_->remove_client(id_);
            const auto _ = state_->leave_to_sessions(id_);
            boost::ignore_unused(_);
            return;
        }

        auto &_socket = socket_.value();
        get_lowest_layer(_socket).expires_never();

        _socket.set_option(
                    boost::beast::websocket::stream_base::timeout::suggested(boost::beast::role_type::server));

        auto _run_at = std::chrono::system_clock::now().time_since_epoch().count();
        _socket.async_accept(boost::beast::bind_front_handler(&client::on_accept, shared_from_this(), _run_at));
    }


    std::optional<boost::beast::websocket::stream<boost::asio::ssl::stream<boost::beast::tcp_stream>>> &client::get_socket() { return socket_; }
} // namespace engine
