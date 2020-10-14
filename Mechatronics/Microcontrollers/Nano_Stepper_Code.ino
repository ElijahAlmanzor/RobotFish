/* Code written by Elijah A. eayea3 uses bits of code written by the dude that wrot AccelStepper and the serial communication by Dr. Jones*/
//nano edition
#include <AccelStepper.h>
//Loads the AccelStepper library

// Motor pin definitions
#define Step1 3
#define Dir1 4


#define DefaultState -1
#define Stepper2Stroke 30

int StateNumber = 0;
enum {numChars = 32}; //more error checking by the IDE so used this instead
char receivedChars[numChars];
long dataNumber = DefaultState;
boolean newData = false; //sets whether there is new data to be received


AccelStepper stepper2(AccelStepper::DRIVER, Step1, Dir1);

void setup() {

  stepper2.setMaxSpeed(500);
  stepper2.setAcceleration(100);
  //stepper2.setSpeed(200);
  stepper2.moveTo(Stepper2Stroke);


  Serial.begin(9600); //initialises the serial communication with the Arduino
  Serial.println("Enter motor state and hit return/Enter");
}//--(end setup )---

void loop()
{

  recvWithEndMarker(); //Written by Dr. Jones
  convertNewNumber(); //Written by Dr. Jones

  if (dataNumber == 1)
  {
    SwitchCase1();
  }
  else if (dataNumber == 0)
  {
    SwitchCaseOff();
  }


}


void recvWithEndMarker()
/* Receive data from serial port finishing with "newline" character.
   Based on http://forum.arduino.cc/index.php?topic=396450 Example 4 */
{
  static byte ndx = 0;
  char endMarker = '\n';
  char rc;

  if (Serial.available() > 0)
  {
    rc = Serial.read();

    if (rc != endMarker)
    {
      receivedChars[ndx] = rc;
      ndx++;
      if (ndx >= numChars)
      {
        ndx = numChars - 1;
      }
    }
    else
    {
      receivedChars[ndx] = '\0'; // terminate the string
      ndx = 0;
      newData = true;
    }
  }
}

bool convertNewNumber()
/* Converts character string to long integer only if there are new
       data to convert, otherwise returns false */
{
  if (newData)
  {
    dataNumber = 0.0;             // new for this version
    dataNumber = atol(receivedChars);   // new for this version
    newData = false;
    return true;
  }
  else
  {
    return false;
  }
}



void SwitchCase1()
{

  if (stepper2.distanceToGo() == 0)
  {
    stepper2.moveTo(-stepper2.currentPosition());
    //when it gets to 2000, reverse the step amount so it moves back there
  }
  stepper2.run();

}

void SwitchCaseOff()
{
  if (stepper2.currentPosition() > 0)
  {
    stepper2.moveTo(0);
    if (stepper2.currentPosition() != 0)
    {
      stepper2.runToPosition();

    }
    stepper2.moveTo(-Stepper2Stroke);
    dataNumber = -1;
  } else
  {
    stepper2.moveTo(0);
    if (stepper2.currentPosition() != 0)
    {
      stepper2.runToPosition();
    }
    stepper2.moveTo(Stepper2Stroke);
    dataNumber = -1;
  }

}
