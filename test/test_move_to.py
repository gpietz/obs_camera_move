import socket

HOST = '127.0.0.1'
PORT = 5680

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))

    # Nachricht senden
    message = 'set_camera_names("scn_facecam")'
    s.sendall(message.encode())

    # Antwort empfangen
    data = s.recv(1024)
    print('Received:', data.decode().strip())

    # Nachricht senden
    message = 'move_to(0,1095,840,10)'
    s.sendall(message.encode())

    # Antwort empfangen
    data = s.recv(1024)
    print('Received:', data.decode().strip())
