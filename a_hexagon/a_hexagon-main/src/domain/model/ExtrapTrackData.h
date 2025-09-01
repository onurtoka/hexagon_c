#ifndef EXTRAP_TRACK_DATA_H 
#define EXTRAP_TRACK_DATA_H 
namespace domain {
namespace model {
class ExtrapTrackData {
public: 
    ExtrapTrackData();
    virtual ~ExtrapTrackData();
    int getTrackId() const { return TrackId; }
    void setTrackId(int id) { TrackId = id; }
    double getXVelocityECEF() const { return XVelocityECEF; }
    void setXVelocityECEF(double v) { XVelocityECEF = v; }
    double getYVelocityECEF() const { return YVelocityECEF; }
    void setYVelocityECEF(double v) { YVelocityECEF = v; }
    double getZVelocityECEF() const { return ZVelocityECEF; }
    void setZVelocityECEF(double v) { ZVelocityECEF = v; }
    double getXPositionECEF() const { return XPositionECEF; }
    void setXPositionECEF(double p) { XPositionECEF = p; }
    double getYPositionECEF() const { return YPositionECEF; }
    void setYPositionECEF(double p) { YPositionECEF = p; }
    double getZPositionECEF() const { return ZPositionECEF; }
    void setZPositionECEF(double p) { ZPositionECEF = p; }
    long getUpdateTime() const { return updateTime; }
    void setUpdateTime(long t) { updateTime = t; }
    long getOriginalUpdateTime() const { return originalUpdateTime; }
    void setOriginalUpdateTime(long t) { originalUpdateTime = t; }
    long getFirstHopSentTime() const { return firstHopSentTime; }
    void setFirstHopSentTime(long t) { firstHopSentTime = t; }
private: 
    int TrackId;
    double XVelocityECEF;
    double YVelocityECEF;
    double ZVelocityECEF;
    double XPositionECEF;
    double YPositionECEF;
    double ZPositionECEF;
    long originalUpdateTime;
    long updateTime;
    long firstHopSentTime;
};
}
}
#endif