# webserver.py
from socket import *
from datetime import datetime

serverPort = 8044  # 8000 + your last 3 digit
serverSocket = socket(AF_INET, SOCK_STREAM)

# ____ bind the server socket to the port and start listening
serverSocket.bind(('', serverPort))
serverSocket.listen(1)

while True:
    print(f'Server ready on port {serverPort}...')
    # AYDIN ADNAN MENDERES UNIVERSITY | CSE204 Computer Networks
    # ____ accept the connection
    connectionSocket, addr = serverSocket.accept()

    try:
        request = connectionSocket.recv(1024).decode()

        # 1. Gerekli değişkenlerin (IP, Hostname, Zaman) çekilmesi
        server_host = gethostname()
        server_ip = gethostbyname(server_host)
        client_ip = addr[0]
        current_time = datetime.now().strftime('%Y-%m-%d %H:%M:%S')

        # 2. HTML sayfasının string olarak oluşturulması (Senin rengin: #E3F2FD)
        html_content = f"""<!DOCTYPE html>
        <html lang="en">
        <head>
            <meta charset="UTF-8">
            <title>ADU CSE204 Web Server Assignment</title>
            <style>
                html, body {{
                    margin: 0;
                    min-height: 100vh;
                    background-color: #E3F2FD;
                    font-family: Arial, sans-serif;
                }}

                body {{
                    padding: 24px;
                    box-sizing: border-box;
                }}
            </style>
        </head>
        <body>
            <h2>ADU CSE204 Web Server Assignment</h2>
            <p><b>Student:</b> Gorkem Bozkaya</p>
            <p><b>Number:</b> 251805044</p>
            <p><b>Server Host:</b> {server_host}</p>
            <p><b>Server IP:</b> {server_ip}</p>
            <p><b>Client IP:</b> {client_ip}</p>
            <p><b>Date/Time:</b> {current_time}</p>
        </body>
        </html>"""

        # 3. HTTP 200 OK başlığı ve HTML metninin birleştirilmesi
        # \r\n\r\n kısmı başlığın (header) bittiğini ve gövdenin (body) başladığını belirtir.
        http_response = "HTTP/1.1 200 OK\r\nContent-Type: text/html; charset=utf-8\r\n\r\n" + html_content

        # 4. Yanıtın encode edilerek (bayta dönüştürülerek) soket üzerinden gönderilmesi
        connectionSocket.send(http_response.encode())

    except IOError:
        # 5. Hata oluşursa HTTP 404 yanıtı gönderme
        error_response = "HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n<html><body><h1>404 Not Found</h1></body></html>"
        connectionSocket.send(error_response.encode())

    finally:
        connectionSocket.close()