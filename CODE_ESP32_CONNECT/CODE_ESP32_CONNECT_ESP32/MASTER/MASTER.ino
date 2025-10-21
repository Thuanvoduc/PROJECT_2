#include "BluetoothSerial.h"

// Khởi tạo đối tượng Bluetooth Serial
BluetoothSerial SerialBT;

void setup() {
  // Bắt đầu Serial monitor
  Serial.begin(9600);

  // Khởi động Bluetooth ở chế độ Master và đặt tên
  if(!SerialBT.begin("ESP32_Master", true)) {
    Serial.println("Bluetooth failed to start");
    return;
  }

  Serial.println("Bluetooth started as Master. Trying to connect to Slave...");

  // Kết nối đến Slave với địa chỉ Bluetooth
  if(SerialBT.connect("C8:2E:18:25:E4:62")) { // Thay thế bằng địa chỉ MAC của ESP32 B
    Serial.println("Connected to Slave");
  } else {
    Serial.println("Failed to connect to Slave");
  }
}

void loop() {
  // NẾU ĐÃ KẾT NỐI VỚI SLAVE
  if (SerialBT.connected()) {
    Serial.println("Sending data to Slave...");
    SerialBT.println("Hello from Master");
    delay(1000);
  } else { // NẾU CHƯA KẾT NỐI VỚI SLAVE
    Serial.println("Trying to reconnect to Slave...");
    SerialBT.connect("C8:2E:18:25:E4:62"); // Thay thế với địa chỉ MAC của Slave
    delay(2000);
  }
}
