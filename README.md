# weather-station
Instruktioner för att operera väderstationen:

För att redigera koden bör man använda visual studio med extensionen platformio. Med denna extension måste man ladda ned 4 st libraries: Adafruit CCS811 Library, ArduinoJson, SparkFun BME280, SparkFun CCS811 Arduino Library. För att få väderstationen att ansluta till internet måste man skriva in sin ssid och lösenord på anvisad plats i koden. För att ladda upp koden till Esp32an trycker man på PIO-ikonen och sedan “project tasks” till “Upload and monitor”. Detta kommer öppna en terminal i Visual Studio där man ser vad som printas till terminalen när Esp32an är kopplad till datorn.

Väderstationen kommunicerar med APIen via den publika IP-adress som nätverket för APIen har. Om båda är på samma nätverk behöver man inte ändra något i koden då väderstationen använder sig av en API som kallas Ipify för att hämta den publika IP-adressen. Detta går att ändra genom att ta bort koden för att hämta IP-adressen och hårdkoda den man vill använda om REST-APIen t.ex ska ligga i molnen.

Från den fysiska lådan kommer en blå USB-sladd som man förser med ström för att sätta på väderstationen.
