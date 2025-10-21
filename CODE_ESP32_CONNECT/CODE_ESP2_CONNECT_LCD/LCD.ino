// VÙNG KHAI BÁO THƯ VIỆN
#include <Wire.h>
#include <LiquidCrystal_I2C.h>  // thư viện LCD
#define LED_PIN 18              // khai báo chân cho con led

// VÙNG KHAI BÁO BIẾN
// Đặt địa chỉ I2C của LCD, ví dụ: 0x27 (tuỳ thuộc vào màn hình của bạn)
LiquidCrystal_I2C lcd(0x27, 16, 2);  // 16 ký tự, 2 dòng
int led_status = 0;

// HÀM KHỞI TẠO
void setup() {
  Serial.begin(9600);
  lcd.init();                // khởi tạo LCD
  pinMode(LED_PIN, OUTPUT);  // khởi tạo led
  lcd.backlight();           // bật đèn nền lcd
}

// VÒNG LẶP
void loop() {
  // chuyển trạng thái của led
  if (led_status == 1) {
    digitalWrite(LED_PIN, HIGH);  // Bật LED
    led_status = 0;
    lcd.setCursor(0, 0);
    lcd.print("                ");  // reset nội dung của hàng LCD rồi mới ghi nội dung mới
    lcd.setCursor(0, 0);
    lcd.print("led on!");
    Serial.println("LED ON!");  // in ra monitor
  } else {
    digitalWrite(LED_PIN, LOW);  // Tắt LED
    led_status = 1;
    lcd.setCursor(0, 0);
    lcd.print("                ");  // reset nội dung của hàng LCD rồi mới ghi nội dung mới
    lcd.setCursor(0, 0);
    lcd.print("led off!");
    Serial.println("LED OFF!");
  }
  // <--------------------------------------->

  // cập nhật mỗi 1s
  delay(2000);
}
