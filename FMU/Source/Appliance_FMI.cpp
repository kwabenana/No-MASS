// Copyright 2016 Jacob Chapman

#include <string>
#include "DataStore.h"
#include "Appliance_FMI.h"

Appliance_FMI::Appliance_FMI() {
}

/**
 * @brief Set the variable name of the variable we want to retrieve at run time
 */
void Appliance_FMI::setFMIVariableName(const std::string & FMIVariableName) {
  this->FMIVariableName = FMIVariableName;
}

/**
 * @brief retrieves the value from the given variable name in the DataStore
 * @details At each simulation timestep with FMI all input variables are written
 * to the data store, this simply returns the variable
 */
void Appliance_FMI::step() {
  double p = DataStore::getValue(FMIVariableName);
  power.push_back(p);
  supply.push_back(0.0);
  supplyCost.push_back(0.0);
}
