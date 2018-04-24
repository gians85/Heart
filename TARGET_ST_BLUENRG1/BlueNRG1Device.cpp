#include "BlueNRG1Device.h"
#include "btle.h"
#include "ble_debug.h"


/**
* The singleton which represents the nRF51822 transport for the BLE.
*/
static BlueNRG1Device& getDeviceInstance() {
  static BlueNRG1Device deviceInstance;
  return deviceInstance;
}


/**
* BLE-API requires an implementation of the following function in order to
* obtain its transport handle.
*/
BLEInstanceBase * createBLEInstance(void){
  return &BlueNRG1Device::Instance(BLE::DEFAULT_INSTANCE);
}

BlueNRG1Device& BlueNRG1Device::Instance(BLE::InstanceID_t instanceId){
  return getDeviceInstance();
}

BlueNRG1Device::BlueNRG1Device(){}

BlueNRG1Device::~BlueNRG1Device(){}

ble_error_t BlueNRG1Device::init(BLE::InstanceID_t instanceID, FunctionPointerWithContext<BLE::InitializationCompleteCallbackContext *> callback)
{
    if (isInitialized) {
        BLE::InitializationCompleteCallbackContext context = {
            BLE::Instance(instanceID),
            BLE_ERROR_ALREADY_INITIALIZED
        };
        callback.call(&context);
        return BLE_ERROR_ALREADY_INITIALIZED;
    }
  
    
    PRINTF("STACK INIT \n\r");
    BlueNRG1_stackInit();
    
    PRINTF("DEVICE INIT \n\r");
    BlueNRG1_deviceInit();


    isInitialized = true;
    BLE::InitializationCompleteCallbackContext context = {
        BLE::Instance(instanceID),
        BLE_ERROR_NONE
    };
    callback.call(&context);
  
    return BLE_ERROR_NONE;
}


void BlueNRG1Device::processEvents() {
    // IMPLEMENTARE : ok   
    //btle_handler();   //ST
    PRINTF("!!! Have to implement processEvents()\n\r");
}


GattServer &BlueNRG1Device::getGattServer(){
    return BlueNRG1GattServer::getInstance();
}


Gap        &BlueNRG1Device::getGap(){
    return BlueNRG1Gap::getInstance();
}

const Gap  &BlueNRG1Device::getGap() const{
    return BlueNRG1Gap::getInstance();
}