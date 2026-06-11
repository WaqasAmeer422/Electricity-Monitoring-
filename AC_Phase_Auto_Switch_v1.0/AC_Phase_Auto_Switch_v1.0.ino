#include <PZEM004Tv30.h>

/*
 * PZEM-004T V3.0 + ESP32 AUTO PHASE SWITCH
 * 
 * LOGIC:
 * 1. Overcurrent Protection: If Current > 10A, switch to AC (Grid).
 * 2. Day/Night Sensing: 
 *    - If Voltage < 190V (Solar weak/off), switch to AC.
 *    - If Voltage > 210V (Solar strong), switch back to Solar.
 * 
 * WIRING:
 * - PZEM TX -> GPIO 16 (RX2)
 * - PZEM RX -> GPIO 17 (TX2)
 * - RELAY IN -> GPIO 25
 * 
 * RELAY SETUP:
 * - NC (Normally Closed) -> SOLAR LIVE
 * - NO (Normally Open)   -> GRID/AC LIVE
 * - COM (Common)         -> HOME LOAD
 */

#define RELAY_PIN 25      
#define MAX_CURRENT 10.0   // Safety Limit in Amps
#define SOLAR_MIN_V 190.0  // Switch to AC if below this
#define SOLAR_OK_V 210.0   // Switch to Solar if above this

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
  
  pinMode(RELAY_PIN, OUTPUT);
  
  // Start with Solar Mode (Relay LOW connects COM to NC)
  digitalWrite(RELAY_PIN, LOW); 
  
  delay(2000);
  Serial.println("\n--- AUTO PHASE SWITCH SYSTEM STARTING ---");
  Serial.println("Monitoring Solar Voltage and Load Current...");
}

void loop() {
  float voltage = pzem.voltage();
  float current = pzem.current();
  float power = pzem.power();

  if(isnan(voltage) || isnan(current)) {
    Serial.println("[ERROR] No data from PZEM. Check wiring!");
  } else {
    // Print real-time data
    Serial.print("V: "); Serial.print(voltage);
    Serial.print("V | I: "); Serial.print(current);
    Serial.print("A | P: "); Serial.print(power); Serial.println("W");

    // --- LOGIC 1: SAFETY (Overload Protection) ---
    if(current > MAX_CURRENT) {
      Serial.println("[ALERT] OVERLOAD (>10A)! Switching to AC for safety.");
      digitalWrite(RELAY_PIN, HIGH); // Switch to AC Grid
    } 
    
    // --- LOGIC 2: SOLAR STRENGTH (Day/Night Switching) ---
    else if(voltage < SOLAR_MIN_V) {
      Serial.println("[INFO] Solar voltage low (Night/Cloud). Switching to AC.");
      digitalWrite(RELAY_PIN, HIGH); // Switch to AC Grid
    }
    
    else if(voltage > SOLAR_OK_V) {
      // Current is safe AND Voltage is good
      if(digital_read_relay() == HIGH) {
        Serial.println("[INFO] Solar power stable. Switching back to SOLAR.");
      }
      digitalWrite(RELAY_PIN, LOW); // Switch to Solar
    }
  }

  Serial.println("---------------------------------------");
  delay(3000);
}

// Helper function to check relay state safely
int digital_read_relay() {
  return digitalRead(RELAY_PIN);
}
