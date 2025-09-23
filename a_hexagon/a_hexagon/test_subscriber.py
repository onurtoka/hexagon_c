#!/usr/bin/env python3
"""
Test Subscriber - ExtrapTrackData alan test uygulaması
Bu script, ana uygulamanızdan tcp://localhost:5556 üzerinden ExtrapTrackData alır.
"""
import zmq
import time

def main():
    # ZeroMQ context ve socket oluştur
    context = zmq.Context()
    socket = context.socket(zmq.PULL)
    socket.bind("tcp://*:5556")
    
    print("Test Subscriber başlatıldı. ExtrapTrackData dinleniyor...")
    print("Ana uygulamanın tcp://localhost:5556 adresine veri göndermesini bekliyorum.")
    print("Çıkmak için Ctrl+C kullanın.")
    print()
    
    message_count = 0
    
    try:
        while True:
            # Mesaj al (her zaman blocking mode)
            message = socket.recv_string()  # zmq.NOBLOCK kaldırıldı
            message_count += 1
            
            # CSV formatını parse et
            parts = message.split(',')
            if len(parts) >= 8:
                track_id = parts[0]
                x_vel, y_vel, z_vel = parts[1], parts[2], parts[3]
                x_pos, y_pos, z_pos = parts[4], parts[5], parts[6]
                update_time = parts[7]
                
                print(f"Mesaj #{message_count}: TrackID={track_id}")
                print(f"  Velocity: ({x_vel}, {y_vel}, {z_vel})")
                print(f"  Position: ({x_pos}, {y_pos}, {z_pos})")
                print(f"  Time: {update_time}")
                print("-" * 50)
            else:
                print(f"Mesaj #{message_count}: {message}")
                
    except KeyboardInterrupt:
        print(f"\nTest sonlandırıldı. Toplam {message_count} mesaj alındı.")
    finally:
        socket.close()
        context.term()

if __name__ == "__main__":
    main()
