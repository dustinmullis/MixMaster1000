#include <Servo.h>
#include <Stepper.h>
#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"
#include <LiquidCrystal.h>
#include <Rotary.h>

// Create the motor shield object with the default I2C address
Adafruit_MotorShield AFMS = Adafruit_MotorShield(); 
// Connect a stepper motor with 200 steps per revolution (1.8 degree) to motor port #2 (M3 and M4)
Adafruit_StepperMotor *myMotor = AFMS.getStepper(200, 2);

// Define Servo
Servo myServo;

//# STEPS TO EACH BOTTLE SLOT FROM HOME POSITION
const int RUM = 455;        //RUM
const int WHISKEY = 970;    //WHISKEY
const int VODKA = 1460;     //VODKA
const int TEQ = 1935;       //TEQUILA
const int TSEC = 2410;      //TRIPLE SEC
const int PEACH = 2900;     //PEACH SCHNAPPS
const int BLUE = 3350;      //BLUE CURACAO
const int OJ = 3860;        //ORANGE JUICE
const int PJ = 4300;        //PINEAPPLE JUICE
const int CJ = 4790;        //CRANBERRY JUICE
const int COKE = 5270;      //COKE
const int END = 5328;       //STEPS FROM RIGHT END STOP TO LEFT END STOP
const int HOME = 20;        //STEPS 

//INTERUPT PINS
const int leftSwitch = 2;
const int rightSwitch = 3;

//END STOP SWITCHES
volatile boolean rightLimit = false;
volatile boolean leftLimit = false;
volatile boolean isHome = false;

//lcd = LiquidCrystal(RS, ENABLE, D4, D5, D6, D7) 
const int LCD_RS = 13;
const int LCD_E = 12;
const int LCD_DB4 = 4;
const int LCD_DB5 = 5;
const int LCD_DB6 = 6;
const int LCD_DB7 = 7;
LiquidCrystal lcd(LCD_RS, LCD_E, LCD_DB4, LCD_DB5, LCD_DB6, LCD_DB7);

//Rotary Encoder = Rotary(DT,CHK);
const int RE_DT = 19;
const int RE_CHK = 18;
const int RE_SW = 20;
Rotary r = Rotary(RE_DT,RE_CHK);

const int FAN_PW = 43;  //fan power

volatile int pos = 0;   //index for drink array
unsigned char result;   //knob turn direction
//int length = 0;

char* drinkItems[] = {"Rum and Coke","Cuba Libre","Whiskey and Coke","Screwdriver",
                      "Tequila Sunrise","Fuzzy Navel","Madras","Vodka Cranberry",
                      "SOTB","Cosmo", "The Abyss", "Bay Breeze", "Soylent Green"};

//////////////////////////////////////////////////////////////

void setup() {
  
  Serial.begin(9600);   // set up Serial library at 9600 bps
  AFMS.begin();         // create with the default frequency 1.6KHz
  myMotor->setSpeed(200); //200 RPM  
  myServo.attach(10);  //atach servo to pin 10
  myServo.write(90);   //maintain center position on servo to start
  
  pinMode(RE_SW, INPUT);          //pin to read encoder button
  lcd.begin(16, 2);               //lcd.begin(cols,rows);

  pinMode(FAN_PW, OUTPUT);      //pin to write to fan power pin
  digitalWrite(FAN_PW, HIGH);   //start the fan
    
  lcd.print(" MIXMASTER 1000");  //print initial message and menu
  delay(3000);                   
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("SELECT A DRINK");
  delay(1000);
  lcd.setCursor(0, 1);
  lcd.print(drinkItems[pos]); 
  

  //end stops
  pinMode(leftSwitch, INPUT_PULLUP);
  pinMode(rightSwitch, INPUT_PULLUP);
  
}//end setup

//////////////////////////////////////////////////////////////

void loop() {

    if (!isHome){
      goHome();
    }
    delay(2000);
    stepBack(CJ);
    delay(2000);
    goHome();
    delay(2000);
    stepBack(COKE);
    delay(2000);

    
    result = r.process();
    if (digitalRead(RE_SW) == LOW)
    {
      if (!isHome)
      {
        goHome();
      }
      makeDrink();
    }
 
    if (result) {
      switch (result) {
        case DIR_CW:          
             pos += 1;
             lcd.clear();
             lcd.setCursor(0,0);
             lcd.print("SELECT A DRINK");
             lcd.setCursor(0,1);
             lcd.print(drinkItems[pos]);
             break;
        case DIR_CCW:
             pos -= 1;
             lcd.clear();
             lcd.setCursor(0,0);
             lcd.print("SELECT A DRINK");
             lcd.setCursor(0,1);
             lcd.print(drinkItems[pos]);
             break;
        case DIR_NONE:
             break;
        default:
             break;
      }//end switch
    }//end if 
}//end loop

void goHome()
{
  while (digitalRead(rightSwitch) == HIGH)
  {
    myMotor->step(1, FORWARD, DOUBLE);
  }
  myMotor->step(HOME, BACKWARD, DOUBLE);
  isHome = true;
}

void stepBack(int target) {
  for (int i = 0; i < target; i++) {
    if (digitalRead(leftSwitch) == HIGH ){
      myMotor->step(1, BACKWARD, DOUBLE);
    }
    else{
      leftLimit = true;
      break;
    }
  }
}

void stepForward(int target) {

  for (int i = 0; i < target; i++) {
    if (digitalRead(rightSwitch) == HIGH){
      myMotor->step(1, FORWARD, DOUBLE);
    }
    else{
      stepBack(HOME);
      isHome = true;
      break;
    }  
  }
}

//left limit flag
void homeLeft() {
  leftLimit = true;
}

//right limit flag
void homeRight() {
  rightLimit = true;
}

void makeDrink() {
     
        switch (pos){
          case 0:
            rumCoke();
            break;
          case 1:
            cubaLibre();
            break;
          case 2:
            whiskeyCoke();
            break;
          case 3:
            screwDriver();
            break;
          case 4:
            tequilaSunrise();
            break;
          case 5:
            fuzzyNavel();
            break;
          case 6: 
            madras();
            break;
          case 7:
            vodkaCranberry();
            break;
          case 8:
            sexOnTheBeach();
            break;
          case 9:
            cosmo();
            break;
          case 10:
            theAbyss();
            break;
          case 11:
            bayBreeze();
            break;
          case 12:
            soylentGreen();
            break;
          default:
            break;               
        }    
}

void armPump()
{
  
  myServo.write(120); //move arm up at full speed
  delay(5500);//wait 5.5 seconds
  myServo.write(80); //move arm down
  delay(1000); //wait 1 second
  myServo.write(90); //stop arm
}//end arm

void rumCoke()
{
  stepBack(RUM);
  delay(50);
  armPump();
  delay(50);
  stepBack(COKE - RUM);
  delay(50);
  armPump();
  delay(50);
  armPump();
  delay(50);
  armPump();
  delay(50);
  armPump();
  delay(50); 
  goHome();
}
void cubaLibre() {

  stepBack(RUM);
  delay(50);
  armPump();
  delay(50);
  stepBack(COKE - RUM);
  delay(50);
  armPump();
  delay(50);
  armPump();
  delay(50);
  armPump();
  delay(50);
  armPump();
  delay(50); 
  goHome();
  //LIME JUICE

}//end cubaLibre

void whiskeyCoke()
{   
  stepBack(WHISKEY);
  delay(50);
  armPump();
  delay(50);
  stepBack(COKE - WHISKEY);
  delay(50);
  armPump();
  delay(50);
  armPump();
  delay(50);
  armPump();
  delay(50);
  armPump();
  delay(50); 
  goHome();
}//end whiskeyCoke

void screwDriver()
{

  stepBack(VODKA);
  delay(50);
  armPump();
  delay(50);
  stepBack(OJ - VODKA);
  delay(50);
  armPump();
  delay(50);
  armPump();
  delay(50);
  armPump();
  delay(50);
  armPump();
  delay(50);
  goHome();
}

void tequilaSunrise()
{
  stepBack(TEQ);
  delay(50);
  armPump();
  delay(50);
  stepBack(OJ - TEQ);
  delay(50);
  armPump();
  delay(50);
  armPump();
  delay(50);
  armPump();
  delay(50);
  armPump();
  delay(50);
  goHome();
  //GRENADINE
}
void fuzzyNavel()
{
  stepBack(VODKA);
  delay(50);
  armPump();
  delay(50);
  stepBack(PEACH - VODKA);
  delay(50);
  stepBack(OJ - PEACH);
  delay(50);
  armPump();
  delay(50);
  armPump();
  delay(50);
  armPump();
  delay(50);
  goHome();
}

void madras()
{
  stepBack(TEQ);
  delay(50);
  armPump();
  delay(50);
  stepBack(OJ - TEQ);
  delay(50);
  armPump();
  delay(50);
  stepBack(PJ - OJ);
  delay(50);
  armPump();
  delay(50);
  armPump();
  delay(50);
  stepBack(CJ - PJ);
  delay(50);
  armPump();
  delay(50);
  goHome();
}

void vodkaCranberry()
{
  stepBack(VODKA);
  delay(50);
  armPump();
  delay(50);
  stepBack(OJ - VODKA);
  delay(50);
  armPump();
  delay(50);
  stepBack(CJ - OJ);
  delay(50);
  armPump();
  delay(50);
  armPump();
  delay(50);
  goHome();
  //LIME
}

void sexOnTheBeach()
{
  stepBack(VODKA);
  delay(50);
  armPump();
  delay(50);
  stepBack(PEACH - VODKA);
  delay(50);
  armPump();
  delay(50);
  stepBack(PJ - PEACH);
  delay(50);
  armPump();
  delay(50);
  armPump();
  delay(50);
  stepBack(CJ - PJ);
  delay(50);
  armPump();
  delay(50);
  goHome();
}

void cosmo()
{
  stepBack(VODKA);
  delay(50);
  armPump();
  delay(50);
  stepBack(TSEC - VODKA);
  delay(50);
  //half pump
  delay(50);
  stepBack(CJ - TSEC);
  delay(50);
  armPump();
  delay(50);
  armPump();
  delay(50);
  goHome();
  //LIME
}
          
void theAbyss()
{
  stepBack(RUM);
  delay(50);
  //half pump
  delay(50);
  stepBack(VODKA - RUM);
  delay(50);
  armPump();
  delay(50);
  stepBack(BLUE - VODKA);
  delay(50);
  //half pump
  delay(50);
  stepBack(PJ - BLUE);
  delay(50);
  armPump();
  delay(50);
  armPump();
  delay(50);
  goHome();
}
          
void bayBreeze()
{
  stepBack(VODKA);
  delay(50);
  armPump();
  delay(50);
  stepBack(PJ - VODKA);
  delay(50);
  armPump();
  delay(50);
  armPump();
  delay(50);
  stepBack(CJ - PJ);
  delay(50);
  armPump();
  delay(50);
  armPump();
  delay(50);
  goHome();
}
          
void soylentGreen()
{
  stepBack(VODKA);
  delay(50);
  armPump();
  delay(50);
  stepBack(BLUE - VODKA);
  delay(50);
  armPump();
  delay(50);
  stepBack(OJ - BLUE);
  delay(50);
  armPump();
  delay(50);
  armPump();
  delay(50);
  goHome();
}














