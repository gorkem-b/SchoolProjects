import socket
import threading

from config import HOST, PORT
from receiver import receive_messages


def main():
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    try:
        client_socket.connect((HOST, PORT))
    except OSError as exc:
        print(f"Sunucuya baglanilamadi: {exc}")
        client_socket.close()
        return

    print(f"{HOST}:{PORT} adresine baglanildi.")
    print("Cikmak icin /quit yazin.")

    stop_event = threading.Event()
    receiver_thread = threading.Thread(
        target=receive_messages,
        args=(client_socket, stop_event),
        daemon=True,
    )
    receiver_thread.start()

    try:
        while not stop_event.is_set():
            message = input("> ").strip()

            if not message:
                continue

            if message == "/quit":
                break

            client_socket.sendall(message.encode("utf-8"))
    except (EOFError, KeyboardInterrupt):
        print()
    except OSError:
        if not stop_event.is_set():
            print("Mesaj gonderilemedi.")
    finally:
        stop_event.set()

        try:
            client_socket.shutdown(socket.SHUT_RDWR)
        except OSError:
            pass

        client_socket.close()
        print("Baglanti kapatildi.")


if __name__ == "__main__":
    main()