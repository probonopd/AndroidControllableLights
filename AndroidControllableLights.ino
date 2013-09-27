#include <EEPROM.h>
#include "EEPROMAnything.h"

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

// Action definitions/settings
#define FADE_STEPS 100.0

#define EEPROM_VERSION  0xAB
#define INIT_FLAGS_ADDR  0
#define CONFIG_ADDR  4

typedef struct
{
  char action;

  // Action variables
  unsigned int actionDelay;
  unsigned int SpeedDelay;

  unsigned char FadeColors[20][3];
  unsigned char FadeColorsCount;
  unsigned char FadeCurrentColor;
  unsigned char FadeCurrentStep;
  float FadeRedChange, FadeGreenChange, FadeBlueChange;

  unsigned char SnapColors[20][3];
  unsigned char SnapColorsCount;
  unsigned char SnapCurrentColor;

} 
stripCfg_t;

stripCfg_t stripCfg;

void checkInitializationFlags() {
  uint8_t initFlag;
  EEPROM_readAnything(INIT_FLAGS_ADDR, initFlag);
  if (initFlag != EEPROM_VERSION) { // Check if the EEPROM version matches the current one
    InitializeEEPromValues();
    EEPROM_updateAnything(INIT_FLAGS_ADDR, EEPROM_VERSION); // After the default values have been restored, set the flags
  } 
  else {
    EEPROM_readAnything(CONFIG_ADDR, stripCfg); // Read existing config from EEProm into RAM
  }
}

void InitializeEEPromValues(void)
{
  stripCfg.action = ACTION_NONE;
  stripCfg.actionDelay = 0;
  stripCfg.SpeedDelay = 100;
  stripCfg.FadeColorsCount = 0;
  stripCfg.FadeCurrentColor = 0;
  stripCfg.FadeCurrentStep = 0;
  stripCfg.SnapColorsCount = 0;
  stripCfg.SnapCurrentColor = 0;

  EEPROM_updateAnything(CONFIG_ADDR, stripCfg);  
}

void saveUpdatedEEPromConfig(void)
{
  EEPROM_updateAnything(CONFIG_ADDR, stripCfg);  
}


void setup() {
  Serial.begin(57600); 
  // setStripColor(255, 200, 50);
  setStripColor(global_red, global_green, global_blue);    
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
          saveUpdatedEEPromConfig();
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
          stripCfg.FadeColors[stripCfg.FadeColorsCount][0] = global_red;
          stripCfg.FadeColors[stripCfg.FadeColorsCount][1] = global_green;
          stripCfg.FadeColors[stripCfg.FadeColorsCount][2] = global_blue;         
          stripCfg.FadeColorsCount++;  
          saveUpdatedEEPromConfig();         
        }
      }   
      commandCount++;
      Serial.print("Command #");      
      Serial.println(commandCount);
    } 

    else if (serialInput == 0x02) { // Reset fade colors
      stripCfg.FadeColorsCount = 0;   
      saveUpdatedEEPromConfig(); 
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
          stripCfg.SpeedDelay = (receiveBuffer[0] - '0') * 10000;
          stripCfg.SpeedDelay += (receiveBuffer[1] - '0') * 1000;           
          stripCfg.SpeedDelay += (receiveBuffer[2] - '0') * 100;
          stripCfg.SpeedDelay += (receiveBuffer[3] - '0') * 10;
          stripCfg.SpeedDelay += (receiveBuffer[4] - '0');  
          saveUpdatedEEPromConfig();       
        }
      }
      commandCount++;
      Serial.print("Command #");      
      Serial.println(commandCount);      
    }

    else if (serialInput == 0x04) { // Enable Fade effect
      stripCfg.action = ACTION_FADE;
      stripCfg.FadeCurrentColor = 0;
      stripCfg.FadeCurrentStep = 0;   
      saveUpdatedEEPromConfig();
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
          stripCfg.SnapColors[stripCfg.SnapColorsCount][0] = global_red;
          stripCfg.SnapColors[stripCfg.SnapColorsCount][1] = global_green;
          stripCfg.SnapColors[stripCfg.SnapColorsCount][2] = global_blue;         
          stripCfg.SnapColorsCount++;    
          saveUpdatedEEPromConfig();       
        }
      } 
      commandCount++;
      Serial.print("Command #");      
      Serial.println(commandCount);      
    }  

    else if (serialInput == 0x06) { // Reset snap colors
      stripCfg.SnapColorsCount = 0;   
      saveUpdatedEEPromConfig(); 
      commandCount++;
      Serial.print("Command #");      
      Serial.println(commandCount);      
    }    

    else if (serialInput == 0x07) { // Enable Snap effect
      stripCfg.action = ACTION_SNAP;
      stripCfg.SnapCurrentColor = 0;
      saveUpdatedEEPromConfig();
      commandCount++;
      Serial.print("Command #");      
      Serial.println(commandCount);      
    }    

    else if (serialInput == 0x0E) { // Disable any effect
      stripCfg.action = ACTION_NONE;
      saveUpdatedEEPromConfig();
      commandCount++;
      Serial.print("Command #");      
      Serial.println(commandCount);      
    }

  }

  if (stripCfg.action != ACTION_NONE && stripCfg.actionDelay > 0) {
    delay(1);
    stripCfg.actionDelay -= 1;
    //saveUpdatedEEPromConfig(); // commented to reduce EEProm bandwidth
  } 
  else if (stripCfg.action == ACTION_NONE && stripCfg.actionDelay > 0) {
    stripCfg.actionDelay = 0; // Reset stripCfg.actionDelay while inactive
    saveUpdatedEEPromConfig();
  } 
  else if (stripCfg.action != ACTION_NONE) {
    if (stripCfg.action == ACTION_FADE && stripCfg.FadeColorsCount > 0) {
      DoFade(); 
      stripCfg.actionDelay = stripCfg.SpeedDelay;
      //saveUpdatedEEPromConfig(); // commented to reduce EEProm bandwidth
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
  analogWrite(RED,(int) red);
  analogWrite(GREEN,(int) green);
  analogWrite(BLUE,(int) blue);
}         

void DoFade(void)
{
  if (stripCfg.FadeCurrentStep == 0) {
    if (stripCfg.FadeCurrentColor < (stripCfg.FadeColorsCount-1)) {
      stripCfg.FadeRedChange = (stripCfg.FadeColors[stripCfg.FadeCurrentColor+1][0] - stripCfg.FadeColors[stripCfg.FadeCurrentColor][0]) / FADE_STEPS;
      stripCfg.FadeGreenChange = (stripCfg.FadeColors[stripCfg.FadeCurrentColor+1][1] - stripCfg.FadeColors[stripCfg.FadeCurrentColor][1]) / FADE_STEPS;
      stripCfg.FadeBlueChange = (stripCfg.FadeColors[stripCfg.FadeCurrentColor+1][2] - stripCfg.FadeColors[stripCfg.FadeCurrentColor][2]) / FADE_STEPS;           
    } 
    else {
      stripCfg.FadeRedChange = (stripCfg.FadeColors[0][0] - stripCfg.FadeColors[stripCfg.FadeCurrentColor][0]) / FADE_STEPS;
      stripCfg.FadeGreenChange = (stripCfg.FadeColors[0][1] - stripCfg.FadeColors[stripCfg.FadeCurrentColor][1]) / FADE_STEPS;
      stripCfg.FadeBlueChange = (stripCfg.FadeColors[0][2] - stripCfg.FadeColors[stripCfg.FadeCurrentColor][2]) / FADE_STEPS;      
    } 
    setStripColor(stripCfg.FadeColors[stripCfg.FadeCurrentColor][0], stripCfg.FadeColors[stripCfg.FadeCurrentColor][1], stripCfg.FadeColors[stripCfg.FadeCurrentColor][2]);    
  } 
  else {
    setStripColor((stripCfg.FadeColors[stripCfg.FadeCurrentColor][0]+(stripCfg.FadeCurrentStep*stripCfg.FadeRedChange)), (stripCfg.FadeColors[stripCfg.FadeCurrentColor][1]+(stripCfg.FadeCurrentStep*stripCfg.FadeGreenChange)), (stripCfg.FadeColors[stripCfg.FadeCurrentColor][2]+(stripCfg.FadeCurrentStep*stripCfg.FadeBlueChange)));    
  }

  stripCfg.FadeCurrentStep++;
  if (stripCfg.FadeCurrentStep > FADE_STEPS) {
    if (stripCfg.FadeCurrentColor < (stripCfg.FadeColorsCount-1)) {
      stripCfg.FadeCurrentColor++;  
    } 
    else {
      stripCfg.FadeCurrentColor = 0;
    }
    stripCfg.FadeCurrentStep = 0;
  }
}

void DoSnap() {
  setStripColor(stripCfg.SnapColors[stripCfg.SnapCurrentColor][0], stripCfg.SnapColors[stripCfg.SnapCurrentColor][1], stripCfg.SnapColors[stripCfg.SnapCurrentColor][2]);

  if (stripCfg.SnapCurrentColor < (stripCfg.SnapColorsCount-1)) {
    stripCfg.SnapCurrentColor++;
  } 
  else {
    stripCfg.SnapCurrentColor = 0;
  }    
}

