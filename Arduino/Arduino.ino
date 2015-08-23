#include <Wire.h>
#include <LiquidCrystal.h>
#include <math.h>
int incoming = 0;

LiquidCrystal lcd(7, 6, 5, 4, 3, 2);
int risingEdgePin = 2;     // Change according to actual pin numbers
// int fuelPumpPin = 52;
int onSwitchPin = 48;
int TPS_Pin = 3;  // Throttle position sensor
int INJ_Pin = 32; // Fuel injector
int IAT_Pin = 4;  // Intake air temperature
int ECT_Pin = 5;  // Engine temperature sensor
int MAP_Pin = 11; // Manifold air pressure
int OIN_Pin = 1;  // Oxygen sensor input
int OHE_Pin = 36; // Oxygen heater, digital
int BUP_Pin = 9;  //Buttons
int BDN_Pin = 13;
int ECT_Val = 0;
int TPS_Val = 0;
int IAT_Val = 0;    // added recently
int MAP_Val = 0;
int OIN_Val = 0;
int BUP_Val = 0;
int BDN_Val = 0;
int RPM = 0;
const int TPS_min = 100;  // Test should be added to check if TPS starts at > 140, or it's clogged open.
const int TPS_max = 927;
const int IAT_min = 0;    // temp value to be changed later
const int IAT_max = 1000;  // temp value to be changed later
const int ECT_min = 50;     // temp value to be changed later
const int ECT_max = 1000;   // temp value to be changed later
boolean err = false;

unsigned long time;          // |
unsigned long lastTime;      // | Used for rotation-per-minute calculations
int lastInput;
int currentInput;
int halfRevolutions = 0;

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
int RPMSamplePoint = 0;
long avg_RPM = 0;
int numSamples = 0;
long delta;

int zeros = 0;

int Up = 0;
int Dn = 0;

int temp;
//
//// Number of relevant intervals in each parameter. Current numbers are wild guesses.
//const int LOADVALUES = 10;
//const int RPMVALUES = 5;
//const int TEMPVALUES = 10;
//int loadIndex;
//int RPMIndex;
//int tempIndex;
//// Arrays of delay timings for fuel injection.
//int onDelayArray[LOADVALUES][RPMVALUES][TEMPVALUES];
//int offDelayArray[LOADVALUES][RPMVALUES][TEMPVALUES];

//max off delay = 115
//min off delay = 60
void toggleOutput(const int Pin, int& Delay, int OnDelay, int OffDelay, 
                  boolean& on, int& LastSampled) {
     // Toggles a pin every increment of time denoted by OffDelay and OnDelay
  //detachInterrupt(2);
  Delay += millis() - LastSampled;

  LastSampled = millis();
  //detachInterrupt(2);
  if (Delay >= OffDelay) {
    Delay = 0;
    digitalWrite(Pin, HIGH);
    delay(OnDelay);
    digitalWrite(Pin, LOW);
  }
  //attachInterrupt(2, rpm, FALLING);
}

void rpm() {
//  currentInput = analogRead(risingEdgePin); //Redundant now that we understand how to interrupt
//  if (lastInput - currentInput > 400) {
//    halfRevolutions++;
//  }
//  lastInput = currentInput;
  
    // Engine RPM calculations
  halfRevolutions++;
  if (halfRevolutions >= 2) {
    RPM = 120000/(millis() - lastTime);
    halfRevolutions = 0;
    lastTime = millis();
  }
}

//void buttonUp() {
//  INJ_OnDelay++;
//}
//
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
  OIN_Val = analogRead(OIN_Pin); // Write error checking
  lastTime = millis();
  attachInterrupt(2, rpm, FALLING);
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
  //delay(5000);
  
  //Initialise average RPM calculation array.
  for (int i = 0; i < 100; i++) {
    RPMSample[i] = 0;
  }
  
//  pinMode(fuelPumpPin, OUTPUT);   //Pre-pressurise the fuel
  pinMode(INJ_Pin, OUTPUT);
  digitalWrite(INJ_Pin, LOW);
  pinMode(onSwitchPin, OUTPUT);
  
  digitalWrite(onSwitchPin, HIGH);
  delay(1000);
  digitalWrite(onSwitchPin, LOW);
  
  Serial.begin(9600);
  
}

void loop () {
//  digitalWrite(onSwitchPin, HIGH);
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
  ECT_Val = analogRead(ECT_Pin);
  logResult = log(ECT_Val*2.75942);
  temperatureCalculator = (1.0 / (.002606445974 + .000000944070347*logResult + .000001722147429*pow(logResult, 3)) - 273.15)*1.8 + 32;
  
  // Turn on the system if flywheel is turning.
  lcd.setCursor(13, 0);
  if (RPM != 0) {
    digitalWrite(onSwitchPin, HIGH);
    INIT_on = true;
    lcd.write("ON ");
    //toggleOutput(onSwitchPin, INIT_Delay, INIT_MaxDelay, INIT_on, INIT_LastSampled);
  } else {
    INIT_on = false;
    digitalWrite(onSwitchPin, LOW);
    lcd.write("OFF");
  }
  
  // End engine RPM calculations
  
  // Begin debug code
  lcd.setCursor(0, 0);
  lcd.print("VAL ");
  lcd.print(INJ_OnDelay);
  lcd.print(" ");
  lcd.print(INJ_OffDelay);
  lcd.print("     ");

//  OIN_Val = analogRead(OIN_Pin);
  temp = digitalRead(21);
  TPS_Val = analogRead(TPS_Pin);
  lcd.setCursor(0, 1);
  lcd.print("PIN ");
  lcd.print(Des_RPM); //used to be ECT_Val
  lcd.print("      ");
  lcd.setCursor(10, 1);
  lcd.print(avg_RPM);
  lcd.print("      ");
  // End debug code

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


//  // Tabulated delay variation code. Requires testing and known values
//  MAP_Val = analogRead(MAP_Pin);
//  loadIndex = 0; // come up with actual formulae later
//  RPMIndex = 0;
//  tempIndex = 0;
//  // number manipulation to change readings into index numbers
//  INJ_OnDelay = onDelayArray[loadIndex][RPMIndex][tempIndex];
//  INJ_OffDelay = onDelayArray[loadIndex][RPMIndex][tempIndex];
  
  //Button controlled delay variation
//  BUP_Val = analogRead(BUP_Pin);
//  BDN_Val = analogRead(BDN_Pin);
//  if (Up != 1) {
//    if (BUP_Val == 0) {
//      INJ_OnDelay++;
//      Up = 1;
//    }
//  } else {
//    if (BUP_Val != 0) {
//      Up = 0;
//    }
//  }
//  if (Dn != 1) {
//    if (BDN_Val == 0) {
//      INJ_OnDelay--;
//      Dn = 1;
//    }
//  } else {
//    if (BDN_Val != 0) {
//      Dn = 0;
//    }
//  }

  if (INIT_on) {
    toggleOutput(INJ_Pin, INJ_Delay, INJ_OnDelay, INJ_OffDelay, INJ_on, INJ_LastSampled);
  } else {
    digitalWrite(INJ_Pin, LOW);
    INJ_on = false;
  }  
}
