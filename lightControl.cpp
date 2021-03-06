 /*
 ****************************************
 Dynamic Light Controller
 Author: Connor J. Edling, CpE
 Version: a0.1
 Date: 5-12-2019
 ****************************************
 */
 //Include Statements
 #include <IRremote.h>
 #include <LiquidCrystal.h>
 #include <string.h>

 //Define statements
 /*IR control codes*/
 #define UP 0xff629d
 #define DOWN 0xffa857
 #define LEFT 0xff22dd
 #define RIGHT 0xffc23d
 #define ENTER 0xff02fd
 #define ZERO 0xff6897
 #define ONE 0xff30cf
 #define TWO 0xff18e7
 #define THREE 0xff7a85
 #define FOUR 0xff10ef
 #define FIVE 0xff38c7
 #define SIX 0xff5aa5
 #define SEVEN 0xff42bd
 #define EIGHT 0xff4ab5
 #define NINE 0xff52ad
 //menu
 #define TOP_MES " R   G   B  MODE"
 #define MENU_BOT 0
 #define MENU_TOP 6
 char menuL[7][10] = {" Off", " Red", " Grn", "Blue", "Purp", "Yell", "Rain"};
//PWM Pins
  const int redPin = 11;                                   //Red Pin
  const int greenPin = 10;                                  //Green Pin
  const int bluePin = 9;                                  //Blue Pin
//LCD
  const int rs = 12, en = 8, d4 = 4, d5 = 5, d6 = 6, d7 = 7;                 //LCD Pin definitions. 
  LiquidCrystal lcd(rs, en, d4, d5, d6, d7);                        //Creates LCD device
//ir 
  const int irR = 2;                                    //IR Recieve Pin
  IRrecv irrecv(irR);                                   //Create IR device
  decode_results results;                                 //Create results variable. This is used to extract IR code

//State Machine
  bool machStart = true;                                  //State Machine startup variable, to be used later
  bool irRECV = false;                                    //IR interrupt flag, to be used later
  bool modeSet = false;                                   //Flag for startup
  enum STATE {st_idle, st_menuSel};//enum STATE {st_modeSet, st_RECV,st_OFF, st_RED, st_GREEN, st_BLUE, st_PURPLE, st_YELLOW, st_SIX, st_setMode};    //State machine state declerations
  u32 store;                                        //IR code storage variable, used for state machine state change
  int arrow; //menu keys, L==Left R==Right                                       //State machine IR code comparasion variable, used for detecting change
  STATE currentState;                                   //Current State variable, used to control the state machine
  int lightMode = 0;                                    //Light Mode variable, used to control the lightControl subroutine
  const char* m_modeC;
  int m_mode = 0;
  char BOT_MES[16];
  unsigned int rgbColour[3];
  bool intF = false;
//PWM Vals
  int curR, curG, curB;
//Colors
int black[3]  = { 0, 0, 0 };
int white[3]  = { 100, 100, 100 };
int red[3]    = { 100, 0, 0 };
int green[3]  = { 0, 100, 0 };
int blue[3]   = { 0, 0, 100 };
int yellow[3] = { 40, 95, 0 };
int dimWhite[3] = { 30, 30, 30 };
// Set initial color
int redVal = black[0];
int grnVal = black[1]; 
int bluVal = black[2];

int wait = 10;      // 10ms internal crossFade delay; increase for slower fades
int hold = 0;       // Optional hold when a color is complete, before the next crossFade
int dbug = 1;      // DEBUG counter; if set to 1, will write values back via serial
int loopCount = 60; // How often should DEBUG report?
int repeat = 3;     // How many times should we loop before stopping? (0 for no stop)
int j = 0;          // Loop counter for repeat

// Initialize color variables
int prevR = redVal;
int prevG = grnVal;
int prevB = bluVal;
bool idle_print = true;
//Begin
void setup() {
  attachInterrupt(digitalPinToInterrupt(3), right, RISING);
  attachInterrupt(digitalPinToInterrupt(2), left, RISING);
  m_modeC = "Off"; //Initialize machine with lights off
  m_mode = 0;    //set the machine mode to 0 (off)
  //initialize pins
  setColourRgb(0,0,0);                                    //Initialize the RGB LED to off
  Serial.begin(9600);                                   //Initialize serial connection
  irrecv.enableIRIn();                                    //enable the IR device
  irrecv.blink13(true);                                   //turn on IR blink 
  // set up the LCD's number of columns and rows:
  lcd.begin(16, 2);                                     //Initialize the LCD device
  // Print a message to the LCD.
  lcd.print("Welcome");                                   //Print a welcome message when setup is complete
  lcd.setCursor(0,1);
  lcd.print("Setup Complete");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0,0);
  
  //Serial.println(LEFT, HEX);//debug
}

void loop() {

  stateMachine();                                     //Loop calls the state machine
  
  
}
void right(){
  intF = true;
  currentState = st_menuSel;
  arrow = 1;
}
void left(){
  intF = true;
  currentState = st_menuSel;
  arrow = 2;
}
int ir(){
    
    if (irrecv.decode(&results)){
    int res = results.value;
    store = results.value;
    if(res != -1){+
    Serial.println(results.value,HEX);
 
    //Serial.println(results.value,HEX);
//    lcd.clear();
//    lcd.setCursor(0,0);
//    lcd.print("CODE:");
//    lcd.setCursor(0,1);
//    lcd.print(results.value, HEX);
    }

    irrecv.resume();
    return store;
  }
  return 1;
}
void lcdPrint(){
    snprintf (BOT_MES, 16, "%03d %03d %03d %s", rgbColour[0], rgbColour[1], rgbColour[2], m_modeC);
    lcd.setCursor(0,0);
    lcd.print(TOP_MES);
    lcd.setCursor(0,1);
    lcd.print(BOT_MES);
}
void stateMachine(){
  
  switch(currentState){
    u32 machStr; 
    case st_idle:  
      u32 dir;
      if(idle_print == true){
      Serial.println("st_idle");
      idle_print = false;
      Serial.println(idle_print);
      }
      lcdPrint();
      
      lcd.print(m_modeC);
//      ir(); //for ir menu control, needs fixing
//      if(machStr != store){
//        store = 0x0;
//        machStr = store;
//        Serial.println("machStr !=");
//        
//      if((store == LEFT || store == RIGHT)){
//        Serial.println("dir detected");
//        currentState = st_menuSel;
//        
//        //currentState = st_setMode;
//        break;
//      }
//        ir();
//        //lightControl(lightMode);
//        
//      }
      break;
    case st_menuSel:
          intF = false;
          Serial.println("dir change");
          Serial.println(arrow);
          if(arrow == 2){ 
            
            Serial.println("left");          
            store = 0x0;
            m_mode = menu(m_mode);
            arrow = 0;  
            lightControl(m_mode);
            currentState = st_idle;
            idle_print = true;
            break;
          }
          else if(arrow == 1){     
            
            Serial.println("right");      
            store = 0x0;
            m_mode = menu(m_mode);
            arrow = 0;  
            lightControl(m_mode);
            currentState = st_idle;
            idle_print = true;
            break;
          }
          currentState = st_idle;
          break;
  } 
}

int menu(int pointer){
    if(arrow == 2){
      if(pointer == MENU_BOT){
        pointer = MENU_TOP;
        m_modeC = menuL[pointer];
        lcd.clear();
      }
      else{
      pointer--;
      m_modeC = menuL[pointer];
      lcd.clear();
      }
    }
    else if(arrow == 1){             
      
      if(pointer == MENU_TOP){
        pointer = MENU_BOT;
        m_modeC = menuL[pointer];
        lcd.clear();
      }
      else{
      pointer++;
      m_modeC = menuL[pointer];
      lcd.clear();
      }
    }
    return pointer;
}

void rgb(){
  
  crossFade(red);
  crossFade(green);
  crossFade(blue);
  
}
void setColourRgb(unsigned int red, unsigned int green, unsigned int blue) {
  analogWrite(redPin, red);
  analogWrite(greenPin, green);
  analogWrite(bluePin, blue);
}
void lightControl(int mode){

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
    rgbColour[0] = 255;
    rgbColour[1] = 0;
    rgbColour[2] = 0;
    for (int decColour = 0; decColour < 3; decColour += 1) {
    int incColour = (decColour + 1)%2;

    // cross-fade the two colours.
    for(int oj = 0; oj < 255; oj += 1) {
      rgbColour[decColour] -= 1;
      rgbColour[incColour] += 1;
      
      setColourRgb(rgbColour[0], rgbColour[1], rgbColour[2]);
      if(intF == true){
        intF == false;
        
        return;
      }
      //lcdPrint();
     delay(5);
    }
    }
    return;
  } 
  return;
  }
  
int calculateStep(int prevValue, int endValue) {
  int step = endValue - prevValue; // What's the overall gap?
  if (step) {                      // If its non-zero, 
    step = 1020/step;              //   divide by 1020
  } 
  return step;
}

/* The next function is calculateVal. When the loop value, i,
*  reaches the step size appropriate for one of the
*  colors, it increases or decreases the value of that color by 1. 
*  (R, G, and B are each calculated separately.)
*/

int calculateVal(int step, int val, int i) {

  if ((step) && i % step == 0) { // If step is non-zero and its time to change a value,
    if (step > 0) {              //   increment the value if step is positive...
      val += 1;           
    } 
    else if (step < 0) {         //   ...or decrement it if step is negative
      val -= 1;
    } 
  }
  // Defensive driving: make sure val stays in the range 0-255
  if (val > 255) {
    val = 255;
  } 
  else if (val < 0) {
    val = 0;
  }
  return val;
}

/* crossFade() converts the percentage colors to a 
*  0-255 range, then loops 1020 times, checking to see if  
*  the value needs to be updated each time, then writing
*  the color values to the correct pins.
*/

void crossFade(int color[3]) {
  // Convert to 0-255
  int R = (color[0] * 255) / 100;
  int G = (color[1] * 255) / 100;
  int B = (color[2] * 255) / 100;

  int stepR = calculateStep(prevR, R);
  int stepG = calculateStep(prevG, G); 
  int stepB = calculateStep(prevB, B);

  for (int i = 0; i <= 1020; i++) {
    redVal = calculateVal(stepR, redVal, i);
    grnVal = calculateVal(stepG, grnVal, i);
    bluVal = calculateVal(stepB, bluVal, i);

    analogWrite(redPin, redVal);   // Write current values to LED pins
    analogWrite(greenPin, grnVal);      
    analogWrite(bluePin, bluVal); 

    delay(wait); // Pause for 'wait' milliseconds before resuming the loop

//    if (dbug) { // If we want serial output, print it at the 
//      if (i == 0 or i % loopCount == 0) { // beginning, and every loopCount times
//        Serial.print("Loop/RGB: #");
//        Serial.print(i);
//        Serial.print(" | ");
//        Serial.print(redVal);
//        Serial.print(" / ");
//        Serial.print(grnVal);
//        Serial.print(" / ");  
//        Serial.println(bluVal); 
//      } 
//      dbug += 1;
//    }
  }
  // Update current values for next loop
  prevR = redVal; 
  prevG = grnVal; 
  prevB = bluVal;
  delay(hold); // Pause for optional 'wait' milliseconds before resuming the loop
}