#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>

//Variables for LCD Screen
#define I2C_ADDR    0x27  // Define I2C Address where the PCF8574A is
#define BACKLIGHT_PIN     3
#define En_pin  2
#define Rw_pin  1
#define Rs_pin  0
#define D4_pin  4
#define D5_pin  5
#define D6_pin  6
#define D7_pin  7

//Defining pins used by the ultrasonic rangers
#define audioPin 9 //Clicks and Alarm
#define AMtransmitPin 10 //Same signal as audioPin 9
#define LEDPin 11 //For Brake and Signal lights (alarm only manual otherwise)
#define echoPinOne  2// Echo Pin
#define trigPinOne  3// Trigger Pin
#define echoPinTwo 4 // Echo Pin
#define trigPinTwo 5 // Trigger Pin
#define echoPinThree 6 // Echo Pin
#define trigPinThree 7 // Trigger Pin
#define alarmPin 8
int passiveCircuitVal = 0;  //We may want to change these val and count are ambiguous
boolean alarmIsCut = false;
//=============================

//Variables for Lock / Unlock System
int buttonVal = 0;
int valueTrack[1] = {0};
int disarmCode[4] = {4,2,3,1};
int armCode[4] = {3,1,4,2};
int runningCode[4] = {0,0,0,0};
int count = 0;

boolean LOCKED = true;
//=====================================

unsigned long toneCycle = 100U;
#define LCDCycle 100U
#define ultrasonicCycle 100U
#define lowCycle 102U 
#define highCycle 110U
#define alarmCycle 500U

unsigned long toneLastMillis = 0;
unsigned long LCDLastMillis = 0;
unsigned long ultrasonicLastMillis = 0;
unsigned long lowLastMillis = 0;
unsigned long highLastMillis = 0;
unsigned long alarmLastMillis = 0;

boolean toneState = false;
boolean LCDState = false;
boolean ultrasonicState = false;
boolean lowState = false;
boolean highState = false;
boolean alarmState = false;

int maximumRange = 200; // Maximum range needed
int minimumRange = 0; // Minimum range needed
long durationOne, durationTwo, durationThree, distanceOne, distanceTwo, distanceThree; // Duration used to calculate distance

int n = 1;  //What is this variable for

LiquidCrystal_I2C       lcd(I2C_ADDR,En_pin,Rw_pin,Rs_pin,D4_pin,D5_pin,D6_pin,D7_pin);

//====================Start of Arduino main loop============================================
void setup() {
 Serial.begin (9600);
 pinMode(trigPinOne, OUTPUT);
 pinMode(echoPinOne, INPUT);
 pinMode(trigPinTwo, OUTPUT);
 pinMode(echoPinTwo, INPUT);
 pinMode(trigPinThree, OUTPUT);
 pinMode(echoPinThree, INPUT);
 
 lcd.begin (20,4);
  
// Switch on the backlight
 lcd.setBacklightPin(BACKLIGHT_PIN,POSITIVE);
 lcd.setBacklight(HIGH);
 lcd.home ();                   // go home
 
 lcd.print("Welcome to BATSS"); 
 
  pinMode(alarmPin,INPUT);
  pinMode(LEDPin, OUTPUT);
  pinMode(audioPin, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(A1,INPUT);  
}


void loop()
{
  //Check if a code is being entered. This precedes all functions because it determines Var - [Locked]
  buttonVal = analogRead(A1);
  delay(10);
  // to counter numbers like 121 122 123 120 121 122
  if((buttonVal > 0) && (buttonVal < 1000))
  {
    if((valueTrack[0] > buttonVal+40)||(valueTrack[0] < buttonVal-40))
    {
      Serial.print(buttonVal);
      systemArming(convertButton(buttonVal));
      valueTrack[0]=buttonVal;
    }
  }
  //========Start of two IF statements that determine actions based on Var [Locked]
  
  //IF THE SYSTEM IS LOCKED
  if(LOCKED)
  {
    //Passive Circuit
    // |-----> Goes to seperate alarm Loop if triggered
    //    |-----> Triggers GSM Shield to send a text
    
      if(cycleCheck(&alarmLastMillis, alarmCycle))
      {
        Serial.println("LOCK CIRCUIT LIVE");
        passiveCircuitVal = digitalRead(alarmPin);
        if (passiveCircuitVal == 1)
        {  
          count++;
           
        }
        if (count > 0)
        {
        
        alarmIsCut = true; 
        Serial.println("ALARM CUT");
        
        }
      
     }
      
      //Stuff to do when alarm is cut
      //MUST HAVE DISABLE FUNCTION BECAUSE BUTTON READ ALWAYS RUNNING. DISABLE ALARM CUTS OFF GSM/LIGHTS/SOUND
      if(alarmIsCut) 
      {
        
        while(LOCKED)
        {
            buttonVal = analogRead(A1);
            delay(10);
            // to counter numbers like 121 122 123 120 121 122
            if((buttonVal > 0) && (buttonVal < 1000))
            {
              if((valueTrack[0] > buttonVal+40)||(valueTrack[0] < buttonVal-40))
               {
                  Serial.print(buttonVal);
                  systemArming(convertButton(buttonVal));
                  valueTrack[0]=buttonVal;
                }
            }
            //Start GSM Shield and send out text message
            //Blink LEDs and Brake Lights using blink(). Pin 2
            //Audio signal sent to speakers Pin 9
            audioAlarm();
            lightAlarm();
        }
      }      
        
  }
  //IF THE SYSTEM IS UNLOCKED
  else if(!LOCKED)
  { 
    //Three Proximity Sensors (Pin 8,9,11,12,13,14)
    //Update LCD Screen (20,4)
    //Distance Clicks (Pin 9)
    //Sending signal to AM Transmitter (Pin 10)
    
      if(cycleCheck(&toneLastMillis, toneCycle))
      {
        tone(9,4000,2);
        toneState = !toneState;
      }
      
      //This code updates the LCD screen
      if(cycleCheck(&LCDLastMillis, LCDCycle))
      {
         if ( distanceThree <= minimumRange)
         {
          /* Send a negative number to computer and Turn LED ON 
           to indicate "out of range" */
           lcd.setCursor (14,3);        // go col 14 of line 3
           lcd.print(-1,DEC);
         }
         else 
         {
           /* Send the distance to the computer using Serial protocol, and
           turn LED OFF to indicate successful reading. */
           //Serial.println(distance);
           lcd.setCursor (14,3);        // go col 14 of line 3
         
         if(distanceThree < 10)
         { lcd.print(distanceThree);
            lcd.print("   "); 
            toneCycle = (distanceThree/3);
         }
         else if(distanceThree < 100)
         {
            lcd.print(distanceThree);
            lcd.print("  "); 
            tone(9,4000,2);
            toneCycle = distanceThree/3;
         }
         else if(distanceThree < 1000)
         {
           lcd.print(distanceThree);
           lcd.print(" "); 
           toneCycle = distanceThree/3 ;
         }
         else if(distanceThree < 10000)
         {
           lcd.print(distanceThree);
         }
      }
        LCDState = !LCDState;
      }
      
      Serial.println(analogRead(A1));
      
      if(cycleCheck(&ultrasonicLastMillis, ultrasonicCycle))
      {
       digitalWrite(trigPinOne, LOW);
       delay(2);
       digitalWrite(trigPinOne, HIGH);
       delay(10);
       digitalWrite(trigPinOne, LOW);
       durationOne = pulseIn(echoPinOne, HIGH, 120000);
       
       digitalWrite(trigPinTwo, LOW);
       delay(2);
       digitalWrite(trigPinTwo, HIGH);
       delay(10);
       digitalWrite(trigPinTwo, LOW);
       durationTwo = pulseIn(echoPinTwo, HIGH, 120000);
       
       digitalWrite(trigPinThree, LOW);
       delay(2);
       digitalWrite(trigPinThree, HIGH);
       delay(10);
       digitalWrite(trigPinThree, LOW);
       durationThree = pulseIn(echoPinThree, HIGH, 120000);
  
       distanceOne = durationOne/58.2;
       distanceTwo = durationTwo/58.2;
       distanceThree = durationThree/58.2;
       
       Serial.print("South-West: ");
       Serial.print(distanceOne);
       Serial.print(" | South: ");
       Serial.print(distanceThree);
       Serial.print(" | South-East: ");
       Serial.println(distanceTwo);
       
       tone(9, 800, distanceThree);
      }
    }
}


//=============Begin Functions for Ultrasonic Rangers=====================
boolean cycleCheck(unsigned long *lastMillis, unsigned int cycle) 
{
  unsigned long currentMillis = millis();
  if(currentMillis - *lastMillis >= cycle)
  {
    *lastMillis = currentMillis;
    return true;
  }
  else
    return false;
}

//============End Functions for Ultrasonic Rangers===========================

//============Begin Functions for 4 digit Lock / Unlock======================

/*This function builds the 4 digit code from single buttons pressed. Each time a button is pressed this
function is called and it places it in the first zero (empty) entry in the 4 item array "runningCode[]"
once all 4 entries have been recorded the function calls codeCheck() to compare to lock/unlock code
*/
boolean systemArming(int x)
{
  if(runningCode[0]==0)
  {
    runningCode[0] = x;
    lcd.clear();
    lcd.print("Entered: ");
    lcd.print(runningCode[0]);
    return true;
  }
  else if(runningCode[1]==0)
  {
    runningCode[1] = x;
    lcd.clear();
    lcd.print("Entered: ");
    lcd.print(runningCode[0]);
    lcd.print(runningCode[1]);
    return true;
  }
  else if(runningCode[2]==0)
  {
    runningCode[2] = x;
    lcd.clear();
    lcd.print("Entered: ");
    lcd.print(runningCode[0]);
    lcd.print(runningCode[1]);
    lcd.print(runningCode[2]);
    return true;
  }
  else if(runningCode[3]==0)
  {
    runningCode[3] = x;
    lcd.clear();
    lcd.print("Entered: ");
    lcd.print(runningCode[0]);
    lcd.print(runningCode[1]);
    lcd.print(runningCode[2]);
    lcd.print(runningCode[3]);
    codeCheck();
    return true;
  }
  else
  {
    return false;
  }
}

// Called by systemArming. Compares the 4 digit code the user has entered to the lock/unlock code
boolean codeCheck()
{
  Serial.print("Checking Code...");
  Serial.println(runningCode[3] + (runningCode[2]*10) + (runningCode[1]*100) + (runningCode[0]*1000));
  //Later we can change 4 to size of the array if they want longer codes
  if(runningCode[0]==disarmCode[0])
  {
    if(runningCode[1]==disarmCode[1])
    {
      if(runningCode[2]==disarmCode[2])
      {
        if(runningCode[3]==disarmCode[3])
        {
          Serial.println("Code Match: Security System Disarmed...");
          lcd.clear();
          lcd.print("SYSTEM IS DISARMED"); 
          antiTheftDisable();
          clearRunningCode();
          return true;
        }
      }
    }
  }
  if(runningCode[0]==armCode[0])
  {
      if(runningCode[1]==armCode[1])
      {
        if(runningCode[2]==armCode[2])
        {
          if(runningCode[3]==armCode[3])
          {
            Serial.println("Code Match: Security System Armed...");
            lcd.clear();
            lcd.print("SYSTEM IS ARMED"); 
            antiTheftEnable();
            clearRunningCode();
            return true;
          }
        }
      }
    }
  Serial.println("No Match...");
  lcd.clear();
  lcd.print("CODE INVALID"); 
  clearRunningCode();
  return false;
}

//Clears the 4 digit array "runningCode[]"
void clearRunningCode()
{
  runningCode[0]=0;
  runningCode[1]=0;
  runningCode[2]=0;
  runningCode[3]=0;
  
  valueTrack[0]=0;
  
}
  
//Called when the 4 digit code entered matches the UNLOCK code. This function sets the global var "Locked"
void antiTheftEnable()
{
  tone(9, 300, 500);
    delay(600);
  tone(9, 500, 500);
    delay(600);
  tone(9, 700, 500);
  
  //Set the Locked variable to true
  LOCKED = true;
  
}
//Called when the 4 digit code entered matches the LOCK code. This function sets the global var "Locked"
void antiTheftDisable()
{
  tone(9, 500, 500);
  delay(600);
  tone(9, 300, 500);
  delay(600);
  tone(9, 500, 500);
  
  //Set the Locked variable to false
  LOCKED = false;
}

/*This changes the analog reading of A1 0(0V) - 1023(5V) to a button value.
Determined by resistors. Ranges are shown below
*/
int convertButton(int aReadInput)
{
  if((aReadInput > 930) && (aReadInput < 1000))
  {
    Serial.println(" Button 1 Pressed");
    return 1;
  }
  else if((aReadInput < 600) && (aReadInput >=550))
  {
     Serial.println(" Button 2 Pressed");
    return 2;
  }
  else if((420 <= aReadInput) && (aReadInput < 549))
  {
     Serial.println(" Button 3 Pressed");
    return 3;
  }
  else if((700 > aReadInput) && (640 < aReadInput))
  {
     Serial.println(" Button 4 Pressed");
    return 4;
  }
  else
  {
    return -1;
  }
}
//=============================End Functions for 4 digit Lock / Unlock======================
  
//=============================Begin Code for Alarm Cut ====================================
void audioAlarm()
{
  tone(9, 800, 500);
  delay(600);
  tone(9, 500, 500);
  delay(600);
  tone(9, 800, 500);
  delay(600);
  tone(9, 500, 500);
  delay(600);
  tone(9, 800, 500);
  delay(600);
  tone(9, 500, 500);
  delay(600);
  tone(9, 800, 500);
  delay(600);
}
void lightAlarm()
{
  //Pin 11 Blink
  digitalWrite(12, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(20);               // wait for a second
  digitalWrite(12, LOW);    // turn the LED off by making the voltage LOW
  delay(20);    
}

void GSMShieldActivate()
{
  //Zane's Code
}
//==========================End Code for Alarm Cut=========================================

  
