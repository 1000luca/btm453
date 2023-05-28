
// #include <Wire.h>
#include <LiquidCrystal.h>
#include <LiquidCrystal_I2C.h>
// #include <WiFi.h>
// #include <HTTPClient.h>
#include <Keypad.h> 
#include "RTClib.h"

unsigned long seconds = 1000L; //Notice the L 
unsigned long minutes = seconds * 60;
const char* ssid = "..";
const char* password = "..";
const char* host = "tr7v02f635.execute-api.ap-northeast-2.amazonaws.com";
const char* path = "/dev/sms";

const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {9,8,7,6};
byte colPins[COLS] = {5,4,3,2};

Keypad keypad = Keypad(makeKeymap(keys),rowPins,colPins,ROWS,COLS);
// SoftwareSerial mySerial(10,11);

String phoneNumber = "";
String timeString = "";
int time = 0;

LiquidCrystal_I2C lcd(0x27,16,2);


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  // mySerial.begin(9600);
  // WiFi.begin(ssid,password);

  // while(WiFi.status() != WL_CONNECTED){
  //   delay(1000);
  //   Serial.println("Connecting to WiFi...");
  // }

  // Serial.println("Connected to WiFi!");

  lcd.begin(16,2);
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Welcome!");
  lcd.setCursor(0,1);
  lcd.print("phone number:   ");
}


enum State{
  Start,
  PhoneNumber,
  Time,
  Alarm
};

enum State state = Start;

void loop() {
  // put your main code here, to run repeatedly:
  char key = keypad.getKey();
  if(key != NO_KEY){
    Serial.println(key);
    switch(state){
      case Start:
        //print "Welcome"
        lcd.setCursor(0,0);
        lcd.print("Welcome!");
        if(key == 'A'){
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("Enter your");
          lcd.setCursor(0,1);
          lcd.print("phone number");
          state = PhoneNumber;
          // print "Enter Phonenumber"
        }else{
          state = Start;
        }
      break;

      case PhoneNumber:
        if(isDigit(key)){
          phoneNumber += key;
          // print phoneNumber
          lcd.clear();
          lcd.setCursor(0,1);
          lcd.print(phoneNumber);
          state = PhoneNumber;
        }else{
          if(key == 'A'){
            if(phoneNumber.length() > 11){
              // print "too many numbers, reset" 
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("too many!! reset");
              phoneNumber = "";
              lcd.setCursor(0,1);
              lcd.print(phoneNumber);
              state = PhoneNumber;
            }else if(phoneNumber.length()==11){
              state = Time;  
              // print "Enter Time"
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("Enter Time");
            }else{
              // print "too few, reset"
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("too few!! reset");
              phoneNumber = "";
              lcd.setCursor(0,1);
              lcd.print(phoneNumber);
              state = PhoneNumber;
            }
          }else if(key == '*'){
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Reset");
            phoneNumber = "";
            lcd.setCursor(0,1);
            lcd.print("Enter phone again");
            state = PhoneNumber;
          }else if(key == '#') {
            phoneNumber = phoneNumber.substring(0,phoneNumber.length()-1);
            lcd.clear();
            lcd.setCursor(0,1);
            lcd.print(phoneNumber);
            state = PhoneNumber;
          }else{
            state = PhoneNumber;
          }
        }
      break;

      case Time:
        if(isDigit(key)){
          timeString += key;
          lcd.clear();
          lcd.setCursor(0,1);
          lcd.print(timeString);
          state = Time;
        }else{
          if(key == 'A'){
            if(timeString.length() > 3){
              // print "too many time, reset"
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("too many!! reset");
              timeString = "";
              lcd.setCursor(0,1);
              lcd.print(timeString);
              state = Time;
            }else if(timeString.length() == 3){
              // print "Complete, Alarm start"
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("Press A to start");
              time = timeString.toInt();
              state = Alarm;
            }else{
              //print "too few time, reset"
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("too few!! reset");
              timeString = "";
              lcd.setCursor(0,1);
              lcd.print(timeString);
              timeString = "";
              state = Time;
            }
          }else if(key == '*'){
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("Reset");
            timeString = "";
            lcd.setCursor(0,1);
            lcd.print("Enter Time again");
            state = Time;
          }else if(key == '#'){
            timeString = timeString.substring(0, timeString.length()-1);
            lcd.clear();
            lcd.setCursor(0,1);
            lcd.print(timeString);
          }else{
            state = Time;
          }
        }
      break;

      case Alarm:
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Alarm Start!");
        Serial.println(timeString);
        Serial.println(time);
        unsigned long t = (unsigned long) time;
        delay(t * minutes);
        // send
        
        // alarm 
        // state = Start;

        Serial.println("Alarm!!");
      break;
    }
  }


}
