// Copyright (c) 2025 Ian Torres <iantorres@outlook.com>.
// All rights reserved.

#include <fmt/base.h>

#include <engine/logger.hpp>
#include <engine/state.hpp>
#include <engine/server.hpp>

#include <engine/version.hpp>

#include <boost/version.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <boost/program_options/options_description.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

int main(const int argc, const char *argv[]) {
    boost::program_options::options_description _options("Options");
    auto _push_option = _options.add_options();

    _push_option("address", boost::program_options::value<std::string>()->default_value("0.0.0.0"));
    _push_option("threads", boost::program_options::value<unsigned short>()->default_value(4));
    _push_option("is_node", boost::program_options::value<bool>()->default_value(false));
    _push_option("sessions_port", boost::program_options::value<unsigned short>()->default_value(11000));
    _push_option("clients_port", boost::program_options::value<unsigned short>()->default_value(12000));
    _push_option("remote_address", boost::program_options::value<std::string>()->default_value("localhost"));
    _push_option("remote_sessions_port", boost::program_options::value<unsigned short>()->default_value(9000));
    _push_option("remote_clients_port", boost::program_options::value<unsigned short>()->default_value(10000));
    _push_option("ssl_client_listener_password", boost::program_options::value<std::string>()->default_value("36e422f3"));
    _push_option("ssl_session_listener_password", boost::program_options::value<std::string>()->default_value("610191e8"));
    _push_option("ssl_session_password", boost::program_options::value<std::string>()->default_value("5ec35a12"));
    _push_option("ssl_client_password", boost::program_options::value<std::string>()->default_value("d96ab300"));

    boost::program_options::variables_map _vm;
    store(parse_command_line(argc, argv, _options), _vm);

    const auto _server = std::make_shared<engine::server>();

    _server->configure(_vm);

    LOG_INFO("state version: {}.{}.{}", engine::version::get_major(), engine::version::get_minor(),
             engine::version::get_patch());
    LOG_INFO("boost version: {}", BOOST_VERSION);
    LOG_INFO("configuration:");
    LOG_INFO("- threads: {}", _vm["threads"].as<unsigned short>());
    LOG_INFO("- address: {}", _vm["address"].as<std::string>());
    LOG_INFO("- sessions_port: {}", _vm["sessions_port"].as<unsigned short>());
    LOG_INFO("- clients_port: {}", _vm["clients_port"].as<unsigned short>());
    LOG_INFO("- is_node: {}", _vm["is_node"].as<bool>());
    LOG_INFO("- remote_address: {}", _vm["remote_address"].as<std::string>());
    LOG_INFO("- remote_sessions_port: {}", _vm["remote_sessions_port"].as<unsigned short>());
    LOG_INFO("- remote_clients_port: {}", _vm["remote_clients_port"].as<unsigned short>());

    _server->start();

    return 0;
}
