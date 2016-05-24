#ifndef CONST_H
#define CONST_H

const int TPS_Pin = 2;
const int IAT_Pin = 14;
const int ECT_Pin = 1;
const int MAP_Pin = 15;
const int OIN_Pin = 3;
const int INJ_Pin = 50;
const int PUMP_Pin = 13;
//const int HEAT_Pin = 52;

const double TPS_MOD = 1;
const double slope = -19.451;
const double intercept = 89.135;
const double voltageConversion = .0049;
const double throttlePositionConversion = 0.401738191;
const double injectionConstant = .000172064;  //volume
const double injectorFuelRate = 2.1333333333; //grams per second

const int TEMP_MIN = 273;
const double TPS_MIN = 0;
const double TPS_MAX_START = 0.1;
const double TPS_MAX_RUNNING = 1.0;
const int MAP_MAX = 150000;
const int RPM_MAX = 7999;
const double IDLE_TPS_MAX = 0.10;

const int timeout = 60000;

const int RPM_INCREMENTS = 250;
const int REVS_PER_RECALCULATION = 30;
const int TABLE_SIZE = 32;
#endif
