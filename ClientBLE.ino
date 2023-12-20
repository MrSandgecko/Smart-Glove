#include "BLEDevice.h"
#include "BLEClient.h"
#include "Servo.h"


#define MotorBewegung
#define BLE_server "ESP32_Server"

static BLEUUID flexServiceUUID("91bad492-b950-4226-aa2b-4ede9fa42f59");
#ifdef MotorBewegung
static BLEUUID ServoMotorBewegungCharacteristicsUUID("cba1d466-344c-4be3-ab3f-189f80dd7518");
#endif

static boolean doConnect = false;
static boolean connected = false;

static BLEAddress *pServerAddress;
static BLERemoteCharacteristic* ServoMotorBewegungCharacteristics;

const uint8_t notificationOn[] = {0x1, 0x0};
const uint8_t notificationOff[] = {0x0, 0x0};

const int servoPin = 13;
Servo servo;


bool connectToServer(BLEAddress pAddress) {
  BLEClient* pClient = BLEDevice::createClient();

  pClient->connect(pAddress);
  Serial.println(" - Connected successfully to server");

  BLERemoteService* pRemoteService = pClient->getService(flexServiceUUID);

  if (pRemoteService == nullptr) {
    Serial.print("Failed to find our service UUID: ");
    Serial.println(flexServiceUUID.toString().c_str());
    return (false);
  }

  ServoMotorBewegungCharacteristics = pRemoteService->getCharacteristic(ServoMotorBewegungCharacteristicsUUID);

  if (ServoMotorBewegungCharacteristics == nullptr) {
    Serial.print("Failed to find our characteristic UUID");
    return false;
  }
  Serial.println(" Characteristics Found!");

  ServoMotorBewegungCharacteristics->registerForNotify(ServoMotorNotifyCallback);

  // Return true to indicate that the connection was successful
  return true;
}

class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
void onResult(BLEAdvertisedDevice advertisedDevice) {
  if (advertisedDevice.getName() == BLE_server) { 
  advertisedDevice.getScan()->stop();
  pServerAddress = new BLEAddress(advertisedDevice.getAddress());

  doConnect = true;
  Serial.println("Device found. Connecting!");
  }
}
};

void ServoMotorNotifyCallback(BLERemoteCharacteristic*pBLERemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify)
{

  int value = atoi((char*)pData);
  servo.write(value);

  Serial.print("Servo position: ");
  Serial.println(value);
  Serial.println("MotorGrad: ");
  Serial.print((char*)pData);
  #ifdef temperatureCelsius
  Serial.print(" °");
  #endif
}


  
  void setup() 
  {
  Serial.begin(9600);
  servo.attach(servoPin);

  Serial.println("Starting Arduino BLE Client application...");
  BLEDevice::init("");
  
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new
  MyAdvertisedDeviceCallbacks());
  pBLEScan->setActiveScan(true);
  pBLEScan->start(30);
  }


  void loop() {
  if (doConnect == true) {
    if (connectToServer(*pServerAddress)) {
      Serial.println("We are now connected to the BLE Server.");
      ServoMotorBewegungCharacteristics->getDescriptor(BLEUUID((uint16_t)0x2902))->writeValue((uint8_t*)notificationOn, 2, true);
      connected = true;
    } else {
      Serial.println("Failed to connect to server!");
    }
    doConnect = false;
  }

  // Check if the connection to the server is still active
  if (connected) {
    if (!ServoMotorBewegungCharacteristics->canNotify()) {
      Serial.println("Lost connection to server, attempting to reconnect...");
      connected = false;
      doConnect = true;
    }
  }

  // Wait a short time before checking the connection again
  delay(500);
}
