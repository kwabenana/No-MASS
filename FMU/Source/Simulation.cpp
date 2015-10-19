/*
 * File:   Simulation.cpp
 * Author: jake
 *
 * Created on September 13, 2013, 10:12 AM
 */

#include <iostream>
#include <string>

#include "Simulation.h"
#include "SimulationConfig.h"
#include "DataStore.h"

Simulation::Simulation() {
        monthCount.push_back(31);
        monthCount.push_back(59);
        monthCount.push_back(90);
        monthCount.push_back(120);
        monthCount.push_back(151);
        monthCount.push_back(181);
        monthCount.push_back(212);
        monthCount.push_back(243);
        monthCount.push_back(273);
        monthCount.push_back(304);
        monthCount.push_back(334);
        monthCount.push_back(365);
        time = 0;
        DataStore::clear();
}


/**
 * @brief Calls the simulation preprocess
 * @details Reads in the configuration file and sends to parser.
 * Sets up the EnergyPlus processor, the AgentModel and the ZoneManager.
 */
void Simulation::preprocess() {
        parseConfiguration(SimulationConfig::FmuLocation + "/SimulationConfig.xml");
        if(!LOG.getError()){
            setupSimulationModel();
        }
}

void Simulation::parseConfiguration(const std::string file){
    SimulationConfig::parseConfiguration(file);
}

void Simulation::setupSimulationModel(){
    SimulationConfig::stepCount = -1;
    DataStore::addVariable("day");
    DataStore::addVariable("month");
    DataStore::addVariable("hour");
    DataStore::addVariable("TimeStep");
    energySolver.setup();
    agentModel.setZones(energySolver.getZones());
    agentModel.setup();
}

/**
 * @brief Calls the simulation prostprocess
 *
 */
void Simulation::postprocess() {
    agentModel.postprocess();
    DataStore::print();
}

/**
 * @brief processes before timestep
 */
void Simulation::preTimeStep() {
        double day = time/86400;
        double hour = time/3600;
#ifdef DEBUG
        if (static_cast<int>(time) % (86400*10) == 0) {
                std::cout << "day: " << day << std::endl;
        }
#endif // DEBUG
        int month = 1;
        for (int mc : monthCount) {
                if (mc > day || month + 1 > 12) {
                        break;
                }
                month = month + 1;
        }
        DataStore::addValue("TimeStep", time);
        DataStore::addValue("day", day);
        DataStore::addValue("hour", hour);
        DataStore::addValue("month", month);
}

/**
 * @brief Increments the timestep for the simulation
 * @details Increments the timestep for the EnergyPlus processor, the AgentModel and the ZoneManager.
 * Also we send any effects the agent have to the zones they are located in.
 */
void Simulation::timeStep() {
        SimulationConfig::step();
        time = time + (3600 / SimulationConfig::info.timeStepsPerHour);
        agentModel.step();
}

/**
 * @brief processes After timestep
 */
void Simulation::postTimeStep() {
}
