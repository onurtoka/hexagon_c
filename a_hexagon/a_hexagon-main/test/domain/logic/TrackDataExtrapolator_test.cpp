#include <iostream>
#include <cmath>
#include <memory>
#include <thread>
#include <chrono>
#include "../../../src/domain/model/TrackData.h"
#include "../../../src/domain/model/ExtrapTrackData.h"
#include "../../../src/domain/logic/TrackDataExtrapolator.h"

using namespace domain::model;
using namespace domain::logic;

#define ASSERT_EQ(a, b) if ((a) != (b)) { std::cerr << "ASSERT_EQ failed: " << #a << " != " << #b << " (" << (a) << " != " << (b) << ")\n"; return false; }
#define EXPECT_DOUBLE_EQ(a, b) if (std::fabs((a) - (b)) > 1e-9) { std::cerr << "EXPECT_DOUBLE_EQ failed: " << #a << " != " << #b << " (" << (a) << " != " << (b) << ")\n"; return false; }
#define EXPECT_NEAR(a, b, tol) if (std::fabs((a) - (b)) > (tol)) { std::cerr << "EXPECT_NEAR failed: " << #a << " != " << #b << " (" << (a) << " != " << (b) << ")\n"; return false; }

// Mock outgoing adapter for testing
class MockOutgoingAdapter : public domain::ports::outgoing::ExtrapTrackDataOutgoingPort {
public:
    std::vector<std::vector<ExtrapTrackData>> sentData;
    
    void sendExtrapTrackData(const std::vector<ExtrapTrackData>& data) override {
        sentData.push_back(data);
    }
    
    void clear() {
        sentData.clear();
    }
};

// ============= processAndForwardTrackData Tests =============

bool Test_processAndForwardTrackData_ConstantVelocity() {
    auto mockAdapter = std::make_shared<MockOutgoingAdapter>();
    TrackDataExtrapolator extrapolator(mockAdapter.get());
    
    TrackData track;
    track.setTrackId(42);
    track.setXPositionECEF(100.0);
    track.setYPositionECEF(200.0);
    track.setZPositionECEF(300.0);
    track.setXVelocityECEF(10.0);
    track.setYVelocityECEF(20.0);
    track.setZVelocityECEF(30.0);
    track.setOriginalUpdateTime(1000);
    
    extrapolator.processAndForwardTrackData(track);
    
    ASSERT_EQ(mockAdapter->sentData.size(), 1);
    auto result = mockAdapter->sentData[0];
    
    ASSERT_EQ(result.size(), 25);
    EXPECT_DOUBLE_EQ(result[0].getXPositionECEF(), 100.0);
    EXPECT_DOUBLE_EQ(result[0].getYPositionECEF(), 200.0);
    EXPECT_DOUBLE_EQ(result[0].getZPositionECEF(), 300.0);
    
    double t = 0.005 * 24;
    EXPECT_NEAR(result[24].getXPositionECEF(), 100.0 + 10.0 * t, 1e-9);
    EXPECT_NEAR(result[24].getYPositionECEF(), 200.0 + 20.0 * t, 1e-9);
    EXPECT_NEAR(result[24].getZPositionECEF(), 300.0 + 30.0 * t, 1e-9);
    
    return true;
}

bool Test_processAndForwardTrackData_ZeroVelocity() {
    auto mockAdapter = std::make_shared<MockOutgoingAdapter>();
    TrackDataExtrapolator extrapolator(mockAdapter.get());
    
    TrackData track;
    track.setTrackId(1);
    track.setXPositionECEF(50.0);
    track.setYPositionECEF(-50.0);
    track.setZPositionECEF(0.0);
    track.setXVelocityECEF(0.0);
    track.setYVelocityECEF(0.0);
    track.setZVelocityECEF(0.0);
    track.setOriginalUpdateTime(500);
    
    extrapolator.processAndForwardTrackData(track);
    
    ASSERT_EQ(mockAdapter->sentData.size(), 1);
    auto result = mockAdapter->sentData[0];
    
    ASSERT_EQ(result.size(), 25);
    for (const auto& extrap : result) {
        EXPECT_DOUBLE_EQ(extrap.getXPositionECEF(), 50.0);
        EXPECT_DOUBLE_EQ(extrap.getYPositionECEF(), -50.0);
        EXPECT_DOUBLE_EQ(extrap.getZPositionECEF(), 0.0);
        EXPECT_DOUBLE_EQ(extrap.getXVelocityECEF(), 0.0);
        EXPECT_DOUBLE_EQ(extrap.getYVelocityECEF(), 0.0);
        EXPECT_DOUBLE_EQ(extrap.getZVelocityECEF(), 0.0);
    }
    
    return true;
}

bool Test_processAndForwardTrackData_NegativeVelocity() {
    auto mockAdapter = std::make_shared<MockOutgoingAdapter>();
    TrackDataExtrapolator extrapolator(mockAdapter.get());
    
    TrackData track;
    track.setTrackId(99);
    track.setXPositionECEF(1000.0);
    track.setYPositionECEF(2000.0);
    track.setZPositionECEF(3000.0);
    track.setXVelocityECEF(-5.0);
    track.setYVelocityECEF(-10.0);
    track.setZVelocityECEF(-15.0);
    track.setOriginalUpdateTime(1500);
    
    extrapolator.processAndForwardTrackData(track);
    
    ASSERT_EQ(mockAdapter->sentData.size(), 1);
    auto result = mockAdapter->sentData[0];
    
    ASSERT_EQ(result.size(), 25);
    double t = 0.005 * 10; // 10th step
    EXPECT_NEAR(result[10].getXPositionECEF(), 1000.0 + (-5.0) * t, 1e-9);
    EXPECT_NEAR(result[10].getYPositionECEF(), 2000.0 + (-10.0) * t, 1e-9);
    EXPECT_NEAR(result[10].getZPositionECEF(), 3000.0 + (-15.0) * t, 1e-9);
    
    return true;
}

bool Test_processAndForwardTrackData_LargeValues() {
    auto mockAdapter = std::make_shared<MockOutgoingAdapter>();
    TrackDataExtrapolator extrapolator(mockAdapter.get());
    
    TrackData track;
    track.setTrackId(12345);
    track.setXPositionECEF(1e6);
    track.setYPositionECEF(2e6);
    track.setZPositionECEF(3e6);
    track.setXVelocityECEF(1000.0);
    track.setYVelocityECEF(2000.0);
    track.setZVelocityECEF(3000.0);
    track.setOriginalUpdateTime(9999999);
    
    extrapolator.processAndForwardTrackData(track);
    
    ASSERT_EQ(mockAdapter->sentData.size(), 1);
    auto result = mockAdapter->sentData[0];
    
    ASSERT_EQ(result.size(), 25);
    ASSERT_EQ(result[0].getTrackId(), 12345);
    EXPECT_DOUBLE_EQ(result[0].getXPositionECEF(), 1e6);
    EXPECT_DOUBLE_EQ(result[0].getYPositionECEF(), 2e6);
    EXPECT_DOUBLE_EQ(result[0].getZPositionECEF(), 3e6);
    
    return true;
}

bool Test_processAndForwardTrackData_TimeProgression() {
    auto mockAdapter = std::make_shared<MockOutgoingAdapter>();
    TrackDataExtrapolator extrapolator(mockAdapter.get());
    
    TrackData track;
    track.setTrackId(7);
    track.setXPositionECEF(0.0);
    track.setYPositionECEF(0.0);
    track.setZPositionECEF(0.0);
    track.setXVelocityECEF(1.0);
    track.setYVelocityECEF(1.0);
    track.setZVelocityECEF(1.0);
    track.setOriginalUpdateTime(2000);
    
    extrapolator.processAndForwardTrackData(track);
    
    ASSERT_EQ(mockAdapter->sentData.size(), 1);
    auto result = mockAdapter->sentData[0];
    
    ASSERT_EQ(result.size(), 25);
    for (int i = 0; i < 25; ++i) {
        long expectedTime = 2000 + static_cast<long>(i * 0.005 * 1000000000); // nanoseconds
        ASSERT_EQ(result[i].getUpdateTime(), expectedTime);
        ASSERT_EQ(result[i].getOriginalUpdateTime(), 2000);
    }
    
    return true;
}

// ============= extrapolate Tests =============

bool Test_extrapolate_BasicFunctionality() {
    auto mockAdapter = std::make_shared<MockOutgoingAdapter>();
    TrackDataExtrapolator extrapolator(mockAdapter.get());
    
    TrackData input;
    input.setTrackId(10);
    input.setXPositionECEF(0.0);
    input.setYPositionECEF(0.0);
    input.setZPositionECEF(0.0);
    input.setXVelocityECEF(100.0);
    input.setYVelocityECEF(200.0);
    input.setZVelocityECEF(300.0);
    input.setOriginalUpdateTime(1000);
    
    auto result = extrapolator.extrapolateTo200Hz(input);
    
    ASSERT_EQ(result.size(), 25);
    ASSERT_EQ(result[0].getTrackId(), 10);
    EXPECT_DOUBLE_EQ(result[0].getXVelocityECEF(), 100.0);
    EXPECT_DOUBLE_EQ(result[0].getYVelocityECEF(), 200.0);
    EXPECT_DOUBLE_EQ(result[0].getZVelocityECEF(), 300.0);
    
    return true;
}

bool Test_extrapolate_PositionCalculation() {
    auto mockAdapter = std::make_shared<MockOutgoingAdapter>();
    TrackDataExtrapolator extrapolator(mockAdapter.get());
    
    TrackData input;
    input.setTrackId(20);
    input.setXPositionECEF(10.0);
    input.setYPositionECEF(20.0);
    input.setZPositionECEF(30.0);
    input.setXVelocityECEF(2.0);
    input.setYVelocityECEF(4.0);
    input.setZVelocityECEF(6.0);
    input.setOriginalUpdateTime(500);
    
    auto result = extrapolator.extrapolateTo200Hz(input);
    
    ASSERT_EQ(result.size(), 25);
    
    // Check 5th element (i=4)
    double t = 0.005 * 4;
    EXPECT_NEAR(result[4].getXPositionECEF(), 10.0 + 2.0 * t, 1e-9);
    EXPECT_NEAR(result[4].getYPositionECEF(), 20.0 + 4.0 * t, 1e-9);
    EXPECT_NEAR(result[4].getZPositionECEF(), 30.0 + 6.0 * t, 1e-9);
    
    return true;
}

bool Test_extrapolate_FirstHopSentTime() {
    auto mockAdapter = std::make_shared<MockOutgoingAdapter>();
    TrackDataExtrapolator extrapolator(mockAdapter.get());
    
    TrackData input;
    input.setTrackId(30);
    input.setXPositionECEF(0.0);
    input.setYPositionECEF(0.0);
    input.setZPositionECEF(0.0);
    input.setXVelocityECEF(0.0);
    input.setYVelocityECEF(0.0);
    input.setZVelocityECEF(0.0);
    input.setOriginalUpdateTime(1234567890);
    
    // İlk çağrı
    auto result1 = extrapolator.extrapolateTo200Hz(input);
    
    // Kısa bir bekleme
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    
    // İkinci çağrı
    auto result2 = extrapolator.extrapolateTo200Hz(input);
    
    ASSERT_EQ(result1.size(), 25);
    ASSERT_EQ(result2.size(), 25);
    
    // İki farklı çağrının firstHopSentTime'ları farklı olmalı
    long firstTime1 = result1[0].getFirstHopSentTime();
    long firstTime2 = result2[0].getFirstHopSentTime();
    
    // firstHopSentTime'lar farklı olmalı (ikinci çağrı daha sonra yapıldığı için)
    ASSERT_EQ(firstTime1 < firstTime2, true);
    
    
    
    return true;
}

bool Test_extrapolate_VelocityPreservation() {
    auto mockAdapter = std::make_shared<MockOutgoingAdapter>();
    TrackDataExtrapolator extrapolator(mockAdapter.get());
    
    TrackData input;
    input.setTrackId(40);
    input.setXPositionECEF(100.0);
    input.setYPositionECEF(200.0);
    input.setZPositionECEF(300.0);
    input.setXVelocityECEF(50.0);
    input.setYVelocityECEF(-25.0);
    input.setZVelocityECEF(75.0);
    input.setOriginalUpdateTime(9876);
    
    auto result = extrapolator.extrapolateTo200Hz(input);
    
    ASSERT_EQ(result.size(), 25);
    
    // Velocity should remain constant in all extrapolated points
    for (const auto& extrap : result) {
        EXPECT_DOUBLE_EQ(extrap.getXVelocityECEF(), 50.0);
        EXPECT_DOUBLE_EQ(extrap.getYVelocityECEF(), -25.0);
        EXPECT_DOUBLE_EQ(extrap.getZVelocityECEF(), 75.0);
    }
    
    return true;
}

bool Test_extrapolate_TrackIdPreservation() {
    auto mockAdapter = std::make_shared<MockOutgoingAdapter>();
    TrackDataExtrapolator extrapolator(mockAdapter.get());
    
    TrackData input;
    input.setTrackId(999888777);
    input.setXPositionECEF(0.0);
    input.setYPositionECEF(0.0);
    input.setZPositionECEF(0.0);
    input.setXVelocityECEF(0.0);
    input.setYVelocityECEF(0.0);
    input.setZVelocityECEF(0.0);
    input.setOriginalUpdateTime(1111);
    
    auto result = extrapolator.extrapolateTo200Hz(input);
    
    ASSERT_EQ(result.size(), 25);
    
    // Track ID should be preserved in all extrapolated points
    for (const auto& extrap : result) {
        ASSERT_EQ(extrap.getTrackId(), 999888777);
        ASSERT_EQ(extrap.getOriginalUpdateTime(), 1111);
    }
    
    return true;
}

int main() {
    int passed = 0, failed = 0;
    
    // processAndForwardTrackData Tests
    if (Test_processAndForwardTrackData_ConstantVelocity()) { 
        std::cout << "Test_processAndForwardTrackData_ConstantVelocity PASSED\n"; 
        ++passed; 
    } else { 
        ++failed; 
    }
    
    if (Test_processAndForwardTrackData_ZeroVelocity()) { 
        std::cout << "Test_processAndForwardTrackData_ZeroVelocity PASSED\n"; 
        ++passed; 
    } else { 
        ++failed; 
    }
    
    if (Test_processAndForwardTrackData_NegativeVelocity()) { 
        std::cout << "Test_processAndForwardTrackData_NegativeVelocity PASSED\n"; 
        ++passed; 
    } else { 
        ++failed; 
    }
    
    if (Test_processAndForwardTrackData_LargeValues()) { 
        std::cout << "Test_processAndForwardTrackData_LargeValues PASSED\n"; 
        ++passed; 
    } else { 
        ++failed; 
    }
    
    if (Test_processAndForwardTrackData_TimeProgression()) { 
        std::cout << "Test_processAndForwardTrackData_TimeProgression PASSED\n"; 
        ++passed; 
    } else { 
        ++failed; 
    }
    
    // extrapolate Tests
    if (Test_extrapolate_BasicFunctionality()) { 
        std::cout << "Test_extrapolate_BasicFunctionality PASSED\n"; 
        ++passed; 
    } else { 
        ++failed; 
    }
    
    if (Test_extrapolate_PositionCalculation()) { 
        std::cout << "Test_extrapolate_PositionCalculation PASSED\n"; 
        ++passed; 
    } else { 
        ++failed; 
    }
    
    if (Test_extrapolate_FirstHopSentTime()) { 
        std::cout << "Test_extrapolate_FirstHopSentTime PASSED\n"; 
        ++passed; 
    } else { 
        ++failed; 
    }
    
    if (Test_extrapolate_VelocityPreservation()) { 
        std::cout << "Test_extrapolate_VelocityPreservation PASSED\n"; 
        ++passed; 
    } else { 
        ++failed; 
    }
    
    if (Test_extrapolate_TrackIdPreservation()) { 
        std::cout << "Test_extrapolate_TrackIdPreservation PASSED\n"; 
        ++passed; 
    } else { 
        ++failed; 
    }
    
    std::cout << "\n" << passed << " tests passed, " << failed << " tests failed.\n";
    return failed == 0 ? 0 : 1;
}