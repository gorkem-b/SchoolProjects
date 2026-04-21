from datetime import datetime
from pathlib import Path
from socket import socket

from common.protocol import recv_message, unpack_header

from .config import CHUNK_SIZE, STORAGE_DIR


def build_target_path(filename: str) -> Path:
    STORAGE_DIR.mkdir(parents=True, exist_ok=True)

    safe_name = Path(filename).name.strip()
    if safe_name in {"", ".", ".."}:
        raise ValueError("Geçersiz dosya adı alındı.")

    target_path = STORAGE_DIR / safe_name
    if not target_path.exists():
        return target_path

    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S_%f")
    return STORAGE_DIR / f"{target_path.stem}_{timestamp}{target_path.suffix}"


def receive_file(connection: socket) -> tuple[Path, int]:
    header = unpack_header(recv_message(connection))
    filename = header["filename"]
    filesize = header["filesize"]

    if not isinstance(filename, str) or not isinstance(filesize, int):
        raise ValueError("Header alanları beklenen tipte değil.")

    target_path = build_target_path(filename)
    total_received = 0

    with target_path.open("wb") as output_file:
        while total_received < filesize:
            chunk = connection.recv(min(CHUNK_SIZE, filesize - total_received))
            if not chunk:
                raise ConnectionError("Bağlantı dosya aktarımı tamamlanmadan kapandı.")

            output_file.write(chunk)
            total_received += len(chunk)

    if target_path.stat().st_size != filesize:
        raise OSError("Kaydedilen dosya boyutu beklenen boyutla eşleşmiyor.")

    return target_path, filesize