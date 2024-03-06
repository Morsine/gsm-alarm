/*************************************************************************************
 *  Created By: Morsine
 ***********************************************************************************/
 
#include <SIM800L.h>

SIM800L sim800l(2, 3); //Rx, Tx
#define LED_PIN 13
bool armed = true;
int inputPin = 9;               // choose the input pin (for PIR sensor)
int pirState = LOW;             // we start, assuming no motion detected
int val = 0;                    // variable for reading the pin status

String PHONE = "+98XXXXXXX"; // your number here
void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  sim800l.begin(9600);
  pinMode(inputPin, INPUT);     // declare sensor as input
  sim800l.setSMSCallback(handleSMS);
  sim800l.setCallCallback(handleCall);
  pinMode(7, OUTPUT);
  digitalWrite(7, HIGH);
}


void handleSMS(String number, String message) {
  Serial.println("number: " + number + "\nMessage: " + message);
  if(message == "on") {
    digitalWrite(LED_PIN, HIGH);
  } 
  else if(message == "off") {
    digitalWrite(LED_PIN, LOW);
  }
}

void changestate(){
  Serial.println("TRIGGERED"); 
  sim800l.tryATcommand("ATH");
  if (armed == true) {
    armed = false;
    Serial.println("DISARMED");
    digitalWrite(7, LOW);
    return;
  }
  if (armed == false) {
    armed = true;
    Serial.println("ARMED");
    digitalWrite(7, HIGH);
    return;
  }
}

void handleCall(String number) {
  Serial.println("New call from " + number + "Armed: " + armed);
  boolean flag = digitalRead(LED_PIN);
  digitalWrite(LED_PIN, !flag);
  if (number == "+98XXXXXXX") { // your number here
    changestate();
  }
}

void loop() {
  sim800l.listen();

  val = digitalRead(inputPin);  // read input value
  if (val == HIGH) {            // check if the input is HIGH
    if (pirState == LOW) {
      // we have just turned on
      if (armed == true) {
        sim800l.makeCall(PHONE);
        delay(30000);
      }
      
      // We only want to print on the output change, not state
      pirState = HIGH;
    }
  } else {
    if (pirState == HIGH){
      // we have just turned of
      Serial.println("Motion ended!");
      if (armed == true) {
        sim800l.tryATcommand("ATH");
      }
      // We only want to print on the output change, not state
      pirState = LOW;
    }
  }
}
