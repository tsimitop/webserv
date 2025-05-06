import socket

# Define the POST request as a raw HTTP string
post_request = (
    "POST /uploads2 HTTP/1.1\r\n"
    "Host: localhost:4242\r\n"
    "Content-Type: application/x-www-form-urlencoded\r\n"
)

body = "adasasdasadssdasas"

i = 0

#while i < 12 * 1000 * 1000:
# while i < 1 * 1000 * 1000 - 1000:
# 	i = i + 1
# 	body += 'a'

post_request += "Content-Length: " + str(len(body)) + "\r\n"+ "\r\n" + body + "\r\n"

# Print the request
print("=== REQUEST ===")
print(post_request)

# Open a socket connection to localhost:4242
with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
    sock.connect(("localhost", 4242))
    sock.sendall(post_request.encode())

    # Receive the response
    response = b""
    while True:
        part = sock.recv(4096)
        if not part:
            break
        response += part

# Print the full response
print("\n=== RESPONSE ===")
print(response.decode(errors="replace"))
