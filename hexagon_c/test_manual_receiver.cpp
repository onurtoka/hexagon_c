#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <fstream>

// ZeroMQ Headers
#include "include/zmq/zmq.hpp"

using namespace std;

int main() {
    // Log file oluştur
    ofstream log_file("zmq_receiver_log.txt", ios::app);
    
    cout << "=== MANUAL ZeroMQ RECEIVER ===" << endl;
    log_file << "=== MANUAL ZeroMQ RECEIVER STARTED ===" << endl;
    
    try {
        // ZeroMQ Context ve Socket
        zmq::context_t context(1);
        zmq::socket_t socket(context, zmq::socket_type::pull);
        
        // Bind to port 7777
        socket.bind("tcp://*:7777");
        cout << "Bound to tcp://*:7777" << endl;
        log_file << "Bound to tcp://*:7777" << endl;
        
        cout << "Waiting for messages..." << endl;
        log_file << "Waiting for messages..." << endl;
        
        int message_count = 0;
        
        while (message_count < 20) {  // 20 mesaj al
            zmq::message_t message;
            
            // Mesaj al (blocking, 5 saniye timeout)
            auto result = socket.recv(message, zmq::recv_flags::none);
            
            if (result) {
                message_count++;
                string received_data = string(static_cast<char*>(message.data()), message.size());
                
                cout << "[" << message_count << "] Received: " << received_data.substr(0, 50) << "..." << endl;
                log_file << "[" << message_count << "] Received: " << received_data << endl;
                log_file.flush();
                
                // Her mesajda timestamp
                auto now = chrono::system_clock::now();
                auto time_t = chrono::system_clock::to_time_t(now);
                cout << "Time: " << ctime(&time_t);
                
            } else {
                cout << "No message received" << endl;
                log_file << "No message received" << endl;
            }
            
            this_thread::sleep_for(chrono::milliseconds(100));
        }
        
        cout << "Finished receiving " << message_count << " messages" << endl;
        log_file << "Finished receiving " << message_count << " messages" << endl;
        
    } catch (const exception& e) {
        cout << "Error: " << e.what() << endl;
        log_file << "Error: " << e.what() << endl;
    }
    
    log_file.close();
    cout << "Check zmq_receiver_log.txt for details" << endl;
    
    // Pause
    cout << "Press Enter to exit..." << endl;
    cin.get();
    
    return 0;
}