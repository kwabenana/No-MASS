// Copyright 2015 Jacob Chapman

#include <vector>
#include <iostream>
#include "SimulationConfig.h"
#include "Utility.h"
#include "DataStore.h"
#include "Agent_Action_Window_Stochastic.h"

Agent_Action_Window_Stochastic::Agent_Action_Window_Stochastic() {
}

void Agent_Action_Window_Stochastic::setup(int windowID, int id) {
  windowStruct ws = SimulationConfig::windows.at(windowID);
  m_window.setDurationVars(ws.aop, ws.bopout, ws.shapeop);
  m_window.setArrivalVars(ws.a01arr, ws.b01inarr, ws.b01outarr,
      ws.b01absprevarr, ws.b01rnarr);
  m_window.setInterVars(ws.a01int, ws.b01inint, ws.b01outint,
      ws.b01presint, ws.b01rnint);
  m_window.setDepartureVars(ws.a01dep, ws.b01outdep, ws.b01absdep,
      ws.b01gddep, ws.a10dep, ws.b10indep, ws.b10outdep, ws.b10absdep,
      ws.b10gddep);

  variableNameWindowDesire = "Agent_Window_Desire" + std::to_string(id);
  DataStore::addVariable(variableNameWindowDesire);
}

void Agent_Action_Window_Stochastic::step(const Building_Zone& zone,
    const bool inZone,
    const bool previouslyInZone, const std::vector<double> &activities) {
  double outdoorTemperature =
    DataStore::getValue("EnvironmentSiteOutdoorAirDrybulbTemperature");

  // double rain = DataStore::getValue("EnvironmentSiteRainStatus");
  double rain = 0;
  double indoorTemperature = zone.getMeanAirTemperature();
  double timeStepLengthInMinutes = SimulationConfig::lengthOfTimestep() / 60;

  m_window.setWindowState(zone.getWindowState());
  if (m_window.getWindowState() == 0) {
    m_window.setDurationOpen(0);
  }
  if (inZone && !previouslyInZone) {
    double previousDuration = getPreviousDurationOfAbsenceState(activities);
    m_window.arrival(indoorTemperature,
        outdoorTemperature, previousDuration, rain, timeStepLengthInMinutes);
  } else if (inZone && previouslyInZone) {
    double currentDuration = getCurrentDurationOfPresenceState(activities);
    m_window.intermediate(indoorTemperature,
        outdoorTemperature, currentDuration, rain, timeStepLengthInMinutes);
  } else if (!inZone && previouslyInZone) {
    double groundFloor = zone.getGroundFloor();
    double futureDuration = getFutureDurationOfAbsenceState(activities);
    m_window.departure(
        indoorTemperature, dailyMeanTemperature, futureDuration, groundFloor);
  }
  result = m_window.getWindowState();
}
