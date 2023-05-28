
#include <Wire.h>
#include <LiquidCrystal.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <LiquidCrystal.h>
#include <Keypad.h> 


const char* ssid = "..";
const char* password = "..";
const char* host = "tr7v02f635.execute-api.ap-northeast-2.amazonaws.com";
const char* path = "/dev/sms";

const byte ROWS = 4;
const byte COLs = 3;
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = {9,8,7,6};
byte colPins[COLS] = {5,4,3};

Keypad keypad = Keypad(makeKeymap(keys),rowPins,colPins,ROWS,COLS);
SoftwareSerial mySerial(10,11);

String phoneNumber = "";
String time = "";

LiquidCrystal_I2C lcd(0x27,16,2);


void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  mySerial.begin(9600);
  WiFi.begin(ssid,password);

  while(WiFi.status() != WL_CONNECTED){
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi!");

  lcd.begin(16,2);
  lcd.print("Phone Number");
  lcd.setCursor(0,1);
  lcd.print("Time:");
}

void loop() {
  // put your main code here, to run repeatedly:
  char key = keypad.getKey();
  if(key ! = NO_KEY){
    if(key == '*'){
      phoneNumber = "";
      time = "";
      }
    else if(key == '#'){
      String url = "https://tr7v02f635.execute-api.ap-northeast-2.amazonaws.com/dev/sms?phoneNumber=" + phoneNumber;
      url += "&time=" + time;
      mySerial
    }
    else if(phoneNumber.length()<11){
      phoneNumber += key;
    }
    else if(time.length()<4){
      time += key;
    }
  }
}
