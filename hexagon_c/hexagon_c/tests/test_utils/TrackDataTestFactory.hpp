#pragma once

#include <string>
#include <fstream>
#include <nlohmann/json.hpp>
#include "domain/model/DelayCalcTrackData.hpp"
#include "domain/model/FinalCalcTrackData.hpp"

namespace test {
namespace utils {

using json = nlohmann::json;

class TrackDataTestFactory {
public:
    static domain::model::DelayCalcTrackData createFromJson(const json& data) {
        domain::model::DelayCalcTrackData trackData;
        
        trackData.setTrackId(data["track_id"]);
        
        if (data.contains("positions")) {
            trackData.setXPositionECEF(data["positions"]["x"]);
            trackData.setYPositionECEF(data["positions"]["y"]);
            trackData.setZPositionECEF(data["positions"]["z"]);
        }
        
        if (data.contains("velocities")) {
            trackData.setXVelocityECEF(data["velocities"]["x"]);
            trackData.setYVelocityECEF(data["velocities"]["y"]);
            trackData.setZVelocityECEF(data["velocities"]["z"]);
        }
        
        if (data.contains("timing")) {
            trackData.setUpdateTime(data["timing"]["update_time"]);
            trackData.setFirstHopTime(data["timing"]["first_hop_time"]);
            trackData.setSecondHopTime(data["timing"]["second_hop_time"]);
        }
        
        return trackData;
    }

    static json loadTestData(const std::string& filename = "track_samples.json") {
        std::ifstream file("tests/test_data/" + filename);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open test data file: " + filename);
        }
        return json::parse(file);
    }
};

} // namespace utils
} // namespace test