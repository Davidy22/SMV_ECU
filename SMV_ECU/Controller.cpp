#include "Controller.h"

#include <string>

bool Controller::getCommand() {
  if (Serial.available() == 9) {
    std::string command = Serial.readString();
    std::string identifier = command.substr(0,5);
    std::string value = command.substr(5);
    if (identifier == "idle ") {
      double multiplier = std::stod(value);
      params->setIdleMultiplier(multiplier);
    }
    else if (identifier == "fuel ") {
      double fuelRatio = std::stod(value);
      params->setFuelRatio(fuelRatio);
    }
    else if (identifier == "dRPM ") {
      int desRPM = std::stoi(value);
      params->setDesiredRPM(desRPM);
    }
    else if (identifier == "deO2 ") {
      double desO2 = std::stod(value);
      params->setDesiredO2(desO2);
    }
    else if (identifier.substr(0,2) == "ar ") {
      int index = std::stoi(identifier.substr(2,2));
      double ratio = std::stod(value);
      params->setFuelTableValue(index, ratio);
    }
  }
  return false;
}
