
#define BLUE   5
#define GREEN  3
#define RED    6

char serialInput;
char receiveBuffer[12];
char i;
long timeOut;

#define TIMEOUT_INTERVAL 5000
unsigned char global_red, global_green, global_blue;
unsigned int commandCount = 0;

#define ACTION_NONE          0
#define ACTION_FADE          1
#define ACTION_SNAP          2
char stripAction = ACTION_NONE;

// Action definitions/settings
#define FADE_STEPS 100.0

// Action variables
unsigned int actionDelay = 0;
unsigned int SpeedDelay = 100;

unsigned char FadeColors[20][3];
unsigned char FadeColorsCount = 0;
unsigned char FadeCurrentColor = 0;
unsigned char FadeCurrentStep = 0;
float FadeRedChange, FadeGreenChange, FadeBlueChange;

unsigned char SnapColors[20][3];
unsigned char SnapColorsCount = 0;
unsigned char SnapCurrentColor = 0;

void setup() {
  Serial.begin(57600); 
  setStripColor(0, 0, 0);
}

void loop() {
  if (Serial.available() > 0) {
    serialInput = Serial.read();
    checkBootloader(serialInput);

    if (serialInput == 0x0F) { // Color command
      timeOut = millis() + TIMEOUT_INTERVAL;
      while (Serial.available() < 12 && timeOut > millis());
      if (timeOut > millis()) {
        for (i = 0; i < 12; i++) {
          receiveBuffer[i] = Serial.read(); 
        }
        if (receiveBuffer[3] == ';' && receiveBuffer[7] == ';' && receiveBuffer[11] == ';') {
          parseColorInput(receiveBuffer);  
          setStripColor(global_red, global_green, global_blue);    
        }
      }     
    }  

    else if (serialInput == 0x01) { // Add fade color
      timeOut = millis() + TIMEOUT_INTERVAL;
      while (Serial.available() < 12 && timeOut > millis());
      if (timeOut > millis()) {
        for (i = 0; i < 12; i++) {
          receiveBuffer[i] = Serial.read(); 
        }
        if (receiveBuffer[3] == ';' && receiveBuffer[7] == ';' && receiveBuffer[11] == ';') {
          parseColorInput(receiveBuffer);      
          FadeColors[FadeColorsCount][0] = global_red;
          FadeColors[FadeColorsCount][1] = global_green;
          FadeColors[FadeColorsCount][2] = global_blue;         
          FadeColorsCount++;           
        }
      }   
      commandCount++;
      Serial.print("Command #");      
      Serial.println(commandCount);
    } 

    else if (serialInput == 0x02) { // Reset fade colors
      FadeColorsCount = 0;    
      commandCount++;
      Serial.print("Command #");      
      Serial.println(commandCount);
    }

    else if (serialInput == 0x03) { // Set speed delay  
      timeOut = millis() + TIMEOUT_INTERVAL;
      while (Serial.available() < 6 && timeOut > millis());
      if (timeOut > millis()) {
        for (i = 0; i < 6; i++) {
          receiveBuffer[i] = Serial.read(); 
        }
        if (receiveBuffer[5] == ';') {
          SpeedDelay = (receiveBuffer[0] - '0') * 10000;
          SpeedDelay += (receiveBuffer[1] - '0') * 1000;           
          SpeedDelay += (receiveBuffer[2] - '0') * 100;
          SpeedDelay += (receiveBuffer[3] - '0') * 10;
          SpeedDelay += (receiveBuffer[4] - '0');         
        }
      }
      commandCount++;
      Serial.print("Command #");      
      Serial.println(commandCount);      
    }

    else if (serialInput == 0x04) { // Enable Fade effect
      stripAction = ACTION_FADE;
      FadeCurrentColor = 0;
      FadeCurrentStep = 0;   
      commandCount++;
      Serial.print("Command #");      
      Serial.println(commandCount);      
    }

    else if (serialInput == 0x05) { // Add Snap color
      timeOut = millis() + TIMEOUT_INTERVAL;
      while (Serial.available() < 12 && timeOut > millis());
      if (timeOut > millis()) {
        for (i = 0; i < 12; i++) {
          receiveBuffer[i] = Serial.read(); 
        }
        if (receiveBuffer[3] == ';' && receiveBuffer[7] == ';' && receiveBuffer[11] == ';') {
          parseColorInput(receiveBuffer);      
          SnapColors[SnapColorsCount][0] = global_red;
          SnapColors[SnapColorsCount][1] = global_green;
          SnapColors[SnapColorsCount][2] = global_blue;         
          SnapColorsCount++;           
        }
      } 
      commandCount++;
      Serial.print("Command #");      
      Serial.println(commandCount);      
    }  

    else if (serialInput == 0x06) { // Reset snap colors
      SnapColorsCount = 0;    
      commandCount++;
      Serial.print("Command #");      
      Serial.println(commandCount);      
    }    

    else if (serialInput == 0x07) { // Enable Snap effect
      stripAction = ACTION_SNAP;
      SnapCurrentColor = 0;
      commandCount++;
      Serial.print("Command #");      
      Serial.println(commandCount);      
    }    

    else if (serialInput == 0x0E) { // Disable any effect
      stripAction = ACTION_NONE;
      commandCount++;
      Serial.print("Command #");      
      Serial.println(commandCount);      
    }

  }

  if (stripAction != ACTION_NONE && actionDelay > 0) {
    delay(1);
    actionDelay -= 1;
  } 
  else if (stripAction == ACTION_NONE && actionDelay > 0) {
    actionDelay = 0; // Reset actionDelay while inactive
  } 
  else if (stripAction != ACTION_NONE) {
    if (stripAction == ACTION_FADE && FadeColorsCount > 0) {
      DoFade(); 
      actionDelay = SpeedDelay;
    }

    else if (stripAction == ACTION_SNAP && SnapColorsCount > 0) {
      DoSnap(); 
      actionDelay = SpeedDelay;
    }
  }

}

void parseColorInput(char * serialBuffer)
{
  global_red = (serialBuffer[0] - '0') * 100;
  global_red += (serialBuffer[1] - '0') * 10;
  global_red += (serialBuffer[2] - '0');    
  global_green = (serialBuffer[4] - '0') * 100;
  global_green += (serialBuffer[5] - '0') * 10;
  global_green += (serialBuffer[6] - '0');    
  global_blue = (serialBuffer[8] - '0') * 100;
  global_blue += (serialBuffer[9] - '0') * 10;
  global_blue += (serialBuffer[10] - '0');   
}

void resetIntoBootloader(void)
{
  analogWrite(7, 0);
}

char bootloaderPrelimary = 0;
void checkBootloader(char serialChar)
{  
  if (bootloaderPrelimary == 0) {
    if (serialChar == '0')
      bootloaderPrelimary = 1;
  } 
  else {
    if (serialChar == ' ')
      resetIntoBootloader();            
    else
      bootloaderPrelimary = 0;
  }        
}

void setStripColor(char red, char green, char blue)
{
  analogWrite(RED,(red));
  analogWrite(BLUE,(blue));
  analogWrite(GREEN,(green));
}         


void DoFade(void)
{
  if (FadeCurrentStep == 0) {
    if (FadeCurrentColor < (FadeColorsCount-1)) {
      FadeRedChange = (FadeColors[FadeCurrentColor+1][0] - FadeColors[FadeCurrentColor][0]) / FADE_STEPS;
      FadeGreenChange = (FadeColors[FadeCurrentColor+1][1] - FadeColors[FadeCurrentColor][1]) / FADE_STEPS;
      FadeBlueChange = (FadeColors[FadeCurrentColor+1][2] - FadeColors[FadeCurrentColor][2]) / FADE_STEPS;           
    } 
    else {
      FadeRedChange = (FadeColors[0][0] - FadeColors[FadeCurrentColor][0]) / FADE_STEPS;
      FadeGreenChange = (FadeColors[0][1] - FadeColors[FadeCurrentColor][1]) / FADE_STEPS;
      FadeBlueChange = (FadeColors[0][2] - FadeColors[FadeCurrentColor][2]) / FADE_STEPS;      
    } 
    setStripColor(FadeColors[FadeCurrentColor][0], FadeColors[FadeCurrentColor][1], FadeColors[FadeCurrentColor][2]);    
  } 
  else {
    setStripColor((FadeColors[FadeCurrentColor][0]+(FadeCurrentStep*FadeRedChange)), (FadeColors[FadeCurrentColor][1]+(FadeCurrentStep*FadeGreenChange)), (FadeColors[FadeCurrentColor][2]+(FadeCurrentStep*FadeBlueChange)));    
  }

  FadeCurrentStep++;
  if (FadeCurrentStep > FADE_STEPS) {
    if (FadeCurrentColor < (FadeColorsCount-1)) {
      FadeCurrentColor++;  
    } 
    else {
      FadeCurrentColor = 0;
    }
    FadeCurrentStep = 0;
  }
}

void DoSnap() {
  setStripColor(SnapColors[SnapCurrentColor][0], SnapColors[SnapCurrentColor][1], SnapColors[SnapCurrentColor][2]);

  if (SnapCurrentColor < (SnapColorsCount-1)) {
    SnapCurrentColor++;
  } 
  else {
    SnapCurrentColor = 0;
  }    
}
