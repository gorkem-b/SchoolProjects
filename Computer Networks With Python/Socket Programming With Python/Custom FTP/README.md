# Custom FTP

A minimal client-server file transfer application built with Python sockets.

This project is **not an implementation of the official FTP protocol**. It is a small educational file transfer system that uses **TCP** and a **custom application-level framing protocol** to upload files from a client to a server reliably.

## Features

- Upload a file from client to server over TCP
- Custom framing protocol for metadata delivery
- Fixed-size chunked transfer to avoid loading entire files into memory
- Automatic creation of runtime directories when needed
- Filename collision handling on the server with timestamp-based renaming
- Basic validation for incoming metadata and transferred file size

## Tech Stack

- **Language:** Python 3.x
- **Networking:** `socket`
- **File system / paths:** `pathlib`
- **Serialization:** `json`
- **Binary packing:** `struct`

## How It Works

TCP provides a continuous byte stream, so it does not preserve message boundaries by itself. To solve this, the application uses a simple framing protocol for the metadata header:

1. **4-byte prefix**: the length of the JSON header in big-endian format
2. **JSON header**: metadata such as filename and file size
3. **Raw payload**: the file bytes streamed in chunks

Example header:

```json
{"filename": "report.pdf", "filesize": 1048576}
```

### Transfer Flow

#### Client

1. Resolve the file path
2. Read file size and file name
3. Build a JSON header
4. Send the 4-byte header length
5. Send the JSON header
6. Stream the file in `4096` byte chunks with `sendall()`

#### Server

1. Accept a TCP connection
2. Read exactly 4 bytes for the header length
3. Read exactly that many bytes for the JSON header
4. Parse `filename` and `filesize`
5. Create the destination path in `server/storage/`
6. Receive the payload until the expected byte count is reached
7. Save the file and verify its final size

## Project Structure

```text
custom_ftp/
├── client/
│   ├── __init__.py
│   ├── config.py
│   ├── data/
│   ├── main.py
│   └── sender.py
├── common/
│   ├── __init__.py
│   └── protocol.py
├── server/
│   ├── __init__.py
│   ├── config.py
│   ├── main.py
│   ├── receiver.py
│   └── storage/
└── README.md
```

## Configuration

Default values are defined in the config files:

### `server/config.py`

- `HOST = "127.0.0.1"`
- `PORT = 5001`
- `CHUNK_SIZE = 4096`
- `STORAGE_DIR = server/storage`

### `client/config.py`

- `SERVER_HOST = "127.0.0.1"`
- `SERVER_PORT = 5001`
- `CHUNK_SIZE = 4096`
- `DATA_DIR = client/data`

You can change the host, port, or chunk size by editing these files.

## Requirements

- Python 3.10+ recommended
- No third-party dependencies

## How to Run

Run the commands from the project root.

### 1. Start the server

```bash
python -m server.main
```

### 2. Send a file from the client

You can pass either:

- a direct file path, or
- a file name located inside `client/data/`

Example:

```bash
python -m client.main client/data/example.txt
```

or

```bash
python -m client.main example.txt
```

The second form works if `example.txt` exists inside `client/data/`.

## Example Workflow

1. Put a test file inside `client/data/`
2. Start the server:

   ```bash
   python -m server.main
   ```

3. In another terminal, send the file:

   ```bash
   python -m client.main example.txt
   ```

4. Check the uploaded file inside `server/storage/`

## Error Handling

The current implementation includes basic protections for common issues:

- connection closes before the expected number of bytes arrives
- invalid or incomplete header data
- invalid filename values
- mismatched final file size after transfer
- filename collision handling with timestamp suffixes

## Current Limitations

This project intentionally stays minimal.

- Upload only: **client → server**
- No resume support
- No authentication or encryption
- No checksum/hash verification inside the protocol
- No multi-client concurrency model
- No official FTP compatibility

## Suggested Manual Tests

- Send a small text file
- Send a medium-sized binary file such as an image
- Verify the saved file size on the server
- Test duplicate filenames and confirm timestamp-based renaming
- Optionally compare hashes before and after transfer

## GitHub Readiness Notes

- Runtime-generated files should not be committed
- Python cache files should not be committed
- Placeholder files are kept so `client/data/` and `server/storage/` remain visible in the repository

## Future Improvements

- transfer progress reporting
- hash-based integrity verification
- resumable uploads
- download support
- multi-client handling
- optional authentication layer

## License

This project is provided for educational purposes.