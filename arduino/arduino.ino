#include <SPI.h>
#include <SD.h>
#include <SoftwareSerial.h>
#include <TinyGPS.h>

TinyGPS gps;
SoftwareSerial swSerial(5, 6);

File file;

int const pinCS = 10;

int fileName = 0;

void setup() {
  Serial.begin(9600);
  swSerial.begin(9600);

  if (!SD.begin(pinCS))
  {
    Serial.println("SD card initialization failed!");
    SPI.end();
    pinMode(LED_BUILTIN, OUTPUT);
    while (1) {
      digitalWrite(LED_BUILTIN, HIGH);
      delay(200);
      digitalWrite(LED_BUILTIN, LOW);
      delay(200);
    }
  }

  Serial.println("SD card initialization success.");

  file = SD.open("config.txt");
  if (file)
  {
    while (file.available())
    {
      String line = readLine(file);
      fileName = line.toInt();
      Serial.println("Current filename:" + String(fileName) + ".CSV");
    }
    file.close();
  }
  else
  {
    Serial.println("File could not be opened for reading (config.txt)!");
  }

  SD.remove("config.txt");

  file = SD.open("config.txt", FILE_WRITE);
  if (file)
  {
    file.println(fileName + 1);
    file.close();
  }
  else
  {
    Serial.println("File could not be opened for writing (config.txt)!");
  }

  file = SD.open(String(fileName) + ".csv", FILE_WRITE);
  if (file)
  {
    file.print("lat");
    file.print(";");
    file.print("lon");
    file.print(";");
    file.print("altitude");
    file.print(";");
    file.print("speed");
    file.print(";");
    file.print("satelites");
    file.print(";");
    file.print("time");
    file.print("\n");

    file.close();
  }
  else
  {
   Serial.println("File could not be opened for writing (data)!");
  }




}

void loop() {
  bool gpsDataReady = false;
 
  for (unsigned long now = millis(); millis() - now < 1000;) {
    while (swSerial.available()) {
      char c = swSerial.read();
      //Serial.write(c);
      if (gps.encode(c)) {
        gpsDataReady = true;
      }
    }
  }

  if (gpsDataReady) {

    float lat, lon;
    unsigned long age;
    int year;
    byte month, day, hour, minute, second, setinaSekundy;

    gps.f_get_position(&lat, &lon, &age);

    file = SD.open(String(fileName) + ".csv", FILE_WRITE);

    gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &setinaSekundy, &age);

    char time[30];

    if (age == TinyGPS::GPS_INVALID_AGE) {
      Serial.println("GPS AGE error!");
    } else {
      sprintf(time, "%02d:%02d:%02d", hour, minute, second);
    }

    if (file)
    {
      lat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : lat, 6;
      lon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : lon, 6;

      static char latString[15];
      static char lonString[15];

      dtostrf(lat, -1, 6, latString);
      dtostrf(lon, -1, 6, lonString);

      file.print(latString);
      file.print(";");
      file.print(lonString);
      file.print(";");
      file.print(gps.f_altitude() == TinyGPS::GPS_INVALID_F_ALTITUDE ? 0 : gps.f_altitude());
      file.print(";");
      file.print(gps.f_speed_kmph() == TinyGPS::GPS_INVALID_F_SPEED ? 0 : gps.f_speed_kmph());
      file.print(";");
      file.print(gps.satellites() == TinyGPS::GPS_INVALID_SATELLITES ? 0 : gps.satellites());
      file.print(";");
      file.print(time);
      file.print("\n");

      file.close();
    }
    else
    {
    Serial.println("File could not be opened for writing (data)!");
    }

/*    
        Serial.println("------------------------------------------------------");
        Serial.print("Latitude: ");
        Serial.print(lat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : lat, 6);
        Serial.print(" Longitude: ");
        Serial.print(lon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : lon, 6);
        Serial.print(" Satelites: ");
        Serial.println(gps.satellites() == TinyGPS::GPS_INVALID_SATELLITES ? 0 : gps.satellites());
        Serial.print(" Accuracy: ");
        Serial.print(gps.hdop() == TinyGPS::GPS_INVALID_HDOP ? 0 : gps.hdop());
        Serial.print(" Altitude: ");
        Serial.print(gps.f_altitude() == TinyGPS::GPS_INVALID_F_ALTITUDE ? 0 : gps.f_altitude());
        Serial.print(" Speed km/h: ");
        Serial.println(gps.f_speed_kmph() == TinyGPS::GPS_INVALID_F_SPEED ? 0 : gps.f_speed_kmph());
        Serial.println(time);
*/
    

  }



}


String readLine(File file) {
  String line = "";
  char ch;
  while (file.available()) {
    ch = file.read();
    if (ch == '\n') {
      return String(line);
    } else {
      line += ch;
    }
  }
  return "";
}
