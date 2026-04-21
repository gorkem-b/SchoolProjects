import json
import struct
from socket import socket


MESSAGE_LENGTH = struct.Struct(">I")


def recv_exact(connection: socket, size: int) -> bytes:
    data = bytearray()

    while len(data) < size:
        chunk = connection.recv(size - len(data))
        if not chunk:
            raise ConnectionError("Bağlantı beklenen veri alınmadan kapandı.")
        data.extend(chunk)

    return bytes(data)


def send_message(connection: socket, payload: bytes) -> None:
    connection.sendall(MESSAGE_LENGTH.pack(len(payload)))
    connection.sendall(payload)


def recv_message(connection: socket) -> bytes:
    raw_length = recv_exact(connection, MESSAGE_LENGTH.size)
    (message_length,) = MESSAGE_LENGTH.unpack(raw_length)
    return recv_exact(connection, message_length)


def pack_header(filename: str, filesize: int) -> bytes:
    header = {
        "filename": filename,
        "filesize": filesize,
    }
    return json.dumps(header, ensure_ascii=False).encode("utf-8")


def unpack_header(payload: bytes) -> dict[str, int | str]:
    header = json.loads(payload.decode("utf-8"))
    filename = header.get("filename")
    filesize = header.get("filesize")

    if not isinstance(filename, str) or not filename.strip():
        raise ValueError("Header içinde geçerli bir filename alanı yok.")

    if not isinstance(filesize, int) or filesize < 0:
        raise ValueError("Header içinde geçerli bir filesize alanı yok.")

    return {
        "filename": filename,
        "filesize": filesize,
    }