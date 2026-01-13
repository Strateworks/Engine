# Engine

Is a C++ network server core intended to serve as the foundational runtime for secure, distributed systems. Built on top of Boost.Asio and Boost.Beast, it provides an asynchronous, multithreaded architecture focused on connection handling, session orchestration, and role-based operation, rather than application-specific logic. The engine can operate as a standalone server or as a node connected to another server, enabling flexible deployment in distributed topologies. It explicitly separates client connections from inter-node sessions, treating them as distinct communication layers with independent lifecycles.

Security and structure are first-class concerns. Engine uses TLS/SSL end-to-end, with separate SSL contexts for incoming client connections, incoming session connections, and outgoing connections, allowing fine-grained control over certificates and trust policies. A central state manager tracks engine identity, timestamps, active sessions, connected clients, and subscriptions, acting as the coordination backbone of the runtime. Engine is not an end-user application, but a reusable backend infrastructure component designed to be extended as the core of secure network services or distributed backends.

## Get Started

### Download

Get the binary from the [releases](https://github.com/Strateworks/Engine/releases).

```shell
wget https://github.com/Strateworks/Engine/releases/download/v1.0.0/release.zip
unzip release.zip
rm release.zip
```

### Usage

Software can be launched using:

```shell
./server [...args]
```

#### Options

The available options are:

| Argument                                       | Details                                        | Default    |
|------------------------------------------------|------------------------------------------------|------------|
| --address=[value:string]                       | DNS record or IP address of this State.        | `0.0.0.0`  |
| --threads=[value:number]                       | No of CPU threads.                             | 4          |
| --is_mode=[value:boolean]                      | Run as node mode.                              | true       |
| --sessions_port=[value:integer]                | Port assigned to Sessions.                     | 11000      |
| --clients_port=[value:integer]                 | Port assigned to Clients.                      | 12000      |
| --remote_address=[value:string]                | DNS record or IP address of existing State.    | 12000      |
| --remote_sessions_port=[value:integer]         | Remote port assigned to Sessions.              | 9000       |
| --remote_clients_port=[value:integer]          | Remote port assigned to Clients.               | 10000      |
| --ssl_client_listener_password=[value:string]  | Password of client listener SSL certificate.   | `36e422f3` |
| --ssl_session_listener_password=[value:string] | Password of sessions listener SSL certificate. | `610191e8` |
| --ssl_client_password=[value:string]           | Password of client SSL certificate.            | `d96ab300` |
| --ssl_session_password=[value:string]          | Password of session SSL certificate.           | `5ec35a12` |

