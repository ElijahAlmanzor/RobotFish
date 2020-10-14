/* Code written by Elijah A. eayea3 - Uses AccelStepper library + serial communication written by Dr. Jones*/
#include <AccelStepper.h>
//Loads the AccelStepper library

// Motor Pin definitions
#define Step1 3
#define Dir1 4
#define EnableLine1 5
#define Step2 6
#define Dir2 7
#define EnableLine2 8
#define Step3 9
#define Dir3 10
#define EnableLine3 11

// Motor stroke lengths
float Stepper1Stroke = 20;
float Stepper1aStroke = 12; //test switch with a different distance to main 20 cm move
float Stepper2Stroke = -10;
float Stepper2aStroke = 10;
float Stepper3Stroke = 30;
float Stepper3aStroke = -24;
//ensures that changing the steps leads to the same time periods - again stops having different frequency between each stepper which would cause desynchronisation
float accel1 = 2*Stepper1Stroke*8.4016;
float accel1a = 2*Stepper1aStroke*8.4016;
float accel2 = 2*Stepper2Stroke*8.4016;
float accel2a = 2*Stepper2aStroke*8.4016;
float accel3= 2*Stepper3Stroke*8.4016;
float accel3a = 2*Stepper3aStroke*8.4016;


//Variables for Serial Communication
#define DefaultState -1
int StateNumber = 0;
enum {numChars = 32}; //more error checking by the IDE so used this instead
char receivedChars[numChars];
long dataNumber = DefaultState;
boolean newData = false; //sets whether there is new data to be received


AccelStepper stepper1(AccelStepper::DRIVER, Step1, Dir1);
AccelStepper stepper1a(AccelStepper::DRIVER, Step1, Dir1);
AccelStepper stepper2(AccelStepper::DRIVER, Step2, Dir2);
AccelStepper stepper2a(AccelStepper::DRIVER,Step2,Dir2);
AccelStepper stepper3(AccelStepper::DRIVER, Step3, Dir3);
AccelStepper stepper3a(AccelStepper::DRIVER, Step3, Dir3);


void setup() {
  //Calculates the accelerations so every stepper has the time period
  //Stops desynchornisation
 float accelone = accel1;
 float acceloneA = accel1a;
 float acceltwo = accel2;
 float acceltwoA = accel2a;
 float accelthree = accel3;
 float accelthreeA = accel3a;
 

//Stepper 1
  stepper1.setMaxSpeed(500); //sets the max speed
  stepper1.setAcceleration(accelone); //obtained from the 1.38 time period
  stepper1.moveTo(Stepper1Stroke); //Length motor oscillates to and 
    stepper1a.setMaxSpeed(500); //sets the max speed
  stepper1a.setAcceleration(acceloneA); //obtained from the 1.38 time period
  stepper1a.moveTo(Stepper1aStroke); //Length motor oscillates to and 

  
//Stepper 2 For reactive System test
  stepper2.setMaxSpeed(500);
  stepper2.setAcceleration(acceltwo); 
  stepper2.moveTo(Stepper2Stroke);

//Stepper 2 For 2nd harmonic Antagonistic system
  stepper2a.setMaxSpeed(500);
  stepper2a.setAcceleration(acceltwoA); 
  stepper2a.moveTo(-1*Stepper2aStroke);
  
//Stepper 3 For reactive system test
  stepper3.setMaxSpeed(500);
  stepper3.setAcceleration(accelthree); 
  stepper3.moveTo(Stepper3Stroke);

//Stepper 3 For Antagonistic system
  stepper3a.setMaxSpeed(500);
  stepper3a.setAcceleration(accelthreeA); 
  stepper3a.moveTo(-1*Stepper3aStroke);

  
  Serial.begin(9600); //initialises the serial communication with the Arduino
  Serial.println("Enter motor state");
}

void loop()
{
 
  recvWithEndMarker(); 
  convertNewNumber(); 
  //Functions written by Dr. Jones
  
  if (dataNumber == 1)
  { //First Harmonic for both Antagonistic and Reactive Systems
    SwitchState1();
    digitalWrite(EnableLine1,LOW);
    digitalWrite(EnableLine2,HIGH);
    digitalWrite(EnableLine3,HIGH);
  }
  else if (dataNumber == 2)
  { //Second Harmonic for Antagonistic System
    SwitchState2();
    digitalWrite(EnableLine1,LOW);
    digitalWrite(EnableLine2,LOW);
    digitalWrite(EnableLine3,HIGH);
  }
  else if (dataNumber == 3)
  { //Second Harmonic for Reactive System
    SwitchState3();
    digitalWrite(EnableLine1,HIGH);
    digitalWrite(EnableLine2,LOW);
    digitalWrite(EnableLine3,HIGH);
  }   
  else if (dataNumber == 4)
  { //Third Harmonic for Ant System
    SwitchState5();
    digitalWrite(EnableLine1,LOW);
    digitalWrite(EnableLine2,LOW);
    digitalWrite(EnableLine3,LOW);    
  }
    else if (dataNumber == 5)
  { //Third Harmonic for Reactive System
    SwitchState6();
    digitalWrite(EnableLine1,HIGH);
    digitalWrite(EnableLine2,HIGH);
    digitalWrite(EnableLine3,LOW);    
  }
   else if (dataNumber == 6)//Both up
  {
    SwitchState7();
    digitalWrite(EnableLine1,LOW);
    digitalWrite(EnableLine2,HIGH);
    digitalWrite(EnableLine3,LOW);    
  } else if (dataNumber == 0)
  {
    SwitchReset();
    digitalWrite(EnableLine1,HIGH);
    digitalWrite(EnableLine2,HIGH);
    digitalWrite(EnableLine3,HIGH);
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



void SwitchState1()
{

  if (stepper1.distanceToGo() == 0)
  {
    stepper1.moveTo(-stepper1.currentPosition());
    //when it gets to zero distance to go to, reverse the step amount so it moves back there
  }
  stepper1.run();

}


void SwitchState2()
{
//making stepper 1 go back and forth to stepper1stroke
  if (stepper1.distanceToGo() == 0)
  {
    stepper1.moveTo(-stepper1.currentPosition());
    //when it gets to zero distance to go to, reverse the step amount so it moves back there
  }
  stepper1.run();
//making stepper 2 go back and forth to stepper2stroke
if (stepper2a.distanceToGo() == 0)
  {
    stepper2a.moveTo(-stepper2a.currentPosition());
    //when it gets to zero distance to go to, reverse the step amount so it moves back there
  }
  stepper2a.run();
}


void SwitchState3()
{

  if (stepper2.distanceToGo() == 0)
  {
    stepper2.moveTo(-stepper2.currentPosition());
    //when it gets to zero distance to go to, reverse the step amount so it moves back there
  }
  stepper2.run();

}

void SwitchState5()
{

  if (stepper1.distanceToGo() == 0)
  {
    stepper1.moveTo(-stepper1.currentPosition());
    //when it gets to zero distance to go to, reverse the step amount so it moves back there
  }
  stepper1.run();


  if (stepper2a.distanceToGo() == 0)
  {
    stepper2a.moveTo(-stepper2a.currentPosition());
    //when it gets to zero distance to go to, reverse the step amount so it moves back there
  }
  stepper2a.run();


  if (stepper3a.distanceToGo() == 0)
  {
    stepper3a.moveTo(-stepper3a.currentPosition());
    //when it gets to zero distance to go to, reverse the step amount so it moves back there
  }
  stepper3a.run();
}

void SwitchState6()
{
  if (stepper3.distanceToGo() == 0)
  {
    stepper3.moveTo(-stepper3.currentPosition());
    //when it gets to zero distance to go to, reverse the step amount so it moves back there
  }
  stepper3.run();
}
void SwitchState7()
{

  if (stepper1a.distanceToGo() == 0)
  {
    stepper1a.moveTo(-stepper1a.currentPosition());
    //when it gets to zero distance to go to, reverse the step amount so it moves back there
  }
  stepper1a.run();

  if (stepper3a.distanceToGo() == 0)
  {
    stepper3a.moveTo(-stepper3a.currentPosition());
    //when it gets to zero distance to go to, reverse the step amount so it moves back there
  }
  stepper3a.run();
}
void SwitchReset()
{
  //Resetting stepper1
  if (stepper1.currentPosition() > 0)
  {
    stepper1.moveTo(0);
    if (stepper1.currentPosition() != 0)
    {
      stepper1.runToPosition();

    } //this bit will keep looping until it gets to zero
    stepper1.moveTo(-Stepper1Stroke); //resets it to that it travels to the original length
    dataNumber = -1;
  } else
  {
    stepper1.moveTo(0);
    if (stepper1.currentPosition() != 0)
    {
      stepper1.runToPosition();
    }
    stepper1.moveTo(Stepper1Stroke);
    dataNumber = -1;
  }
 
  
  //Resetting stepper2
  if (stepper2.currentPosition() > 0)
  {
    stepper2.moveTo(0);
    if (stepper2.currentPosition() != 0)
    {
      stepper2.runToPosition();

    } //this bit will keep looping until it gets to zero
    stepper2.moveTo(-Stepper2Stroke); //resets it to that it travels to the original length
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
  //Resetting stepper2a
  if (stepper2a.currentPosition() > 0)
  {
    stepper2a.moveTo(0);
    if (stepper2a.currentPosition() != 0)
    {
      stepper2a.runToPosition();

    } //this bit will keep looping until it gets to zero
    stepper2a.moveTo(-Stepper2aStroke); //resets it to that it travels to the original length
    dataNumber = -1;
  } else
  {
    stepper2a.moveTo(0);
    if (stepper2a.currentPosition() != 0)
    {
      stepper2a.runToPosition();
    }
    stepper2a.moveTo(Stepper2aStroke);
    dataNumber = -1;
  }
  //Resetting stepper3
   if (stepper3.currentPosition() > 0)
  {
    stepper3.moveTo(0);
    if (stepper3.currentPosition() != 0)
    {
      stepper3.runToPosition();

    } //this bit will keep looping until it gets to zero
    stepper3.moveTo(-Stepper3Stroke); //resets it to that it travels to the original length
    dataNumber = -1;
  } else
  {
    stepper3.moveTo(0);
    if (stepper3.currentPosition() != 0)
    {
      stepper3.runToPosition();
    }
    stepper3.moveTo(Stepper3Stroke);
    dataNumber = -1;
  }
  //resetting stepper 3a
  if (stepper3a.currentPosition() > 0)
  {
    stepper3a.moveTo(0);
    if (stepper3a.currentPosition() != 0)
    {
      stepper3a.runToPosition();

    } //this bit will keep looping until it gets to zero
    stepper3a.moveTo(-Stepper3aStroke); //resets it to that it travels to the original length
    dataNumber = -1;
  } else
  {
    stepper3a.moveTo(0);
    if (stepper3a.currentPosition() != 0)
    {
      stepper3a.runToPosition();
    }
    stepper3a.moveTo(Stepper3aStroke);
    dataNumber = -1;
  }
}
