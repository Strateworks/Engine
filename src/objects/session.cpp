// Copyright (c) 2025 Ian Torres <iantorres@outlook.com>.
// All rights reserved.

#include <engine/session.hpp>

#include <engine/state.hpp>
#include <engine/kernel.hpp>

#include <engine/logger.hpp>
#include <engine/response.hpp>
#include <boost/asio/ssl/host_name_verification.hpp>
#include <boost/asio/ssl/stream_base.hpp>
#include <boost/core/ignore_unused.hpp>

#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/json/parse.hpp>
#include <boost/json/serialize.hpp>

namespace engine {
    session::session(const std::shared_ptr<state> &state,
                     boost::asio::ip::tcp::socket &&socket, const session_context context, const boost::uuids::uuid id)
        : context_(context), state_(state),
          id_(id),
          socket_(std::move(socket),
                  context == remote ? state->get_session_ssl_context() : state->get_session_listener_ssl_context()) {
        LOG_INFO("state_id=[{}] action=[session_allocated] session_id=[{}]", to_string(state_->get_id()),
                 to_string(id_));
    }

    session::~session() {
        LOG_INFO("state_id=[{}] action=[session_released] session_id=[{}]", to_string(state_->get_id()),
                 to_string(id_));

        state_->remove_state_of_session(id_);
    }

    boost::uuids::uuid session::get_id() const { return id_; }

    boost::beast::websocket::stream<boost::asio::ssl::stream<boost::beast::tcp_stream> > &session::get_socket() {
        return socket_;
    }

    void session::send(std::shared_ptr<std::string const> const &data) {
        boost::ignore_unused(data);

        if (socket_.is_open()) {
            post(socket_.get_executor(), boost::beast::bind_front_handler(&session::on_send, shared_from_this(), data));
        }
    }

    void session::run() {
        dispatch(socket_.get_executor(),
                 boost::beast::bind_front_handler(&session::on_run, shared_from_this()));
    }

    unsigned short session::get_sessions_port() const {
        return sessions_port_;
    }

    void session::set_sessions_port(const unsigned short port) {
        sessions_port_ = port;
    }

    unsigned short session::get_clients_port() const {
        return clients_port_;
    }

    void session::set_clients_port(const unsigned short port) {
        clients_port_ = port;
    }

    std::string session::get_host() const {
        return host_;
    }

    void session::set_host(const std::string &host) {
        host_ = host;
    }

    void session::mark_as_registered() {
        registered_.store(true, std::memory_order_release);
    }

    bool session::get_registered() const {
        return registered_.load(std::memory_order_acquire);
    }

    void session::on_run() {
        switch (context_) {
            case local: {
                get_lowest_layer(socket_).expires_after(std::chrono::seconds(30));

                socket_.next_layer().async_handshake(
                    boost::asio::ssl::stream_base::server,
                    boost::beast::bind_front_handler(
                        &session::on_server_handshake,
                        shared_from_this()));
                break;
            }
            case remote: {
                const auto _host = fmt::format("{}:{}", state_->get_config()->remote_address_,
                                               std::to_string(
                                                   state_->get_config()->remote_sessions_port_.load(
                                                       std::memory_order_acquire)));

                socket_.next_layer().async_handshake(
                    boost::asio::ssl::stream_base::client,
                    boost::beast::bind_front_handler(
                        &session::on_client_handshake,
                        shared_from_this()));
                break;
            }
        }
    }

    void session::on_accept(const boost::beast::error_code &ec) {
        if (ec) {
            state_->remove_session(id_);
            return;
        }

        host_ = socket_.next_layer().lowest_layer().remote_endpoint().address().to_string();

        // Sí esta sesión es para conectarse a una instancia remota, entonces:
        if (context_ == remote) {
            // Apenas se conecta procede a registrarse
            auto const &_config = state_->get_config();
            const boost::json::object _response = {
                {"transaction_id", to_string(boost::uuids::random_generator()())},
                {"action", "register"},
                {
                    "params", {
                        {"registered", _config->registered_.load(std::memory_order_acquire)},
                        {"clients_port", _config->clients_port_.load(std::memory_order_acquire)},
                        {"sessions_port", _config->sessions_port_.load(std::memory_order_acquire)},
                    }
                },
            };
            // Para evitar que las siguientes conexiones remitan el listado de sesiones se marca una bandera
            _config->registered_.store(true, std::memory_order_release);

            send(std::make_shared<std::string const>(serialize(_response)));
        }

        do_read();
    }

    void session::on_client_handshake(const boost::beast::error_code &ec) {
        if (ec) {
            LOG_INFO(
                "session_id=[{}] on_client_handshake ec=[{}:{}] msg=[{}]",
                to_string(id_),
                ec.category().name(),
                ec.value(),
                ec.message()
            );
            state_->remove_session(id_);

            do_tls_shutdown();
            return;
        }

        const auto _host = fmt::format("{}:{}", state_->get_config()->remote_address_,
                                       std::to_string(
                                           state_->get_config()->remote_sessions_port_.load(
                                               std::memory_order_acquire)));

        socket_.async_handshake(
            _host,
            "/",
            boost::beast::bind_front_handler(&session::on_accept,
                                             shared_from_this())
        );
    }

    void session::on_server_handshake(const boost::beast::error_code &ec) {
        if (ec) {
            LOG_INFO(
                "session_id=[{}] on_server_handshake ec=[{}:{}] msg=[{}]",
                to_string(id_),
                ec.category().name(),
                ec.value(),
                ec.message()
            );
            state_->remove_session(id_);
            do_tls_shutdown();
            return;
        }

        get_lowest_layer(socket_).expires_never();

        socket_.set_option(
                    boost::beast::websocket::stream_base::timeout::suggested(boost::beast::role_type::server));

        socket_.async_accept(
            boost::beast::bind_front_handler(&session::on_accept, shared_from_this()));
    }

    void session::do_read() {
        socket_.async_read(buffer_, boost::beast::bind_front_handler(&session::on_read, shared_from_this()));
    }

    void session::on_read(const boost::system::error_code &ec, std::size_t bytes_transferred) {
        boost::ignore_unused(bytes_transferred);

        if (ec == boost::beast::websocket::error::closed) {
            LOG_INFO(
                "session_id=[{}] on_read ec=[{}:{}] msg=[{}]",
                to_string(id_),
                ec.category().name(),
                ec.value(),
                ec.message()
            );
            state_->remove_session(id_);
            do_tls_shutdown();
            return;
        }

        if (ec) {
            LOG_INFO(
                "session_id=[{}] on_read ec=[{}:{}] msg=[{}]",
                to_string(id_),
                ec.category().name(),
                ec.value(),
                ec.message()
            );
            state_->remove_session(id_);
            do_tls_shutdown();
            return;
        }

        const auto _read_at = std::chrono::system_clock::now().time_since_epoch().count();
        const auto _stream = boost::beast::buffers_to_string(buffer_.data());

        boost::system::error_code _parse_ec;

        if (auto _data = boost::json::parse(_stream, _parse_ec); !_parse_ec && _data.is_object()) {
            if (const auto _response = kernel(state_, _data.as_object(), on_session, get_id()); !_response->is_ack()) {
                send(std::make_shared<std::string const>(serialize(_response->get_data())));
            }
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

    void session::on_send(std::shared_ptr<std::string const> const &data) {
        queue_.push_back(data);

        if (queue_.size() > 1)
            return;

        const auto _message = *queue_.begin();

        socket_.async_write(boost::asio::buffer(*queue_.front()),
                            boost::beast::bind_front_handler(&session::on_write, shared_from_this()));
    }

    void session::on_write(const boost::beast::error_code &ec, std::size_t bytes_transferred) {
        if (ec)
            return;

        queue_.erase(queue_.begin());

        if (!queue_.empty())
            socket_.async_write(
                boost::asio::buffer(*queue_.front()),
                boost::beast::bind_front_handler(
                    &session::on_write,
                    shared_from_this()));
    }

    void session::do_tls_shutdown() {
        if (tls_shutdown_started_.exchange(true, std::memory_order_acq_rel))
            return;

        dispatch(
            socket_.get_executor(),
            [self = shared_from_this()] {
                self->on_tls_shutdown(boost::system::error_code{});
            }
        );
    }

    void session::on_tls_shutdown(const boost::system::error_code &ec) {
        socket_.next_layer().async_shutdown(
            boost::beast::bind_front_handler(
                &session::on_tls_shutdown_complete,
                shared_from_this()
            )
        );
    }

    void session::on_tls_shutdown_complete(const boost::system::error_code &ec) {
        boost::system::error_code ignored;
        get_lowest_layer(socket_).socket().shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored);
        get_lowest_layer(socket_).socket().close(ignored);
    }
} // namespace engine
