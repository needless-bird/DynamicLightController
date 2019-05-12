 /*
 ****************************************
 Dynamic Light Controller
 Author: Connor J. Edling, CpE
 Version: a0.1
 Date: 5-12-2019
 ****************************************
 /*
 //Include Statements
 #include <IRremote.h>
 #include <LiquidCrystal.h>
 #include <string.h>
//PWM Pins
  const int redPin = 11; 																	//Red Pin
  const int greenPin = 10;																	//Green Pin
  const int bluePin = 9;																	//Blue Pin
//LCD
  const int rs = 3, en = 8, d4 = 7, d5 = 6, d6 = 5, d7 = 4;									//LCD Pin definitions. 
  LiquidCrystal lcd(rs, en, d4, d5, d6, d7);												//Creates LCD device
//ir 
  const int irR = 2;																		//IR Recieve Pin
  IRrecv irrecv(irR);																		//Create IR device
  decode_results results;																	//Create results variable. This is used to extract IR code
//State Machine
  bool machStart = true;																	//State Machine startup variable, to be used later
  bool irRECV = false;																		//IR interrupt flag, to be used later
  enum STATE {st_IDLE, st_RECV, st_ONE, st_TWO, st_THREE, st_FOUR, st_FIVE, st_SIX};		//State machine state declerations
  u32 store;																				//IR code storage variable, used for state machine state change
  u32 machStr;																				//State machine IR code comparasion variable, used for detecting change
  STATE currentState;																		//Current State variable, used to control the state machine
  int lightMode = 0;																		//Light Mode variable, used to control the lightControl subroutine
//PWM Vals
  int curR, curG, curB;
  

//Begin
void setup() {
  
  //initialize pins
  setColourRgb(0,0,0);																		//Initialize the RGB LED to off
  Serial.begin(9600);																		//Initialize serial connection
  irrecv.enableIRIn();																		//enable the IR device
  irrecv.blink13(true);																		//turn on IR blink 
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);																			//Initialize the LCD device
  // Print a message to the LCD.
  lcd.print("Welcome");																		//Print a welcome message when setup is complete
  lcd.setCursor(0,1);
  lcd.print("Setup Complete");
  
  
}

void loop() {

  stateMachine();																			//Loop calls the state machine
  
  
}
int ir(){
    
    if (irrecv.decode(&results)){
    int res = results.value;
    store = results.value;
    if(res != -1){+
    Serial.println(results.value,HEX);
 
    //Serial.println(results.value,HEX);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("CODE:");
    lcd.setCursor(0,1);
    lcd.print(results.value, HEX);
    }

    irrecv.resume();
    return store;
  }
  return 1;
}
void lcdPrint(STATE cur){
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("State:");
    lcd.setCursor(0,1);
    lcd.print(cur);
}

void stateMachine(){
  u32 code;
  switch(currentState){
    case st_IDLE:
      if(machStr != store){
        
        machStr = store;
        Serial.println("CODE RECIEVED");
        currentState = st_RECV;
      }
      ir();
      lightControl(lightMode);
      break;
    case st_RECV:
      if(store == 0xFF30CF){
        Serial.println("CHANGE");
        currentState = st_ONE;
      }
      else if(store == 0xFF18e7){
        Serial.println("CHANGE");
        currentState = st_TWO;
      }
      else if(store == 0xFF7a85){
        Serial.println("CHANGE");
        currentState = st_THREE;
      }
      else if(store == 0xFF10ef){
        Serial.println("CHANGE");
        currentState = st_FOUR;
      }
      else if(store == 0xFF38c7){
        Serial.println("CHANGE");
        currentState = st_FIVE;
      }
      else if(store == 0xFF5aa5){
        Serial.println("CHANGE");
        currentState = st_SIX;
      }
    else {
    Serial.println("Bad Code");
    currentState = st_IDLE;
    }
      break;
    case st_ONE: 
      Serial.println("STATE ONE");
      lightMode = 1;
    
      //lightControl(lightMode);
      currentState = st_IDLE;
      break;
    case st_TWO: 
      Serial.println("STATE TWO");
      lightMode = 2;
      //lightControl(lightMode);
      currentState = st_IDLE;
      break;
    case st_THREE: 
      Serial.println("STATE THREE");
      lightMode = 3;
      //lightControl(lightMode);
      currentState = st_IDLE;
      break;
    case st_FOUR: 
      Serial.println("STATE FOUR");
      lightMode = 4;
      //lightControl(lightMode);
      currentState = st_IDLE;
      break;
    case st_FIVE: 
      Serial.println("STATE FIVE");
      lightMode = 5;
      //lightControl(lightMode);
      currentState = st_IDLE;
      break;
    case st_SIX: 
      Serial.println("STATE SIX");
      lightMode = 6;
      //lightControl(lightMode);
      currentState = st_IDLE;
      break;
  }
}

void rgb(){
  unsigned int rgbColour[3];
  while(machStr == store){
  rgbColour[0] = 255;
  rgbColour[1] = 0;
  rgbColour[2] = 0;
  setColourRgb(rgbColour[0],rgbColour[1],rgbColour[2]);
  for(int i = 0; i < 3; i++){
      int j = 1 == 2? 0:i+1;
        for(int k = 0; k < 255; k++){
          
          rgbColour[i]--;
          rgbColour[j]++;

          setColourRgb(rgbColour[0],rgbColour[1],rgbColour[2]);
          delay(5);
          ir();
    }
  }
  }
}
void setColourRgb(unsigned int red, unsigned int green, unsigned int blue) {
  analogWrite(redPin, red);
  analogWrite(greenPin, green);
  analogWrite(bluePin, blue);
}
void lightControl(int mode){
  unsigned int rgbColour[3];
  //Begin off

  if(mode == 0){
      rgbColour[0] = 0;
      rgbColour[1] = 0;
      rgbColour[2] = 0;
      setColourRgb(rgbColour[0],rgbColour[1],rgbColour[2]);
    return;
  }
  else if(mode == 1){
      rgbColour[0] = 255;
      rgbColour[1] = 0;
      rgbColour[2] = 0;
      setColourRgb(rgbColour[0],rgbColour[1],rgbColour[2]);
    return;
  }
  else if(mode == 2){
      rgbColour[0] = 0;
      rgbColour[1] = 255;
      rgbColour[2] = 0;
      setColourRgb(rgbColour[0],rgbColour[1],rgbColour[2]);
    return;
  }
  else if(mode == 3){
      rgbColour[0] = 0;
      rgbColour[1] = 0;
      rgbColour[2] = 255;
      setColourRgb(rgbColour[0],rgbColour[1],rgbColour[2]);
    return;
  }
  else if(mode == 4){
      rgbColour[0] = 255;
      rgbColour[1] = 0;
      rgbColour[2] = 255;
      setColourRgb(rgbColour[0],rgbColour[1],rgbColour[2]);
    return;
  }
  else if(mode == 5){
      rgbColour[0] = 255;
      rgbColour[1] = 255;
      rgbColour[2] = 0;
      setColourRgb(rgbColour[0],rgbColour[1],rgbColour[2]);
    return;
  }
  else if(mode == 6){
      rgb();
  } 
  return;
  }
  