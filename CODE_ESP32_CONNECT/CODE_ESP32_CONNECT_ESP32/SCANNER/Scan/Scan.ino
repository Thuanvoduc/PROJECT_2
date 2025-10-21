// THƯ VIỆN
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>
#include <BLEClient.h>
#include <Keypad.h>
#include <LiquidCrystal_I2C.h>  // thư viện LCD

// BIẾN GLOBAL
int scanTime = 5;                                // Thời gian quét BLE (giây)
BLEScan* pBLEScan;                               // Con trỏ để quản lí việc quét BLE
BLEClient* pClient;                              // Con trỏ đến client BLE để kết nối đến server BLE
bool doConnect = false;                          // Biến để xác định có thực hiện kết nối hay không
bool connected = false;                          // Biến để xác định trạng thái đã kết nối hay chưa
BLEAdvertisedDevice* myDevice;                   // Con trỏ đến đối tượng BLEAdvertisedDevice đại diện cho thiết bị đã tìm thấy
BLERemoteCharacteristic* pRemoteCharacteristic;  // Con trỏ đến đặc tính của server BLE
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Định nghĩa layout của bàn phím
const byte ROWS = 4;  // Số hàng
const byte COLS = 4;  // Số cột
char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};

// trên esp32 từ chân 1-8
byte rowPins[ROWS] = { 18, 23, 12, 13 };  // Các chân kết nối với các hàng
byte colPins[COLS] = { 14, 15, 16, 17 };  // Các chân kết nối với các cột

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

String inputData = "";     // Chuỗi lưu dữ liệu nhập từ bàn phím
String receivedData = "";  // Biến lưu dữ liệu nhận từ server

// THÔNG TIN VỀ SERVER
const char* targetName = "BLE_SERVER";            // tên
const char* targetAddress = "c8:2e:18:25:e4:62";  // địa chỉ
BLEUUID serviceUUID("4fafc201-1fb5-459e-8fcc-c5c9c331914b");
BLEUUID charUUID("beb5483e-36e1-4688-b7f5-ea07361b26a8");

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    Serial.printf("Advertised Device: %s \n", advertisedDevice.toString().c_str());

    if (advertisedDevice.getName() == targetName || advertisedDevice.getAddress().toString() == targetAddress) {
      Serial.println("Target device found. Stopping scan...");
      pBLEScan->stop();
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
    }
  }
};

// Hàm callback khi nhận thông báo từ server BLE
void notifyCallback(BLERemoteCharacteristic* pRemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify) {
  receivedData = "";  // Clear dữ liệu cũ trước khi ghi dữ liệu mới
  for (size_t i = 0; i < length; i++) {
    receivedData += (char)pData[i];
  }

  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print("rc:" + receivedData);

  Serial.print("Received data: ");
  Serial.println(receivedData);
}

// Kết nối tới server BLE và đăng ký callback
void connectToServer() {
  Serial.println("Connecting to server...");
  pClient = BLEDevice::createClient();
  if (pClient->connect(myDevice)) {
    Serial.println("Connected to server.");
    connected = true;

    // Tìm server service "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
    BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
    if (pRemoteService == nullptr) {
      Serial.println("Failed to find the service UUID. Disconnecting...");
      pClient->disconnect();
      connected = false;
      return;
    }

    // Tìm server charUUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"
    pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
    if (pRemoteCharacteristic == nullptr) {
      Serial.println("Failed to find the characteristic UUID. Disconnecting...");
      pClient->disconnect();
      connected = false;
      return;
    }

    // Đăng ký hàm callback để nhận thông báo từ server
    if (pRemoteCharacteristic->canNotify()) {
      pRemoteCharacteristic->registerForNotify(notifyCallback);
      Serial.println("Registered for notifications.");
    }
  } else {
    Serial.println("Failed to connect to the server.");
  }
}

void setup() {
  Serial.begin(9600);
  Serial.println("Starting BLE scan...");
  
  lcd.init();       // khởi tạo LCD
  lcd.backlight();  // bật đèn nền lcd
  lcd.setCursor(0, 0);
  lcd.print("sd:");
  lcd.setCursor(0, 1);
  lcd.print("rc:");

  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
  pBLEScan->setInterval(100);
  pBLEScan->setWindow(99);

  keypad.setDebounceTime(50);  // Tăng thời gian debounce lên 50ms
}

void loop() {
  if (connected) {
    char key = keypad.getKey();
    if (key) {
      Serial.print("Key pressed: ");
      Serial.println(key);

      if (key == '#') {
        lcd.setCursor(0, 0);
        lcd.print("                ");
        if (pRemoteCharacteristic->canWrite() && inputData.length() > 0) {
          pRemoteCharacteristic->writeValue((uint8_t*)inputData.c_str(), inputData.length(), false);
          Serial.print("Data sent to server: ");
          Serial.println(inputData);
          lcd.setCursor(0, 0);
          lcd.print("sd:" + inputData);
          inputData = "";
        }
      } else {
        inputData += key;
      }
    }
    delay(100);
    return;
  }

  if (doConnect) {
    connectToServer();
    doConnect = false;
  } else {
    BLEScanResults* foundDevices = pBLEScan->start(scanTime, false);
    Serial.print("Devices found: ");
    Serial.println(foundDevices->getCount());
    Serial.println("Scan done!");
    pBLEScan->clearResults();
    delay(2000);
  }
}
