#include "BluetoothSerial.h"

// Khởi tạo đối tượng Bluetooth Serial
BluetoothSerial SerialBT;

void setup() {
  // Bắt đầu Serial monitor
  Serial.begin(9600);

  SerialBT.begin("ESP32_Slave");
  Serial.println("ESP32 Slave is ready for pairing.");
  Serial.println("The MAC address of this ESP32 is:");
  Serial.println(SerialBT.getBtAddressString());
}

void loop() {
  if (SerialBT.available()) {
    String receivedData = SerialBT.readString();
    Serial.println("Received: " + receivedData);
  }
}
