# libmajorna
# Python interface for majorna

import socket
import time
from libmajorna import socket_path

class MajornaPipe:
    def __init__(self, socket_path, timeout=0.1, retry_interval=0.001):
        self.socket_path = socket_path
        self.timeout = timeout
        self.retry_interval = retry_interval
        self.client_socket = None
        self.connect()

    def connect(self):
        if self.client_socket:
            self.close()
        while True:
            try:
                sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
                sock.settimeout(self.timeout)
                sock.connect(self.socket_path)
                self.client_socket = sock
                return
            except (socket.error, FileNotFoundError):
                time.sleep(self.retry_interval)

    def send_string(self, send_str):
        try:
            if not self.client_socket:
                self.connect()
            self.client_socket.sendall(send_str.encode('utf-8'))
            response = self.client_socket.recv(1024)
            return response.decode('utf-8') if response else None
        except (socket.error, BrokenPipeError):
            # Retry connect once, then try sending again
            self.connect()
            self.client_socket.sendall(send_str.encode('utf-8'))
            response = self.client_socket.recv(1024)
            return response.decode('utf-8') if response else None

    def close(self):
        if self.client_socket:
            try:
                self.client_socket.close()
            finally:
                self.client_socket = None

    def __del__(self):
        self.close()

majorna_pipe = MajornaPipe(socket_path)