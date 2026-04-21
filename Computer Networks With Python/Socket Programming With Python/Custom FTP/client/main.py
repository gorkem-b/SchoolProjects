import argparse
import socket
from pathlib import Path

from .config import DATA_DIR, SERVER_HOST, SERVER_PORT
from .sender import send_file


def parse_args() -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Custom FTP istemcisi")
    parser.add_argument(
        "file_path",
        help="Gönderilecek dosya yolu. İsterseniz client/data altındaki dosya adını da verebilirsiniz.",
    )
    return parser.parse_args()


def resolve_file_path(file_path: str) -> Path:
    candidate = Path(file_path)
    if candidate.exists():
        return candidate

    fallback = DATA_DIR / file_path
    if fallback.exists():
        return fallback

    return candidate


def main() -> int:
    DATA_DIR.mkdir(parents=True, exist_ok=True)

    args = parse_args()
    file_path = resolve_file_path(args.file_path)

    try:
        with socket.create_connection((SERVER_HOST, SERVER_PORT)) as connection:
            filesize = send_file(connection, file_path)
    except (ConnectionError, OSError, ValueError) as exc:
        print(f"Gönderim başarısız: {exc}")
        return 1

    print(f"Gönderim tamamlandı: {file_path.name} ({filesize} bytes)")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())