#include <iostream>
#include <chrono>
#include <vector>
#include <numeric>
#include "include/zmq/zmq.hpp"

using namespace std;

int main() {
    cout << "=== ZeroMQ LATENCY TEST RECEIVER ===" << endl;
    
    try {
        zmq::context_t context(1);
        zmq::socket_t socket(context, zmq::socket_type::pull);
        
        // PERFORMANS OPTİMİZASYONLARI
        int linger = 0;
        socket.set(zmq::sockopt::linger, linger);
        
        int hwm = 1000;
        socket.set(zmq::sockopt::rcvhwm, hwm);
        
        socket.bind("tcp://*:7777");
        cout << "Bound to tcp://*:7777" << endl;
        cout << "Waiting for latency test messages..." << endl;
        
        vector<double> latencies;
        
        for (int i = 1; i <= 100; ++i) {
            zmq::message_t message;
            auto result = socket.recv(message, zmq::recv_flags::none);
            
            if (result) {
                // Alım zamanı
                auto receive_time = chrono::high_resolution_clock::now();
                auto receive_time_ns = chrono::duration_cast<chrono::nanoseconds>(
                    receive_time.time_since_epoch()).count();
                
                // Mesajdan gönderim zamanını çıkar
                string received_data = string(static_cast<char*>(message.data()), message.size());
                
                if (received_data.substr(0, 5) == "PING:") {
                    long long sent_time_ns = stoll(received_data.substr(5));
                    
                    // Gecikme hesapla (nanosaniye)
                    long long latency_ns = receive_time_ns - sent_time_ns;
                    double latency_ms = latency_ns / 1000000.0; // ms'ye çevir
                    
                    latencies.push_back(latency_ms);
                    
                    cout << "[" << i << "] RECEIVED - Latency: " << fixed << setprecision(3) 
                         << latency_ms << " ms (" << latency_ns << " ns)" << endl;
                    
                    // Gecikme kategorisi
                    if (latency_ms < 1.0) cout << "    🟢 EXCELLENT (< 1ms)";
                    else if (latency_ms < 5.0) cout << "    🟢 VERY GOOD (< 5ms)";
                    else if (latency_ms < 10.0) cout << "    🟡 GOOD (< 10ms)";
                    else if (latency_ms < 50.0) cout << "    🟠 ACCEPTABLE (< 50ms)";
                    else cout << "    🔴 POOR (≥ 50ms)";
                    cout << endl;
                }
            }
        }
        
        // İstatistikler
        if (!latencies.empty()) {
            double avg_latency = accumulate(latencies.begin(), latencies.end(), 0.0) / latencies.size();
            double min_latency = *min_element(latencies.begin(), latencies.end());
            double max_latency = *max_element(latencies.begin(), latencies.end());
            
            cout << "\n=== LATENCY STATISTICS ===" << endl;
            cout << "Messages: " << latencies.size() << endl;
            cout << "Average Latency: " << fixed << setprecision(3) << avg_latency << " ms" << endl;
            cout << "Min Latency: " << fixed << setprecision(3) << min_latency << " ms" << endl;
            cout << "Max Latency: " << fixed << setprecision(3) << max_latency << " ms" << endl;
            
            // Performans değerlendirmesi
            cout << "\nPERFORMANCE ASSESSMENT:" << endl;
            if (avg_latency < 1.0) cout << "🟢 EXCELLENT - Sub-millisecond latency!" << endl;
            else if (avg_latency < 5.0) cout << "🟢 VERY GOOD - Low latency achieved" << endl;
            else if (avg_latency < 10.0) cout << "🟡 GOOD - Target achieved" << endl;
            else if (avg_latency < 50.0) cout << "🟠 NEEDS OPTIMIZATION - Above target" << endl;
            else cout << "🔴 CRITICAL - High latency detected!" << endl;
        }
        
        cout << "Latency test completed!" << endl;
        
    } catch (const exception& e) {
        cout << "Error: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}