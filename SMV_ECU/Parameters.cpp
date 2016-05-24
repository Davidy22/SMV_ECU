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
  //TODO: Figure out why this is triggering >=MAP_MAX
  if (map >= MAP_MAX || map <= 0) {
    //Serial.println("Bad value on MAP");
    return MAP;
    restart();
  }
  MAP = map;
  return map; //Output in Pa
}

double Parameters::calcO2() {
  double O2V = analogRead(OIN_Pin)*voltageConversion;
  if (O2V < 0) {
    //Serial.println("Bad O2 value");
    return O2;
    restart();
  }
  O2 = O2V;
  return O2V;
}

double Parameters::calcECT() {
  double temp = (analogRead(ECT_Pin)*voltageConversion*(slope)+intercept) + 273;
  if (temp <= TEMP_MIN) {
    //Serial.println("ECT Too Low");
    return ECT;
    restart();
  }
  ECT = temp;
  return temp;
}

double Parameters::calcIAT() {
  double temp = (analogRead(IAT_Pin)*voltageConversion*(slope)+intercept) + 273;
  if (temp <= TEMP_MIN) {
    //Serial.println("IAT Too Low");
    return IAT;
    restart();
  }
  IAT = temp;
  return temp;
}

double Parameters::calcTPS() { //gets throttle position based off of the percentage of throttle area open
  double angle = sin((throttlePositionConversion * (analogRead(TPS_Pin)*voltageConversion -.84)));
  if (angle < 0) {
    //Serial.println("Bad value on TPS");
    return TPS;
    restart();
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
  int index = RPM / RPM_INCREMENTS;
  if (index > TABLE_SIZE - 1)
  {
    index = TABLE_SIZE - 1;
  }
  totalPulseTimeInRPMRange[index] += actualTimePulsed;
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

void Parameters::setDesiredRPM(int rpm) {
  desiredRPM = rpm;
}

void Parameters::setDesiredO2(double O2V) {
  desiredO2 = O2V;
}

void Parameters::setFuelTableValue(int index, double value) {
  fuelRatioTable[index] = value;
}

//Getters
double Parameters::getFuelRatioForRPM(int RPM) {
  int index = RPM / RPM_INCREMENTS;
  if (index > RPMRangeTableSize) {
    index = RPMRangeTableSize - 1;
  }
  return fuelRatioTable[index];
}

void restart () {
  return //used temporarily for testing
  detachInterrupt(2);
  asm volatile (" jmp 0");
}
