// #include <ESP8266WiFi.h>
// #include <ESP8266HTTPClient.h>
// #include <WiFiClient.h>
#include <Keypad.h> 
#include "RTClib.h"
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

SoftwareSerial mySerial(2,3); //RX,TX

unsigned long seconds = 1000L; //Notice the L 
unsigned long minutes = seconds * 60;

String ssid = "Admin";
String password = "12345678";
String host = "3.39.234.65";
String path = "/dev/sms";

const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {11,10,9,8};
byte colPins[COLS] = {7,6,5,4};

Keypad keypad = Keypad(makeKeymap(keys),rowPins,colPins,ROWS,COLS);

String phoneNumber = "";
String timeString = "";
int timeInt = 0;
// Wire.begin(I2C_SDA, I2C_SCL);
LiquidCrystal_I2C lcd(0x27,16,2);

int connectWifi(){
  String join ="AT+CWJAP=\""+ssid+"\",\""+password+"\"";
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Connect Wifi...");
  Serial.println("Connect Wifi...");
  Serial.println(join);
  mySerial.println(join);

  // while(1){
  //   if(mySerial.available())
  //   {
  //     Serial.write(mySerial.read());
  //   }
  //   if(Serial.available())
  //   {
  //     mySerial.write(Serial.read());
  //   }
  // }
  delay(10000);
  // Serial.println(mySerial.read());
  if(mySerial.find("WIFI CONNECTED"))
  {
    Serial.print("WIFI connect\n");
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("WIFI connect");
    delay(5000);
    return 1;
  }else
  {
   Serial.println("connect timeout\n");   
   lcd.clear(); 
   lcd.setCursor(0,0);
   lcd.print("connect timeout");
   delay(5000);
   return 0;
  }
}



void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  mySerial.begin(9600);
  lcd.init();
  lcd.backlight();

  while(1){
    if(connectWifi()) break;
  }
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Welcome!");
  lcd.setCursor(0,1);
  lcd.print("phone number:   ");
  Serial.println();

}

int httpclient(String phoneNumber){
  delay(100);
  Serial.println("connect TCP...");
  mySerial.println("AT+CIPSTART=\"TCP\",\""+host+"\",3000");
  delay(500);
  if(Serial.find("ERROR")) return 0;
  
  Serial.println("Send data...");
  // String url = host + path + "?phoneNumber=" +"82"+ phoneNumber;
  String cmd = "GET /sms?phoneNumber=82" + phoneNumber + " HTTP/1.0\r\n\r\n";
  mySerial.print("AT+CIPSEND=");
  mySerial.println(cmd.length());
  Serial.print("AT+CIPSEND=");
  Serial.println(cmd.length());
  if(mySerial.find("OK"))
  {
    Serial.print(">");
  }else
  {
    mySerial.println("AT+CIPCLOSE");
    Serial.println("connect timeout");
    delay(1000);
    return 0;
  }
  delay(500);
       
  mySerial.println(cmd);
  Serial.println(cmd);
  delay(100);
  if(Serial.find("ERROR")) return 0;
  mySerial.println("AT+CIPCLOSE");
  delay(100);
}  



enum State{
  Start,
  PhoneNumber,
  Time,
  Alarm
};

enum State state = Start;


// int sendRequest(String phoneNumber){
//   if(WiFi.status() == WL_CONNECTED){
//     WiFiClient client;
//     HTTPClient http;
//     String url = host + path + "?phoneNumber=" + phoneNumber;
//     Serial.println(url);
//     http.begin(client, url.c_str());

//     int httpResponseCode = http.GET();
//     if(httpResponseCode > 0){
//       Serial.print("HTTP Response code: ");
//       Serial.println(httpResponseCode);
//       String payload = http.getString();
//       Serial.println(payload);
//       http.end();
//       return 1;
//     }else {
//       Serial.print("Error code: ");
//       Serial.println(httpResponseCode);
//       http.end();
//       return -1;
//     }
//   }else{
//     Serial.println("WiFi Disconnected");
//     return -1;
//   }
// }

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
        if(httpclient(phoneNumber)){
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
