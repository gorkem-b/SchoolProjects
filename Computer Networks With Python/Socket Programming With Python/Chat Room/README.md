# Python TCP Chat Room

This project is a simple console-based chat room built with Python's standard library. A central TCP server accepts multiple client connections and forwards each incoming message to the other connected clients.

The implementation is intentionally minimal: no external dependencies, no GUI, and no unnecessary abstraction. The goal is to demonstrate basic socket programming, concurrency with threads, and the client-server model.

## Features

- TCP / IPv4 socket communication
- Centralized client-server architecture
- Multiple clients connected at the same time
- Thread-per-connection handling on the server
- Dedicated receiver thread on the client
- Broadcast messaging to all clients except the sender
- Graceful cleanup when a client disconnects
- No third-party packages required

## Tech Stack

- **Language:** Python 3.x
- **Networking:** `socket`
- **Concurrency:** `threading`
- **Dependencies:** None beyond the Python standard library

## How It Works

### Server

The server:

1. Creates a TCP socket.
2. Binds to a host and port.
3. Starts listening for incoming connections.
4. Accepts each client connection in a loop.
5. Creates a dedicated thread for every connected client.
6. Receives messages from that client.
7. Broadcasts each message to the other connected clients.
8. Removes and closes the socket when the client disconnects.

### Client

The client:

1. Creates a TCP socket.
2. Connects to the server.
3. Starts a background receiver thread.
4. Uses the main thread to read user input from the console.
5. Sends typed messages to the server.
6. Closes the connection when the user enters `/quit` or interrupts the program.

## Current Project Structure

```text
.
├── client/
│   ├── config.py
│   ├── main.py
│   └── receiver.py
├── server/
│   ├── client_handler.py
│   ├── config.py
│   └── main.py
├── GUIDE.md
└── README.md
```

### File Overview

- `server/main.py` - starts the server, listens for connections, and creates client handler threads
- `server/client_handler.py` - receives messages, broadcasts them, and removes disconnected clients
- `server/config.py` - stores server-side network settings
- `client/main.py` - connects to the server, reads console input, and sends messages
- `client/receiver.py` - continuously listens for messages from the server in a separate thread
- `client/config.py` - stores client-side network settings

## Configuration

Both server and client currently use the following default values:

- `HOST = "127.0.0.1"`
- `PORT = 5555`
- `BUFFER_SIZE = 1024`

If you want to run the server across a local network:

- Update `server/config.py` to bind to an accessible address such as `0.0.0.0`
- Update `client/config.py` so `HOST` matches the server machine's IP address

## How to Run

Run all commands from the project root.

### 1. Start the server

```bash
python server/main.py
```

### 2. Start one or more clients

Open one terminal per client and run:

```bash
python client/main.py
```

### 3. Chat

- Type a message and press Enter to send it.
- Type `/quit` to leave the chat.
- Messages are forwarded to the other connected clients.

## Example Local Test

1. Start the server in one terminal.
2. Open two or more additional terminals.
3. Start a client in each terminal.
4. Send a message from one client.
5. Confirm that the other clients receive it.
6. Close one client and confirm that the server continues running.

## Notes

- The current implementation is **single-room** only.
- There is **no username system**, authentication, persistence, or encryption.
- The sender does **not** receive its own message back; only the other clients do.
- Some runtime console messages in the current implementation are printed in Turkish.

## Why There Is No `requirements.txt`

This project only uses Python's standard library, so there are no external packages to install.

## Repository Readiness

This repository is ready to be pushed to GitHub with:

- English documentation
- A project structure that matches the current implementation
- Standard Python ignore rules in `.gitignore`