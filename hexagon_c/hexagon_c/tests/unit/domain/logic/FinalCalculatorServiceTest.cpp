#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <memory>

#include "domain/logic/TrackDataProcessor.hpp"
#include "domain/ports/outgoing/TrackDataPublisher.hpp"
#include "domain/model/DelayCalcTrackData.hpp"
#include "domain/model/FinalCalcTrackData.hpp"

using namespace testing;

class MockDataSender : public IDataSender {
public:
    MOCK_METHOD(void, sendData, (const domain::model::FinalCalcTrackData&), (override));
};

class TestFinalCalculatorService : public FinalCalculatorService {
public:
    explicit TestFinalCalculatorService(std::unique_ptr<IDataSender> sender)
        : FinalCalculatorService(std::move(sender)) {}

    domain::model::FinalCalcTrackData testCalculateFinalDelay(
        const domain::model::DelayCalcTrackData& input
    ) {
        return calculateFinalDelay(input);
    }
};

class FinalCalculatorServiceTest : public Test {
protected:
    void SetUp() override {
        mockSender = std::make_unique<MockDataSender>();
        senderPtr = mockSender.get();
        service = std::make_unique<TestFinalCalculatorService>(std::move(mockSender));
    }

    std::unique_ptr<MockDataSender> mockSender;
    MockDataSender* senderPtr;
    std::unique_ptr<TestFinalCalculatorService> service;
};

TEST_F(FinalCalculatorServiceTest, CalculatesFinalDelayCorrectly) {
    // Arrange
    domain::model::DelayCalcTrackData inputData;
    inputData.setTrackId(1001);
    inputData.setXPositionECEF(1000.0);
    inputData.setYPositionECEF(2000.0);
    inputData.setZPositionECEF(3000.0);
    inputData.setUpdateTime(1696932000000); // 10 Oct 2023 12:00:00

    // Act
    auto result = service->testCalculateFinalDelay(inputData);

    // Assert
    EXPECT_EQ(result.getTrackId(), 1001);
    EXPECT_NEAR(result.getXPositionECEF(), 1000.0, 0.001);
    EXPECT_NEAR(result.getYPositionECEF(), 2000.0, 0.001);
    EXPECT_NEAR(result.getZPositionECEF(), 3000.0, 0.001);
}