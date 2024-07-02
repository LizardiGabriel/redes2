import socket
import threading
import os
import time
import pickle
import base64
from pybloom_live import BloomFilter

# Configuration
BROADCAST_PORT = 10000
BROADCAST_INTERVAL = 5  # Seconds between broadcasts
SERVER_PORT = 3000  # TCP port for file requests
MAX_UDP_PAYLOAD = 65507  # Maximum UDP payload size (theoretical limit is 65535, but leaving some room for headers)

# Bloom filter settings (adjust for your needs)
BLOOM_FILTER_CAPACITY = 10000
BLOOM_FILTER_ERROR_RATE = 0.01

def create_bloom_filter(directory):
    """Creates a Bloom filter for files in a directory."""
    bf = BloomFilter(capacity=BLOOM_FILTER_CAPACITY, error_rate=BLOOM_FILTER_ERROR_RATE)
    for filename in os.listdir(directory):
        bf.add(filename)
    return bf

def broadcast_files(sock, directory, file_index, bloom_filter):
    """Broadcasts file information and Bloom filter in chunks."""
    while True:
        files = os.listdir(directory)
        serialized_bf = pickle.dumps(bloom_filter)

        # Create a message containing file list and serialized Bloom filter
        message = f"{socket.gethostbyname(socket.gethostname())},{','.join(files)},{base64.b64encode(serialized_bf).decode()}"

        # Split the message into chunks
        msg_chunks = [message[i:i+MAX_UDP_PAYLOAD] for i in range(0, len(message), MAX_UDP_PAYLOAD)]
        num_chunks = len(msg_chunks)

        # Send each chunk with header information
        for i, chunk in enumerate(msg_chunks):
            header = f"{i+1},{num_chunks},"  # Chunk number and total chunks
            sock.sendto((header + chunk).encode(), ('<broadcast>', BROADCAST_PORT))
            time.sleep(0.1)

        time.sleep(BROADCAST_INTERVAL)

def receive_broadcasts(sock, file_index):
    """Receives file information broadcasts and reassembles the message."""
    msg_buffer = {}  # Buffer to store message chunks
    while True:
        data, addr = sock.recvfrom(1024)
        chunk_num, total_chunks, message_part = data.decode().split(',', 2)  # Split header from message
        chunk_num, total_chunks = int(chunk_num), int(total_chunks)
        msg_buffer.setdefault(addr, {})[chunk_num] = message_part

        # Check if all chunks have been received
        if len(msg_buffer[addr]) == total_chunks:
            # Reassemble the message
            full_message = ''.join([msg_buffer[addr][i] for i in range(1, total_chunks + 1)])
            ip, files, bf_base64 = full_message.split(',', 2)
            bf = pickle.loads(base64.b64decode(bf_base64))

            # Update the file index
            for file in files.split(','):
                file_index.setdefault(file, []).append((ip, bf))

            # Clear the buffer for this address
            del msg_buffer[addr]

def handle_client(conn, addr, file_index):
    """Handles client file requests."""
    filename = conn.recv(1024).decode()
    potential_servers = []
    for ip, bf in file_index.get(filename, []):
        if filename in bf:
            potential_servers.append(ip)
    if potential_servers:
        conn.sendall(','.join(potential_servers).encode())
    else:
        conn.sendall(b"File not found")
    conn.close()

def start_instance(directory):
    """Starts a server instance."""
    file_index = {}
    bloom_filter = create_bloom_filter(directory)

    # Create a UDP socket for broadcasting and receiving broadcasts
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)  # Enable broadcasting
    sock.bind(('', BROADCAST_PORT))  # Listen on all interfaces

    # Start threads for broadcasting and receiving
    threading.Thread(target=broadcast_files, args=(sock, directory, file_index, bloom_filter)).start()
    threading.Thread(target=receive_broadcasts, args=(sock, file_index)).start()

    # Create a TCP socket for handling client file requests
    server_sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_sock.bind(('0.0.0.0', SERVER_PORT))  # Listen on all interfaces
    server_sock.listen(5)

    while True:
        conn, addr = server_sock.accept()
        threading.Thread(target=handle_client, args=(conn, addr, file_index)).start()

if __name__ == "__main__":
    # Get the directory from command-line arguments
    import sys
    if len(sys.argv) < 2:
        print("Usage: python server.py <directory>")
        sys.exit(1)
    directory = sys.argv[1]

    start_instance(directory)
