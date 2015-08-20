/*
 * TimeRTC.pde
 * example code illustrating Time library with Real Time Clock.
 * 
 */

#include <Time.h>  
#include <Wire.h>  
#include <DS1307RTC.h>  // a basic DS1307 library that returns time as a time_t
#include <TimeAlarms.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define Pin 2
#define fan 8
#define CO2 9

int pinWhite=5;
int pinRed = 6;
int pinBlue = 7;

// Set the LCD address to 0x27 for a 16 chars and 2 line display
LiquidCrystal_I2C lcd(0x27, 16, 2);

OneWire ourWire(Pin);
DallasTemperature sensors(&ourWire);

void setup()  {
  Serial.begin(9600);
  //while (!Serial) ; // wait until Arduino Serial Monitor opens
  setSyncProvider(RTC.get);   // the function to get the time from the RTC
  if(timeStatus()!= timeSet) 
     Serial.println("Unable to sync with the RTC");
  else
     Serial.println("RTC has set the system time");
  //Alarm.timerRepeat(60, digitalClockDisplay);
  pinMode(pinWhite,OUTPUT);
  pinMode(pinRed,OUTPUT);
  pinMode(pinBlue,OUTPUT);
  pinMode(CO2,OUTPUT);
  analogWrite(pinWhite,0);
  analogWrite(pinRed,0);
  analogWrite(pinBlue,0); 
  lcd.begin();
  lcd.backlight();
  sensors.begin();
  Alarm.alarmRepeat(6,0,0, turnOnCO2); //Turn CO2 on, every day at 6 am.
  Alarm.alarmRepeat(18,0,0, turnOffCO2); //Turn CO2 on, every day at 6 pm.
  checkCO2();
}

void loop()
{
  checkLight();
  updateLCD();
  digitalClockDisplay();
  Alarm.delay(1000);
}

void updateLCD(){
  sensors.requestTemperatures();
  // digital clock display of the time
  lcd.setCursor(0,0);
  lcd.print(hour());
  printDigitsLCD(minute());
//  printDigitsLCD(second());
  lcd.print(" ");
  lcd.print(day());
  lcd.print("/");
  lcd.print(month());
  lcd.print("/");
  lcd.print(year()); 
  lcd.setCursor(0,1);
  lcd.print(sensors.getTempCByIndex(0));
  lcd.print(" Celcius");
}

void printDigitsLCD(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  lcd.print(":");
  if(digits < 10)
    lcd.print('0');
  lcd.print(digits);
}

void digitalClockDisplay(){
  // digital clock display of the time
  Serial.print(hour());
  printDigits(minute());
  printDigits(second());
  Serial.print(" ");
  Serial.print(day());
  Serial.print(" ");
  Serial.print(month());
  Serial.print(" ");
  Serial.print(year()); 
  Serial.println(); 
}

void printDigits(int digits){
  // utility function for digital clock display: prints preceding colon and leading 0
  Serial.print(":");
  if(digits < 10)
    Serial.print('0');
  Serial.print(digits);
}

void checkLight(){
  int eMin = ((hour())*60)+minute();
  if (hour() == 23){
    eMin = minute();
  } 
  //Serial.print("eMin ");
  //Serial.println(eMin);
  int val;
  if(eMin >= 360 && eMin <= 539){ //6am - 9am
    Serial.println("Amanecer");
    val=map(eMin,360,539,0,255);
    Serial.print("White - ");
    Serial.println(val);
    analogWrite(pinWhite,val);
    val=map(eMin,360,539,10,100);
    Serial.print("Red - ");
    Serial.println(val);
    analogWrite(pinRed,val);
    analogWrite(pinBlue,0);
    analogWrite(fan, 0);
  }else if(eMin >= 540 && eMin <= 720){ //9am - 12pm
    Serial.println("Luz de dia");
    analogWrite(pinWhite,255);
    val=map(eMin,540,720,100,255);
    analogWrite(pinRed,val);
    analogWrite(pinBlue,0);
    analogWrite(fan, 150);
    //Serial.print("White - ");
    //Serial.println(val);
  }else if(eMin >= 721 && eMin <= 1079){ //12pm - 6pm
    Serial.println("Luz de dia2");
    analogWrite(pinWhite,255);
    analogWrite(pinRed,255);
    analogWrite(pinBlue,0);
    analogWrite(fan, 175);
  }else if(eMin >= 1080 && eMin <= 1109){ //6pm - 6:30pm
    Serial.println("Atardecer");
    val=map(eMin,1080,1109,255,0);
    analogWrite(pinRed,val);
    //Serial.println(val);
    val=map(eMin,1080,1109,255,10); 
    analogWrite(pinWhite,val);
    analogWrite(pinBlue,0);
    analogWrite(fan, 150);
  }else if(eMin >= 1110 && eMin <= 1199){ //6_30pm - 8pm
    Serial.println("Anochecer");
    val=map(eMin,1110,1199,10,0); 
    analogWrite(pinWhite,val);
    val=map(eMin,1110,1199,0,50); 
    analogWrite(pinBlue,val);
    analogWrite(pinRed,0);
    analogWrite(fan, 0);
  }else if(eMin >= 1200 && eMin <= 1319){ //8pm - 10pm
    Serial.println("Anochecer2");
    analogWrite(pinWhite,0);
    analogWrite(pinBlue,50);
    analogWrite(pinRed,0);
    analogWrite(fan, 0);
  }else if(eMin >= 1320 && eMin <= 1438){ //10pm - 11:59pm
    Serial.println("Noche");
    val=map(eMin,1320,1430,50,10);
    analogWrite(pinBlue,val); 
    analogWrite(pinWhite,0); 
    analogWrite(pinRed,0); 
    analogWrite(fan, 0);
  }else if(eMin >= 0 && eMin <= 359){ //0am - 6am
    val=map(eMin,330,359,10,0);
    analogWrite(pinBlue,val);
    analogWrite(pinWhite,0); 
    analogWrite(pinRed,0); 
    analogWrite(fan, 0);
  }
}

void turnOnCO2(){
  digitalWrite(CO2,HIGH);
}

void turnOffCO2(){
  digitalWrite(CO2,LOW);
}

void checkCO2(){
  if((hour()>=6) && (hour()<=18)){
    turnOnCO2();
  }else{
    turnOffCO2();
  }
}
