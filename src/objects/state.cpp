// Copyright (c) 2025 Ian Torres <iantorres@outlook.com>.
// All rights reserved.


#include <engine/session.hpp>
#include <engine/state.hpp>
#include <engine/logger.hpp>
#include <engine/subscription.hpp>
#include <engine/request.hpp>

#include <boost/uuid/random_generator.hpp>
#include <boost/json/serialize.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <ranges>
#include <unordered_set>

#include <engine/utils.hpp>

namespace engine {
    state::state(const std::shared_ptr<config> &config)
        : session_listener_ssl_context_(boost::asio::ssl::context::sslv23), session_ssl_context_(boost::asio::ssl::context::sslv23), client_listener_ssl_context_(boost::asio::ssl::context::sslv23), client_ssl_context_(boost::asio::ssl::context::sslv23), config_(config), id_(boost::uuids::random_generator()()), created_at_(std::chrono::system_clock::now()) {
        LOG_INFO("state_id=[{}] action=[state_allocated]", to_string(id_));

        session_listener_ssl_context_.set_options(
           boost::asio::ssl::context::no_sslv2 | boost::asio::ssl::context::default_workarounds |
           boost::asio::ssl::context::single_dh_use);

        session_listener_ssl_context_.set_verify_mode(boost::asio::ssl::verify_peer | boost::asio::ssl::verify_fail_if_no_peer_cert);

        session_listener_ssl_context_.load_verify_file(config_->ssl_ca_certificate_);

        session_listener_ssl_context_.set_password_callback([&](auto, auto) { return config_->ssl_session_listener_password_; });
        session_listener_ssl_context_.use_certificate_chain_file(config_->ssl_session_listener_chain_certificate_);
        session_listener_ssl_context_.use_private_key_file(config_->ssl_session_listener_private_key_, boost::asio::ssl::context::pem);
        session_listener_ssl_context_.use_tmp_dh_file(config_->ssl_session_dh_params_);

        session_ssl_context_.set_options(
           boost::asio::ssl::context::no_sslv2 | boost::asio::ssl::context::default_workarounds |
           boost::asio::ssl::context::single_dh_use);

        session_ssl_context_.set_verify_mode(boost::asio::ssl::verify_peer);

        session_ssl_context_.load_verify_file(config_->ssl_ca_certificate_);

        session_ssl_context_.set_password_callback([&](auto, auto) { return config_->ssl_session_password_; });

        session_ssl_context_.use_certificate_chain_file(config_->ssl_session_chain_certificate_);
        session_ssl_context_.use_private_key_file(config_->ssl_session_private_key_, boost::asio::ssl::context::pem);


        client_listener_ssl_context_.set_options(
           boost::asio::ssl::context::no_sslv2 | boost::asio::ssl::context::default_workarounds |
           boost::asio::ssl::context::single_dh_use);

        client_listener_ssl_context_.set_verify_mode(boost::asio::ssl::verify_peer | boost::asio::ssl::verify_fail_if_no_peer_cert);

        client_listener_ssl_context_.load_verify_file(config_->ssl_ca_certificate_);

        client_listener_ssl_context_.set_password_callback([&](auto, auto) { return config_->ssl_client_listener_password_; });
        client_listener_ssl_context_.use_certificate_chain_file(config_->ssl_client_listener_chain_certificate_);
        client_listener_ssl_context_.use_private_key_file(config_->ssl_client_listener_private_key_, boost::asio::ssl::context::pem);
        client_listener_ssl_context_.use_tmp_dh_file(config_->ssl_client_dh_params_);

        client_ssl_context_.set_options(
           boost::asio::ssl::context::no_sslv2 | boost::asio::ssl::context::default_workarounds |
           boost::asio::ssl::context::single_dh_use);

        client_ssl_context_.set_verify_mode(boost::asio::ssl::verify_peer);

        client_ssl_context_.load_verify_file(config_->ssl_ca_certificate_);

        client_ssl_context_.set_password_callback([&](auto, auto) { return config_->ssl_client_password_; });

        client_ssl_context_.use_certificate_chain_file(config_->ssl_client_chain_certificate_);
        client_ssl_context_.use_private_key_file(config_->ssl_client_private_key_, boost::asio::ssl::context::pem);
    }

    state::~state() {
        LOG_INFO("state_id=[{}] action=[state_released]", to_string(id_));
    }

    boost::uuids::uuid state::get_id() const { return id_; }

    std::chrono::system_clock::time_point state::get_created_at() const {
        return created_at_;
    }

    std::vector<std::shared_ptr<session> > state::get_sessions() const {
        std::shared_lock _lock(sessions_mutex_);
        std::vector<std::shared_ptr<session> > _result;
        _result.reserve(sessions_.size());

        for (auto &_session: sessions_ | std::views::values)
            _result.push_back(_session);

        return _result;
    }

    std::vector<std::shared_ptr<client> > state::get_clients() const {
        std::shared_lock _lock(clients_mutex_);

        const auto &_index = clients_.get<clients_by_client>();

        std::vector<std::shared_ptr<client> > _result;
        _result.reserve(clients_.size());

        for (const auto &_client: _index)
            _result.push_back(_client);

        return _result;
    }

    std::vector<subscription> state::get_subscriptions() const {
        std::shared_lock _lock(subscriptions_mutex_);

        const auto &_index = subscriptions_.get<subscriptions_by_session>();

        std::vector<subscription> _result;
        _result.reserve(subscriptions_.size());

        for (const auto &_subscription: _index)
            _result.push_back(_subscription);

        return _result;
    }

    std::optional<std::shared_ptr<session> > state::get_session(
        const boost::uuids::uuid id) const {
        std::shared_lock _lock(sessions_mutex_);

        const auto _iterator = sessions_.find(id);
        if (_iterator == sessions_.end()) {
            return std::nullopt;
        }
        return _iterator->second;
    }

    std::optional<std::shared_ptr<client> > state::get_client(
        const boost::uuids::uuid id) const {
        std::shared_lock _lock(clients_mutex_);

        auto &_index = clients_.get<clients_by_client>();

        const auto _iterator = _index.find(id);

        if (_iterator == _index.end()) {
            return std::nullopt;
        }
        return *_iterator;
    }

    bool state::add_session(std::shared_ptr<session> session) {
        std::unique_lock _lock(sessions_mutex_);
        auto [_, _inserted] = sessions_.emplace(session->get_id(), session);
        return _inserted;
    }

    bool state::remove_session(const boost::uuids::uuid id) {
        std::unique_lock _lock(sessions_mutex_);
        return sessions_.erase(id) > 0;
    }

    bool state::add_client(const std::shared_ptr<client> &client) {
        std::unique_lock _lock(clients_mutex_);

        auto &_index = clients_.get<clients_by_client_session>();
        auto [_it, _inserted] =
                _index.insert(client);

        return _inserted;
    }

    bool state::remove_client(const boost::uuids::uuid client_id) {
        std::unique_lock _lock(clients_mutex_);

        auto &_index = clients_.get<clients_by_client>();
        auto [_begin, _end] = _index.equal_range(client_id);

        const std::size_t _count = std::distance(_begin, _end);
        _index.erase(_begin, _end);
        return _count;
    }

    bool state::subscribe(const boost::uuids::uuid &session_id, const boost::uuids::uuid &client_id,
                          const std::string &channel) {
        std::unique_lock _lock(subscriptions_mutex_);

        auto &_index =
                subscriptions_.get<subscriptions_by_session_client_channel>();

        auto [_it, _inserted] =
                _index.insert(subscription{session_id, client_id, channel});

        return _inserted;
    }

    bool state::unsubscribe(const boost::uuids::uuid &session_id, const boost::uuids::uuid &client_id,
                            const std::string &channel) {
        std::unique_lock _lock(subscriptions_mutex_);

        auto &_index =
                subscriptions_.get<subscriptions_by_session_client_channel>();

        const auto _iterator = _index.find(
            boost::make_tuple(session_id, client_id, channel)
        );

        if (_iterator == _index.end())
            return false;

        _index.erase(_iterator);
        return true;
    }

    bool state::is_subscribed(const boost::uuids::uuid &client_id,
                              const std::string &channel) {
        std::shared_lock _lock(subscriptions_mutex_);


        const auto &_idx = subscriptions_.get<subscriptions_by_client_channel>();

        return _idx.find(std::make_tuple(client_id, channel)) != _idx.end();
    }

    std::size_t state::broadcast_to_sessions(const request &request,
                                             const boost::uuids::uuid client_id,
                                             const boost::json::object &data) const {
        const auto _data = make_broadcast_request_object(request, client_id, data);

        return send_to_sessions(_data);
    }

    std::size_t state::broadcast_to_clients(const request &request, const boost::uuids::uuid session_id,
                                            const boost::uuids::uuid client_id, const boost::json::object &data) const {
        const auto _data = std::make_shared<boost::json::object>(
            make_broadcast_request_object(request, client_id, data)
        );

        return send_to_others_clients(_data, session_id, client_id);
    }

    std::size_t state::send_to_subscribed_sessions(const boost::json::object &data, const std::string &channel) const {
        std::unordered_set<boost::uuids::uuid> _receivers; {
            std::shared_lock _lock(subscriptions_mutex_);
            const auto &_idx = subscriptions_.get<subscriptions_by_channel>();
            for (auto [_it, _end] = _idx.equal_range(channel); _it != _end; ++_it) {
                if (const auto _subscription = *_it; !_receivers.contains(_subscription.session_id_)) {
                    _receivers.insert(_subscription.session_id_);
                }
            }
        }

        if (_receivers.empty()) {
            return 0;
        }

        auto _sessions = get_sessions();
        auto const _message = std::make_shared<std::string const>(serialize(data));

        for (const auto &_session: _sessions) {
            if (_receivers.contains(_session->get_id()))
                _session->send(_message);
        }

        return _receivers.size();
    }

    std::size_t state::publish_to_sessions(const request &request,
                                           const boost::uuids::uuid client_id, const std::string &channel,
                                           const boost::json::object &data) const {
        const auto _data = make_publish_request_object(request, client_id, channel, data);

        return send_to_subscribed_sessions(_data, channel);
    }

    std::size_t state::publish_to_clients(const request &request, const boost::uuids::uuid session_id,
                                          const boost::uuids::uuid client_id, const std::string &channel,
                                          const boost::json::object &data) const {
        const auto _data = std::make_shared<boost::json::object>(
            make_publish_request_object(request, client_id, channel, data)
        );

        return send_to_others_clients(_data, session_id, client_id);
    }

    std::size_t state::join_to_sessions(const boost::uuids::uuid client_id) const {
        const auto _data = make_join_request_object(client_id);

        return send_to_sessions(_data);
    }

    std::size_t state::leave_to_sessions(const boost::uuids::uuid client_id) const {
        const auto _data = make_leave_request_object(client_id);

        return send_to_sessions(_data);
    }

    std::size_t state::subscribe_to_sessions(const request &request,
                                             const boost::uuids::uuid client_id, const std::string &channel) const {
        const auto _data = make_subscribe_request_object(request, client_id, channel);

        return send_to_sessions(_data);
    }

    bool state::push_client(const std::shared_ptr<client> &client) {
        std::unique_lock _lock(clients_mutex_);

        auto &_index = clients_.get<clients_by_client_session>();
        auto [_it, _inserted] =
                _index.insert(client);

        return _inserted;
    }

    void state::sync(const std::shared_ptr<session> &session, const bool registered) {
        if (!registered) {
            for (const auto &_session: get_sessions()) {
                // Si el identificador de la sesión en iteración es igual al identificador del estado entonces
                // implicaría que no debería ser considerada para ser reportada a la sesión porque ya está conectada
                // a esta instancia.
                const auto _is_state = _session->get_id() == get_id();

                // Sí el identificador de la sesión en iteración es igual al identificador de la sesión
                // implicaría que no debería ser considerada para ser reportada a la sesión porque es ella misma.
                const auto _is_same = _session->get_id() == session->get_id();

                // Sí la sesión en iteración no está registrada entonces está en proceso a ser registrada.
                const auto _is_unregistered = !_session->get_registered();

                if (_is_state || _is_same || _is_unregistered)
                    continue;

                boost::json::object _data = {
                    {"action", "session"},
                    {"transaction_id", to_string(boost::uuids::random_generator()())},
                    {
                        "params", {
                            {"host", _session->get_host()},
                            {"sessions_port", _session->get_sessions_port()},
                            {"clients_port", _session->get_clients_port()},
                        }
                    }
                };

                auto const _message = std::make_shared<std::string const>(serialize(_data));
                session->send(_message);
            }
        } {
            std::shared_lock _lock(clients_mutex_);

            const auto &_index = clients_.get<clients_by_session>();

            for (auto [_it, _end] = _index.equal_range(get_id()); _it != _end; ++_it) {
                const auto _client = *_it;

                boost::json::object _data = {
                    {"action", "join"},
                    {"transaction_id", to_string(boost::uuids::random_generator()())},
                    {
                        "params", {
                            {"client_id", to_string(_client->get_id())},
                        }
                    }
                };

                auto const _message = std::make_shared<std::string const>(serialize(_data));
                session->send(_message);
            }
        } {
            std::shared_lock _lock(subscriptions_mutex_);

            const auto &_index = subscriptions_.get<subscriptions_by_session>();

            for (auto [_it, _end] = _index.equal_range(get_id()); _it != _end; ++_it) {
                auto _subscription = *_it;

                boost::json::object _data = {
                    {"action", "subscribe"},
                    {"transaction_id", to_string(boost::uuids::random_generator()())},
                    {
                        "params", {
                            {"client_id", to_string(_subscription.client_id_)},
                            {"channel", _subscription.channel_}
                        }
                    }
                };

                auto const _message = std::make_shared<std::string const>(serialize(_data));
                session->send(_message);
            }
        }
    }

    boost::asio::io_context &state::get_ioc() {
        return ioc_;
    }

    std::size_t state::unsubscribe_to_sessions(const request &request, const boost::uuids::uuid client_id,
                                               const std::string &channel) const {
        const auto _data = make_unsubscribe_request_object(request, client_id, channel);

        return send_to_sessions(_data);
    }

    void state::remove_state_of_session(const boost::uuids::uuid id) { {
            std::unique_lock _lock(clients_mutex_);

            auto &_index = clients_.get<clients_by_session>();
            auto [_begin, _end] = _index.equal_range(id);

            _index.erase(_begin, _end);
        } {
            std::unique_lock _lock(subscriptions_mutex_);

            auto &_index = subscriptions_.get<subscriptions_by_session>();
            auto [_begin, _end] = _index.equal_range(id);

            _index.erase(_begin, _end);
        }
    }

    void state::send_to_session(const boost::uuids::uuid session_id, const boost::uuids::uuid from_client_id,
                                const boost::uuids::uuid to_client_id, const boost::json::object &payload) const {
        auto _sessions = get_sessions();

        const boost::json::object _data = {
            {"transaction_id", to_string(boost::uuids::random_generator()())},
            {"action", "send"},
            {
                "params", {
                    {"from_client_id", to_string(from_client_id)},
                    {"to_client_id", to_string(to_client_id)},
                    {"payload", payload},
                }
            }
        };

        auto const _message = std::make_shared<std::string const>(serialize(_data));

        for (const auto &_session: _sessions) {
            if (_session->get_id() == session_id) {
                _session->send(_message);
            }
        }
    }

    std::shared_ptr<config> state::get_config() {
        return config_;
    }

    boost::asio::ssl::context & state::get_session_listener_ssl_context() {
        return session_listener_ssl_context_;
    }

    boost::asio::ssl::context & state::get_session_ssl_context() {
        return session_ssl_context_;
    }

    boost::asio::ssl::context & state::get_client_listener_ssl_context() {
        return client_listener_ssl_context_;
    }

    boost::asio::ssl::context & state::get_client_ssl_context() {
        return client_ssl_context_;
    }

    std::size_t state::send_to_sessions(const boost::json::object &data) const {
        auto _sessions = get_sessions();

        auto const _message = std::make_shared<std::string const>(serialize(data));

        for (const auto &_session: _sessions) {
            _session->send(_message);
        }

        return _sessions.size();
    }

    std::size_t state::send_to_others_clients(const std::shared_ptr<boost::json::object> &data,
                                              const boost::uuids::uuid session_id,
                                              const boost::uuids::uuid client_id) const {
        // Obtenemos todos los clientes
        auto _clients = get_clients();

        // Construimos el mensaje compartido
        const auto _data = std::make_shared<std::string const>(serialize(*data));

        std::size_t _count = 0;

        // Por cada cliente en clientes
        for (const auto &_client: _clients) {
            // Con excepción del cliente emisor y los clientes que no sean de la actual sesión
            if (_client->get_id() == client_id || _client->get_session_id() != session_id)
                continue;

            // Se envía la transmisión
            _client->send(_data);
            _count++;
        }

        // Se retorna la cantidad de clientes con excepción.
        return _count;
    }
} // namespace engine
