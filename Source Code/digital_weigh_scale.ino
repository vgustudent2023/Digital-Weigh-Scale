#include <Led4digit74HC595.h>
#include <HX711_ADC.h>

Led4digit74HC595 disp(A5, A4, A3);   // SCLK, RCLK, DIO
HX711_ADC LoadCell(A0, A1);          // DOUT, SCK

float calFactor = 214.4f;            // start with your computed value

static int clampToDisplay(long g) {
  if (g < 0) g = 0;
  if (g > 9999) g = 9999;
  return (int)g;
}

void setup() {
  Serial.begin(115200);

  disp.setDecimalPoint(0);

  LoadCell.begin();
  LoadCell.start(2000, true);     // tare at start
  LoadCell.setCalFactor(calFactor);

  Serial.println("Commands:");
  Serial.println("  t            = tare");
  Serial.println("  c <grams>     = calibrate using known mass in grams (e.g., c 205)");
  Serial.print("Current calFactor = ");
  Serial.println(calFactor, 6);
}

void loop() {
  disp.loopShow();
  LoadCell.update();

  static unsigned long t = 0;
  if (millis() - t >= 100) {
    float grams_f = LoadCell.getData();
    long grams = lround(grams_f);

    disp.setNumber(clampToDisplay(grams));

    Serial.print("g=");
    Serial.println(grams);

    t = millis();
  }

  // -------- Serial commands --------
  if (Serial.available()) {
    String line = Serial.readStringUntil('\n');
    line.trim();
    if (line.length() == 0) return;

    if (line.equalsIgnoreCase("t")) {
      LoadCell.tareNoDelay();
      Serial.println("Tare requested.");
      return;
    }

    // "c 205" -> calibrate
    if (line[0] == 'c' || line[0] == 'C') {
      float realG = line.substring(1).toFloat();
      if (realG <= 0) {
        Serial.println("Usage: c <grams>  (e.g., c 205)");
        return;
      }

      // Let readings settle a moment
      unsigned long t0 = millis();
      while (millis() - t0 < 800) { disp.loopShow(); LoadCell.update(); }

      float measured = LoadCell.getData(); // current reading with current calFactor

      // Update factor using ratio
      // newCal = oldCal * measured / real
      calFactor = calFactor * (measured / realG);
      LoadCell.setCalFactor(calFactor);

      Serial.print("Measured=");
      Serial.print(measured, 3);
      Serial.print(" g, Real=");
      Serial.print(realG, 3);
      Serial.print(" g -> New calFactor=");
      Serial.println(calFactor, 6);

      Serial.println("Now tare (t), then check again with the same mass.");
    }
  }
}
