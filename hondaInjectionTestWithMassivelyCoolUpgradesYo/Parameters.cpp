#include "Arduino.h"
#include "Parameters.h"

#include <math.h>

//Calculation-Based Functions

void Parameters::calcSensors() {
  calcMAP();
  calcO2();
  calcECT();
  calcIAT();
  calcTPS();
}

double Parameters::calcMAP() {
  double map = (18.8636364*analogRead(MAP_Pin)*voltageConversion + 20)*1000;
  if (map >= MAP_MAX) {
    Serial.println("Bad value on MAP");
    return -1;
  }
  MAP = map;
  return map; //Output in Pa
}

double Parameters::calcO2() {
  O2 = analogRead(OIN_Pin)*voltageConversion;
  return O2;
}

/*double Parameters::calcTemp(int pin) {
  double temp = (analogRead(pin)*voltageConversion*(slope)+intercept) + 273;
  if (temp <= TEMP_MIN) {
    Serial.print("Bad value on ");
    if (pin == ECT_Pin) {
      Serial.print("ECT");
    }
    else if (pin == IAT_Pin) {
      Serial.print("IAT");
    }
    else {
      Serial.print("Pin input");
    }
    Serial.print("\n");
    return -1;
  }
  if (pin == ECT_Pin) {
    ECT = temp;
  }
  else if (pin == IAT_Pin) {
    IAT = temp;
  }
  return temp;
}*/

double Parameters::calcECT() {
  double temp = (analogRead(ECT_Pin)*voltageConversion*(slope)+intercept) + 273;
  if (temp <= TEMP_MIN) {
    Serial.println("ECT Too Low");
    //TODO: Initiate Restart Procedure
    return -1;
  }
  ECT = temp;
  return temp;
}

double Parameters::calcIAT() {
  double temp = (analogRead(IAT_Pin)*voltageConversion*(slope)+intercept) + 273;
  if (temp <= TEMP_MIN) {
    Serial.println("IAT Too Low");
    //TODO: Initiate Restart Procedure
    return -1;
  }
  IAT = temp;
  return temp;
}

double Parameters::calcTPS() { //gets throttle position based off of the percentage of throttle area open
  double angle = sin((throttlePositionConversion * (analogRead(TPS_Pin)*voltageConversion -.84)));
  if (angle < 0) {
    Serial.println("Bad value on TPS");
    return -1;
  }
  TPS = angle;
  return angle;
}

int Parameters::calcRPM() {
  //60000 = millisecs per minute
  RPM = (60000*revsSinceLastRPMCalculation) /
        (double)(millis() - lastRPMReadTime);
  return RPM;
}


//Setters

void Parameters::incrementRevs() {
  revsSinceLastRPMCalculation++;
  totalRevs++;
}

void Parameters::setInjectorEnabled() {
  lastInjectorEnableTime = millis();
}

void Parameters::setInjectionDelay(int delay) {
  if (delay < 0) {
    Serial.println("Bad injection delay time");
    return;
  }
  delayBeforeInjection = delay;
}

void Parameters::setActualTimePulsed(long time) {
  if (time < 0) {
    Serial.println("Bad injection pulse time");
    return;
  }
  actualTimePulsed = time;
}

void Parameters::addPulseTimeToRPMRange(int RPM) {
  totalPulseTimeInRPMRange[RPM / RPMIncrements] += actualTimePulsed;
}

void Parameters::setPulseTimeAtRPMIndex(int index, int time) {
  totalPulseTimeInRPMRange[index] = time;
}

void Parameters::setInterrupted() {
  lastInterruptTime = millis();
}

void Parameters::setThrottleMultiplier(double mult) {
  if (mult < 0) {
    Serial.println("Bad throttle multiplier");
    return;
  }
  throttleMultiplier = mult;
}

void Parameters::setFuelRatio(double ratio) {
  if (ratio < 0) {
    Serial.println("Bad fuel ratio");
    return;
  }
  fuelRatio = ratio;
}

void Parameters::setTheoreticalPulseTime(long time) {
  if (time < 0) {
    Serial.println("Bad theoretical pulse time");
    return;
  }
  theoreticalPulseTime = time;
}

void Parameters::setIdleMultiplier(double mult) {
  if (mult < 0) {
    Serial.println("You're a bad person");
    return;
  }
  idleMultiplier = mult;
}

void Parameters::setStartupMultiplier(double mult) {
  if (mult < 0) {
    Serial.println("You're a bad person");
    return;
  }
  startupMultiplier = mult;
}

void Parameters::setRPMCalculated() {
  revsSinceLastRPMCalculation = 0;
}

void Parameters::setIdle() {
  RPM = 0;
  startupMultiplier = resetMultiplier;
  totalRevs = 0;
  totalPulseTime = 0;
  for (int x = 0; x < RPMRangeTableSize; x++) {
    totalPulseTime + totalPulseTimeInRPMRange[x];
  }
}

void Parameters::setSerialOutputted() {
  lastSerialOutputTime = millis();
}

//Getters
double Parameters::getFuelRatioForRPM(int RPM) {
  int index = RPM / RPMIncrements;
  if (index > RPMRangeTableSize) {
    index = RPMRangeTableSize - 1;
  }
  return fuelRatioTable[index];
}
