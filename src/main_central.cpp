#ifdef BLE_CENTRAL

  #include <Arduino.h>
  #include "BLEDevice.h"
  #include "BLEScan.h"
  #include <Wire.h>


  //BLE Server name
  #define bleServerName "4AHIT_TEMP"

  /* UUID's of the service, characteristic*/
  // BLE Service
  static BLEUUID bleServiceUUID("01f4710c-90d0-11ed-a1eb-0242ac120002");
  // BLE Characteristics
  static BLEUUID temperatureCharacteristicUUID("d7b554a8-2904-4ee8-9aa0-5c07e9ad1ca2");
 

  //Flags
  static boolean doConnect = false;
  static boolean connected = false;
  static boolean dataNotify = false;

  //Address of the peripheral device.
  static BLEAddress *pServerAddress;
  
  //Characteristic
  static BLERemoteCharacteristic* temperatureCharacteristic;

  //Notification
  const uint8_t notificationOn[] = {0x1, 0x0};

  //Data
  uint8_t *pSensorData;
  size_t dataLength;

    //When the BLE Server sends a new temperature reading with the notify property
  static void temperatureNotifyCallback(BLERemoteCharacteristic* pBLERemoteCharacteristic, 
                                          uint8_t* pData, size_t length, bool isNotify) {
    pSensorData = pData;
    dataLength = length;
    dataNotify = true;
  }

  //Connect to the BLE Server that has the name, Service, and Characteristics
  bool connectToServer(BLEAddress pAddress) {
    BLEClient* pClient = BLEDevice::createClient();
  
    // Connect to the BLE peripheral.
    pClient->connect(pAddress);
    Serial.println(" - Connected to peripheral");
  
    // Obtain a reference to the service
    BLERemoteService* pRemoteService = pClient->getService(bleServiceUUID);
    if (pRemoteService == nullptr) {
      Serial.print("Failed to find the service UUID: ");
      Serial.println(bleServiceUUID.toString().c_str());
      return false;
    }
  
    // Obtain a reference to the characteristics in the service
    // TODO
    temperatureCharacteristic = pRemoteService->getCharacteristic(temperatureCharacteristicUUID);

    //Assign callback functions for the Characteristics
    temperatureCharacteristic->registerForNotify(temperatureNotifyCallback);
    return true;
  }

  //Callback function that gets called, when advertisement has been received
  class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
    void onResult(BLEAdvertisedDevice advertisedDevice) {
      // Serial.println(advertisedDevice.getName().c_str());
      if (advertisedDevice.getName() == bleServerName) { //Check if the name of the advertiser matches
        advertisedDevice.getScan()->stop(); //Scan can be stopped
        pServerAddress = new BLEAddress(advertisedDevice.getAddress()); //Address of advertiser
        doConnect = true;
        Serial.println("Device found. Connecting!");
      }
    }
  };
  

  void setup() {
    //Start serial communication
    Serial.begin(115200);
    Serial.println("Starting BLE Client ...");

    //Init BLE device
    BLEDevice::init("");
  
    // Retrieve a Scanner and set the callback
    // Active scanning
    // Start scanning to run for 30 seconds
    // TODO
    BLEScan* pBLEScan = BLEDevice::getScan();
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setActiveScan(true);
    pBLEScan->start(30);
  }

  void loop() {
    // TODO
    Serial.println("loop");
    if (doConnect == true) {
      if (connectToServer(*pServerAddress)) {
        temperatureCharacteristic->getDescriptor(BLEUUID((uint16_t)0x2902))->writeValue((uint8_t*)notificationOn, 2, true);
        connected = true;
        doConnect = false;
        Serial.println("connected");
      }
    } else if (dataNotify == true) {
      for (size_t i = 0; i < dataLength; i++)
      {
        Serial.println(*(pSensorData+i));
      }
      dataNotify = false;
    }
    delay(1000); // Delay a second between loops.
  }

#endif