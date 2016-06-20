// Copyright 2015 Jacob Chapman

#ifndef BUILDING_ZONE_H
#define	BUILDING_ZONE_H
#include <string>
#include <vector>

#include "SimulationConfig.h"
#include "Model_Windows.h"

class Building_Zone {
public:
    Building_Zone();
    Building_Zone(const ZoneStruct & zoneStruct);

    void step();
    void setup();
    void setName(std::string name);
    void setCurrentAgentGains(double currentAgentGains);
    void setGroundFloor(bool groundFloor);
    void setActive(bool active);
    void setWindowDurationOpen(double windowDurationOpen);
    void setWindowState(bool windowState);
    void setLightState(bool lightState);
    void setBlindState(double state);
    void setHeatingState(double state);
    void setOccupantFraction(double occupantFraction);
    void setAppFraction(double appFraction);

    bool isActive() const;
    bool hasActivity(int activity) const;
    int getCurrentOccupantCount() const;
    int getId() const;
    float getOccupantFraction() const;
    double getCurrentAgentGains() const;
    double getWindowState() const;
    double getBlindState() const;
    double getHeatingState() const;
    double getGroundFloor() const;
    double getAirSystemSensibleHeatingRate() const;
    double getLightState() const;
    double getMeanAirTemperature() const;
    double getAirRelativeHumidity() const;
    double getMeanRadiantTemperature() const;
    double getDaylightingReferencePoint1Illuminance() const;
    double getWindowDurationOpen() const;
    std::vector<int> getActivities() const;
    int getNumberOfActivities() const;
    std::string getName() const;

private:

    bool groundFloor;
    bool active;
    bool lightState;
    bool windowState;
    double currentAgentGains;
    double blindState;
    double occupantFraction;
    double heatingState;
    double appFraction;

    int id;
    std::string name;
    std::string variableNameBlindFraction;
    std::string variableNameNumberOfOccupants;
    std::string variableNameAverageGains;
    std::string variableNameLight;
    std::string variableNameHeating;
    std::string variableNameAppFraction;
    std::vector<std::string> variableNameWindow;
    std::vector<int> activities;

};

#endif	/* BUILDING_ZONE_H */
