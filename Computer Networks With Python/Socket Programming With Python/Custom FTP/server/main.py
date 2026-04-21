import socket

from .config import HOST, PORT, STORAGE_DIR
from .receiver import receive_file


def run_server() -> None:
    STORAGE_DIR.mkdir(parents=True, exist_ok=True)

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server_socket:
        server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        server_socket.bind((HOST, PORT))
        server_socket.listen()

        print(f"Sunucu dinlemede: {HOST}:{PORT}")

        while True:
            connection, address = server_socket.accept()

            with connection:
                print(f"Bağlantı kabul edildi: {address[0]}:{address[1]}")

                try:
                    saved_path, filesize = receive_file(connection)
                except (ConnectionError, OSError, ValueError) as exc:
                    print(f"Aktarım başarısız: {exc}")
                    continue

                print(f"Dosya kaydedildi: {saved_path} ({filesize} bytes)")


if __name__ == "__main__":
    try:
        run_server()
    except KeyboardInterrupt:
        print("\nSunucu durduruldu.")