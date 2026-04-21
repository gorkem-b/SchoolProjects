from config import BUFFER_SIZE


def receive_messages(client_socket, stop_event):
    while not stop_event.is_set():
        try:
            data = client_socket.recv(BUFFER_SIZE)

            if not data:
                print("\n[INFO] Server baglantisi kapandi. Cikmak icin Enter'a basin.")
                stop_event.set()
                break

            print(f"\n{data.decode('utf-8', errors='replace')}")
        except OSError:
            stop_event.set()
            break