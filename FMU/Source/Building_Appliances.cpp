// Copyright 2016 Jacob Chapman

#include <string>
#include <list>
#include <vector>
#include "SimulationConfig.h"
#include "DataStore.h"
#include "Building_Appliances.h"

Building_Appliances::Building_Appliances() {
    PowerRequested = 0;
    PowerGenerated = 0;
}

void Building_Appliances::setup(const buildingStruct & b) {
  buildingID = b.id;
  buildingString = "Building" + std::to_string(buildingID) + "_Appliance";
  DataStore::addVariable(buildingString + "_Sum_Recieved");
  DataStore::addVariable(buildingString + "_Sum_Small");
  DataStore::addVariable(buildingString + "_Sum_Large");
  DataStore::addVariable(buildingString + "_Sum_Cost");

  std::vector<appPVStruct> appPV =
                  b.AppliancesPV;
  for (const appPVStruct s : appPV) {
    pv.push_back(Appliance_PV());
    pv.back().setID(s.id);
    pv.back().setPriority(s.priority);
    pv.back().setHourlyCost(s.cost);
    pv.back().setBuildingID(buildingID);
    pv.back().setFileName(s.file);
    pv.back().setup();
    pv.back().setIDString(buildingString + std::to_string(s.id));
    pv.back().saveSetup();
    //  addAppToDataStrore(s.id);
  }

  std::vector<appLargeStruct> app =
                  b.AppliancesLarge;
  for (const appLargeStruct &s : app) {
    large.push_back(Appliance_Large());
    large.back().setID(s.id);
    large.back().setPriority(s.priority);
    large.back().setBuildingID(buildingID);
    large.back().setActivities(s.activities);
    large.back().setup();
    large.back().setIDString(buildingString + std::to_string(s.id));
    large.back().saveSetup();
    //  addAppToDataStrore(s.id);
  }

  app = b.AppliancesLargeLearning;
  for (const appLargeStruct &s : app) {
    largeLearning.push_back(Appliance_Large_Learning());
    largeLearning.back().setEpsilon(s.epsilon);
    largeLearning.back().setAlpha(s.alpha);
    largeLearning.back().setGamma(s.gamma);
    largeLearning.back().setUpdate(s.update);
    largeLearning.back().setID(s.id);
    largeLearning.back().setPriority(s.priority);
    largeLearning.back().setBuildingID(buildingID);
    largeLearning.back().setActivities(s.activities);
    largeLearning.back().setup();
    largeLearning.back().setIDString(buildingString + std::to_string(s.id));
    largeLearning.back().saveSetup();
    //  addAppToDataStrore(s.id);
  }

  std::vector<appSmallStruct> appSmall =
                  b.AppliancesSmall;
  for (const appSmallStruct s : appSmall) {
    small.push_back(Appliance_Small());
    small.back().setID(s.id);
    small.back().setPriority(s.priority);
    small.back().setWeibullParameters(s.WeibullParameters);
    small.back().setStateProbabilities(s.StateProbabilities);
    small.back().setFractions(s.Fractions);
    small.back().setSumRatedPowers(s.SumRatedPowers);
    small.back().setup();
    small.back().setIDString(buildingString + std::to_string(s.id));
    small.back().saveSetup();
    //  addAppToDataStrore(s.id);
  }

  std::vector<appFMIStruct> appFMI =
                  b.AppliancesFMI;
  for (const appFMIStruct s : appFMI) {
    fmi.push_back(Appliance_FMI());
    fmi.back().setID(s.id);
    fmi.back().setPriority(s.priority);
    fmi.back().setFMIVariableName(s.variableName);
    fmi.back().setup();
    fmi.back().setIDString(buildingString + std::to_string(s.id));
    fmi.back().saveSetup();
    //  addAppToDataStrore(s.id);
  }
}

void Building_Appliances::preprocess() {
  std::list<int> pop = Utility::randomIntList(small.size());
  for (int a : pop) {
      small[a].preprocess();
  }
  pop = Utility::randomIntList(pv.size());
  for (int a : pop) {
      pv[a].preprocess();
  }
}

bool Building_Appliances::sendContractGlobal(const contract & c) {
  bool send = (c.requested > c.recieved || c.suppliedLeft > 0);
  if (send) {
    contract x;
    x.id = c.id;
    x.buildingID = buildingID;
    x.recievedCost = c.recievedCost;
    x.requested = c.requested - c.recieved;
    x.recieved = c.recieved;
    x.supplied = c.suppliedLeft;
    x.suppliedCost = c.suppliedCost;
    globalContracts.push_back(x);
  }
  return send;
}

void Building_Appliances::sendContractLocal(const Appliance & a) {
  int stepcount = SimulationConfig::getStepCount();
  contract c;
  c.id = a.getID();
  c.buildingID = buildingID;
  c.requested = a.powerAt(stepcount);
  c.priority = a.getPriority();
  c.supplied = a.supplyAt(stepcount);
  c.suppliedCost = a.supplyCostAt(stepcount);
  c.recievedCost = 0;
  c.recieved = 0;
  app_negotiation.submit(c);
  PowerRequested += c.requested;
  PowerGenerated += c.supplied;
}

void Building_Appliances::stepLocalLarge() {
  std::list<int> pop = Utility::randomIntList(large.size());
  for (int a : pop) {
    large[a].hasActivities(currentStates);
    large[a].step();
    sendContractLocal(large[a]);
  }
}

void Building_Appliances::stepLocalLargeLearning() {
  std::list<int> pop = Utility::randomIntList(largeLearning.size());
  for (int a : pop) {
    largeLearning[a].hasActivities(currentStates);
    largeLearning[a].step();
    sendContractLocal(largeLearning[a]);
  }
}

void Building_Appliances::stepLocalSmall() {
  std::list<int> pop = Utility::randomIntList(small.size());
  for (int a : pop) {
    sendContractLocal(small[a]);
  }
}

void Building_Appliances::stepLocalPV() {
  std::list<int> pop = Utility::randomIntList(pv.size());
  for (int a : pop) {
    sendContractLocal(pv[a]);
  }
}

void Building_Appliances::stepLocalFMI() {
  std::list<int> pop = Utility::randomIntList(fmi.size());
  for (int a : pop) {
    fmi[a].step();
    sendContractLocal(fmi[a]);
  }
}

void Building_Appliances::stepLocal() {
  PowerRequested = 0;
  PowerGenerated = 0;
  stepLocalPV();
  stepLocalLarge();
  stepLocalSmall();
  stepLocalFMI();
  stepLocalLargeLearning();
}


void Building_Appliances::stepLocalNegotiation() {
  sum_large = 0;
  sum_cost = 0;
  sum_fmi = 0;
  sum_small = 0;
  app_negotiation.process();
  localNegotiationSmall();
  localNegotiationLarge();
  localNegotiationLargeLearning();
  localNegotiationFMI();
  localNegotiationPV();
  app_negotiation.clear();
}

void Building_Appliances::globalNegotiationSmall(
                                const LVN_Negotiation & building_negotiation) {
  std::list<int> pop = Utility::randomIntList(small.size());
  for (int a : pop) {
    if (small[a].isGlobal()) {
      int appid = small[a].getID();
      contract c = building_negotiation.getContract(buildingID, appid);
      double power = c.recieved;
      double cost = c.recievedCost;
      small[a].setRecieved(power);
      small[a].setRecievedCost(cost);
      small[a].save();
      sum_small += power;
      sum_cost += cost;
    }
  }
}

void Building_Appliances::localNegotiationSmall() {
  std::list<int> pop = Utility::randomIntList(small.size());
  for (int a : pop) {
    int appid = small[a].getID();
    contract c = app_negotiation.getContract(buildingID, appid);
    small[a].setGlobal(sendContractGlobal(c));
    if (!small[a].isGlobal()) {
      double power = c.recieved;
      double cost = c.recievedCost;
      small[a].setRecieved(power);
      small[a].setRecievedCost(cost);
      small[a].save();
      sum_small += power;
      sum_cost += cost;
    }
  }
}

void Building_Appliances::globalNegotiationLarge(const LVN_Negotiation & building_negotiation) {
  std::list<int> pop = Utility::randomIntList(large.size());
  for (int a : pop) {
    if (large[a].isGlobal()) {
      int appid = large[a].getID();
      contract c = building_negotiation.getContract(buildingID, appid);
      double power = c.recieved;
      double cost = c.recievedCost;
      large[a].setRecieved(power);
      large[a].setRecievedCost(cost);
      large[a].save();
      sum_large += power;
      sum_cost += cost;
    }
  }
}

void Building_Appliances::localNegotiationLarge() {
  std::list<int> pop = Utility::randomIntList(large.size());
  for (int a : pop) {
    int appid = large[a].getID();
    contract c = app_negotiation.getContract(buildingID, appid);
    large[a].setGlobal(sendContractGlobal(c));
    if (!large[a].isGlobal()) {
      double power = c.recieved;
      double cost = c.recievedCost;
      large[a].setRecieved(power);
      large[a].setRecievedCost(cost);
      large[a].save();
      sum_large += power;
      sum_cost += cost;
    }
  }
}

void Building_Appliances::globalNegotiationLargeLearning(
                                const LVN_Negotiation & building_negotiation) {
  std::list<int> pop = Utility::randomIntList(largeLearning.size());
  for (int a : pop) {
    if (largeLearning[a].isGlobal()) {
      int appid = largeLearning[a].getID();
      contract c = building_negotiation.getContract(buildingID, appid);
      double power = c.recieved;
      double cost = c.recievedCost;
      largeLearning[a].addToCost(cost);
      largeLearning[a].setRecieved(power);
      largeLearning[a].setRecievedCost(cost);
      largeLearning[a].save();
      sum_large += power;
      sum_cost += cost;
    }
  }
}

void Building_Appliances::localNegotiationLargeLearning() {
  std::list<int> pop = Utility::randomIntList(largeLearning.size());
  for (int a : pop) {
    int appid = largeLearning[a].getID();
    contract c = app_negotiation.getContract(buildingID, appid);
    largeLearning[a].setGlobal(sendContractGlobal(c));
    if (!largeLearning[a].isGlobal()) {
      double power = c.recieved;
      double cost = c.recievedCost;
      largeLearning[a].addToCost(cost);
      largeLearning[a].setRecieved(power);
      largeLearning[a].setRecievedCost(cost);
      largeLearning[a].save();
      sum_large += power;
      sum_cost += cost;
    }
  }
}

void Building_Appliances::globalNegotiationPV(
                                const LVN_Negotiation & building_negotiation) {
  std::list<int> pop = Utility::randomIntList(pv.size());
  for (int a : pop) {
    if (pv[a].isGlobal()) {
    int appid = pv[a].getID();
    contract c = building_negotiation.getContract(buildingID, appid);
    double power = c.recieved;
    double cost = c.recievedCost;
    pv[a].setRecieved(power);
    pv[a].setRecievedCost(cost);
    pv[a].save();
    }
  }
}

void Building_Appliances::localNegotiationPV() {
  std::list<int> pop = Utility::randomIntList(pv.size());
  for (int a : pop) {
    int appid = pv[a].getID();
    contract c = app_negotiation.getContract(buildingID, appid);
    pv[a].setGlobal(sendContractGlobal(c));
    if (!pv[a].isGlobal()) {
      double power = c.recieved;
      double cost = c.recievedCost;
      pv[a].setRecieved(power);
      pv[a].setRecievedCost(cost);
      pv[a].save();
    }
  }
}

void Building_Appliances::globalNegotiationFMI(
                                const LVN_Negotiation & building_negotiation) {
  std::list<int> pop = Utility::randomIntList(fmi.size());
  for (int a : pop) {
    if (fmi[a].isGlobal()) {
    int appid = fmi[a].getID();
    contract c = building_negotiation.getContract(buildingID, appid);
    double power = c.recieved;
    double cost = c.recievedCost;
    fmi[a].setRecieved(power);
    fmi[a].setRecievedCost(cost);
    fmi[a].save();
    sum_fmi += power;
    sum_cost += cost;
    }
  }
}

void Building_Appliances::localNegotiationFMI() {
  std::list<int> pop = Utility::randomIntList(fmi.size());
  for (int a : pop) {
    int appid = fmi[a].getID();
    contract c = app_negotiation.getContract(buildingID, appid);
    fmi[a].setGlobal(sendContractGlobal(c));
    if (!fmi[a].isGlobal()) {
      double power = c.recieved;
      double cost = c.recievedCost;
      fmi[a].setRecieved(power);
      fmi[a].setRecievedCost(cost);
      fmi[a].save();
      sum_fmi += power;
      sum_cost += cost;
    }
  }
}

void Building_Appliances::stepGlobalNegotiation(const LVN_Negotiation & building_negotiation) {

  globalNegotiationLarge(building_negotiation);
  globalNegotiationLargeLearning(building_negotiation);
  globalNegotiationSmall(building_negotiation);
  globalNegotiationFMI(building_negotiation);
  globalNegotiationPV(building_negotiation);
  globalContracts.clear();
  double totalPowerRecieved = sum_small + sum_large + sum_fmi;
  DataStore::addValue(buildingString + "_Sum_Small", sum_small);
  DataStore::addValue(buildingString + "_Sum_Large", sum_large);
  DataStore::addValue(buildingString + "_Sum_fmi", sum_fmi);
  DataStore::addValue(buildingString + "_Sum_Recieved", totalPowerRecieved);
  DataStore::addValue(buildingString + "_Sum_Cost", sum_cost);

  totalPower = PowerRequested - PowerGenerated;
  currentStates.clear();
  app_negotiation.clear();
}

void Building_Appliances::postprocess() {
  std::list<int> pop = Utility::randomIntList(largeLearning.size());
  for (int a : pop) {
    largeLearning[a].postprocess();
  }
}

void Building_Appliances::postTimeStep() {
}

double Building_Appliances::getTotalPower() const {
  return totalPower;
}

void Building_Appliances::addCurrentStates(const int stateid) {
    currentStates.push_back(stateid);
}

void Building_Appliances::addContactsTo(
                                      LVN_Negotiation * building_negotiation) {
  for (const contract & c : globalContracts) {
    building_negotiation->submit(c);
  }
}
