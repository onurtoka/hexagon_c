#include <iostream>
#include <string>
#include <chrono>
#include <thread>

// ZeroMQ Headers
#include "include/zmq/zmq.hpp"

using namespace std;

int main() {
    cout << "=== ZeroMQ SIMPLE RECEIVER TEST ===" << endl;
    
    try {
        cout << "1. Creating ZeroMQ context..." << endl;
        zmq::context_t context(1);
        cout << "   ✅ Context created successfully!" << endl;
        
        cout << "2. Creating PULL socket..." << endl;
        zmq::socket_t socket(context, zmq::socket_type::pull);
        cout << "   ✅ Socket created successfully!" << endl;
        
        cout << "3. Binding to tcp://*:7777..." << endl;
        socket.bind("tcp://*:7777");
        cout << "   ✅ Bound successfully!" << endl;
        
        cout << "4. Waiting for messages (will receive 5 messages)..." << endl;
        
        for (int i = 1; i <= 5; ++i) {
            zmq::message_t message;
            
            cout << "   🔄 [" << i << "] Waiting for message..." << endl;
            auto result = socket.recv(message, zmq::recv_flags::none);
            
            if (result) {
                string received_data = string(static_cast<char*>(message.data()), message.size());
                cout << "   ✅ [" << i << "] Received: " << received_data << endl;
            } else {
                cout << "   ❌ [" << i << "] Failed to receive" << endl;
            }
        }
        
        cout << "5. Test completed successfully!" << endl;
        cout << "🎉 ZeroMQ is working properly!" << endl;
        
    } catch (const exception& e) {
        cout << "❌ ZeroMQ Error: " << e.what() << endl;
        cout << "💡 Make sure libzmq.dll is in the same directory" << endl;
        return 1;
    }
    
    cout << "\nPress Enter to exit..." << endl;
    cin.get();
    return 0;
}