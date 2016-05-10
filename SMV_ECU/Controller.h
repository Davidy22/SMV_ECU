#ifndef CONTROLLER_H
#define CONTROLLER_H

//#include <queue>

//////////////////////////////////////////////////////////////////////////////
// CONTROLLER CLASS                                                         //
// ======================================================================== //
// Facilitates real time communication between a laptop and the Arduino.    //
// Should be able to take input from the serial port and efficiently        //
// dispatch that input as commands for adjusting parameters in the Arduino. //
// ======================================================================== //
// PARAMETERS TO ADJUST:                                                    //
// ======================================================================== //
// idleVal                                                                  //
// fuelRatio                                                                //
// desiredRPM                                                               //
// desired02                                                                //
// an array of doubles corresponding to our fuel adjustment table           //
//////////////////////////////////////////////////////////////////////////////

/*Syntax:
  idle ____
  fuel ____
  dRPM ____
  deO2 ____
  ar19 ____
*/

const int MAX_QUEUE_SIZE = 100;

// CommandType can be changed if necessary

class Parameters;

class Controller
{
public:

    void setParameters(Parameters* p) {params = p;}

    bool getCommand();

private:
    Parameters* params;
};

#endif // CONTROLLER_H
