#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <iomanip>

// ZeroMQ Headers
#include "include/zmq/zmq.hpp"

using namespace std;

int main() {
    cout << "=======================================" << endl;
    cout << "    hexagon_c - ZeroMQ DATA RECEIVER    " << endl;
    cout << "=======================================" << endl;
    cout << "Port: tcp://*:7777" << endl;
    cout << "Waiting for data from hexagon_b..." << endl;
    cout << "=======================================" << endl;
    
    try {
        // ZeroMQ Context ve Socket
        zmq::context_t context(1);
        zmq::socket_t socket(context, zmq::socket_type::pull);
        
        // Bind to port 7777
        socket.bind("tcp://*:7777");
        cout << "✅ RECEIVER READY - Listening on port 7777" << endl;
        cout << "---------------------------------------" << endl;
        
        int message_count = 0;
        auto start_time = chrono::steady_clock::now();
        
        while (message_count < 15) {  // 15 mesaj al
            zmq::message_t message;
            
            cout << "🔄 [" << (message_count + 1) << "] Waiting for message..." << flush;
            auto result = socket.recv(message, zmq::recv_flags::none);
            
            if (result) {
                message_count++;
                string received_data = string(static_cast<char*>(message.data()), message.size());
                
                // Zamanı al
                auto now = chrono::system_clock::now();
                auto time_t = chrono::system_clock::to_time_t(now);
                auto ms = chrono::duration_cast<chrono::milliseconds>(now.time_since_epoch()) % 1000;
                
                cout << "\r📨 [" << message_count << "] RECEIVED DATA:" << endl;
                cout << "   Time: " << put_time(localtime(&time_t), "%H:%M:%S") << "." << setfill('0') << setw(3) << ms.count() << endl;
                cout << "   Size: " << message.size() << " bytes" << endl;
                cout << "   Data: " << received_data.substr(0, 80);
                if (received_data.length() > 80) cout << "...";
                cout << endl;
                
                // Basit parsing
                if (received_data.find("DelayCalcTrackData") != string::npos) {
                    cout << "   Type: ✅ DelayCalcTrackData detected" << endl;
                } else {
                    cout << "   Type: ℹ️  Test message" << endl;
                }
                
                cout << "---------------------------------------" << endl;
                
            } else {
                cout << "\r❌ No message received" << endl;
            }
        }
        
        auto total_time = chrono::steady_clock::now() - start_time;
        auto seconds = chrono::duration_cast<chrono::seconds>(total_time).count();
        
        cout << "🎉 RECEPTION COMPLETED!" << endl;
        cout << "   Total Messages: " << message_count << endl;
        cout << "   Total Time: " << seconds << " seconds" << endl;
        if (seconds > 0) {
            cout << "   Average Rate: " << fixed << setprecision(2) 
                 << (double)message_count / seconds << " msg/s" << endl;
        }
        cout << "=======================================" << endl;
        
    } catch (const exception& e) {
        cout << "❌ ZeroMQ Error: " << e.what() << endl;
        return 1;
    }
    
    cout << "Press Enter to exit..." << endl;
    cin.get();
    return 0;
}