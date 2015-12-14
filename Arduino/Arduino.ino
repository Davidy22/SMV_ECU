#include <Wire.h>
#include <LiquidCrystal.h>
#include <math.h>
int incoming = 0;

LiquidCrystal lcd(7, 6, 5, 4, 3, 2);
int HES_Pin = 2;  // Hall effect sensor
int PMP_Pin = 48; // Fuel pump
int TPS_Pin = 3;  // Throttle position sensor
int INJ_Pin = 32; // Fuel injector
int IAT_Pin = 4;  // Intake air temperature
int ECT_Pin = 5;  // Engine temperature sensor
int MAP_Pin = 11; // Manifold air pressure
int OIN_Pin = 1;  // Oxygen sensor input
int OHE_Pin = 36; // Oxygen heater, digital
int BUP_Pin = 9;  // Button one
int BDN_Pin = 13; // Button two

int ECT_Val = 0;
int TPS_Val = 0;
int IAT_Val = 0;
int MAP_Val = 0;
int OIN_Val = 0;
int BUP_Val = 0;
int BDN_Val = 0;

int RPM = 0;

// Boundary values
const int TPS_min = 100;  // Test should be added to check if TPS starts at > 140, or it's clogged open.
const int TPS_max = 927;
const int IAT_min = 0;    // temp value to be changed later
const int IAT_max = 1000;  // temp value to be changed later
const int ECT_min = 50;     // temp value to be changed later
const int ECT_max = 1000;   // temp value to be changed later
boolean err = false;

unsigned long time;          // |
unsigned long lastTime;      // | Used for rotation-per-minute calculations

double logResult;
int temperatureCalculator;

int INJ_Delay = 32; // Tracking variables for injector delay
int INJ_LastSampled = 0; // Time at which this was last sampled.
boolean INJ_on = false;
boolean INIT_on = false;
//Results:
// Off time: TPS value
// 100: 200
// 95: 

//*******************************************************************//
int INJ_OnDelay = 5; // May be modified during run-time
int INJ_OffDelay = 160; // 115 when hot
int INJ_MaxOffDelay = 160;
int INJ_MinOffDelay = 30;
int Des_RPM = 1900; //Desired RPM
int Last_RPM = 0;
double k_p = .002;
double k_d = .06;
int der = 1;
double error = 0; // Changed these to change the frequency that these are cycled.
//*******************************************************************//

const int sampleSize = 50;
int RPMSample[sampleSize];
long avg_RPM = 0;
int numSamples = 0;

unsigned long output;

void toggleOutput(const int Pin, int& Delay, int OnDelay, int OffDelay, 
                  boolean& on, int& LastSampled) {
     // Toggles a pin every increment of time denoted by OffDelay and OnDelay
  Delay += millis() - LastSampled;

  LastSampled = millis();
  if (Delay >= OffDelay) {
    Delay = 0;
    digitalWrite(Pin, HIGH);
    delay(OnDelay);
    digitalWrite(Pin, LOW);
  }
}

void rpm() {
    RPM = 60000/(millis() - lastTime);
    lastTime = millis();
}

double getThrottlePosition() { //gets throttle position based off of the percentage of throttle area open
  double angle = (throttlePositionConversion * (analogRead(TPS_Pin)*voltageConversion -.84));
  return sin(angle);
}

double getTemp(int Pin) {
  return analogRead(Pin)*voltageConversion*(-19.451)+89.135;
}

//void buttonUp() {
//  INJ_OnDelay++;
//}

//void buttonDown() {
//  INJ_OnDelay--;
//}


void setup () {
  lcd.begin(16, 2);    // Initialization.
  lcd.setCursor(0,0);
  lcd.display();
  TPS_Val = analogRead(TPS_Pin);
  ECT_Val = analogRead(ECT_Pin);
  MAP_Val = analogRead(MAP_Pin);
  IAT_Val = analogRead(IAT_Pin);
  OIN_Val = analogRead(OIN_Pin); // Write error checking for this one
  lastTime = millis();
  attachInterrupt(HES_Pin, rpm, FALLING);
  
    // Error Checking
  if (ECT_Val < ECT_min || ECT_Val > ECT_max)
  {
    lcd.print("ECT Err");
    err = true;
  }
  lcd.setCursor(0,0);  // move cursor to print next message if needed
  
  if (TPS_Val < TPS_min || TPS_Val > TPS_max)
  {
    lcd.print("TPS");
    lcd.print(TPS_Val);
    err = true;
  }
  if (TPS_Val > 250) {
    lcd.print("TPS jam");
    lcd.print(TPS_Val);
    err = true;
  }
  
  lcd.setCursor(0,1);
  if (IAT_Val < IAT_min || IAT_Val > IAT_max)
  {
    lcd.print("IAT Err");
    err = true;
  }
  
  // Finish error checking
  
  if (!err) {
    lcd.print("Ready 2 KickAzz!");
  }
  
  Serial.print("All systems Check!");
  
  //Initialise average RPM calculation array.
  for (int i = 0; i < 100; i++) {
    RPMSample[i] = 0;
  }
  
//  pinMode(fuelPumpPin, OUTPUT);   //Pre-pressurise the fuel
  pinMode(INJ_Pin, OUTPUT);
  digitalWrite(INJ_Pin, LOW);
  pinMode(PMP_Pin, OUTPUT);
  
  digitalWrite(PMP_Pin, HIGH);
  delay(1000);
  digitalWrite(PMP_Pin, LOW);
  
  Serial.begin(9600);
  
}

void loop () {
//  digitalWrite(PMP_Pin, HIGH);
if (millis() - lastTime > 450) {
    RPM = 0;
  }
  
  // RPM averaging code
  if (RPM > 1000 && numSamples < sampleSize){
    RPMSample[numSamples] = RPM;
    numSamples++;
  }
  
//  if( numSamples == sampleSize - 1)
//  {
//    for(int i = 0; i < sampleSize ; i++){
//      avg_RPM += RPMSample[i];
//    }
//    avg_RPM = avg_RPM/sampleSize;
//    numSamples = 0;
//  }
  
  //Temperature calculation  
//  ECT_Val = analogRead(ECT_Pin);
//  logResult = log(ECT_Val*2.75942);
//  temperatureCalculator = (1.0 / (.002606445974 + .000000944070347*logResult + .000001722147429*pow(logResult, 3)) - 273.15)*1.8 + 32;
  
  // Turn on the system if flywheel is turning.
  lcd.setCursor(13, 0);
  if (RPM != 0) {
    digitalWrite(PMP_Pin, HIGH);
    INIT_on = true;
    lcd.write("ON ");
    //toggleOutput(PMP_Pin, INIT_Delay, INIT_MaxDelay, INIT_on, INIT_LastSampled);
  } else {
    INIT_on = false;
    digitalWrite(PMP_Pin, LOW);
    lcd.write("OFF");
  }
  
  // End engine RPM calculations

  TPS_Val = getThrottlePosition()*1024;
  INJ_OffDelay = INJ_MaxOffDelay - (TPS_Val - 190)/2;
  Des_RPM = 5000 + (TPS_Val - 190)*5;
  
  if(Des_RPM >= 12000){
     Des_RPM = 12000;
  }else if(Des_RPM <= 5000){
     Des_RPM = 5000; 
  }  
  
  //INJ_OffDelay += (avg_RPM - Des_RPM)*k_p;
  
  if(INJ_OffDelay >= INJ_MaxOffDelay){
     INJ_OffDelay = INJ_MaxOffDelay;
  }else if(INJ_OffDelay <= INJ_MinOffDelay){
     INJ_OffDelay = INJ_MinOffDelay; 
  }
  

  // Injector delay variation code below. Change this, it's insane.
  if( RPM != 0){
    der = RPM - Last_RPM;
    Last_RPM = RPM;
    error = (RPM - Des_RPM)*k_p + der*k_d;
    INJ_OffDelay = INJ_OffDelay + error;
    //lcd.print(INJ_OffDelay);
    if(INJ_OffDelay >= INJ_MaxOffDelay){
       INJ_OffDelay = INJ_MaxOffDelay;
    }else if(INJ_OffDelay <= INJ_MinOffDelay){
       INJ_OffDelay = INJ_MinOffDelay; 
    }
  }


  if (INIT_on) {
    toggleOutput(INJ_Pin, INJ_Delay, INJ_OnDelay, INJ_OffDelay, INJ_on, INJ_LastSampled);
  } else {
    digitalWrite(INJ_Pin, LOW);
    INJ_on = false;
  }
  
  // Begin debug code
  if (millis()-lastTime>=500) {
    lcd.setCursor(0, 0);
    lcd.print("VAL ");
    lcd.print(INJ_OnDelay);
    lcd.print(" ");
    lcd.print(INJ_OffDelay);
    lcd.print("     ");
    
    lcd.setCursor(0, 1);
    lcd.print("PIN ");
    lcd.print(Des_RPM); //used to be ECT_Val
    lcd.print("      ");
    lcd.setCursor(10, 1);
    lcd.print(avg_RPM);
    lcd.print("      ");
    
    // Serial output
    Serial.print("#S|TEXT|[ECT: ");
    Serial.print(getTemp(ECT_Pin));
    Serial.print(" IAT: ");
    Serial.print(getTemp(IAT_Pin));
    Serial.print(" MAP: ");
    Serial.print(analogRead(MAP_Pin)*voltageConversion);
    Serial.print(" HES: ");
    Serial.print(analogRead(HES_Pin)*voltageConversion);
    Serial.print(" TPS: ");
    Serial.print(getThrottlePosition());
    Serial.println("]#");
    
    Serial.print("#S|EXCEL|[");
    Serial.print(getTemp(ECT_Pin));
    Serial.print(",");
    Serial.print(getTemp(IAT_Pin));
    Serial.print(",");
    Serial.print(analogRead(MAP_Pin)*voltageConversion);
    Serial.print(",");
    Serial.print(analogRead(HES_Pin)*voltageConversion);
    Serial.print(",");
    Serial.print(getThrottlePosition());
    Serial.println("]#");
    lastTime = millis();
  }
  // End debug code
}
