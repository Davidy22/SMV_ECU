#ifndef PARAMETERS_H
#define PARAMETERS_H
#include "Constants.h"
class Parameters {
public:

  Parameters(int increment) {
    RPMRangeTableSize = (RPM_MAX/increment) + 1;
    totalPulseTimeInRPMRange = new int[RPMRangeTableSize];
  }
  ~Parameters() {
    delete [] totalPulseTimeInRPMRange;
  }

  //Calculation-Based Functions
  double calcMAP();
  double calcO2 ();
  double calcECT();
  double calcIAT();
  double calcTPS();
  int calcRPM();
  void calcSensors();

  //Getters
  unsigned long getTotalRevs() { return totalRevs; }
  long getTheoreticalPulseTime() { return theoreticalPulseTime; }
  int getInjectionDelay() { return delayBeforeInjection; }
  long getLastInjectorEnableTime() { return lastInjectorEnableTime; }
  int getRPM() { return RPM; }
  double getO2() { return O2; }
  double getMAP() { return MAP; }
  double getECT() { return ECT; }
  double getIAT() { return IAT; }
  double getTPS() { return TPS; }
  int getTableSize() { return RPMRangeTableSize; }
  int getDesiredRPM() { return desiredRPM; }
  double getDesiredO2() { return desiredO2; }
  double getFuelRatioForRPM(int RPM);
  double getFuelRatio() { return fuelRatio; }
  double getStartupMultiplier() { return startupMultiplier; }
  double getIdleMultiplier() { return idleMultiplier; }
  double getThrottleMultiplier() { return throttleMultiplier; }
  int getRevsSinceLastCalculation() { return revsSinceLastRPMCalculation; }
  long getLastRPMReadTime() { return lastRPMReadTime; }
  unsigned long getTotalPulseTime() { return totalPulseTime; }
  long getActualPulseTime() { return actualTimePulsed; }
  long getLastSerialOutputTime() { return lastSerialOutputTime; }

  //Setters
  void incrementRevs();
  void setInjectorEnabled (); //Sets last injector enable time to current time
  void setInjectionDelay(int delay);
  void setActualTimePulsed(long time);
  void addPulseTimeToRPMRange(int index);
  void setPulseTimeAtRPMIndex(int index, int time);
  void setInterrupted(); //Sets last interrupt time to current time
  void setThrottleMultiplier(double mult);
  void setFuelRatio(double ratio);
  void setTheoreticalPulseTime(long time);
  void setIdleMultiplier(double mult);
  void setStartupMultiplier(double mult);
  void setRPMCalculated(); //Resets revs since calculation to 0
  void setIdle();
  void setSerialOutputted();

private:

  //Revolution Counters
  int revsSinceLastRPMCalculation = 0; //revolutions
  unsigned long totalRevs = 0;         //revolutions2

  //RPM Counters
  long lastRPMReadTime = 0;            //lastTimeRPM

  //Time Counters
  long lastSerialOutputTime = 0;       //lastTime
  long lastInjectorEnableTime = 0;     //lastTime2
  long actualTimePulsed = 0;           //pulseTimeReal
  long lastInterruptTime;              //lastInterrupt
  int delayBeforeInjection = 0;        //delayCount

  //Feedback Loop Control
  int desiredRPM = 3000;
  double desiredO2 = 0.5;

  //Pulse Width Counters
  long theoreticalPulseTime;           //pulseTime
  int* totalPulseTimeInRPMRange;       //totalPulse
  int RPMRangeTableSize;               //arraySize
  unsigned long totalPulseTime = 0;    //sumPulse

  //Pulse Adjustment Values
  double fuelRatio = 15.5;
  double idleMultiplier = 1.2;         //idleVal
  double resetMultiplier = 1.2;        //resetVal
  double startupMultiplier = 2;        //startupVal
  double throttleMultiplier = 0;       //TPSx
  //Multipliers adjust the fuel ratio based on certain conditions

  //Sensor Readings
  int RPM = 0;
  double O2 = 0;                       //O2V
  double MAP = 0;
  double ECT = 0;
  double IAT = 0;
  double TPS = 0;                      //lastTPS

  //RPM TABLE
  double fuelRatioTable [32] =
   //                  0000-0249 0250-0499 0500-0749 0750-0999
                    {    14.7  ,   14.7  ,   14.7  ,   14.7,  //0000 - 0999
                         14.7  ,   14.7  ,   14.7  ,   14.7,  //1000 - 1999
                         14.7  ,   14.7  ,   14.7  ,   14.7,  //2000 - 2999
                         14.7  ,   14.7  ,   14.7  ,   14.7,  //3000 - 3999
                         14.7  ,   14.7  ,   14.7  ,   14.7,  //4000 - 4999
                         14.7  ,   14.7  ,   14.7  ,   14.7,  //5000 - 5999
                         14.7  ,   14.7  ,   14.7  ,   14.7,  //6000 - 6999
                         14.7  ,   14.7  ,   14.7  ,   14.7 };//7000 - 7999
};

void restart();

#endif
