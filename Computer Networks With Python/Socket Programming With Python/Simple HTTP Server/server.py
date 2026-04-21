import os
import socket
from urllib.parse import unquote

from config import DOCUMENT_ROOT, HOST, PORT
from core.parser import parse_request
from core.response import build_response, guess_content_type


def resolve_file_path(request_path):
    decoded_path = unquote(request_path.split("?", 1)[0])

    if decoded_path == "/":
        decoded_path = "/index.html"

    relative_path = decoded_path.lstrip("/\\")
    safe_path = os.path.normpath(relative_path)

    if safe_path.startswith("..") or os.path.isabs(safe_path):
        return None

    return os.path.join(DOCUMENT_ROOT, safe_path)


def read_file_body(file_path):
    with open(file_path, "rb") as file:
        return file.read()


def get_not_found_body():
    not_found_path = os.path.join(DOCUMENT_ROOT, "404.html")
    if os.path.isfile(not_found_path):
        return read_file_body(not_found_path)
    return b"<h1>404 Not Found</h1>"


def handle_request(request_text):
    request = parse_request(request_text)
    if not request or request["method"] != "GET":
        body = get_not_found_body()
        return build_response(404, body, "text/html; charset=utf-8")

    file_path = resolve_file_path(request["path"])
    if file_path and os.path.isfile(file_path):
        body = read_file_body(file_path)
        content_type = guess_content_type(file_path)
        return build_response(200, body, content_type)

    body = get_not_found_body()
    return build_response(404, body, "text/html; charset=utf-8")


def main():
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server_socket:
        server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        server_socket.bind((HOST, PORT))
        server_socket.listen(5)

        print(f"Server running on http://{HOST}:{PORT}")

        while True:
            client_socket, _ = server_socket.accept()
            with client_socket:
                request_data = client_socket.recv(4096)
                if not request_data:
                    continue

                request_text = request_data.decode("utf-8", errors="ignore")
                response = handle_request(request_text)
                client_socket.sendall(response)


if __name__ == "__main__":
    main()