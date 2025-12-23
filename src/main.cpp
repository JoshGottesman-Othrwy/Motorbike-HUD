#include <Arduino.h>

void setup() {
  Serial.begin(115200);
  delay(500);
  Serial.println();
  Serial.println("LilyGO T-Display S3 AMOLED Plus boot");
  Serial.printf("SDK: %s\n", ESP.getSdkVersion());
  Serial.printf("Chip: %s, Cores: %u\n", ESP.getChipModel().c_str(), ESP.getChipCores());
}

void loop() {
  static uint32_t last_log_ms = 0;
  const uint32_t now = millis();
  if (now - last_log_ms >= 1000) {
    last_log_ms = now;
    Serial.printf("Uptime: %lu ms\n", static_cast<unsigned long>(now));
  }
}
