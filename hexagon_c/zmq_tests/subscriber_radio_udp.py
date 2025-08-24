import zmq

# C++ radio_udp.cpp uygulamasının BAĞLANDIĞI (connect) UDP multicast adresi
ENDPOINT_UDP = "udp://239.255.0.1:7779"
GROUP_UDP = "TRACK_DATA_UDP"

# C++ radio_epgm.cpp uygulamasının BAĞLANDIĞI (connect) EPGM multicast adresi
ENDPOINT_EPGM = "epgm://eth0;239.255.0.1:7780"
GROUP_EPGM = "TRACK_DATA_EPGM"

context = zmq.Context()

# UDP için DISH soketi
socket_udp = context.socket(zmq.DISH)
socket_udp.bind(ENDPOINT_UDP)
socket_udp.join(GROUP_UDP.encode('utf-8'))
print(f"DISH UDP Subscriber: Bound to {ENDPOINT_UDP} and joined group '{GROUP_UDP}'")

# EPGM için DISH soketi
socket_epgm = context.socket(zmq.DISH)
socket_epgm.bind(ENDPOINT_EPGM)
socket_epgm.join(GROUP_EPGM.encode('utf-8'))
print(f"DISH EPGM Subscriber: Bound to {ENDPOINT_EPGM} and joined group '{GROUP_EPGM}'")


poller = zmq.Poller()
poller.register(socket_udp, zmq.POLLIN)
poller.register(socket_epgm, zmq.POLLIN)

print("\nWaiting for messages from RADIO publishers...")

try:
    while True:
        events = dict(poller.poll(timeout=1000))
        
        if socket_udp in events:
            # DEĞİŞİKLİK BURADA: Artık recv() ile tek parça mesaj alıyoruz.
            payload_bytes = socket_udp.recv()
            message_text = payload_bytes.decode('utf-8')
            print(f"[RADIO_UDP] Received: {message_text}")

        if socket_epgm in events:
            # DEĞİŞİKLİK BURADA: Artık recv() ile tek parça mesaj alıyoruz.
            payload_bytes = socket_epgm.recv()
            message_text = payload_bytes.decode('utf-8')
            print(f"[RADIO_EPGM] Received: {message_text}")
            
except KeyboardInterrupt:
    print("\nSubscriber stopping.")

finally:
    socket_udp.close()
    socket_epgm.close()
    context.term()