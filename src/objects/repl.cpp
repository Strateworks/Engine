// Copyright (c) 2025 — 2026 Ian Torres <iantorres@outlook.com>.
// All rights reserved.

#include <engine/repl.hpp>

#include <engine/session.hpp>
#include <engine/state.hpp>

#include <boost/asio/posix/stream_descriptor.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/asio/placeholders.hpp>

#include <fmt/format.h>

#include <boost/uuid/uuid_io.hpp>

#include <iostream>

namespace engine {
    repl::repl(const std::shared_ptr<state> &state)
        : input_(state->get_ioc(), ::dup(STDIN_FILENO)),
          output_(state->get_ioc(), ::dup(STDOUT_FILENO)),
          state_(state) {
        start();
    }

    void repl::start() {
        boost::asio::async_read_until(input_, boost::asio::dynamic_buffer(buffer_), '\n',
                                      boost::bind(&repl::read, this,
                                                  boost::asio::placeholders::error,
                                                  boost::asio::placeholders::bytes_transferred));
    }

    void repl::read(const boost::system::error_code &error, std::size_t bytes_transferred) {
        if (!error) {
            std::string _line(buffer_.substr(0, bytes_transferred - 1));
            buffer_.erase(0, bytes_transferred);


            if (_line == "dump") {
                const auto _sessions = state_->get_sessions();

                fmt::print("sessions {}\n", _sessions.size());
                fmt::print("============\n");

                for (auto & _session : _sessions) {
                    fmt::print("id #{}\n", to_string(_session->get_id()));
                    fmt::print("sessions_port={} clients_port={}\n\n", _session->get_sessions_port(), _session->get_clients_port());
                }
                fmt::print("============\n");

                const auto _clients = state_->get_clients();

                fmt::print("clients {}\n", _clients.size());
                fmt::print("============\n");

                for (auto & _client : _clients) {
                    fmt::print("id {} session_id={}\n\n", to_string(_client->get_id()), to_string(_client->get_session_id()));
                }
                fmt::print("============\n");

                const auto _subscriptions = state_->get_subscriptions();


                fmt::print("subscriptions {}\n", _subscriptions.size());
                fmt::print("============\n");
                for (auto & _subscription : _subscriptions) {
                    fmt::print("client_id={} session_id={} channel={}\n\n", to_string(_subscription.client_id_), to_string(_subscription.session_id_), _subscription.channel_);
                }
                fmt::print("============\n");
            }

            if (_line == "exit") {
                return;
            }
            std::string result = "EVALUATED: " + _line + "\n";

            async_write(output_, boost::asio::buffer(result),
                        boost::bind(&repl::write, this,
                                    boost::asio::placeholders::error));
        } else if (error == boost::asio::error::eof) {
            std::cout << "EOF received. Exiting REPL.\n";
        } else {
            std::cerr << "Error during read: " << error.message() << std::endl;
        }
    }

    void repl::write(const boost::system::error_code &error) {
        if (!error) {
            start();
        } else {
            std::cerr << "Error during write: " << error.message() << std::endl;
        }
    }
} // namespace engine
