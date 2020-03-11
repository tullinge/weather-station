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
HTTPClient http;

const char *ssid = "Shreks andra Nätverk";
const char *password = "Pepparkakan";

void setup()
{

  Serial.begin(9600);

  WiFi.begin(ssid, password);
  Serial.println("");
  Serial.println("Trying to connect to WiFi");

  int dot = 0;

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println(".");
    dot++;
    delay(500);
    if (dot == 24)
    {
      break;
    }
  }
  if (WiFi.status() != WL_CONNECTED)
  {
    Serial.println("Failed to connect to WiFi");
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
    while (1)
      ;
  }
  else
  {
    Serial.println("BME280 online");
  }
  Serial.println("Sensor operational!");
  Serial.println();

} //End setup

void loop()
{
  String json;
  if (MyCCS811.dataAvailable())
  {
    //Calling this function updates the global tVOC and eCO2 variables
    MyCCS811.readAlgorithmResults();
    //printData fetches the values of tVOC and eCO2
    //printData();

    float BMEtempC = MyBME280.readTempC();
    String TEMP = String(BMEtempC, DEC);

    float BMEhumid = MyBME280.readFloatHumidity();
    String HUM = String(BMEhumid, DEC);

    //altering values from set enviromentaldata^^

    int BMEalt = MyBME280.readFloatAltitudeMeters();
    String ALT = String(BMEalt, DEC);

    int BMEpres = MyBME280.readFloatPressure();
    String PRES = String(BMEpres, DEC);

    int CCS811CO2 = MyCCS811.getCO2();
    String CO2 = String(CCS811CO2, DEC);

    int CCS811TVOC = MyCCS811.getTVOC();
    String TVOC = String(CCS811TVOC, DEC);

    DynamicJsonDocument doc(2048);
    doc["temperature"] = TEMP;
    doc["Humidity"] = HUM;
    doc["Altitude"] = ALT;
    doc["Pressure"] = PRES;
    doc["CO2"] = CO2;
    doc["TVOC"] = TVOC;

    serializeJson(doc, json);
    Serial.println(json);

    MyCCS811.setEnvironmentalData(BMEhumid, BMEtempC);
  }
  else if (MyCCS811.checkForStatusError())
  {
    Serial.println(MyCCS811.getErrorRegister()); //Prints whatever CSS811 error flags are detected
  }

  if (WiFi.status() == WL_CONNECTED)
  {
    delay(500);
    http.begin("http://192.168.10.127:5000/insert");
    delay(500);                                         //Specify destination for HTTP request
    http.addHeader("Content-Type", "application/json"); //Specify content-type header
    int httpResponseCode = http.POST(json);             //Send the actual POST request

    if (httpResponseCode > 0)
    {

      String response = http.getString(); //Get the response to the request

      Serial.println(httpResponseCode); //Print return code
      Serial.println(response);         //Print request answer
    }
    else
    {

      Serial.print("Error on sending POST: ");
      Serial.println(httpResponseCode);
    }

    http.end(); //Free resources
  }
  else
  {

    Serial.println("Error in WiFi connection");
  }

  delay(5000); //Wait for next reading

} //End void loop
