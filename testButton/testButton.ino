int buttonVal = 0;
int valueTrack[1] = {0};
int disarmCode[4] = {4,2,3,1};
int armCode[4] = {1,3,2,4};
int runningCode[4] = {0,0,0,0};
void setup()
{
  Serial.begin(9600);
  pinMode(A1, INPUT);
}

void loop()
{
  buttonVal = analogRead(A1);
  delay(10);
    // to counter numbers like 121 122 123 120 121 122
    if(buttonVal > 0)
    {
      if((valueTrack[0] > buttonVal+40)||(valueTrack[0] < buttonVal-40))
      {
        Serial.print(buttonVal);
        systemArming(convertButton(buttonVal));
        valueTrack[0]=buttonVal;
      }
    }
}

boolean systemArming(int x)
{
  if(runningCode[0]==0)
  {
    runningCode[0] = x;
    return true;
  }
  else if(runningCode[1]==0)
  {
    runningCode[1] = x;
    return true;
  }
  else if(runningCode[2]==0)
  {
    runningCode[2] = x;
    return true;
  }
  else if(runningCode[3]==0)
  {
    runningCode[3] = x;
    codeCheck();
    return true;
  }
  else
  {
    return false;
  }
}

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
            antiTheftEnable();
            clearRunningCode();
            return true;
          }
        }
      }
    }
  Serial.println("No Match...");
  clearRunningCode();
  return false;
}

void clearRunningCode()
{
  runningCode[0]=0;
  runningCode[1]=0;
  runningCode[2]=0;
  runningCode[3]=0;
  
  valueTrack[0]=0;
}
  

void antiTheftEnable()
{
  tone(9, 300, 500);
    delay(600);
  tone(9, 500, 500);
    delay(600);
  tone(9, 700, 500);
}
void antiTheftDisable()
{
  tone(9, 500, 500);
  delay(600);
  tone(9, 300, 500);
  delay(600);
  tone(9, 500, 500);
}

int convertButton(int aReadInput)
{
  if((aReadInput > 0) && (aReadInput < 125))
  {
    Serial.println(" Button 1 Pressed");
    return 1;
  }
  else if((aReadInput < 250) && (aReadInput >=125))
  {
     Serial.println(" Button 2 Pressed");
    return 2;
  }
  else if((250 <= aReadInput) && (aReadInput < 350))
  {
     Serial.println(" Button 3 Pressed");
    return 3;
  }
  else if(350 < aReadInput)
  {
     Serial.println(" Button 4 Pressed");
    return 4;
  }
  else
  {
    return -1;
  }
}
  
