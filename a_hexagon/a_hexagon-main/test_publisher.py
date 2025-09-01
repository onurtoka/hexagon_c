#!/usr/bin/env python3
"""
Test Publisher - TrackData gönderen test uygulaması
Bu script, ana uygulamanıza tcp://localhost:5555 üzerinden TrackData gönderir.
"""
import zmq
import time
import random

def main():
    # ZeroMQ context ve socket oluştur
    context = zmq.Context()
    socket = context.socket(zmq.PUSH)
    socket.connect("tcp://localhost:5555")
    
    print("Test Publisher başlatıldı. TrackData gönderiliyor...")
    print("Ana uygulamanın tcp://*:5555 üzerinde dinlediğinden emin olun.")
    print()
    
    # Test verileri gönder
    for i in range(5):
        # Örnek TrackData (CSV formatında)
        track_id = i + 1
        x_velocity = random.uniform(-100, 100)
        y_velocity = random.uniform(-100, 100)
        z_velocity = random.uniform(-50, 50)
        x_position = random.uniform(1000, 10000)
        y_position = random.uniform(1000, 10000)
        z_position = random.uniform(100, 1000)
        update_time = int(time.time() * 1000)  # milliseconds
        
        # CSV formatında veri oluştur
        csv_data = f"{track_id},{x_velocity:.2f},{y_velocity:.2f},{z_velocity:.2f},{x_position:.2f},{y_position:.2f},{z_position:.2f},{update_time}"
        
        print(f"Gönderiliyor #{i+1}: TrackID={track_id}, Pos=({x_position:.1f},{y_position:.1f},{z_position:.1f})")
        
        # Veriyi gönder
        socket.send_string(csv_data)
        
        # 2 saniye bekle
        time.sleep(2)
    
    print("\nTest tamamlandı. 5 adet TrackData gönderildi.")
    socket.close()
    context.term()

if __name__ == "__main__":
    main()
