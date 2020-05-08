# Weather Station
![](Extras/weather-station.jpg)
![](Extras/Schematics.png)
## Requirements
- VS Code
- C++
- platformIO 
- C++ libraties (Adafruit CCS811 Library, ArduinoJson, SparkFun BME280, SparkFun CCS811 Arduino Library)

## Instructions
**Code (for troubleshooting and setup)** 

1. Install VS Code and C++
2. Install platformIO in the extensions tab
3. Install the necessary libraries from requirements
4. Clone repository and upload the code to the ESP32 with “Upload and Monitor” in the “PROJECT TASKS” drop down menu in the platformIO tab

**Operating physical weather station**

1. Plug in USB to a 5 volt power source
2. Check for the green LED light to flash 3 times followed by one long. (If the red LED ever start to shine an error has occurred)


## Troubleshooting

- 3 green flashes = setup completed
- Green + Red shine = Trying to connect to wifi
- 1 long green flash = http post request successful

If the red LED starts to shine after giving the weather station power then 3 possible errors has occurred.
1. Can't connect to wireless network.
2. Error in [BME280](https://www.bosch-sensortec.com/products/environmental-sensors/humidity-sensors-bme280/) sensor.
3. Error in [CCS811](https://cdn.sparkfun.com/assets/learn_tutorials/1/4/3/CCS811_Datasheet-DS000459.pdf) sensor.

If the red LED starts to shine after the 3 green flashes then the weather station has issues connecting to [REST API](https://github.com/tullinge/weather-station-api).

## Contributors
- Oskar Löf (JoeMamasXD) <Oskar.lof@skola.botkyrka.se>
- Max Lundberg (Fajterman) <Max.lunberg@skola.botkyrka.se>

