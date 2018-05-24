/*  Digital Clock with Alarm Function
 *  More info: http://www.ardumotive.com/
 *  Dev: Michalis Vasilakis Data: 19/11/2016 Ver: 1.0
 *  http://www.ardumotive.com/arduino-digital-clock-with-alarm-functionen.html
 *  Display 16x2:         Setup:            Setup Alarm
 *  +----------------+  +----------------+ +----------------+ 
 *  |HH:MM:SS|* HH:MM|  |    >HH :>MM    | |   Set Alarm    |
 *  |DD/MM/YY|  ALARM|  |>DD />MM />YYYY | |   >HH :>MM     |
 *  +----------------+  +----------------+ +----------------+
 */
// adaptat in priza programabila de Vlad Gheorghe

//Libraries
#include <Wire.h>
#include "RTClib.h"
#include <LiquidCrystal_I2C.h>
#include <EEPROM.h>

//Connections and constants 
//LiquidCrystal lcd(8,7,6,5,4,3); //LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);
RTC_DS1307 rtc; //DS1307 i2c
char daysOfTheWeek[7][12] = {"Sunday","Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
const int btSet = A0;
const int btAdj = A1;
const int btAlarm = A2;
const int buzzer = 13;


//Variables
int DD,MM,YY,H,M,S,set_state, adjust_state, alarm_state,AH,AM,AHo,AMo, shake_state;
//int shakeTimes=0;
int i =0;
int btnCount = 0;
String sDD;
String sMM;
String sYY;
String sH;
String sM;
String sS;
String aH="12";
String aM="00";
String aHo="12";
String aMo="00";
String alarm = " ";


//Boolean flags
boolean setupScreen = false;
boolean alarmON=true;
boolean turnItOn = false;
   
void setup() {
  //Init RTC and LCD library items
  rtc.begin();
  lcd.begin();
  //Set outputs/inputs
  pinMode(btSet,INPUT_PULLUP);
  pinMode(btAdj,INPUT_PULLUP);
  pinMode(btAlarm, INPUT_PULLUP);
  pinMode(buzzer, OUTPUT);
  //Check if RTC has a valid time/date, if not set it to 00:00:00 01/01/2018.
  //This will run only at first time or if the coin battery is low.
  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    // This line sets the RTC with an explicit date & time, for example to set
    // January 1, 2018 at 00:00am you would call:
   // rtc.adjust(DateTime(2018, 01, 01, 00, 00, 0)); 
  }
  delay(100);
  //Read alarm time from EEPROM memmory
  AH=EEPROM.read(0);
  AM=EEPROM.read(1);
  AHo=EEPROM.read(2);
  AMo=EEPROM.read(3);
  
  //Check if the numbers that you read are valid. (Hours:0-23 and Minutes: 0-59)
  if (AH>23){
    AH=0;
  }
  if (AM>59){
    AM=0;
  }
  if (AHo>23){
    AHo=0;
  }
  if (AMo>59){
    AMo=0;
  }
 
 
}

void loop() {
  readBtns();       //Read buttons 
  getTimeDate();    //Read time and date from RTC
  if (!setupScreen){
    lcdPrint();     //Normanlly print the current time/date/alarm to the LCD
  
    if (alarmON){
     callAlarm();   // and check the alarm if set on
    }
  }
  else{
    timeSetup();    //If button set is pressed then call the time setup function
  }
  
}

/*************** Functions ****************/
//Read buttons state
void readBtns(){
  set_state = digitalRead(btSet);
  adjust_state = digitalRead(btAdj);
  alarm_state = digitalRead(btAlarm);
//  EEPROM.read(4, aa);
  if(!setupScreen){
    if (alarm_state==LOW){
      if (alarmON){
        alarm=" ";
        alarmON=false;
        digitalWrite(buzzer, LOW);
     
      }
      else{
        alarm="*";
        alarmON=true;
        digitalWrite(buzzer, HIGH);
      
      }
     
      delay(500);
      
    }
    
  }
  if (set_state==LOW){
    if(btnCount<9){
      btnCount++;
      setupScreen = true;
        if(btnCount==1){
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("------SET------");
          lcd.setCursor(0,1);
          lcd.print("-TIME and DATE-");
          delay(2000);
          lcd.clear();
        }
    } 
    else{
      lcd.clear();
      rtc.adjust(DateTime(YY, MM, DD, H, M, 0)); //Save time and date to RTC IC
      EEPROM.write(0, AH);  //Save the alarm on hours to EEPROM 0
      EEPROM.write(1, AM);  //Save the alarm on minuted to EEPROM 1
       EEPROM.write(2, AHo);  //Save the alarm off hours to EEPROM 0
      EEPROM.write(3, AMo);  //Save the alarm  off minuted to EEPROM 1
     
      lcd.print("Saving....");
      delay(2000);
      lcd.clear();
      setupScreen = false;
      btnCount=0;
    }
    delay(500);
  }
}

//Read time and date from rtc ic
void getTimeDate(){
  if (!setupScreen){
    DateTime now = rtc.now();
    DD = now.day();
    MM = now.month();
    YY = now.year();
    H = now.hour();
    M = now.minute();
    S = now.second();
  }
  //Make some fixes...
  if (DD<10){ sDD = '0' + String(DD); } else { sDD = DD; }
  if (MM<10){ sMM = '0' + String(MM); } else { sMM = MM; }
  sYY=YY-2000;
  if (H<10){ sH = '0' + String(H); } else { sH = H; }
  if (M<10){ sM = '0' + String(M); } else { sM = M; }
  if (S<10){ sS = '0' + String(S); } else { sS = S; }
  if (AH<10){ aH = '0' + String(AH); } else { aH = AH; }
  if (AM<10){ aM = '0' + String(AM); }  else { aM = AM; }
  if (AHo<10){ aHo = '0' + String(AHo); } else { aHo = AHo; }
  if (AMo<10){ aMo = '0' + String(AMo); }  else { aMo = AMo; }
 
}
//Print values to the display
void lcdPrint(){
  String line1 = sH+":"+sM+":"+sS+" P "+aH+":"+aM;  //ora +ora start
  //String line2 = sDD+"/"+sMM+"/"+sYY +" P "+alarm;
   String line2 = sDD+"-"+sMM+"-"+sYY +" O "+aHo+":"+aMo; //data +ora stop
  
  lcd.setCursor(0,0); //First row
  lcd.print(line1);
  lcd.setCursor(0,1); //Second row
  lcd.print(line2);  
}

//Setup screen
void timeSetup(){
  int up_state = adjust_state;
  int down_state = alarm_state;
  if(btnCount<=5){
    if (btnCount==1){         //Set Hour
      lcd.setCursor(4,0);
      lcd.print(">"); 
      if (up_state == LOW){   //Up button +
        if (H<23){
          H++;
        }
        else {
          H=0;
        }
        delay(350);
      }
      if (down_state == LOW){ //Down button -
        if (H>0){
          H--;
        }
        else {
          H=23;
        }
        delay(350);
      }
    }
    else if (btnCount==2){      //Set  Minutes
      lcd.setCursor(4,0);
      lcd.print(" ");
      lcd.setCursor(9,0);
      lcd.print(">");
      if (up_state == LOW){
        if (M<59){
          M++;
        }
        else {
          M=0;
        }
        delay(350);
      }
      if (down_state == LOW){
        if (M>0){
          M--;
        }
        else {
          M=59;
        }
        delay(350);
      }
    }
    else if (btnCount==3){      //Set Day
      lcd.setCursor(9,0);
      lcd.print(" ");
      lcd.setCursor(0,1);
      lcd.print(">");
      if (up_state == LOW){
        if (DD<31){
          DD++;
        }
        else {
          DD=1;
        }
        delay(350);
      }
      if (down_state == LOW){
        if (DD>1){
          DD--;
        }
        else {
          DD=31;
        }
        delay(350);
      }
    }
    else if (btnCount==4){      //Set Month
      lcd.setCursor(0,1);
      lcd.print(" ");
      lcd.setCursor(5,1);
      lcd.print(">");
      if (up_state == LOW){
        if (MM<12){
          MM++;
        }
        else {
          MM=1;
        }
        delay(350);
      }
      if (down_state == LOW){
        if (MM>1){
          MM--;
        }
        else {
          MM=12;
        }
        delay(350);
      }
    }
    else if (btnCount==5){      //Set Year
      lcd.setCursor(5,1);
      lcd.print(" ");
      lcd.setCursor(10,1);
      lcd.print(">");
      if (up_state == LOW){
        if (YY<2099){
          YY++;
        }
        else {
          YY=2017;
        }
        delay(350);
      }
      if (down_state == LOW){
        if (YY>2018){
          YY--;
        }
        else {
          YY=2999;
        }
        delay(350);
      }
    }
    lcd.setCursor(5,0);
    lcd.print(sH);
    lcd.setCursor(8,0);
    lcd.print(":");
    lcd.setCursor(10,0);
    lcd.print(sM);
    lcd.setCursor(1,1);
    lcd.print(sDD);
    lcd.setCursor(4,1);
    lcd.print("/");
    lcd.setCursor(6,1);
    lcd.print(sMM);
    lcd.setCursor(9,1);
    lcd.print("/");
    lcd.setCursor(11,1);
    lcd.print(sYY);
  }
  else{
    setAlarmTime();
    setAlarmOffTime();
  }
}

//Set alarm time
void setAlarmTime(){
  int up_state = adjust_state;
  int down_state = alarm_state;
  String line2;
  lcd.setCursor(0,0);
  lcd.print("SET          on");
  if (btnCount==6){             //Set alarm Hour
    if (up_state == LOW){
      if (AH<23){
        AH++;
      }
      else {
        AH=0;
      }
      delay(350);
    }
    if (down_state == LOW){
      if (AH>0){
        AH--;
      }
      else {
        AH=23;
      }
      delay(350);
    }
    line2 = "    >"+aH+" : "+aM+"    ";
  }
  else if (btnCount==7){        //Set alarm Minutes
    if (up_state == LOW){
      if (AM<59){
        AM++;
      }
      else {
        AM=0;
      }
      delay(350);
    }
    if (down_state == LOW){
      if (AM>0){
        AM--;
      }
      else {
        AM=59;
      }
      delay(350);
    }
    line2 = "     "+aH+" :>"+aM+"    ";    
  }
  lcd.setCursor(0,1);
  lcd.print(line2);
  
  
}

void setAlarmOffTime(){
  int up_state = adjust_state;
  int down_state = alarm_state;
  String line2;
  lcd.setCursor(0,0);
  lcd.print("SET         off");
  if (btnCount==8){             //Set alarm Hour
    if (up_state == LOW){
      if (AHo<23){
        AHo++;
      }
      else {
        AHo=0;
      }
      delay(350);
    }
    if (down_state == LOW){
      if (AHo>0){
        AHo--;
      }
      else {
        AHo=23;
      }
      delay(350);
    }
    line2 = "    >"+aHo+" : "+aMo+"    ";
  }
  else if (btnCount==9){        //Set alarm Minutes
    if (up_state == LOW){
      if (AMo<59){
        AMo++;
      }
      else {
        AMo=0;
      }
      delay(350);
    }
    if (down_state == LOW){
      if (AMo>0){
        AMo--;
      }
      else {
        AMo=59;
      }
      delay(350);
    }
    line2 = "     "+aHo+" :>"+aMo+"    ";    
  }
  lcd.setCursor(0,1);
  lcd.print(line2);
}


void callAlarm(){
  if (aM==sM && aH==sH && S>=0 && aH==sH&& S<=2){
    turnItOn = true;
  }
   if (aMo==sM && aHo==sH && S>=0 && aHo==sH&& S<=2){
    turnItOn = false;
  }
 
 
  if (turnItOn){
  
  digitalWrite(buzzer, HIGH);
  }
  else{
 
   digitalWrite(buzzer, LOW);
  }
}

