/*
	RCMutil.h
	Copyright (C) Randy Miller 2015
  
  Classes included in this include:
    RCMutil       Contains heartbeat LED and pause(mS) plus support methods.
    DataSmooth    Contains n point data smoothing method.

  
	UPDATE HISTORY
  11 Sep 2015 0.00.05 Added Com port storage and retreival in EEPROM  setComPort() and getComPort().
                      Also timeout_mS, a simple timeout() method.
  25 Aug 2015 0.00.04 Added class: DataSmooth.  n point data smoothing (averaging)
                      Also added version to RCMutil to display the version of the Library.
  19 Aug 2015 0.00.03 Changed .begin(pin) so that it sets pinMode for the heartbeat LED pin passed 
  16 Aug 2015         fixed setHeartbitPin(n) so it can be turned off.
	14 Aug 2015 0.00.01 Initial release V0.00.01 
	
	
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.
*/
#ifndef _RCMUTILH_
#define _RCMUTILH_

#if ARDUINO >= 100
 #include "Arduino.h"
 #include "Print.h"
#else
 #include "WProgram.h"
#endif

#include <avr/EEPROM.h>


#define VERSIONRCMUTIL  "RCMutil v0.00.05"

#define OK         0
#define FAIL      -1
#define READ	   -1
#define COM_EE_ADDY 0xC0    // this is the EEPROM address where the Arduino COM port number is stored

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/****************************************************************************************
*
* RCMutil is the class that conteins the heartmbeat led functions: Set pin the led is on
*  set the delay for the heartbeat and attach a user function to the blink routine
*  plus the pause() routine which calls blink while it waits.
*/
class RCMutil
{
  private:
	int heartbeatLEDpin;
	unsigned long heartbeatInterval;
	void (*usrHeartbeatFunc)(void);
	int enaUsrHeartbeat = false;
	
  public:
	
	// constructor  (maybe set pinMode here in future instead of begin ?)
	RCMutil();
	
  // returns the version string
  char * version();
  
  // initalize class  pass it the pin no for the heartbeat led
  void begin(int pin);
  
	/*************************************************************************************/
  // set the time in mS between toggles of the LED
	long setHeartbeatDelay(long);  // send -1 to READ Default is 250mS
	
	/*************************************************************************************/
  // set the digital pin to toggle for the heartbeat LED default is 13
  // returns pin set or FAIL (-1) if invalid pin selected
	int setHeartbeatPin(int);		// valid pins 2 - 13. 0 means no heartbeat, 
  	
	/*************************************************************************************/
  // pass a user defined function to run in blink()
  void setUsrHeartbeatFunc(void(*func)());	
	
	/* ***********************************************************************************
	 *  void pause(ms)
	 *  like delay() but calls blink while waiting for (ms) to elapse.
	 */
	void pause(unsigned long delay);               // pause for (delay milliseconds) like delay but calls _blink()

	/* ***********************************************************************************
	*  void blink()
	*  function checks timer to see if (_blinkInterval has elapsed, if so toggle the 
	*  LEDpin
	*  call this in your loop() to keep the led blinking 
	*/
	void blink(void);                    // toggles led if interval has expired
	
  /* ***********************************************************************************/
	int pinToggle( int pin );            // toggle output of (pin) req: pinMode set to (INPUT | OUTPUT)

  /*************************************************************************************
  * char * getComPort()
  *
  * I find this handy for projects which have a display, so at a glance I know which
  * COM port the Arduino board is operating on.
  * 
  * This method reads the value stored at COM_EE_ADDY (0xC0) 
  * if it is > 2 and less than 0xFF it returns a string of which com port
  * this board uses.  
  *
  * Port Num must be set the first time a new board is used. Plug in the board 
  * determine what port it is. Edit the example-uBlink to set the proper number.
  * after the first run the value is permanently set until the EEPROM dies.
  *
  */
  char * getComPort(void);
  
  /*************************************************************************************
  * void RCMutil::setComPort(int val);
  *
  * This function sets the EEPROM at COM_EE_ADDY (0xC0) to the passed (val)
  * this function only needs to be called once to set the value for each board.
  * 
  */
  void setComPort(int val);

};
///////////////////////////////// RCMutil //////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
/***************************************************************************************
*
* timeout_mS
*
*
*/

class Timeout_mS
{
  private:
  unsigned long endMillisec;         // holder fot the count to expire at.
    
    
  public:
  // constructor 
  Timeout_mS();  
  
  // timeout(unsigned long mS) Set the expiration period of milliseconds
  // call timeout(void)  to determine when the previously set period has expired.
  // returns: false if period has not expired.
  //          true  once the period has expired.
  bool timeout(void);               
  bool timeout(unsigned long mS);   

};
//////////////////////////// timeout_mS ////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////
/***************************************************************************************
*
* smooth is a simple n point smoothing routine
* when it is created the number of points to keep is passed in the constructor
* the smooth method returns the average of the last n calls
*
*   // to use create an instance of DataSmooth() with the number of data points to average.
* DataSmooth ds3 = DataSmooth(3);
*   // Then pass the data you want smoothed it returns the avg of the last n measurments
* Serial.print(ds3.smooth(data));
*
*   // If you have multiple different values you need smoothed you can create multiple 
*   // instances of the object.
* DataSmooth temp3p = DataSmooth(3);
* DataSmooth pres3p = DataSmooth(3);
*
* Serial.print(temp3p.smooth(tempIn));
* Serial.print(pres3p.smooth(presIn));
*
* Notes: be careful of scope.  If you create the instance locally it has local scope, the values
* will be lost on reentry to the function. You may be best off to create your instance globally.

*/
class DataSmooth
{
  public:  // Constructor  this initalizes the internal variables. Pass it the number of points of smmoothing
  // you desire npte range: 2 - 9
  DataSmooth(int npts);    // when instatiated pass the nummmber of points to average between 2 and 10
  
  // This is the smoothing routine it returns the average of the last n points you have sent it.
  // for the first call it returns the passed value for the sucessive calls up to npts it returns the
  // average of the current number of valid measurments.
  float smooth(float val);
  
  
  private:  
  int numPoints;    // number of points to be stored and averaged.
  int curDatPtr;    // pointer to values array current entry
  float values[10]; // array to store passed values in
  int nCall;        // This counts from 0 to numPoints on the first calls
  double sumVal;    // holds the summ of the values in values[]
   
};
///////////////////////////////// DataSmooth ///////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////

#endif // _RCMUTILH_