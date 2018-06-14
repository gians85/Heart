
#include "mbed.h"
#include "ble/BLE.h"


DigitalOut led1(LED1, 0);
DigitalOut led3(LED3);
DigitalIn  button(PUSH1);


#define THERMOMETER




#ifdef THERMOMETER

#include "mbed.h"
#include "ble/BLE.h"
#include "ble/services/HealthThermometerService.h"

#define TEMP_MIN 36.6
#define TEMP_MAX 39.0
 
static HealthThermometerService *thermometerServicePtr;

 
static const char     DEVICE_NAME[]        = "THR_Gian";
static const uint16_t uuid16_list[]        = {GattService::UUID_HEALTH_THERMOMETER_SERVICE};
static volatile bool  triggerSensorPolling = false;
static float          currentTemperature   = TEMP_MIN;
 
/* Restart Advertising on disconnection*/
void disconnectionCallback(const Gap::DisconnectionCallbackParams_t *params)
{
    BLE::Instance().gap().startAdvertising();
}
 

 
void bleInitComplete(BLE::InitializationCompleteCallbackContext *params)
{
    BLE&        ble   = params->ble;
    ble_error_t error = params->error;
 
    /* Check BLE errors */
    if (error != BLE_ERROR_NONE) {
        return;
    }
 
    /* Ensure that it is the default instance of BLE */
    if(ble.getInstanceID() != BLE::DEFAULT_INSTANCE) {
        return;
    }
 
    ble.gap().onDisconnection(disconnectionCallback);
 
    /* Setup primary service. */
    thermometerServicePtr = new HealthThermometerService(ble, currentTemperature, HealthThermometerService::LOCATION_EAR);
 
    /* setup advertising */
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::BREDR_NOT_SUPPORTED | GapAdvertisingData::LE_GENERAL_DISCOVERABLE);
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LIST_16BIT_SERVICE_IDS, (uint8_t *)uuid16_list, sizeof(uuid16_list));
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::THERMOMETER_EAR);
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LOCAL_NAME, (uint8_t *)DEVICE_NAME, sizeof(DEVICE_NAME));
    ble.gap().setAdvertisingType(GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED);
    ble.gap().setAdvertisingInterval(1000); /* 1000ms */
    ble.gap().startAdvertising();    
}
 
int main(void)
{
    //led1 = 1;
    //Ticker ticker;
    //ticker.attach(periodicCallback, 1);
 
    BLE &ble = BLE::Instance();
    ble.init(bleInitComplete);
    
    /* SpinWait for initialization to complete. This is necessary because the
     * BLE object is used in the main loop below. */
    while (ble.hasInitialized()  == false) { /* spin loop */ }
 
//    while (true) {
//        if (triggerSensorPolling && ble.gap().getState().connected) {
//            triggerSensorPolling = false;
// 
//            /* In our case, we simply update the dummy temperature measurement. */
//            currentTemperature += 0.1;
//            thermometerServicePtr->updateTemperature(currentTemperature);
//        } else {
//            ble.waitForEvent();
//        }
//    }
    
     // infinite loop
    while (1) {
        led1 = 1;
        wait_ms(500);
        
        if (triggerSensorPolling && ble.gap().getState().connected) {
            triggerSensorPolling = false;
 
            /* In our case, we simply update the dummy temperature measurement. */
            currentTemperature += 0.2;
            if (currentTemperature > TEMP_MAX) {
                currentTemperature = TEMP_MIN;
            }
            thermometerServicePtr->updateTemperature(currentTemperature);
        }
        ble.waitForEvent();

        led1 = 0;
        wait_ms(500);
    }
}

#endif //THERMOMETER








#ifdef HEARTRATE

#include "ble/services/HeartRateService.h"
#include "ble/services/BatteryService.h"
#include "ble/services/DeviceInformationService.h"

const static char     DEVICE_NAME[]        = "HRM_ble_gian";
static const uint16_t uuid16_list[]        = {GattService::UUID_HEART_RATE_SERVICE,
                                              GattService::UUID_DEVICE_INFORMATION_SERVICE};

uint8_t hrmCounter = 60; // init HRM to 60bps

HeartRateService         *hrService;
DeviceInformationService *deviceInfo;

void disconnectionCallback(const Gap::DisconnectionCallbackParams_t *params)
{
    BLE::Instance(BLE::DEFAULT_INSTANCE).gap().startAdvertising(); // restart advertising
}


void bleInitComplete(BLE::InitializationCompleteCallbackContext *params)
{
    BLE &ble          = params->ble;
    ble_error_t error = params->error;
    
    /* Check BLE errors */
    if (error != BLE_ERROR_NONE) {
        return;
    }
    
    /* Define diconnection action */
    ble.gap().onDisconnection(disconnectionCallback);

    /* Setup primary service. */
    hrService = new HeartRateService(ble, hrmCounter, HeartRateService::LOCATION_FINGER);

    /* Setup auxiliary service. */
    deviceInfo = new DeviceInformationService(ble, "ARM", "Model1", "SN1", "hw-rev1", "fw-rev1", "soft-rev1");

    /* Setup advertising. */
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::BREDR_NOT_SUPPORTED | GapAdvertisingData::LE_GENERAL_DISCOVERABLE);
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LIST_16BIT_SERVICE_IDS, (uint8_t *)uuid16_list, sizeof(uuid16_list));
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::GENERIC_HEART_RATE_SENSOR);
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LOCAL_NAME, (uint8_t *)DEVICE_NAME, sizeof(DEVICE_NAME));
    ble.gap().setAdvertisingType(GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED);
    ble.gap().setAdvertisingInterval(1000); /* 1000ms */
    ble.gap().startAdvertising();
}

int main(void)
{  
    
    //Ticker ticker;
    //ticker.attach(periodicCallback, 1); // blink LED every second

    BLE& ble = BLE::Instance(BLE::DEFAULT_INSTANCE);
    ble.init(bleInitComplete);

    /* SpinWait for initialization to complete. This is necessary because the
     * BLE object is used in the main loop below. */
    while (ble.hasInitialized()  == false) { /* spin loop */ }
    
    
    // infinite loop
    while (1) {
        led1 = 1;
        wait_ms(500);
        
        hrmCounter++;
        if (hrmCounter == 100) {
            hrmCounter = 60;
        }

        if ( ble.getGapState().connected ) {
            hrService->updateHeartRate(hrmCounter);      
        } 
        ble.waitForEvent();

        led1 = 0;
        wait_ms(500);
    }
}

#endif //HEARTRATE