#include "Arduino.h"
#include "Controller.h"

#include "String.h"
#include "Parameters.h"

bool Controller::getCommand() {
  if (Serial.available() == 9) {
    String command = Serial.readString();
    String identifier = command.substring(0,5);
    String value = command.substring(5);
    if (identifier == "idle ") {
      double multiplier = value.toFloat();
      params->setIdleMultiplier(multiplier);
    }
    else if (identifier == "fuel ") {
      double fuelRatio = value.toFloat();
      params->setFuelRatio(fuelRatio);
    }
    else if (identifier == "dRPM ") {
      int desRPM = value.toInt();
      params->setDesiredRPM(desRPM);
    }
    else if (identifier == "deO2 ") {
      double desO2 = value.toFloat();
      params->setDesiredO2(desO2);
    }
    else if (identifier.substring(0,2) == "ar ") {
      int index = identifier.substring(2,2).toInt();
      double ratio = value.toFloat();
      params->setFuelTableValue(index, ratio);
    }
    else {
      return false;
    }
    return true;
  }
  return false;
}
