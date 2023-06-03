#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <Keypad.h> 
#include "RTClib.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>


#define STA
#define I2C_SDA 21
#define I2C_SCL 20

unsigned long seconds = 1000L; //Notice the L 
unsigned long minutes = seconds * 60;

const char* ssid = "Admin";
const char* password = "12345678";
String host = "https://www.tr7v02f635.execute-api.ap-northeast-2.amazonaws.com";
String path = "/dev/sms";

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

String phoneNumber = "";
String timeString = "";
int timeInt = 0;
// Wire.begin(I2C_SDA, I2C_SCL);
LiquidCrystal_I2C lcd(0x27,16,2);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  Serial.begin(115200);
  WiFi.begin(ssid,password);

  while(WiFi.status() != WL_CONNECTED){
    // WiFi.begin(ssid,password);
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi!");
  Serial.println(WiFi.localIP());

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Welcome!");
  lcd.setCursor(0,1);
  lcd.print("phone number:   ");
  Serial.println();

}


enum State{
  Start,
  PhoneNumber,
  Time,
  Alarm
};

enum State state = Start;


int sendRequest(String phoneNumber){
  if(WiFi.status() == WL_CONNECTED){
    WiFiClient client;
    HTTPClient http;
    String url = host + path + "?phoneNumber=" + phoneNumber;
    Serial.println(url);
    http.begin(client, url.c_str());

    int httpResponseCode = http.GET();
    if(httpResponseCode > 0){
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      String payload = http.getString();
      Serial.println(payload);
      http.end();
      return 1;
    }else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
      http.end();
      return -1;
    }
  }else{
    Serial.println("WiFi Disconnected");
    return -1;
  }
}

void loop() {
  // Serial.println("I'm alive");
  // put your main code here, to run repeatedly:
  char key = keypad.getKey();
  // Serial.println(key);
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
              timeInt = timeString.toInt();
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
        Serial.println(timeInt);
        unsigned long t = (unsigned long) timeInt;
        delay(t * minutes);
        // send
        if(sendRequest(phoneNumber)){
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("SMS Send");
          lcd.setCursor(0,1);
          lcd.print("Complete");
        }else{
          lcd.clear();
          lcd.setCursor(0,0);
          lcd.print("SMS Send");
          lcd.setCursor(0,1);
          lcd.print("Failed");
        }
        delay(10000);
        // alarm 
        // state = Start;
        Serial.println("Alarm!!");
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Welcome!");
        state = Start;
        phoneNumber = "";
        timeString = "";
        timeInt = 0;
      break;
    }
  }
}
