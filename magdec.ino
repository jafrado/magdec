#include "magdec.h"
#include "magdec2.c"

extern int years[];
extern location_t locations[] ;
extern int n_locations;
extern int n_years;

void setup() {  
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(9600);
  while (!Serial); // Wait for the Serial Monitor to open
  Serial.println("Setup complete and connected.");
}

void loop() {
  unsigned long startTime;
  unsigned long endTime;
  unsigned long averageTime;

   // Run every Five seconds
  static unsigned long lastRead = 0;
  if (millis() - lastRead > 5000) {
    lastRead = millis();

    double declination = 0;
    int year = 0;
    averageTime = 0;
    digitalWrite(LED_BUILTIN, HIGH);
    for (int i = 0; i < n_locations; i++) {
      for (int j = 0; j < n_years; j++) { 		
        year = years[j];
        startTime = micros();
        declination = wmm_declination(locations[i].lat, locations[i].lon, year);
        endTime = micros();

        Serial.print(locations[i].city);
        Serial.print(" ");
        Serial.print(locations[i].state);
        Serial.print(" ");
        Serial.print(locations[i].country);
        Serial.print(" (");
        Serial.print(locations[i].lat);
        Serial.print(",");
        Serial.print(locations[i].lon);
        Serial.print(") in ");
        Serial.print(year);
        Serial.print(" ");
        Serial.print(declination);
        Serial.print(" degrees (");
        Serial.print(endTime - startTime);
        Serial.println(" usecs)");
        averageTime += (endTime - startTime);
      }
    }
    digitalWrite(LED_BUILTIN, LOW);
    Serial.print("\n--------------------\nAverage time:");
    Serial.print(averageTime/(n_years * n_locations));
    Serial.println( " usecs");

  }
}
