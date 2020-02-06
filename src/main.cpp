#include <Arduino.h>
#include <WiFi.h>


const char*  ssid ="Shreks andra Nätverk";
const char* password = "Pepparkakan";

void setup() {

Serial.begin(9600);

WiFi.begin(ssid,password);
Serial.println("Trying to connect to WiFi");
while(WiFi.status() !=WL_CONNECTED){
delay(500);
Serial.println(".");
}
Serial.println("Succesfully connected to WiFi");

}

void loop() {
  // put your main code here, to run repeatedly:
}