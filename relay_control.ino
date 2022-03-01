/*Mustafa Kaya Controling relays with esp32 ble*/

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <string>
#include <iostream>

BLECharacteristic *pCharacteristic;
bool deviceConnected = false;
float txValue = 0;

const int relayA = 34; // Use GPIO number. See ESP32 board pinouts
const int relayB = 35; // Could be different depending on the dev board. I used the DOIT ESP32 dev board.
const int relayC = 32; // Use GPIO number. See ESP32 board pinouts
const int relayD = 33;
const int LED = 2;
int relayTime;
char relay;
double relayTimeA; // keeps relay times
double relayTimeB;
double relayTimeC;
double relayTimeD;
int setA;
int setB;
int setC;
int setD;
bool fA=0,fAA=0,fB=0,fBB=0,fC=0,fCC=0,fD=0,fDD=0; // control flags
std::string rxValue; // Could also make this a global var to access it in loop()

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" // UART service UUID
#define CHARACTERISTIC_UUID_RX "6E400002-B5A3-F393-E0A9-E50E24DCCA9E"
#define CHARACTERISTIC_UUID_TX "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      rxValue = pCharacteristic->getValue();
      relay=rxValue[0];
      char relayTimeSTR[3];
      
      if (rxValue.length() > 0) {
        Serial.println("*********");
        Serial.print("Received Value: ");
        
        for (int i = 0; i < rxValue.length(); i++) {
          Serial.print(rxValue[i]);
          relayTimeSTR[i]=rxValue[i+1];
        }
      relayTime = atoi(relayTimeSTR);
        Serial.println();
        Serial.println();
        Serial.println("*********");
                
     
      if (relay == 'A') { 
        if (rxValue.find("C") != -1){
          digitalWrite(LED, HIGH);
          fAA=1;
          }
        else if (rxValue.find("K") != -1){
          digitalWrite(LED, LOW);
          fAA=0;
          }
        else{
            relayTimeA=millis();
            setA=relayTime*1000;
            digitalWrite(LED, HIGH);
            fA=1;
            }
        }
      else if (relay == 'B') {
         if (rxValue.find("C") != -1){
          digitalWrite(relayB, HIGH);
          fBB=1;
          }
        else if (rxValue.find("K") != -1){
          digitalWrite(relayB, LOW);
          fBB=0;
          }
        else{
            relayTimeB=millis();
            setB=relayTime*1000;
            digitalWrite(relayB, HIGH);
            fB=1;
            }
      }
      else if (relay == 'C') {
         if (rxValue.find("C") != -1 && rxValue.find("A") != -1){
          digitalWrite(relayC, HIGH);
          fCC=1;
          }
        else if (rxValue.find("K") != -1){
          digitalWrite(relayC, LOW);
          fCC=0;
          }
        else{
            relayTimeC=millis();
            setB=relayTime*1000;
            digitalWrite(relayB, HIGH);
            fC=1;
            }
        }
      else if (relay == 'D') {
        if (rxValue.find("C") != -1 ){
          digitalWrite(relayD, HIGH);
          fDD=1;
          }
        else if (rxValue.find("K") != -1){
          digitalWrite(relayD, LOW);
          fDD=0;
          }
        else{
            relayTimeD=millis();
            setD=relayTime*1000;
            digitalWrite(relayD, HIGH);
            fD=1;
            }
       }
    }
  }
   
};

void setup() {
  Serial.begin(115200);

  pinMode(relayA, OUTPUT);
  pinMode(relayB, OUTPUT);
  pinMode(relayC, OUTPUT);
  pinMode(relayD, OUTPUT);

  // Create the BLE Device
  BLEDevice::init("ESP32 relays"); // Give it a name

  // Create the BLE Server
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  pCharacteristic = pService->createCharacteristic(
                      CHARACTERISTIC_UUID_TX,
                      BLECharacteristic::PROPERTY_NOTIFY
                    );
                      
  pCharacteristic->addDescriptor(new BLE2902());

  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID_RX,
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  pCharacteristic->setCallbacks(new MyCallbacks());

  // Start the service
  pService->start();

  // Start advertising
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");
}

void loop() {
  
  if (deviceConnected) {
    if (rxValue.find("I") != -1 && rxValue.find("A") != -1){
        if (fA||fAA) {
        pCharacteristic->setValue("RelayA ON");
        pCharacteristic->notify();
        }else{
        pCharacteristic->setValue("RelayA OFF");
        pCharacteristic->notify();
          }
      }
    else if (rxValue.find("I") != -1 && rxValue.find("B") != -1){
      if (fB||fBB) {
        pCharacteristic->setValue("RelayB ON");
        pCharacteristic->notify();
        }else{
        pCharacteristic->setValue("RelayB OFF");
        pCharacteristic->notify();
          }
      }
    else if (rxValue.find("I") != -1 && rxValue.find("C") != -1){
      if (fC||fCC) {
        pCharacteristic->setValue("RelayC ON");
        pCharacteristic->notify();
        }else{
        pCharacteristic->setValue("RelayC OFF");
        pCharacteristic->notify();
          }
      }
    else if (rxValue.find("I") != -1 && rxValue.find("D") != -1){
      if (fD||fDD) {
        pCharacteristic->setValue("RelayD ON");
        pCharacteristic->notify();
        }else{
        pCharacteristic->setValue("RelayD OFF");
        pCharacteristic->notify();
          }
      }


  }
  if (fA||fB||fC||fD){
    if (millis()-relayTimeA>setA) { 
      Serial.println("A");
      digitalWrite(LED, LOW);
      fA=0;
      fAA=0;
    }
    if (millis()-relayTimeB>setB) {
      Serial.println("B!");
      digitalWrite(relayB, LOW);
      fB=0;
      fBB=0;
    }
    if (millis()-relayTimeC>setC) {
      Serial.println("C");
      digitalWrite(relayC, LOW);
      fC=0;
      fCC=0;
    }
    if (millis()-relayTimeD>setD) {
      Serial.println("D");
      digitalWrite(relayD, LOW);
      fD=0;
      fDD=0;
    }
  } 

  delay(1000);
}
