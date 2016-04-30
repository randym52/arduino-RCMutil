/**********************************************************************************************
 *
 * uBlink
 *
 * Simple test sketch to illustrate the use of 
 *  the RCMutil class library.  
 *  RCMutil methods:
 *    blink()
 *    pause()
 *    setUsrHeartbeatFunc(function)
 *    setComPort(num)
 *    getComPort()
 *    
 *    class 
 *      DataSmooth() 
 *    
 *  Hardware: Add a led with/resistor between Digital pin 2 and Gnd.

 *	UPDATE HISTORY:  See RCMutil.h for complete notes
 */
#include <RCMutil.h>

#define SERIALBPS       19200
#define HEARTBEAT_PIN   13
#define HEARTBEAT_DELAY 125
#define HB2LED          2

// define an instance of RCMutil, this has global scope ut can be used any where.
RCMutil ut = RCMutil();

/***********************************************************************************************/
void setup() {

  // put your setup code here, to run once:
  pinMode(HB2LED, INPUT | OUTPUT);   // D2 output

  ut.begin(HEARTBEAT_PIN);                 // This sets up the pinMode and assigns the heartbeatPin
  ut.setHeartbeatDelay(HEARTBEAT_DELAY);   // you can skip this it defaults to 250mS

  //ut.setComPort(0);                       // Change this to the proper number for the com port for this Arduino board.
                                            // After it has run once comment out this line again.

  // here is how you pass your heartbeat function to the RCMutil Library
  // below is a function named "void heartbeat(void)" in this call you
  // are passing the address of the function to the library so that it
  // may be run when blink() determmines it is time to toggle your
  // heartbeat LED.
  ut.setUsrHeartbeatFunc(heartbeat);

  Serial.begin(SERIALBPS);            // start Serial so we can see output in the monitor
  Serial.println(" ");                //
  Serial.println(ut.version());       // display the version of RCMutil you have running
  Serial.println(ut.getComPort());    // display this board's COM port
 
  DataSmooth ds3 = DataSmooth(3);     // create an instance of the smoothing object
                                      // if you had a second input you wanted to smooth
                                      // change the name(ds3) to something else like pressure3pt
                                      // This hhas local scope. It will go away when you exit setup()

  // here is some simple fixed input to demonstrate the smoothing
  Serial.print("ds3.smooth(10.0) = "); Serial.println( ds3.smooth(10.0) );
  Serial.print("ds3.smooth(9.0) = ");  Serial.println( ds3.smooth( 9.0) );
  Serial.print("ds3.smooth(11.0) = "); Serial.println( ds3.smooth(11.0) );
  Serial.print("ds3.smooth(10.0) = "); Serial.println( ds3.smooth(10.0) );
  Serial.print("ds3.smooth(9.0) = ");  Serial.println( ds3.smooth( 9.0) );
  Serial.print("ds3.smooth(11.0) = "); Serial.println( ds3.smooth(11.0) );
  
} // setup()

/***********************************************************************************************/
void loop() {

  // put your main code here, to run repeatedly:

  ut.pause(1000);  // like delay(ms) but blinks the LED on D13
                   // instead of using pause you mmay also call blink() at any point 
  ut.blink();       // in your loop() if you do not want to actually waste time pausing.
  
} // loop()

/***********************************************************************************************
 * void heartbeat(void)
 * This is a function you want to execute each time the heartbeat LED (13) toggles.
 * 
 * You may name it anything but it must return nothing and take no arguments, also the same 
 * name goes in the setUsrHeartbeatFunc call above.
 * 
 */

void heartbeat(void) 
{
  ut.pinToggle(HB2LED);
}

