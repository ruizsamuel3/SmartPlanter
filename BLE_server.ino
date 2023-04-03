#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
//#include "C:\Program Files (x86)\Arduino\libraries\WiFi\src\WiFi.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

#define SERVICE_UUID        "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID "beb5483e-36e1-4688-b7f5-ea07361b26a8"

bool deviceConnected = false;
String nodata = "No data received";
BLECharacteristic *writeCharacteristic;
char* ssid; 
const char* password;
char* receivedData;

void postData(){
  StaticJsonDocument<1024> doc;
  doc["dataSource"] = "PlantWays";
  doc["database"] = "todo";
  doc["collection"] = "SensorDataTest";
  JsonObject document = doc.createNestedObject("document");
  document["temperature"] = 60;
  document["humidity"] = 50;
  document["water level"] = "half";
  String json;
  serializeJson(doc, json);
    
  if(WiFi.status() == WL_CONNECTED){
    Serial.println("Getting ready to post");
    HTTPClient http;
    http.begin("https://us-east-1.aws.data.mongodb-api.com/app/data-bbjek/endpoint/data/v1/action/insertOne");
    http.addHeader("Content-Type", "application/json");
    http.addHeader("apiKey", "Pc1wgWw5hIsH0cqWen5qw2Qyoob69MPCI4ZCTKJq4rfOLznH6JQIs9St3oJ6TXUu");
    int httpResponseCode = http.POST(json);
    if(httpResponseCode == 200){
      Serial.println("Data sent");
    }else{
      Serial.println("Error sending data");
    }
    http.end();
  }
}

void connectToWifi(){
  char ssid_char[32], password_char[32];
  strcpy(ssid_char, ssid);
  strcpy(password_char, password);

  WiFi.begin(ssid_char, password_char);
  while(WiFi.status() != WL_CONNECTED){
    delay(1000);
    Serial.println("Connecting to WiFi...");
    }
  
  Serial.println("Connected to WiFi");
  postData();
  //pServer -> removeService(pService);
}

class MyServerCallbacks : public BLEServerCallbacks{
  void onConnect(BLEServer* pServer){
    deviceConnected = true;
  };
};

class onWriteCallback : public BLECharacteristicCallbacks{
  void onWrite(BLECharacteristic *pCharacteristic){
    receivedData = const_cast<char*>(pCharacteristic -> getValue().c_str());
    ssid = strtok(receivedData, ",");
    password = strtok(NULL, ",");
    Serial.println(ssid);
    Serial.println(password);
    connectToWifi();
  };
};

void setup() {
  Serial.begin(115200);
  Serial.println("Starting Bluetooth");

  BLEDevice::init("ESP32");
  BLEServer *pServer = BLEDevice::createServer();
  pServer -> setCallbacks(new MyServerCallbacks());
  BLEService *pService = pServer->createService(SERVICE_UUID);
  writeCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );
  writeCharacteristic->setValue("WRITE_HERE");
  writeCharacteristic->setCallbacks(new onWriteCallback());
  pService->start();
  // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
 Serial.println("ESP32 is discoverable");
}

void loop() {

}
