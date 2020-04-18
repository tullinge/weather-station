#include <Arduino.h>
#include <SparkFunBME280.h>
#include <SparkFunCCS811.h>
#include <Wire.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>

#define CCS811_ADDR 0x5B
//Global sensor objects
BME280 MyBME280;
CCS811 MyCCS811(CCS811_ADDR);

//wind sensor
const int windSensorPin = 32;

float voltageMin = 0.4;
float windSpeedMin = 0;

float voltageMax = 2.0;
float windSpeedMax = 32;

float windSpeed;

//Server connection +Json
HTTPClient http, http1;
int dot = 0;
String IP;
int errorCode;
int watchdogCount = 0;
int httpResponseCode;

//Network
const char *ssid = "Shreks andra Nätverk"; //Insert ssid here
const char *password = "Pepparkakan";      //Insert password

//Rain sensor
const int rainSensorPin = 27;
boolean isRaining = false;
String RAIN;

//LED
int ledPinGreen = 25;
int ledPinRed = 19;

void setup()
{
  Serial.begin(9600);

  delay(500);
  WiFi.begin(ssid, password);

  Serial.println();
  Serial.println("Trying to connect to WiFi");

  pinMode(ledPinGreen, OUTPUT);
  pinMode(ledPinRed, OUTPUT);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println(".");
    dot++;
    delay(500);
    if (dot == 12)
    {
      break;
    }
  }
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Failed to connect to WiFi");
    digitalWrite(ledPinRed, HIGH);
    while (1)
      ;
  }
  else
  {
    Serial.println("Succesfully connected to WiFi");
  }
  delay(500);
  Serial.println();
  Serial.println("Initializing I2C connection and BME280/CCS811 functionality");
  delay(500);
  Serial.println();
  Wire.begin(); //initialize I2C bus

  if (!MyCCS811.begin())
  {
    Serial.println("Problem with CCS811 ");
    digitalWrite(ledPinRed, HIGH);
    while (1)
      ;
  }
  else
  {
    Serial.println("CCS811 online");
  }

  //Setup for BME280
  MyBME280.settings.commInterface = I2C_MODE;
  MyBME280.settings.I2CAddress = 0x77;
  MyBME280.settings.runMode = 3;
  MyBME280.settings.tStandby = 0;
  MyBME280.settings.filter = 0;
  MyBME280.settings.tempOverSample = 1;
  MyBME280.settings.pressOverSample = 1;
  MyBME280.settings.humidOverSample = 1;

  delay(20); // BME280 requires 2ms to start up.

  if (!MyBME280.begin())
  {
    Serial.println("Problem with BME280");
    digitalWrite(ledPinRed, HIGH);
    while (1)
      ;
  }
  else
  {
    Serial.println("BME280 online");
  }
  Serial.println();
  Serial.println("Sensor operational!");
  Serial.println();
  Serial.println("Getting network IP-address");

  if (WiFi.status() == WL_CONNECTED)
  {
    http1.begin("http://api.ipify.org/");
    errorCode = http1.GET();
    IP = http1.getString();

    Serial.print("Status Code: ");
    Serial.println(errorCode);

    Serial.print("IP-address ");
    Serial.println(IP);

    http1.end();
    Serial.println();
    Serial.println("Everything ready!");
    for (int x = 0; x < 3; x++)
    {
      digitalWrite(ledPinGreen, HIGH);
      delay(100);
      digitalWrite(ledPinGreen, LOW);
      delay(100);
    }
    Serial.println();
  } //end if getting IP-address

} //End setup

void loop()
{
  String json; //json decleration
  isRaining = !(digitalRead(rainSensorPin));

  if (isRaining == true)
  {
    RAIN = "true";
  }
  else
  {
    RAIN = "false";
  }

  float sensorValue = analogRead(windSensorPin);
  float sensorVoltage = sensorValue * (0.4 / 340);

  if (sensorVoltage <= 0.42)
  {
    windSpeed = 0;
  }
  else
  {
    windSpeed = (sensorVoltage - voltageMin) * windSpeedMax / (voltageMax - voltageMin);
  }

  String WIND = String(windSpeed, DEC);

  delay(200);
  MyCCS811.readAlgorithmResults();
  delay(200);

  float BMEtempC = MyBME280.readTempC();
  String TEMP = String(BMEtempC, DEC);

  float BMEhumid = MyBME280.readFloatHumidity();
  String HUM = String(BMEhumid, DEC);

  MyCCS811.setEnvironmentalData(BMEhumid, BMEtempC);

  int BMEpres = MyBME280.readFloatPressure();
  String PRES = String(BMEpres, DEC);

  int CCS811CO2 = MyCCS811.getCO2();
  String CO2 = String(CCS811CO2, DEC);

  int CCS811TVOC = MyCCS811.getTVOC();
  String TVOC = String(CCS811TVOC, DEC);

  DynamicJsonDocument doc(2048);
  doc["Temp"] = TEMP;
  doc["Hum"] = HUM;
  doc["Press"] = PRES;
  doc["CO2"] = CO2;
  doc["TVOC"] = TVOC;
  doc["Rain"] = RAIN;
  doc["Wind"] = WIND;

  serializeJson(doc, json);

  if (WiFi.status() == WL_CONNECTED)
  {
    http.begin("http://" + IP + ":69/measurements");
    http.addHeader("Content-Type", "application/json");
    httpResponseCode = http.POST(json);
    if (httpResponseCode > 0)
    {
      digitalWrite(ledPinRed, LOW); //reseting error LED
      watchdogCount = 0;            //reseting watchdog

      Serial.print("Json sent: ");
      Serial.println(json);
      String response = http.getString(); //Get the response to the request
      Serial.print("Status code: ");
      Serial.println(httpResponseCode); //Print return code
      Serial.print("Response: ");
      Serial.println(response);        //Print request answer
      digitalWrite(ledPinGreen, HIGH); //Physical LED showing it is sent
      delay(3000);
      digitalWrite(ledPinGreen, LOW);
    }
    else
    {
      Serial.print("Error in sending POST: ");
      Serial.println(httpResponseCode);
    }

    http.end(); //Free resources
  }             //end wifi.status if
  else
  {
    Serial.println("Error in WiFi connection");
  }
  if (httpResponseCode != 200)
  {
    digitalWrite(ledPinRed, HIGH);
    watchdogCount++;
    Serial.println();
    Serial.print("***Failed to connect: ");
    Serial.print(watchdogCount);
    Serial.println(" / 3***");
    if (watchdogCount == 3)
    {
      digitalWrite(ledPinRed, LOW);
      delay(500);
      for (int x = 0; x < 3; x++)
      {
        digitalWrite(ledPinRed, HIGH);
        delay(100);
        digitalWrite(ledPinRed, LOW);
        delay(100);
      }

      ESP.restart();
    }
  } //End if watchdog

  delay(10000); //Wait for next reading

} //End void loop
