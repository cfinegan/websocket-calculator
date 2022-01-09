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
cd server/
make
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

To run the server, from the project root directory:

```
./server/srv
```

The server will run until it is interrupted (e.g., with `Ctrl`+`C`);

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