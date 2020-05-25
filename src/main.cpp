#include <Arduino.h>
#include <SparkFunBME280.h>
#include <SparkFunCCS811.h>
#include <Wire.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <HTTPClient.h>
#include <base64.h>

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
HTTPClient http;
int dot = 0;
int watchdogCount = 0;
int httpResponseCode;
boolean successfulConnection;
String authUsername = ""; //Insert Admin authorization
String authPassword = ""; // Insert Admin password
String IP = "";           //Insert IP Address
String auth = base64::encode(authUsername + ":" + authPassword);

//Network
const char *ssid = "";     //Insert ssid
const char *password = ""; //Insert password

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
    digitalWrite(ledPinRed, HIGH);
    digitalWrite(ledPinGreen, HIGH);

    delay(500);
    Serial.println(".");
    dot++;
    delay(500);
    if (WiFi.status() == WL_CONNECTED)
    {
      digitalWrite(ledPinRed, LOW);
      digitalWrite(ledPinGreen, LOW);
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
  Serial.println("Everything ready!");
  for (int x = 0; x < 3; x++)
  {
    digitalWrite(ledPinGreen, HIGH);
    delay(100);
    digitalWrite(ledPinGreen, LOW);
    delay(100);
  }
  Serial.println();
  //end if getting IP-address

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

  float sensorADCValue = analogRead(windSensorPin);
  float sensorVoltage = sensorADCValue / (4095 / 2); //ADC to voltage
  //0.0 volt = 0.0 ADC
  //2.0 volt = 4095 ADC

  if (sensorVoltage <= voltageMin)
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

  http.begin("http://" + IP + ":69/measurements");
  http.addHeader("Content-Type", "application/json");
  http.addHeader("Authorization", "Basic " + auth);
  delay(250);
  httpResponseCode = http.POST(json);
  if (httpResponseCode > 0)
  {
    digitalWrite(ledPinRed, LOW); //reseting error LED
    watchdogCount = 0;            //reseting watchdog
    successfulConnection = true;
    Serial.print("Json sent: ");
    Serial.println(json);
    String response = http.getString(); //Get the response to the request
    Serial.print("Status code: ");
    Serial.println(httpResponseCode); //Print return code
    Serial.print("Response: ");
    Serial.println(response);        //Print request answer
    digitalWrite(ledPinGreen, HIGH); //Physical LED showing data has been sent
    delay(3000);
    digitalWrite(ledPinGreen, LOW);
  }
  else
  {
    Serial.print("Error in sending POST: ");
    Serial.println(httpResponseCode);
  }

  http.end(); //Free resources

  if (httpResponseCode != 200)
  {
    successfulConnection = false;
    digitalWrite(ledPinRed, HIGH);
    watchdogCount++;
    Serial.println();
    Serial.print("***Failed to connect: ");
    Serial.print(watchdogCount);
    Serial.println(" / 3***");
    if (watchdogCount == 3)
    {
      delay(900000);
      digitalWrite(ledPinRed, LOW);
      for (int x = 0; x < 3; x++)
      {
        digitalWrite(ledPinRed, HIGH);
        delay(100);
        digitalWrite(ledPinRed, LOW);
        delay(100);
      }

      ESP.restart();
    } //End if watchdog
  }   //End if response != 200
      //Wait for next reading

  if (successfulConnection == true)
  {
    delay(900000);
  }
  else
  {
    delay(60000);
  }

} //End void loop
