#include <SPI.h>
#include <SD.h>
#include <SoftwareSerial.h>
#include <TinyGPS.h>
#include "U8glib.h"

U8GLIB_SSD1306_128X64 odisplay(U8G_I2C_OPT_NONE);

TinyGPS gps;
SoftwareSerial GPSSerial(5, 6);

File file;
int const pinCS = 10;

String fileName = "null.csv";

unsigned long lastDraw = millis();


bool parseFileName();

float maxAlt = 0;
float altCal = 99999;

bool displayData = true;

void setup() {
  GPSSerial.begin(9600);

  if (!SD.begin(pinCS))
  {
    odisplay.firstPage();
    do {
      odisplay.setFont(u8g_font_unifont);
      odisplay.setPrintPos(35, 20);
      odisplay.print("SD KARTA");
      odisplay.setPrintPos(35, 40);
      odisplay.print("SELHALA");
    } while ( odisplay.nextPage() );

    while (1) {
      delay(1000);
    }
  }


  while (!parseFileName()) {
    odisplay.firstPage();
    do {
      odisplay.setFont(u8g_font_unifont);
      odisplay.setPrintPos(0, 20);
      odisplay.print("GPS inicializace");
      odisplay.setPrintPos(55, 40);
      odisplay.print(millis() / 1000);
    } while ( odisplay.nextPage() );
  }

  file = SD.open(fileName, FILE_WRITE);
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

}



bool parseFileName() {
  if (handleGPSData()) {
    float x;
    unsigned long stariDat;
    int rok;
    byte mesic, den, hodina, minuta, y;
    gps.f_get_position(&x, &x, &stariDat);
    gps.crack_datetime(&rok, &mesic, &den, &hodina, &minuta, &y, &y, &stariDat);

    if (stariDat != TinyGPS::GPS_INVALID_AGE) {
      char datumCas[13];
      sprintf(datumCas, "%02d%02d%02d%02d.csv", mesic, den, hodina, minuta);
      fileName = String(datumCas);
      return true;
    }
  }
  return false;
}


bool handleGPSData() {
  bool gpsDataReady = false;

  for (unsigned long now = millis(); millis() - now < 1000;) {
    while (GPSSerial.available()) {
      char c = GPSSerial.read();
      //Serial.write(c);
      if (gps.encode(c)) {
        gpsDataReady = true;
      }
    }

  }
  return gpsDataReady;
}




void reDrawDisplay(float altitude, unsigned short satelites) {

  if (altitude > altCal + 60 ) {
    if (displayData) {
      displayData = false;
      odisplay.firstPage();
      do {
      } while ( odisplay.nextPage() );
    }
    return;
  }

  displayData = true;
  odisplay.firstPage();
  do {
    odisplay.setFont(u8g_font_unifont);

    odisplay.setPrintPos(0, 10);
    odisplay.print("A:");
    odisplay.print(altitude);
    odisplay.print("|");
    odisplay.print(maxAlt);

    odisplay.setPrintPos(0, 25);
    odisplay.print("Sat:");
    odisplay.print(satelites);

    odisplay.setPrintPos(0, 40);
    odisplay.print(fileName);

    odisplay.setPrintPos(40, 55);
    odisplay.print(millis() / 1000);
    odisplay.print(" vterin");
  } while ( odisplay.nextPage() );
}

void writeGPSData() {

  float lat, lon;
  unsigned long age;
  int year;
  byte month, day, hour, minute, second, setinaSekundy;

  gps.f_get_position(&lat, &lon, &age);

  file = SD.open(fileName, FILE_WRITE);
  gps.crack_datetime(&year, &month, &day, &hour, &minute, &second, &setinaSekundy, &age);

  char time[9];

  if (age != TinyGPS::GPS_INVALID_AGE) {
    sprintf(time, "%02d:%02d:%02d", hour, minute, second);
  }

  if (file)
  {
    lat == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : lat, 6;
    lon == TinyGPS::GPS_INVALID_F_ANGLE ? 0.0 : lon, 6;

    file.print(lat * 1000);
    file.print(";");
    file.print(lon * 1000);
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


  if ((millis() / 1000) > 60 && altCal == 99999) {
    altCal = (gps.f_altitude() == TinyGPS::GPS_INVALID_F_ALTITUDE ? 0 : gps.f_altitude());
  }

  if (maxAlt < (gps.f_altitude() == TinyGPS::GPS_INVALID_F_ALTITUDE ? 0 : gps.f_altitude()) ) {
    maxAlt = gps.f_altitude() == TinyGPS::GPS_INVALID_F_ALTITUDE ? 0 : gps.f_altitude();
  }



  if (millis() - lastDraw > 5000)
  {
    lastDraw = millis();
    reDrawDisplay(gps.f_altitude() == TinyGPS::GPS_INVALID_F_ALTITUDE ? 0 : gps.f_altitude(),
                  gps.satellites() == TinyGPS::GPS_INVALID_SATELLITES ? 0 : gps.satellites() );
  }

}





void loop(void) {
  if (handleGPSData()) {



    writeGPSData();

  }
}
