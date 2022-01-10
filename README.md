# websocket-calculator

A simple calculator that uses JSON over a WebSocket. Currently only supports addition and subtraction.

This project was developed and tested on Ubuntu 16.04 LTS.

# Installation

## Server

The server has the following dependencies: `libwebsocketpp-dev` and `libasio-dev`. You can install these dependencies with the following commands: 

```
sudo apt-get install libwebsocketpp-dev
sudo apt-get install libasio-dev
```

To build the server, run the following commands from the project root directory:

```
cmake server/
make -C server/
```

## Client

The client uses Node.js, so Node and NPM must be installed before use. Requires at least Node version 13.0.

Before using the client, run the following commands from the project root directory:

```
cd client/
npm install
```

# Usage

## Server

To run the server from the project root directory:

```
./server/srv
```

The server will run until it is interrupted (e.g., with `Ctrl`+`C`).

## Client

To run the client from the project root directory:

```
node client/main.js <type> <a> <b>
```

`<type>` specifies the operation for the calculator to perform and must be either `"add"` or `"subtract"`. `<a>` and `<b>` specify the arguments to the operation, and must both be numbers.

The client will parse the response from the server and display the result of the computation.

### Examples:

```
$ node client/main.js add 2 3
sum: 5
```

```
$ node client/main.js subtract 9 2
difference: 7
```

# Issues & Discussion

## Exception handling in message callback

Generally speaking, servers are expected to be more resilient to exceptions than other types of programs, as it can be unnecessarily disruptive to let an error caused by one connection crash the server and prematurely end all other connections. It's common practice in industrial-grade servers (e.g. Spring MVC) to automatically catch all unhanlded exceptions thrown from user callbacks and translate them into error responses to the client.

In *websocket-calculator*'s `onMessage` callback, we make an effort to catch *expected* errors and translate them into `close` responses to the client, but we don't have a completely *general* strategy for avoiding server crashes. Points of contention include:

- In keeping with the websocketpp [example programs](https://github.com/zaphoyd/websocketpp/blob/master/examples/echo_server/echo_server.cpp#L29), we guard our call to `send` with a `try`/`catch` block and log failed sends. But we don't do anything similar with our calls to `close`. Should we? Can `close` even throw exceptions if it's called with a valid connection handle? The docs don't make it clear. It could even depend on which underlying async IO implementation is being used. 

- Should we guard the entire message callback with a `try`/`catch` block to guard against `std::exception`? The idea here would be to prevent the server from crashing just because one connection caused an exception that we did not expect. This goes back to the previous bullet point, where certain operations may or may not throw depending on nuances of the system in which they're used.

    - In this case, should we make an exception for `std::bad_alloc`? Recovering from allocation failure is notoriously sensitive and error prone, but may improve reliability. (Unlikely to make much of a difference on Linux where memory overcommit is the the default.)

    - Should we go even further and use `catch (...)` in our message callback? This is generally considered a Bad Idea&trade; (especially on Windows where, depending on your compiler settings, you can catch Structured Exceptions with this construct), but may improve reliability in Linux environments. (Again, unlikely to make much of a difference in practice because we don't invoke user-supplied code.)

These concerns may be trite for a proof-of-concept program as simple as this one, but would warrant careful consideration when building a larger system with industrial-grade reliability requirements. 

## Clean exit for the server

Currently, we don't provide any way to "gently" close down the server by signaling that it should finish its outstanding work, then shut down. There are two main advantages of shutting down the server properly instead of just terminating the process:

1) The server has the opportunity to cleanly close out any existing connections before stopping.

2) The server can signal to the OS that it is done using its address/port, avoiding "address is in use" errors when restarting the server.

Currently, we avoid "address is in use" errors by using `SO_REUSEADDR`.  This is fine for development/debugging but can be a serious security liability in production use cases.

According to the [websocketpp FAQ](https://docs.websocketpp.org/faq.html#autotoc_md92), the best way to cleanly shut down an asio server is to call `stop_listening` on the endpoint object, but it was discovered through trial and error that this method is not safe to call from another thread, nor from a signal handler. That leaves out the obvious solution of simply overloading `Ctrl`+`C` to call `stop_listening`.

Looking forward, there are two main candidate solutions to this problem:

1) We could extend the current messaging protocol to include a new message type, named `"shutdown"` or something similar. The server would react to a message of this type by calling `stop_listening` from the message callback. This is the approach used by the websocketpp [example programs](https://github.com/zaphoyd/websocketpp/blob/master/examples/echo_server/echo_server.cpp#L24). The main advantages of this approach are that it is simple, and that it allows for remote shutdown of the server. The primary disadvantage is that is creates a security concern, as any client with access to the server could trigger a shutdown, denying service to all other clients. If we were to implement this strategy, we would need to include some kind of validation that only authorized clients are allowed to shut down the server.

2) Instead of using `run`, we could use `run_one`/`poll`/`poll_one` to manually drive the I/O loop. Once we have control of the I/O loop, we can  use another thread with `sigwait` (or a signal handler) to signal to the main loop that it is ready to exit (through `std::atomic` or some similar thread-safe signaling mechanism). Then the main thread can call `stop_listening` from within the I/O loop. This has the advantage that we can overload the UNIX signals (`SIGINT`, `SIGTERM`, etc.) to cleanly close down the server instead of just terminating the process. This makes the server more robust. The main disadvantage of this approach is the additional complexity of managing our own I/O loop. The [websocketpp FAQ](https://docs.websocketpp.org/faq.html#autotoc_md92) says that servers manually driving their I/O loop may need to "adjust usage" to "correctly recognize the 'done with work' and 'not done but idiling / `io_service::work`' cases", but no further explanation or examples are given. This approach is attracive, but demands care in its implementation. 

Ultimately, for a production-ready server, we may want to implement both of these strategies, as each covers a use case that the other does not.
