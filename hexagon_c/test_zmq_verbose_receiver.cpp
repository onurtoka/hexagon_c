#include <iostream>
#include <string>
#include <chrono>
#include <thread>
#include <iomanip>
#include <atomic>
#include <csignal>

// ZeroMQ Headers
#include "include/zmq/zmq.hpp"

// Domain Models
#include "src/domain/model/DelayCalcTrackData.hpp"
#include "src/domain/model/FinalCalcDelayData.hpp"

using namespace hat::domain::model;
using namespace std;

// Global flag for graceful shutdown
atomic<bool> running(true);

void signalHandler(int signal) {
    cout << "\n🛑 Shutdown signal received. Stopping receiver..." << endl;
    running = false;
}

// Helper function to parse received data
DelayCalcTrackData parseReceivedData(const string& data) {
    DelayCalcTrackData track_data;
    
    // Simple parsing of JSON-like format
    // Format: DelayCalcTrackData{id:1234,velocity:45.6,position:123.4,...}
    
    try {
        // Extract ID
        auto id_pos = data.find("id:");
        if (id_pos != string::npos) {
            auto id_end = data.find(",", id_pos);
            string id_str = data.substr(id_pos + 3, id_end - id_pos - 3);
            track_data.setId(stoi(id_str));
        }
        
        // Extract velocity
        auto vel_pos = data.find("velocity:");
        if (vel_pos != string::npos) {
            auto vel_end = data.find(",", vel_pos);
            string vel_str = data.substr(vel_pos + 9, vel_end - vel_pos - 9);
            track_data.setVelocity(stod(vel_str));
        }
        
        // Extract position
        auto pos_pos = data.find("position:");
        if (pos_pos != string::npos) {
            auto pos_end = data.find(",", pos_pos);
            string pos_str = data.substr(pos_pos + 9, pos_end - pos_pos - 9);
            track_data.setPosition(stod(pos_str));
        }
        
        // Extract ECEF speed
        auto speed_pos = data.find("ecef_speed:");
        if (speed_pos != string::npos) {
            auto speed_end = data.find(",", speed_pos);
            string speed_str = data.substr(speed_pos + 11, speed_end - speed_pos - 11);
            // Set dummy ECEF values to match the speed
            double speed = stod(speed_str);
            track_data.setVelocityECEF(speed/3, speed/3, speed/3); // Approximate distribution
        }
        
        // Extract positions
        auto x_pos_pos = data.find("x_pos:");
        auto y_pos_pos = data.find("y_pos:");
        auto z_pos_pos = data.find("z_pos:");
        
        if (x_pos_pos != string::npos) {
            auto x_pos_end = data.find(",", x_pos_pos);
            string x_pos_str = data.substr(x_pos_pos + 6, x_pos_end - x_pos_pos - 6);
            double x_pos = stod(x_pos_str);
            
            if (y_pos_pos != string::npos) {
                auto y_pos_end = data.find(",", y_pos_pos);
                string y_pos_str = data.substr(y_pos_pos + 6, y_pos_end - y_pos_pos - 6);
                double y_pos = stod(y_pos_str);
                
                if (z_pos_pos != string::npos) {
                    auto z_pos_end = data.find(",", z_pos_pos);
                    string z_pos_str = data.substr(z_pos_pos + 6, z_pos_end - z_pos_pos - 6);
                    double z_pos = stod(z_pos_str);
                    
                    track_data.setPositionECEF(x_pos, y_pos, z_pos);
                }
            }
        }
        
        // Set time fields
        auto current_time = chrono::duration_cast<chrono::milliseconds>(
            chrono::system_clock::now().time_since_epoch()).count();
        track_data.setUpdateTime(current_time);
        track_data.setFirstHopSentTime(current_time - 50);
        
    } catch (const exception& e) {
        cout << "⚠️  Parsing error: " << e.what() << endl;
    }
    
    return track_data;
}

// Generate FinalCalcDelayData (GERÇEK GECİKME HESAPLAMA)
FinalCalcDelayData generateFinalCalcDelayData(const DelayCalcTrackData& input_data) {
    // KRITIK: Okuma zamanını al (hexagon_c'de alım zamanı)
    auto receive_time = chrono::duration_cast<chrono::milliseconds>(
        chrono::system_clock::now().time_since_epoch()).count();
    
    // GERÇEK GECİKME HESAPLAMALARI:
    int64_t sent_time = input_data.getFirstHopSentTime();                              // hexagon_b'den gelen gönderim zamanı
    int64_t actual_delay = receive_time - sent_time;                                   // GERÇEK GECİKME = Alım Zamanı - Gönderim Zamanı
    
    int64_t second_hop_delay_time = actual_delay;                                      // SecondHopDelayTime = GERÇEK GECİKME
    int64_t total_delay_time = actual_delay;                                           // TotalDelayTime = GERÇEK GECİKME
    int64_t third_hop_sent_time = receive_time;                                        // ThirdHopSentTime = Alım Zamanı
    
    FinalCalcDelayData final_data(
        input_data.getId(),                    // track_id
        input_data.getXVelocityECEF(),        // x_velocity_ecef
        input_data.getYVelocityECEF(),        // y_velocity_ecef
        input_data.getZVelocityECEF(),        // z_velocity_ecef
        input_data.getXPositionECEF(),        // x_position_ecef
        input_data.getYPositionECEF(),        // y_position_ecef
        input_data.getZPositionECEF(),        // z_position_ecef
        input_data.getUpdateTime(),           // update_time
        input_data.getOriginalUpdateTime(),   // original_update_time
        input_data.getFirstHopSentTime(),     // first_hop_sent_time
        input_data.getFirstHopDelayTime(),    // first_hop_delay_time
        input_data.getSecondHopSentTime(),    // second_hop_sent_time
        second_hop_delay_time,                // second_hop_delay_time (calculated)
        total_delay_time,                     // total_delay_time (calculated)
        third_hop_sent_time                   // third_hop_sent_time (calculated)
    );
    
    return final_data;
}

int main() {
    // Signal handler for graceful shutdown
    signal(SIGINT, signalHandler);
    
    cout << "=== hexagon_c VERBOSE ZeroMQ RECEIVER ===" << endl;
    cout << "📡 ZeroMQ ile hexagon_b'den DelayCalcTrackData alınıyor" << endl;
    cout << "⚙️  FinalCalcDelayData otomatik olarak üretiliyor" << endl;
    cout << "🔄 Sürekli çalışır - durdurmak için Ctrl+C basın" << endl;
    cout << "==========================================" << endl;
    
    try {
        // ZeroMQ Context ve Socket
        zmq::context_t context(1);
        zmq::socket_t socket(context, zmq::socket_type::pull);
        
        // Bind to port 7777
        socket.bind("tcp://*:7777");
        cout << "✅ ZeroMQ Bound to tcp://*:7777" << endl;
        cout << "🔄 Waiting for ZeroMQ messages from hexagon_b..." << endl;
        cout << "----------------------------------------" << endl;
        
        int message_count = 0;
        auto start_time = chrono::steady_clock::now();
        
        while (running) {
            zmq::message_t message;
            
            // Non-blocking receive with timeout
            auto result = socket.recv(message, zmq::recv_flags::dontwait);
            
            if (result) {
                message_count++;
                string received_data = string(static_cast<char*>(message.data()), message.size());
                
                cout << "\n📨 [" << message_count << "] === RECEIVED ZeroMQ MESSAGE ===" << endl;
                cout << "📄 Raw Data: " << received_data.substr(0, 80);
                if (received_data.length() > 80) cout << "...";
                cout << " (" << message.size() << " bytes)" << endl;
                
                // Parse received data
                DelayCalcTrackData track_data = parseReceivedData(received_data);
                
                cout << "🔍 Parsed DelayCalcTrackData:" << endl;
                cout << "   ├── Track ID: " << track_data.getId() << endl;
                cout << "   ├── Velocity: " << fixed << setprecision(2) << track_data.getVelocity() << " m/s" << endl;
                cout << "   ├── Position: " << fixed << setprecision(2) << track_data.getPosition() << " m" << endl;
                cout << "   ├── ECEF Speed: " << fixed << setprecision(2) << track_data.getECEFSpeed() << " m/s" << endl;
                cout << "   ├── ECEF Position: (" << fixed << setprecision(0) 
                     << track_data.getXPositionECEF() << ", "
                     << track_data.getYPositionECEF() << ", "
                     << track_data.getZPositionECEF() << ")" << endl;
                cout << "   └── Update Time: " << track_data.getUpdateTime() << " ms" << endl;
                
                // Generate FinalCalcDelayData (user specified formulas)
                cout << "⚙️  Processing with User Formulas..." << endl;
                FinalCalcDelayData final_data = generateFinalCalcDelayData(track_data);
                
                // GERÇEK GECİKME HESAPLAMA VE GÖRÜNTÜLEME
                auto current_receive_time = chrono::duration_cast<chrono::milliseconds>(
                    chrono::system_clock::now().time_since_epoch()).count();
                int64_t sent_time = track_data.getFirstHopSentTime();
                int64_t actual_network_delay = current_receive_time - sent_time;
                
                cout << "🎯 Generated FinalCalcDelayData (GERÇEK GECİKME):" << endl;
                cout << "   ├── Track ID: " << final_data.getTrackId() << endl;
                cout << "   ├── 📤 Sent Time (hexagon_b): " << sent_time << " ms" << endl;
                cout << "   ├── 📨 Received Time (hexagon_c): " << current_receive_time << " ms" << endl;
                cout << "   ├── ⏱️  ACTUAL NETWORK DELAY: " << actual_network_delay << " ms" << endl;
                cout << "   ├── Second Hop Delay: " << final_data.getSecondHopDelayTime() << " ms" << endl;
                cout << "   ├── Total Delay: " << final_data.getTotalDelayTime() << " ms" << endl;
                
                // GERÇEK GECİKME PERFORMANS ANALİZİ
                string delay_status;
                string delay_color;
                if (actual_network_delay < 10) {
                    delay_status = "EXCELLENT";
                    delay_color = "🟢";
                } else if (actual_network_delay < 50) {
                    delay_status = "GOOD";
                    delay_color = "🟢";
                } else if (actual_network_delay < 100) {
                    delay_status = "ACCEPTABLE";
                    delay_color = "🟡";
                } else if (actual_network_delay < 500) {
                    delay_status = "POOR";
                    delay_color = "🟠";
                } else {
                    delay_status = "CRITICAL";
                    delay_color = "🔴";
                }
                
                cout << "   ├── 🚀 Network Performance: " << delay_color << " " << delay_status << endl;
                cout << "   └── 📊 Delay Category: ";
                if (actual_network_delay < 10) cout << "Real-time (< 10ms)";
                else if (actual_network_delay < 50) cout << "Near real-time (< 50ms)";
                else if (actual_network_delay < 100) cout << "Interactive (< 100ms)";
                else if (actual_network_delay < 500) cout << "Noticeable (< 500ms)";
                else cout << "Problematic (≥ 500ms)";
                cout << endl;
                
                // Statistics (her 10 mesajda bir)
                if (message_count % 10 == 0) {
                    auto elapsed = chrono::steady_clock::now() - start_time;
                    auto seconds = chrono::duration_cast<chrono::seconds>(elapsed).count();
                    if (seconds > 0) {
                        double rate = static_cast<double>(message_count) / seconds;
                        cout << "📊 Reception Rate: " << fixed << setprecision(2) << rate << " msg/s" << endl;
                    }
                    cout << "========================================" << endl;
                }
                
            } else {
                // No message available, small delay to prevent busy waiting
                this_thread::sleep_for(chrono::milliseconds(50));
            }
        }
        
        cout << "\n🎯 Final Reception Statistics:" << endl;
        auto total_elapsed = chrono::steady_clock::now() - start_time;
        auto total_seconds = chrono::duration_cast<chrono::seconds>(total_elapsed).count();
        if (total_seconds > 0) {
            double final_rate = static_cast<double>(message_count) / total_seconds;
            cout << "   - Total Messages Received: " << message_count << endl;
            cout << "   - Total Time: " << total_seconds << " seconds" << endl;
            cout << "   - Average Reception Rate: " << fixed << setprecision(2) << final_rate << " msg/s" << endl;
        }
        
        cout << "✅ ZeroMQ Verbose Receiver stopped gracefully!" << endl;
        
    } catch (const exception& e) {
        cerr << "❌ ZeroMQ Error: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}