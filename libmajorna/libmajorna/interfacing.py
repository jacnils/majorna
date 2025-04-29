# libmajorna
# Python interface for majorna

import socket
from libmajorna import socket_path

def send_string(send_str):
    with socket.socket(socket.AF_UNIX, socket.SOCK_STREAM) as client_socket:
        client_socket.connect(socket_path)
        client_socket.sendall(send_str.encode('utf-8'))
        response = client_socket.recv(1024)

        if response:
            print(response.decode('utf-8'))
