#ifdef BLE_PERIPHERAL

    #include <Arduino.h>
    #include <BLEDevice.h>
    #include <BLEServer.h>
    #include <BLEUtils.h>
    #include <BLE2902.h>
    #include <Wire.h>

    //BLE server name
    #define bleServerName "4AHIT_TEMP"


    #define SERVICE_UUID ""

    // Temperature Characteristic and Descriptor
    BLECharacteristic bleTemperatureCelsiusCharacteristics("", BLECharacteristic::PROPERTY_NOTIFY);
    BLEDescriptor bleTemperatureCelsiusDescriptor(BLEUUID((uint16_t)0x2902));


    //Setup callbacks onConnect and onDisconnect
    class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
        // TODO
    };
    void onDisconnect(BLEServer* pServer) {
        // TODO
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
    BLEService *bmeService = pServer->createService(SERVICE_UUID);
    //TODO

    
    // Start the service
    bmeService->start();

    // Start advertising
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    //TODO
    Serial.println("Waiting a client connection to notify...");
    }

    void loop() {
      //TODO
    }

#endif