/*
    RCMutil.cpp
	12 August 2015
	
	by Randy Miller [RCM]
	Aug 2015 
	
	
	Utility routines for Arduino
	
 void pause(int delay)               pause for (delay milliseconds) like delay but calls _blink()
 void _blink(void)			             toggles led if interval has expired
 int setBlinkLEDpin(int pin) 1 - 13  default 13 (the built on led)
 int setBlinkInterval(int interval)  default 250, 1 - 10000
 int pinToggle( int pin )			       toggle output of (pin) req: pinMode set to (INPUT | OUTPUT)

 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.

 UPDATE HISTORY   See RCMutil.h for update history
    
	14 Aug 2015 Initial release V0.0.1 [RCM]
  12 Aug 2015  [RCM] Inital 'C' version
*/

#include <RCMutil.h>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// constructor
RCMutil::RCMutil()
{
  // just to be sure lets default the private vars
 
  enaUsrHeartbeat = false;
  heartbeatLEDpin = 13;
  heartbeatInterval = 250;
}

//////  class members  //////

char * RCMutil::version(void)
{
  return((char*)VERSIONRCMUTIL);
}

void RCMutil::begin(int pin)
{
  enaUsrHeartbeat = false;
  heartbeatLEDpin = pin;
  heartbeatInterval = 250;
  
  pinMode(pin, INPUT | OUTPUT);
}

/************************************************************************************/
int RCMutil::setHeartbeatPin(int pin)
{
	if (pin != READ) {
		if ( (pin < 14) && (pin > 1) ) { // 0&1 are serial
      heartbeatLEDpin = pin;
      pinMode(pin, INPUT | OUTPUT);
    } else {
      if (pin == 0) { // means heartbeat off
        heartbeatLEDpin = 0;
      } else return (FAIL);
    }
	} 
 	return( heartbeatLEDpin );
}

/************************************************************************************/
long RCMutil::setHeartbeatDelay(long newDelay)
{
	if (newDelay != READ ) {
	  heartbeatInterval = newDelay;	
	}
	return (heartbeatInterval);
}

/************************************************************************************/
int RCMutil::pinToggle( int pin )
{
  if (pin == -1)  return( -1 );
  int ret = !digitalRead(pin);
  ret == LOW ? digitalWrite(pin, LOW) : digitalWrite(pin, HIGH);
  return(ret);
}

/* **********************************************************************************/
void RCMutil::pause(unsigned long delayms)
 {
  unsigned long endMillis = millis() + delayms;

    while (millis() < endMillis) {
    blink();
  }
 }
 
/**************************************************************************************/
void RCMutil::setUsrHeartbeatFunc( void (*func)(void) )
 {
	 usrHeartbeatFunc = func;
	 enaUsrHeartbeat = true;
}
/* ***********************************************************************************
 *  void blink()
 *  function checks timer to see if (_blinkInterval has elapsed, if so toggle the 
 *  LEDpin
 *  
 *  returns: true if _blinkInterval has elapsed, else false
 */
void RCMutil::blink(void)
{
  unsigned long currentMillis = millis();
  static unsigned long nextMillis = heartbeatInterval;  // store val for next toggle
  
  if (currentMillis >= nextMillis ) {                   // time to toggle?
     nextMillis = currentMillis + heartbeatInterval;
    if (nextMillis <= currentMillis) {                  // rollover check (49.7 days)
      nextMillis = heartbeatInterval;                   // not accurate but better than nothing
    }
    if ( enaUsrHeartbeat == true) {
      usrHeartbeatFunc();                                 // execute the user heartbeat function
    }
    pinToggle(heartbeatLEDpin);                         // toggle heartbeat LED
  }
}
/*************************************************************************************************************
 * read the value stored at EEPROM addy 0xC0 Where the number for the comm port is stored.
 * to set this execute the following once for each board
 */
char * RCMutil::getComPort(void)
{
   static char rvs[8];
   int cp;
   
   __EEGET(cp, COM_EE_ADDY);
   if ( (cp > 2) && (cp < 0xff) ) {
      sprintf(rvs,"COM%d", cp);
      return(rvs);
   }
   return((char*)"COM port not set");
}
/*************************************************************************************************************
 * Set the com port number.
 */
void RCMutil::setComPort(int val)
{
    __EEPUT( COM_EE_ADDY, val);
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/***********************************************************************
* Class timeout_Ms
* constructor    nothing happens...
*/
Timeout_mS::Timeout_mS() 
{
  endMillisec  = millis();  // init already timed out.
}

bool Timeout_mS::timeout(unsigned long ms)
{
   endMillisec = millis() + ms;
   return true;   
}

bool Timeout_mS::timeout(void)
{
   if ( endMillisec < millis()  ) {
      return true;
   }
   return false;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/***********************************************************************
* Constructor
* when instatiated pass the number of points to average between 2 and 10
*/
 DataSmooth::DataSmooth(int npts)    
 {
   
  numPoints = 0;
  if ( npts < 2) {                // undefined averaging
    Serial.print("DataSmooth: Invalid number of points to smooth. You want the average of less than 2??");
    return;
  }
  if ( npts > 9) {                // too much memory usage 10 pt smoothing is too much, fix your hardware.
    Serial.print("Invalid number of points to smooth. Must be less than 10.");
    return;
  }
  numPoints = npts;               // Set valid number of points to be stored and averaged. 
  curDatPtr = 0;                  // Init private vars
  nCall = 0;                      // 
  memset(values, 0.0, numPoints); // including the array. 
 }
  
/***********************************************************************/
float DataSmooth::smooth(float val)
{
  if (numPoints == 0) {
    Serial.print("FAIL: DataSmooth() Not initalized properly.");
    return(0.0);
  }
    
  values[curDatPtr] = val;            // store in array

  if (++curDatPtr >= numPoints)       // increment pointer check range
    curDatPtr = 0;                    // back to zero (circular buffer)

    if (nCall < numPoints) {          // until we have filled the values array 
      nCall++;                        // nCall will increment up to numPoints and no more
      if (nCall == 1) {
        return(val);                  // first call return the passed value
    }
  }
  
  sumVal = 0.0;                       // init the sum
  for (int i=0; i < nCall; i++) {    // get the sum of values
    sumVal += values[i];    
  }

  return( sumVal / nCall );           // return the average
}
  
  
 
//////////////////  end of class members  ////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

