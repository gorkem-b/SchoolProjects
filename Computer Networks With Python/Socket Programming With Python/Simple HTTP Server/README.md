# Simple HTTP Server

A minimal HTTP server built with Python sockets and the standard library.

This project demonstrates how a browser sends a raw HTTP request over a TCP connection and how a server parses that request, reads a file from disk, builds an HTTP response, and sends it back to the client.

## Features

- Built with **Python 3** only
- Uses the standard library `socket` module for TCP communication
- Serves static files from the `public/` directory
- Supports the `GET` method for basic file requests
- Returns:
  - `200 OK` for existing files
  - `404 Not Found` for missing files
- Redirects `/` to `/index.html`
- Includes basic path traversal protection for requests like `../`
- Detects content type with `mimetypes`

## How It Works

1. The server opens a TCP socket on `127.0.0.1:8080`.
2. A browser or client connects and sends a raw HTTP request.
3. The server reads the request bytes and decodes them into text.
4. The request line is parsed to extract the HTTP method, path, and version.
5. The requested path is mapped to a file inside `public/`.
6. If the file exists, the server builds a `200 OK` response.
7. If the file does not exist, the server returns the `404.html` page with a `404 Not Found` response.
8. The response is sent back to the client and the connection is closed.

## Project Structure

```text
simple_http_server/
├── config.py
├── server.py
├── README.md
├── GUIDE.md
├── core/
│   ├── parser.py
│   └── response.py
└── public/
    ├── index.html
    ├── about.html
    └── 404.html
```

## File Overview

- `config.py`  
  Stores `HOST`, `PORT`, and `DOCUMENT_ROOT`.

- `server.py`  
  Main entry point. Creates the socket, accepts client connections, reads requests, resolves file paths, and sends HTTP responses.

- `core/parser.py`  
  Parses the first line of the HTTP request and returns a dictionary with:
  - `method`
  - `path`
  - `version`

- `core/response.py`  
  Builds raw HTTP responses and guesses the correct `Content-Type` header.

- `public/`  
  Contains the static files exposed by the server.

## Requirements

- Python 3.x
- No external dependencies

## Configuration

Configuration is defined in `config.py`:

```python
HOST = "127.0.0.1"
PORT = 8080
DOCUMENT_ROOT = "./public"
```

If port `8080` is already in use on your machine, change `PORT` to another available port.

## Run the Server

From the project root:

```bash
python server.py
```

If the server starts successfully, open your browser and visit:

```text
http://127.0.0.1:8080/
```

## Example Requests

### Home page

```text
GET / HTTP/1.1
Host: 127.0.0.1:8080
```

Expected result: `200 OK` and `public/index.html`

### About page

```text
GET /about.html HTTP/1.1
Host: 127.0.0.1:8080
```

Expected result: `200 OK` and `public/about.html`

### Missing page

```text
GET /missing.html HTTP/1.1
Host: 127.0.0.1:8080
```

Expected result: `404 Not Found` and `public/404.html`

## Testing

You can test the server with a browser or `curl`:

```bash
curl -i http://127.0.0.1:8080/
curl -i http://127.0.0.1:8080/about.html
curl -i http://127.0.0.1:8080/missing.html
```

## Current Limitations

This project is intentionally simple and educational. It does **not** aim to be a production-ready HTTP server.

Current limitations include:

- Only `GET` requests are supported
- Unsupported or malformed requests are not handled with full HTTP compliance
- Single-process, sequential request handling
- No logging layer
- No multithreading or multiprocessing
- No HTTPS support

## Troubleshooting

### Port 8080 is already in use

If you get a socket bind error when starting the server, another process may already be using port `8080`.

Possible solutions:

1. Stop the conflicting process
2. Change the `PORT` value in `config.py`
3. Restart the server

### Page returns 404

Make sure the requested file exists inside the `public/` directory.

## Educational Goal

The purpose of this project is to show the relationship between:

- **HTTP** at the application layer
- **TCP sockets** at the transport layer
- **file system access** on the server side

It is a compact example of how a web server works internally without relying on web frameworks.