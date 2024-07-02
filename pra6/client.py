import socket
import random
import time

# Configuration
BROADCAST_PORT = 10000
BROADCAST_ADDR = '<broadcast>' # Broadcast to all available addresses

def request_file(filename):
    """Requests a file from the P2P network using broadcast."""

    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as sock:
        sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)  # Enable broadcasting

        # Send the file request broadcast
        sock.sendto(filename.encode(), (BROADCAST_ADDR, BROADCAST_PORT))
        print(f"Broadcasting request for file '{filename}'...")

        # Receive responses from servers that have the file
        sock.settimeout(1)  # Set a timeout for receiving responses
        potential_servers = []
        while True:
            try:
                data, addr = sock.recvfrom(1024)
                potential_servers.extend(data.decode().split(','))
            except socket.timeout:
                break

        if potential_servers:
            # Choose a random server from the list
            selected_server = random.choice(potential_servers)
            print(f"File found on server: {selected_server}")

            # Connect to the selected server and download the file
            # ... (implementation for downloading the file)

        else:
            print("File not found in the network.")

if __name__ == "__main__":
    # Get the file name from command-line arguments
    import sys
    if len(sys.argv) < 2:
        print("Usage: python client.py <filename>")
        sys.exit(1)
    filename = sys.argv[1]

    request_file(filename)
