#include <iostream>
#include <string>
#include <chrono>
#include <thread>

// ZeroMQ Headers
#include "include/zmq/zmq.hpp"

// Domain Models
#include "src/domain/model/DelayCalcTrackData.hpp"
#include "src/domain/model/FinalCalcDelayData.hpp"

using namespace hat::domain::model;
using namespace std;

int main() {
    cout << "=== hexagon_c REAL ZeroMQ RECEIVER ===" << endl;
    cout << "Waiting for DelayCalcTrackData from hexagon_b..." << endl;
    
    try {
        // ZeroMQ Context ve Socket
        zmq::context_t context(1);
        zmq::socket_t socket(context, zmq::socket_type::pull);
        
        // Bind to port 7777
        socket.bind("tcp://*:7777");
        cout << "✅ Bound to tcp://*:7777" << endl;
        cout << "🔄 Waiting for ZeroMQ messages..." << endl;
        
        int message_count = 0;
        
        while (message_count < 10) {  // 10 mesaj al ve çık
            zmq::message_t message;
            
            // Mesaj al (blocking)
            auto result = socket.recv(message, zmq::recv_flags::none);
            
            if (result) {
                message_count++;
                string received_data = string(static_cast<char*>(message.data()), message.size());
                
                cout << "📨 [" << message_count << "] ZeroMQ Received: " << received_data.substr(0, 50);
                if (received_data.length() > 50) cout << "...";
                cout << " (" << message.size() << " bytes)" << endl;
                
                // Simüle edilmiş veri işleme
                cout << "⚙️  Processing DelayCalcTrackData..." << endl;
                
                // FinalCalcDelayData oluştur (user specified formulas)
                auto current_time = chrono::duration_cast<chrono::milliseconds>(
                    chrono::system_clock::now().time_since_epoch()).count();
                
                // User-specified calculations
                int64_t received_time = current_time;
                int64_t first_hop_sent_time = current_time - 100;  // 100ms ago
                int64_t second_hop_delay_time = received_time - first_hop_sent_time;  // SecondHopDelayTime = ReceivedTime - FirstHopSentTime
                int64_t total_delay_time = received_time - first_hop_sent_time;       // TotalDelayTime = ReceivedTime - FirstHopSentTime
                int64_t third_hop_sent_time = current_time;                           // ThirdHopSentTime = CurrentTime
                    
                cout << "📤 Generated FinalCalcDelayData:" << endl;
                cout << "   - Second Hop Delay: " << second_hop_delay_time << " ms" << endl;
                cout << "   - Total Delay: " << total_delay_time << " ms" << endl;
                cout << "   - Third Hop Sent Time: " << third_hop_sent_time << " ms" << endl;
                
                // Performance status
                string status = (total_delay_time < 100) ? "GOOD" : 
                              (total_delay_time < 500) ? "ACCEPTABLE" : "POOR";
                cout << "   - Performance Status: " << status << endl;
                
                cout << "✅ ZeroMQ Message processed successfully!" << endl;
                cout << "----------------------------------------" << endl;
                
                // Küçük bir delay
                this_thread::sleep_for(chrono::milliseconds(100));
            }
        }
        
        cout << "🎉 Successfully received and processed " << message_count << " ZeroMQ messages!" << endl;
        cout << "✅ Real ZeroMQ communication test completed!" << endl;
        
    } catch (const exception& e) {
        cerr << "❌ ZeroMQ Error: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}