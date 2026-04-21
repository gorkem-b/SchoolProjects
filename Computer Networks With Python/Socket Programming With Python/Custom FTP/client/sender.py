from pathlib import Path
from socket import socket

from common.protocol import pack_header, send_message

from .config import CHUNK_SIZE


def send_file(connection: socket, file_path: Path) -> int:
    path = Path(file_path)

    if not path.is_file():
        raise ValueError(f"Dosya bulunamadı: {path}")

    filesize = path.stat().st_size
    header = pack_header(path.name, filesize)

    send_message(connection, header)

    with path.open("rb") as input_file:
        while True:
            chunk = input_file.read(CHUNK_SIZE)
            if not chunk:
                break

            connection.sendall(chunk)

    return filesize