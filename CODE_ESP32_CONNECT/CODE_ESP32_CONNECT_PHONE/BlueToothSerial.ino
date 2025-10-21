// KHAI BÁO THƯ VIỆN
#include "BluetoothSerial.h"
#include <LiquidCrystal_I2C.h>  // thư viện LCD
#include <Keypad.h>             // thư viện key_4x4

// KHAI BÁO BIẾN
BluetoothSerial BT;
LiquidCrystal_I2C lcd(0x27, 16, 2);
// #define LED_PIN 18  // khai báo chân cho con led
const byte ROWS = 4;  // Số hàng
const byte COLS = 4;  // Số cột

// Định nghĩa layout của bàn phím
char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};

// trên esp32 từ chân 1-8
// cột: 1-4 {14, 15, 16, 17}
// hàng: 5-8 {18, 19, 23, 5}
byte rowPins[ROWS] = { 18, 19, 23, 5 };   // Các chân kết nối với các hàng
byte colPins[COLS] = { 14, 15, 16, 17 };  // Các chân kết nối với các cột

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

String inputString = "";  // Chuỗi để lưu ký tự

// HÀM KHỞI TẠO
void setup() {
  Serial.begin(115200);
  // pinMode(LED_PIN, OUTPUT);  // khởi tạo led
  BT.begin("Esp32");
  Serial.println("The device started, now you can pair it with bluetooth!");
  lcd.init();       // khởi tạo LCD
  lcd.backlight();  // bật đèn nền lcd
}

// VÒNG LẶP
void loop() {
  // key 4x4
  char key = keypad.getKey();
  if (key) {
    // Thêm ký tự nhấn vào chuỗi
    inputString += key;

    // Nếu chuỗi dài hơn 32 ký tự, xóa và bắt đầu lại
    if (inputString.length() > 32) {
      inputString = "";
      lcd.clear();
    }

    // Hiển thị chuỗi lên LCD
    lcd.clear();
    if (inputString.length() <= 16) {
      // Nếu chuỗi ngắn hơn hoặc bằng 16 ký tự, hiển thị trên dòng đầu tiên
      lcd.setCursor(0, 0);
      lcd.print(inputString);
    } else {
      // Nếu chuỗi dài hơn 16 ký tự, chia thành 2 phần
      String line1 = inputString.substring(0, 16);   // 16 ký tự đầu
      String line2 = inputString.substring(16, 32);  // 16 ký tự tiếp theo
      lcd.setCursor(0, 0);
      lcd.print(line1);
      lcd.setCursor(0, 1);
      lcd.print(line2);
    }

    // In chuỗi ra Serial Monitor (tuỳ chọn)
    Serial.println(inputString);
    BT.println(inputString);
  }

  // Đọc và xử lý dữ liệu từ Serial
  if (Serial.available()) {
    BT.write(Serial.read());
  }

  // Đọc và xử lý dữ liệu từ Bluetooth
  if (BT.available()) {
    String btData = "";  // Biến để lưu chuỗi dữ liệu từ Bluetooth
    while (BT.available()) {
      char data1 = BT.read();  // Đọc từng ký tự từ Bluetooth

      // Kiểm tra xem ký tự có phải là ký tự in được không (ASCII từ 32 đến 126)
      if (data1 >= 32 && data1 <= 126) {
        btData += data1;  // Thêm ký tự hợp lệ vào chuỗi
      }
    }

    lcd.clear();  // Xóa toàn bộ LCD trước khi in dữ liệu mới

    // Kiểm tra nếu chuỗi dài hơn 16 ký tự
    if (btData.length() > 16) {
      // In 16 ký tự đầu lên hàng 0
      lcd.setCursor(0, 0);
      lcd.print(btData.substring(0, 16));

      // In phần còn lại xuống hàng 1
      lcd.setCursor(0, 1);
      lcd.print(btData.substring(16));
    } else {
      // Nếu chuỗi ngắn hơn hoặc bằng 16 ký tự, in lên hàng 0
      lcd.setCursor(0, 0);
      lcd.print(btData);
    }

    Serial.println(btData);  // In dữ liệu lên Serial để kiểm tra
  }

  delay(20);  // Tránh quá tải bộ xử lý
}
