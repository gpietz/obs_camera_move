import socket

HOST = '127.0.0.1'
PORT = 5680

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))

    # Nachricht senden
    message = "ECHO:Hello, World!\n"
    s.sendall(message.encode())

    # Antwort empfangen
    data = s.recv(1024)
    print('Received:', data.decode().strip())
