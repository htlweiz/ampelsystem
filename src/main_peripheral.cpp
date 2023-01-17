#ifdef BLE_PERIPHERAL

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEService.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <Wire.h>

//BLE server name
#define bleServerName "4AHIT_TEMP"


#define SERVICE_UUID "01f4710c-90d0-11ed-a1eb-0242ac120002"
#define CHARACTERISTICS_UUID "d7b554a8-2904-4ee8-9aa0-5c07e9ad1ca2"

#define redLight 18
#define greenLight 19

#define rightButton 26
#define leftButton 27 

int loopcounter = 0;
bool connected = false;
int signal = 0;



// Temperature Characteristic and Descriptor
BLECharacteristic bleTemperatureCelsiusCharacteristics("d7b554a8-2904-4ee8-9aa0-5c07e9ad1ca2", BLECharacteristic::PROPERTY_NOTIFY);
BLEDescriptor bleTemperatureCelsiusDescriptor(BLEUUID((uint16_t)0x2902));


//Setup callbacks onConnect and onDisconnect
class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
        // TODO
        connected = true;
    };
    void onDisconnect(BLEServer* pServer) {
        // TODO
        connected = false;
    }
};


void setup() {
    Serial.begin(115200);

    // Create the BLE Device
    BLEDevice::init(bleServerName);

    // Create the BLE Server
    BLEServer *pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    /* Create the GATT Profile */
    // Create the BLE Service
    BLEService *bleService = pServer->createService(SERVICE_UUID);
    //TODO
    //Hier will ich mein GATT Profiel bauen
    bleService->addCharacteristic(&bleTemperatureCelsiusCharacteristics);
    bleTemperatureCelsiusCharacteristics.addDescriptor(&bleTemperatureCelsiusDescriptor);

    //BLECharacteristic *pCharacteristics = pServer->createCharacteristic(
    //    BLEUUID(CHARACTERISTICS_UUID), 
    //    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE
    //);
    //pCharacteristics->setValue("Hello World!");
    
    // Start the service
    bleService->start();

    // Start advertising
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    //TODO
    //UUID von ersten Service hernehmen 
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pServer->getAdvertising()->start();

    Serial.println("Waiting a client connection to notify..."); 

    // Set pinMode of rightButton, letfButton, redLight and greenLight
    pinMode(redLight, HIGH);
    pinMode(greenLight, LOW);

    pinMode(rightButton, LOW);
    pinMode(leftButton, LOW);
}

void loop() {
    //TODO
    //every 10 seconds write a new value into bleTemperatureCelsiusCharacteristics
    //then tell with notify that somethings has changed
    Serial.println("looop");

    if (rightButton == HIGH or leftButton == HIGH)
    {
        signal = 1;
        bleTemperatureCelsiusCharacteristics.setValue(signal);
        bleTemperatureCelsiusCharacteristics.notify();
        sleep(5);

    }
    
    sleep(10); 
    loopcounter++;
}

#endif