#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>  // thư viện LCD

#define SERVICE_UUID "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

// Cấu hình bàn phím 4x4
const byte ROWS = 4;  // 4 hàng
const byte COLS = 4;  // 4 cột
char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};

byte rowPins[ROWS] = { 18, 23, 12, 13 };  // Kết nối đến các chân của hàng
byte colPins[COLS] = { 14, 15, 16, 17 };  // Kết nối đến các chân của cột

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

LiquidCrystal_I2C lcd(0x27, 16, 2);
String inputData = "";  // Biến để lưu dữ liệu từ các phím nhấn

BLECharacteristic *pCharacteristic;

// Tạo một lớp để xử lý việc nhận dữ liệu từ client
class MyCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    String value = pCharacteristic->getValue();  // Sử dụng String thay vì std::string
    if (value.length() > 0) {
      lcd.setCursor(0, 1);
      lcd.print("                ");
      lcd.setCursor(0, 1);
      lcd.print("rc:" + value);
      Serial.print("Received Value: ");
      Serial.println(value);  // Hiển thị dữ liệu nhận được từ client
    }
  }
};

void setup() {
  Serial.begin(9600);
  Serial.println("Starting BLE work!");

  lcd.init();       // khởi tạo LCD
  lcd.backlight();  // bật đèn nền lcd
  lcd.setCursor(0, 0);
  lcd.print("sd:");
  lcd.setCursor(0, 1);
  lcd.print("rc:");

  BLEDevice::init("BLE_SERVER");
  BLEServer *pServer = BLEDevice::createServer();
  BLEService *pService = pServer->createService(SERVICE_UUID);
  pCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_UUID,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY);

  pCharacteristic->setValue("Hello World says Neil");
  pCharacteristic->setCallbacks(new MyCallbacks());  // Đăng ký callback
  pService->start();

  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();

  keypad.setDebounceTime(50);  // Tăng thời gian debounce lên 50ms

  Serial.println("Characteristic defined! Now you can read it in your phone!");
}

void loop() {
  char key = keypad.getKey();  // Lấy phím được nhấn từ bàn phím 4x4

  if (key) {
    Serial.print("Key pressed: ");
    Serial.println(key);

    if (key == '#') {
      lcd.setCursor(0, 0);
      lcd.print("                ");
      // Gửi dữ liệu khi nhấn phím '#'
      if (inputData.length() > 0) {
        pCharacteristic->setValue(inputData.c_str());  // Cập nhật giá trị mới cho characteristic
        pCharacteristic->notify();                     // Gửi thông báo tới scanner

        Serial.print("Data sent to scanner: ");
        Serial.println(inputData);

        lcd.setCursor(0, 0);
        lcd.print("sd:" + inputData);

        inputData = "";  // Xóa chuỗi sau khi gửi
      }
    } else {
      // Thêm ký tự vào inputData nếu không phải là phím '#'
      inputData += key;
    }
  }

  delay(100);  // Giảm tải cho vi xử lý
}
