from config import BUFFER_SIZE


def remove_client(client_socket, clients, clients_lock):
    with clients_lock:
        if client_socket in clients:
            clients.remove(client_socket)

    try:
        client_socket.close()
    except OSError:
        pass


def broadcast(message, sender_socket, clients, clients_lock):
    with clients_lock:
        targets = [client for client in clients if client is not sender_socket]

    failed_clients = []

    for client_socket in targets:
        try:
            client_socket.sendall(message)
        except OSError:
            failed_clients.append(client_socket)

    if not failed_clients:
        return

    with clients_lock:
        for client_socket in failed_clients:
            if client_socket in clients:
                clients.remove(client_socket)

            try:
                client_socket.close()
            except OSError:
                pass


def handle_client(client_socket, client_address, clients, clients_lock):
    try:
        while True:
            data = client_socket.recv(BUFFER_SIZE)

            if not data:
                break

            text = data.decode("utf-8", errors="replace").strip()

            if not text:
                continue

            message = f"{client_address[0]}:{client_address[1]} > {text}".encode("utf-8")
            broadcast(message, client_socket, clients, clients_lock)
    except (ConnectionResetError, OSError):
        pass
    finally:
        remove_client(client_socket, clients, clients_lock)
        leave_message = f"[SYSTEM] {client_address[0]}:{client_address[1]} ayrildi.".encode("utf-8")
        broadcast(leave_message, client_socket, clients, clients_lock)