
#include <Wire.h>
#include <LiquidCrystal.h>
#include <ThreeWire.h>
#include <RtcDS1302.h>
#include <Keypad.h>

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
      mySerial.println("AT+
    }
    else if(phoneNumber.length()<11){
      phoneNumber += key;
    }
    else if(time.length()<4){
      time += key;
    }
  }
}
