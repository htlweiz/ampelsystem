#ifdef BLE_CENTRAL

  #include <Arduino.h>
  #include "BLEDevice.h"
  #include "BLEScan.h"
  #include <Wire.h>


  //BLE Server name
  #define bleServerName "4AHIT_TEMP"

  //Output pins
  #define PEDESTRIAN_GREEN 23
  #define PEDESTRIAN_RED 22
  #define VEHICLE_GREEN 18
  #define VEHICLE_ORANGE 19
  #define VEHICLE_RED 21

  /* UUID's of the service, characteristic*/
  // BLE Service
  static BLEUUID bleServiceUUID("01f4710c-90d0-11ed-a1eb-0242ac120002");
  // BLE Characteristics
  static BLEUUID temperatureCharacteristicUUID("d7b554a8-2904-4ee8-9aa0-5c07e9ad1ca2");
 

  //Flags
  static boolean doConnect = false;
  static boolean connected = false;
  static boolean dataNotify = false;

  static uint8_t state = 0;
  static boolean lightSequence = false;

  static uint64_t millisSinceChange = 0;

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
      if (advertisedDevice.getName() == bleServerName) { //Check if the name of the advertiser matches
        advertisedDevice.getScan()->stop(); //Scan can be stopped
        pServerAddress = new BLEAddress(advertisedDevice.getAddress()); //Address of advertiser
        doConnect = true;
        Serial.println("Device found. Connecting!");
      }
    }
  };
  
  boolean timeHasPassed(uint64_t deltaTime_ms) {
    if (millis() - millisSinceChange >= deltaTime_ms) {
      millisSinceChange = millis();
      return true;
    }
    return false;
  }

  /*State machine*/
  void controlLights() {
    // wait 2s
    if (state == 0 && timeHasPassed(2000)) state++;
    // blink vehicle green light 4 times than orange
    if (state == 1) {
      for (uint8_t i = 0; i < 4; i++)
      {
        digitalWrite(VEHICLE_GREEN, HIGH);
        delay(1000);
        digitalWrite(VEHICLE_GREEN, LOW);
        delay(1000);
      }
      digitalWrite(VEHICLE_ORANGE, HIGH);
      state++;
    }
    // switch traffic light to red
    if (state == 2 && timeHasPassed(2000))
    {
      digitalWrite(VEHICLE_ORANGE, LOW);
      digitalWrite(VEHICLE_RED, HIGH);
      state++;
    }
    // switch pedestian light to green
    if (state == 3 && timeHasPassed(2000)) {
      digitalWrite(PEDESTRIAN_RED, LOW);
      digitalWrite(PEDESTRIAN_GREEN, HIGH);
      state++;
    }
    // switch pedestrian light to red
    if (state == 4 && timeHasPassed(10000)) {
      digitalWrite(PEDESTRIAN_GREEN, LOW);
      digitalWrite(PEDESTRIAN_RED, HIGH);
      state++;
    }
    // switch traffic light to orange
    if (state == 5 && timeHasPassed(2000)) {
      digitalWrite(VEHICLE_RED, LOW);
      digitalWrite(VEHICLE_ORANGE, HIGH);
      state++;
    }
    // switch traffic light to green
    if (state == 6 && timeHasPassed(2000)){
      digitalWrite(VEHICLE_ORANGE, LOW);
      digitalWrite(VEHICLE_GREEN, HIGH);
      state++;
    }
    // reset state
    if (state == 7 && timeHasPassed(10000)){
      lightSequence = false;
      state = 0;
    }
  }

  void setup() {
    //Start serial communication
    Serial.begin(115200);
    Serial.println("Starting BLE Client ...");

    pinMode(PEDESTRIAN_GREEN, OUTPUT);
    pinMode(PEDESTRIAN_RED, OUTPUT);
    pinMode(VEHICLE_GREEN, OUTPUT);
    pinMode(VEHICLE_ORANGE, OUTPUT);
    pinMode(VEHICLE_RED, OUTPUT);

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

    digitalWrite(PEDESTRIAN_RED, HIGH);

    // lightSequence = true;
  }

  void loop() {
    // TODO
    if (doConnect == true) {
      if (connectToServer(*pServerAddress)) {
        temperatureCharacteristic->getDescriptor(BLEUUID((uint16_t)0x2902))->writeValue((uint8_t*)notificationOn, 2, true);
        connected = true;
        doConnect = false;
        Serial.println("connected");
      }
    } else if (dataNotify == true) {
      // initiate light sequence on notify
      lightSequence = true;
      dataNotify = false;
    }
    if (lightSequence == true) {
      controlLights();
    } else
    {
      // used to calculate the time between the last state change
      millisSinceChange = millis();
    }
    
    delay(1000); // Delay a second between loops.
  }

#endif