#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>  //Die benötigte Libraries
#include <BLE2902.h>

//-----------------------------------------------------------------------------------------------------------------------------------
#define MotorBewegung
#define BLE_server "ESP32_Server"  //Bluetooth-Low-Energy Server einen Namen vergeben

#define SERVICE_UUID "91bad492-b950-4226-aa2b-4ede9fa42f59"  //Einen Universaly-Unique-Identefier generieren

#ifdef MotorBewegung
BLECharacteristic ServoMotorBewegungCharacteristics("cba1d466-344c-4be3-ab3f-189f80dd7518", BLECharacteristic::PROPERTY_NOTIFY);
BLEDescriptor ServoMotorBewegungDescriptor(BLEUUID((uint16_t)0x2902));
#endif
//-----------------------------------------------------------------------------------------------------------------------------------


//-----------------------------------------------------------------------------------------------------------------------------------
const int ADC_pin = 14;              //Analog-Digital-Converter
const float VCC = 5;                 //Quellspannung
const float fixed_resistor = 22000;  //Vorwiderstand Spannungsteiler
//-----------------------------------------------------------------------------------------------------------------------------------


//-----------------------------------------------------------------------------------------------------------------------------------
bool device_connected = false;  //Überprüfen ob der ESP32 verbunden ist

BLEServer* pServer;  // define pServer as a global variable

class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    device_connected = true;
    Serial.println("Connected Erfolgreich");
  };
  void onDisconnect(BLEServer* pServer) {
    device_connected = false;
    Serial.println("Connection dropped");
  }
};
//-----------------------------------------------------------------------------------------------------------------------------------


//-----------------------------------------------------------------------------------------------------------------------------------
void setup() {

  Serial.begin(9600);
  BLEDevice::init(BLE_server);
  BLEServer* pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  BLEService* flexService = pServer->createService(SERVICE_UUID);

  pinMode(ADC_pin, INPUT);  //Analog-Digital-Converter als Input setzen

#ifdef MotorBewegung
  flexService->addCharacteristic(&ServoMotorBewegungCharacteristics);
  ServoMotorBewegungDescriptor.setValue("FlexSensor in (°))");
  ServoMotorBewegungCharacteristics.addDescriptor(new BLE2902());
#endif


  flexService->start();
  pServer->getAdvertising()->start();
  Serial.println("Waiting for ESP32 client connection...");
}
//-----------------------------------------------------------------------------------------------------------------------------------


//-----------------------------------------------------------------------------------------------------------------------------------
void loop() {
  MyServerCallbacks();
  int flex_ADC = analogRead(ADC_pin);                                            //Spannungsteiler wert messen
  float flex_voltage = flex_ADC * VCC / 1023.0;                                  //Spannungsdifferenz berechnen
  float flex_resistance = (fixed_resistor * (VCC / flex_voltage + 1.0)) / 1000;  //Widerstandswert berechnen
 // Serial.print("ADC value: ");
 // Serial.println(flex_ADC);  //Gemessene Spannung Ausgeben
  //Serial.print("Volatge value: ");
 // Serial.println(flex_voltage);  //Berechnete Spannung ausgeben

  if (device_connected = true) {
    
    float MotorWinkel = flex_resistance;  
                                   //float = 11.11;

#ifdef MotorBewegung
    static char Motor_Winkel[7];
    dtostrf(MotorWinkel, 6, 2, Motor_Winkel);
    ServoMotorBewegungCharacteristics.setValue(Motor_Winkel);
    ServoMotorBewegungCharacteristics.notify();
    Serial.print("MotorWinkel: ");
    Serial.println(MotorWinkel);
    Serial.print(" *°");

#endif
  }
  //delay(1000);
}
//-----------------------------------------------------------------------------------------------------------------------------------

