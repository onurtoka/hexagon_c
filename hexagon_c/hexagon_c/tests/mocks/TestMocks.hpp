#pragma once

#include <gmock/gmock.h>
#include "domain/ports/outgoing/TrackDataPublisher.hpp"
#include "domain/model/FinalCalcTrackData.hpp"

namespace test {
namespace mocks {

class MockDataSender : public IDataSender {
public:
    MOCK_METHOD(void, sendData, (const domain::model::FinalCalcTrackData&), (override));
};

class MockDataReceiver : public IDataReceiver {
public:
    MOCK_METHOD(void, onDataReceived, (const domain::model::DelayCalcTrackData&), (override));
};

class MockZMQContext {
public:
    MOCK_METHOD(bool, send, (const std::vector<uint8_t>&), ());
    MOCK_METHOD(bool, receive, (std::vector<uint8_t>&), ());
};

} // namespace mocks
} // namespace test