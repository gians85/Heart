#include "BlueNRG1_ble.h"


#ifdef __cplusplus
extern "C" {
#endif
#include "ble_const.h"
#include "ble_status.h"
#include "bluenrg1_api.h"
#include "bluenrg1_events.h"
#include "bluenrg1_gap.h"
#include "bluenrg1_gatt_server.h"
#include "bluenrg1_hal.h"
#include "bluenrg1_stack.h"
#include "hci_const.h"
#include "link_layer.h "
#include "sm.h"
#ifdef __cplusplus
}
#endif

//#include "btle.h"

/**
* The singleton which represents the nRF51822 transport for the BLE.
*/
static BlueNRG1_ble& getDeviceInstance() {
  static BlueNRG1_ble deviceInstance;
  return deviceInstance;
}


/**
* BLE-API requires an implementation of the following function in order to
* obtain its transport handle.
*/
BLEInstanceBase * createBLEInstance(void){
  return &BlueNRG1_ble::Instance(BLE::DEFAULT_INSTANCE);
}

BlueNRG1_ble& BlueNRG1_ble::Instance(BLE::InstanceID_t instanceId){
  return getDeviceInstance();
}

BlueNRG1_ble::BlueNRG1_ble(){}

BlueNRG1_ble::~BlueNRG1_ble(){}

ble_error_t BlueNRG1_ble::init(BLE::InstanceID_t instanceID, FunctionPointerWithContext<BLE::InitializationCompleteCallbackContext *> callback)
{
    if (isInitialized) {
        BLE::InitializationCompleteCallbackContext context = {
            BLE::Instance(instanceID),
            BLE_ERROR_ALREADY_INITIALIZED
        };
        callback.call(&context);
        return BLE_ERROR_ALREADY_INITIALIZED;
    }
  
    // Init the BlueNRG/BlueNRG-MS stack
    //btleInit();  //ST
    // BlueNRG-1 stack init
    /*ret = BlueNRG_Stack_Initialization(&BlueNRG_Stack_Init_params);
    if (ret != BLE_STATUS_SUCCESS) {
        printf("Error in BlueNRG_Stack_Initialization() 0x%02x\r\n", ret);
        while(1);
    }*/
    printf("STACK INIT \n\r");

    isInitialized = true;
    BLE::InitializationCompleteCallbackContext context = {
        BLE::Instance(instanceID),
        BLE_ERROR_NONE
    };
    callback.call(&context);
  
    return BLE_ERROR_NONE;
}



void BlueNRG1_ble::processEvents() {
    //btle_handler();   //ST
  printf("btle_handler()\n\r");
}


GattServer &BlueNRG1_ble::getGattServer(){
    return BlueNRG1_GattServer::getInstance();
}


Gap        &BlueNRG1_ble::getGap(){
    return BlueNRG1_Gap::getInstance();
}

const Gap  &BlueNRG1_ble::getGap() const{
    return BlueNRG1_Gap::getInstance();
}