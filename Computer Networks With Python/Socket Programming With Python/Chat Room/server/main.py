import socket
import threading

from client_handler import handle_client
from config import HOST, PORT


def main():
    clients = []
    clients_lock = threading.Lock()
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server_socket.bind((HOST, PORT))
    server_socket.listen()

    print(f"Server dinleniyor: {HOST}:{PORT}")

    try:
        while True:
            client_socket, client_address = server_socket.accept()

            with clients_lock:
                clients.append(client_socket)

            print(f"Yeni baglanti: {client_address[0]}:{client_address[1]}")

            client_thread = threading.Thread(
                target=handle_client,
                args=(client_socket, client_address, clients, clients_lock),
                daemon=True,
            )
            client_thread.start()
    except KeyboardInterrupt:
        print("\nServer kapatiliyor...")
    finally:
        with clients_lock:
            active_clients = list(clients)
            clients.clear()

        for client_socket in active_clients:
            try:
                client_socket.close()
            except OSError:
                pass

        server_socket.close()


if __name__ == "__main__":
    main()