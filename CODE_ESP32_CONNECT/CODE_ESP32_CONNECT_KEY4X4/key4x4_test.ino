#include <Keypad.h>

const byte ROWS = 4; // Số hàng
const byte COLS = 4; // Số cột

// Định nghĩa layout của bàn phím
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

// trên esp32 từ chân 1-8
// cột: 1-4 {14, 15, 16, 17}
// hàng: 5-8 {18, 19, 23, 5}
byte rowPins[ROWS] = {18, 19, 23, 5}; // Các chân kết nối với các hàng
byte colPins[COLS] = {14, 15, 16, 17}; // Các chân kết nối với các cột

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

void setup() {
  Serial.begin(115200);
}

void loop() {
  char key = keypad.getKey();

  if (key) {
    Serial.println(key);
  }
}
