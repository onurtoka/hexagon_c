import zmq

# DEĞİŞİKLİK: 'lo' yerine doğrudan loopback IP'sini kullanıyoruz.
ENDPOINT = "epgm://127.0.0.1;239.255.0.1:7777"

print(f"EPGM Subscriber (Simple Text): Connecting to {ENDPOINT}...")

context = zmq.Context()
socket = context.socket(zmq.SUB)
socket.connect(ENDPOINT)
socket.setsockopt_string(zmq.SUBSCRIBE, "")

print("EPGM Subscriber: Successfully connected and subscribed. Waiting for messages...")

try:
    while True:
        payload_bytes = socket.recv()
        message_text = payload_bytes.decode('utf-8')
        print(f"Received: {message_text}")

except KeyboardInterrupt:
    print("\nSubscriber stopping.")
finally:
    socket.close()
    context.term()