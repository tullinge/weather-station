# Weather Station
## Requirements
- VS Code
- C++
- platformIO 
- C++ libraties (Adafruit CCS811 Library, ArduinoJson, SparkFun BME280, SparkFun CCS811 Arduino Library)

## Instructions

To use the source code it is recommended to use Visual Studios with the extension PlatformIO. In PlatformIO you need to download the four required libraries. To upload actual code to the Weather station you need to click on the platformIO icon on the left most panel (ant shaped) and navigate to “project tasks” and then “Upload and monitor”. This opens a terminal showing you what is uploaded to the ESP32 and when thats is completed, what's printed by the weather station in the terminal. For the Weather station to be able to connect to the internet you need to insert your network ssid and your password in the code by the “ssid” and “password” const char. The weather station communicates with the API using the public IP-address of the API. This is automatically changed in the code using an API called Ipify but this can be changed to a static and hard coded IP-address that is not on the same network. To use the automatic IP inserter the weather station and the weather station API need to be connected to the same network. If the API is uploaded to a cloud based service you simply remove the Ipify part of the code and insert the IP-address of the cloud server.

Operating the weather station is very simply and only requires you to give it power through the USB contact and it starts running. The 2 LEDs on the side shows if it’s running correctly. The green indicates that the data collected is sent to the API with a 200 response. If the red LED is lit then something wrong has happened and the weather station doesn’t work. This can either be the climate sensor or an error in sending the data to the API.  


## Contributors
- Oskar Löf (JoeMamasXD) <Oskar.lof@skola.botkyrka.se>
- Max Lundberg (Fajterman) <Max.lunberg@skola.botkyrka.se>

