#include "Constants.h"
#include "Controller.h"
#include "Parameters.h"
#include "TimerThree.h"
#include "math.h"

Parameters p = Parameters(RPMIncrements);
Controller c;

void serialOutput();

void countRevolution() {
  //Serial.println("Revolution counted");
  p.incrementRevs();
  if (p.getTotalRevs() % 2 == 1 || p.getTotalRevs() < 25) {
    digitalWrite(INJ_Pin, HIGH);
    p.setInjectorEnabled();
    p.setInjectionDelay(0);
    Timer3.setPeriod(p.getTheoreticalPulseTime());
    Timer3.restart();
  }
}

void pulseOff() {
  //ISR for Timer3 interrupt
  if (p.getInjectionDelay() == 1) {
    digitalWrite(INJ_Pin, LOW);
    p.setActualTimePulsed(millis() - p.getLastInjectorEnableTime());
    p.addPulseTimeToRPMRange(p.getRPM());
  }
  p.setInjectionDelay(p.getInjectionDelay() + 1);
}

void inject() {
  // Explanation of the math performed here
  // (displ * rpm)/ 2 = cm^3/s air per power stroke
  // ideal gas law PV = nRT
  // T = IAT
  // P = MAP
  // mult n by mm of air, divide by 14.7
  p.setThrottleMultiplier(1 + TPS_MOD * (abs(p.getTPS() - p.calcTPS())));
  if (p.getTotalRevs() > 2 && p.getTPS() <= IDLE_TPS_MAX) {
    //RPM Based Inject Feedback Loop

    int RPM = p.getRPM();
    int desiredRPM = p.getDesiredRPM();
    if (RPM > desiredRPM) {
      p.setIdleMultiplier(p.getIdleMultiplier() + .0001);
    }
    else if (RPM < desiredRPM){
      p.setIdleMultiplier(p.getIdleMultiplier() - .0001);
    }

    //O2 Based Inject Feedback Loop

    /*double O2 = p.calcO2();
    double desiredO2 = p.getDesiredO2();
    if (O2 > desiredO2) {
      p.setIdleMultiplier(p.getIdleMultiplier() + .0001);
    }
    else if (O2 < desiredO2){
      p.setIdleMultiplier(p.getIdleMultiplier() - .0001);
    }*/
  }
  else {
    p.setFuelRatio(p.getFuelRatioForRPM(p.getRPM()));
  }
  double val = p.getMAP() * injectionConstant /
              (p.getIAT() * p.getFuelRatio() * injectorFuelRate);
  p.setTheoreticalPulseTime(val * 1000000 * p.getIdleMultiplier() *
                            p.getStartupMultiplier() * p.getThrottleMultiplier()
                            + 350);
}

void setup() {
  pinMode(INJ_Pin, OUTPUT);
  Serial.begin(9600);
  Serial.println("Time, ECT,    IAT,    MAP,     TPS,  OIN,  RPM,  DesPW,PWRe,Revs,IV,TPW");
  c.setParameters(&p);
  p.calcTPS();
  attachInterrupt(2, countRevolution, FALLING);
  inject();
  Timer3.initialize(100000);
  Timer3.attachInterrupt(pulseOff, p.getTheoreticalPulseTime());
  p.setInjectionDelay(4);
  p.setActualTimePulsed(0);
  p.setInjectorEnabled();
  p.setInterrupted();
  for (int x = 0; x < p.getTableSize(); x++) {
    p.setPulseTimeAtRPMIndex(x, 0);
  }
}

void loop() {
  p.calcSensors();
  if (millis() == timeout) {
    digitalWrite(INJ_Pin, LOW);
    p.setIdleMultiplier(0);
    detachInterrupt(2);
    while(millis() - timeout < 10000) //Wait 10 seconds for some reason
    {}
  }

  if (p.getRevsSinceLastCalculation() >= REVS_PER_RECALCULATION) {
    p.calcRPM();
    p.setStartupMultiplier(1);
    p.setRPMCalculated();
  }

  inject();
  //Timer3.setPeriod(p.getTheoreticalPulseTime()); //No idea what this is

  //Routine for when the engine is idling
  if (millis() - p.getLastRPMReadTime() >= 1000) {
    p.setIdle();
    Serial.println(p.getTotalPulseTime());
  }

  if (millis() - p.getLastSerialOutputTime() >= 500) {
    serialOutput();
    p.setSerialOutputted();
  }
}

void serialOutput() {
  Serial.print(millis());
  Serial.print(", ");
  Serial.print(p.getECT());
  Serial.print(", ");
  Serial.print(p.getIAT());
  Serial.print(", ");
  Serial.print(p.getMAP());
  Serial.print(", ");
  Serial.print(p.getTPS());
  Serial.print(", ");
  Serial.print(p.getO2());
  Serial.print(", ");
  Serial.print(p.getRPM());
  Serial.print(", ");
  Serial.print(p.getTheoreticalPulseTime());
  Serial.print(", ");
  Serial.print(p.getActualPulseTime());
  Serial.print(", ");
  Serial.print(p.getTotalRevs());
  Serial.print(", ");
  Serial.println(p.getIdleMultiplier());
}
