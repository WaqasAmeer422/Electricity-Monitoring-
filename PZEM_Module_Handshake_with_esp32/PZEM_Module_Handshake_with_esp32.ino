#include <PZEM004Tv30.h>

/*
 * PZEM-004T V3.0 + ESP32
 * * WIRING:
 * - PZEM TX -> GPIO 16 (RX2)
 * - PZEM RX -> GPIO 17 (TX2)
 */

#if defined(CONFIG_IDF_TARGET_ESP32C3)
  // XIAO ESP32-C3 setup
  #define PZEM_SERIAL Serial1
  #define PZEM_RX 20 // D7
  #define PZEM_TX 21 // D6
#else
  // Standard ESP32 setup
  #define PZEM_SERIAL Serial2
  #define PZEM_RX 16
  #define PZEM_TX 17
#endif

PZEM004Tv30 pzem(PZEM_SERIAL, PZEM_RX, PZEM_TX);

void setup() {
  Serial.begin(115200);
  
  #if defined(CONFIG_IDF_TARGET_ESP32C3)
    // Explicitly start Serial1 for C3
    Serial1.begin(9600, SERIAL_8N1, PZEM_RX, PZEM_TX);
  #endif
  
  delay(2000);
}

void loop() {
  float voltage = pzem.voltage();
  float current = pzem.current();
  float power = pzem.power();
  float frequency = pzem.frequency();
  float pf = pzem.pf();

  if(isnan(voltage) || isnan(current) || isnan(power) || isnan(frequency) || isnan(pf)) {
    Serial.println("Reading values... FAILED!");
    Serial.println("[ERROR] Check wiring or power source connection.");
  } else {
    Serial.println("Reading values... SUCCESS!");
    
    Serial.print("Voltage:   "); Serial.print(voltage); Serial.println("V");
    Serial.print("Current:   "); Serial.print(current); Serial.println("A");
    Serial.print("Power:     "); Serial.print(power); Serial.println("W");
    Serial.print("Frequency: "); Serial.print(frequency); Serial.println("Hz");
    Serial.print("PF:        "); Serial.println(pf);
  }

  Serial.println("---------------------------------------");
  delay(3000);
}