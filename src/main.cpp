#include <Arduino.h>
#include <WiFi.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>


#define SEALEVELPRESSURE_HPA (1013.25)

const char*  ssid ="Shreks andra Nätverk";
const char* password = "Pepparkakan";

Adafruit_BME280 bme;

void setup() {
Serial.begin(9600);



WiFi.begin(ssid,password);
Serial.println("");
Serial.println("Trying to connect to WiFi");
while(WiFi.status() !=WL_CONNECTED){
delay(500);
Serial.println(".");
}
Serial.println("Succesfully connected to WiFi");
//Starting internet connection

for(int x=0; x<=3; x++){
Serial.println(".");
delay(200);
}

Serial.println(F("BME280 Test"));
Serial.println();

bool status;

status=bme.begin();
if(!status){
  Serial.println("Can't find BME280 sensor");
  }

else{
  Serial.println("BME280 sensor found");
}

}

void loop() {
Serial.print("Temperature = ");
    Serial.print(bme.readTemperature());
    Serial.println(" *C");
delay(5000);

}

/*void printValues() {
    Serial.print("Temperature = ");
    Serial.print(bme.readTemperature());
    Serial.println(" *C");

    Serial.print("Pressure = ");

    Serial.print(bme.readPressure() / 100.0F);
    Serial.println(" hPa");

    Serial.print("Approx. Altitude = ");
    Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
    Serial.println(" m");

    Serial.print("Humidity = ");
    Serial.print(bme.readHumidity());
    Serial.println(" %");

    Serial.println();
}*/