#include <Wire.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>

#define I2C_ADDR    0x27  // Define I2C Address where the PCF8574A is
#define BACKLIGHT_PIN     3
#define En_pin  2
#define Rw_pin  1
#define Rs_pin  0
#define D4_pin  4
#define D5_pin  5
#define D6_pin  6
#define D7_pin  7

#define echoPin 7 // Echo Pin
#define trigPin 8 // Trigger Pin
#define LEDPin 13 // Onboard LED
int alarmpin = 0;
int val = 0;
int count = 0;

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
long duration, distance; // Duration used to calculate distance


int n = 1;

LiquidCrystal_I2C       lcd(I2C_ADDR,En_pin,Rw_pin,Rs_pin,D4_pin,D5_pin,D6_pin,D7_pin);






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




void setup() {
 Serial.begin (9600);
 pinMode(trigPin, OUTPUT);
 pinMode(echoPin, INPUT);
 pinMode(LEDPin, OUTPUT); // Use LED indicator (if required)
 
 lcd.begin (20,4);
  
// Switch on the backlight
 lcd.setBacklightPin(BACKLIGHT_PIN,POSITIVE);
 lcd.setBacklight(HIGH);
 lcd.home ();                   // go home
 
 lcd.print("Distance"); 
 

 
  pinMode(alarmpin,INPUT);
  pinMode(A1,INPUT);
  

}


void loop()
{
  if(cycleCheck(&toneLastMillis, toneCycle))
  {
    tone(9,4000,2);
    toneState = !toneState;
  }
  
  
  
  if(cycleCheck(&LCDLastMillis, LCDCycle))
  {
     if ( distance <= minimumRange){
  /* Send a negative number to computer and Turn LED ON 
 to indicate "out of range" */
 Serial.println(distance);
  lcd.setCursor (14,3);        // go col 14 of line 3
  lcd.print(-1,DEC);
  
   }
 else {
 /* Send the distance to the computer using Serial protocol, and
 turn LED OFF to indicate successful reading. */
   //Serial.println(distance);
   lcd.setCursor (14,3);        // go col 14 of line 3
   
   if(distance < 10)
    { lcd.print(distance);
      lcd.print("   "); 
      toneCycle = (distance/3);
    }
   else if(distance < 100){
     
     lcd.print(distance);
      lcd.print("  "); 
      tone(9,4000,2);
      toneCycle = distance/3;
     }
   else if(distance < 1000){

     lcd.print(distance);
     lcd.print(" "); 
     toneCycle = distance/3 ;
   }
   else if(distance < 10000){

     lcd.print(distance);
   
   }
   
 }
  LCDState = !LCDState;
  }
  
  Serial.println(analogRead(A1));
  
  if(cycleCheck(&ultrasonicLastMillis, ultrasonicCycle))
  {
       digitalWrite(trigPin, LOW);
       delay(2);


       digitalWrite(trigPin, HIGH);
       delay(10);
   
 
       digitalWrite(trigPin, LOW);
       duration = pulseIn(echoPin, HIGH);
 
       
       distance = duration/58.2;
 
  
  
       ultrasonicState = !ultrasonicState;
  
  
  
  }
  
  
  if(cycleCheck(&alarmLastMillis, alarmCycle)){
  val = digitalRead(alarmpin);
  if (val == 1){
  count++;
  
  }
  if (count >= 0){
  Serial.println("alarm is cut");
  
  
  
  }
  alarmState = !alarmState;
  
  
  
  
  
  }
  
  
  
}


