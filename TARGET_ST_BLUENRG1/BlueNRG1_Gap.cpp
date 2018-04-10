#include "BlueNRG1_Gap.h"

ble_error_t BlueNRG1_Gap::setAdvertisingData(const GapAdvertisingData &advData, const GapAdvertisingData &scanResponse)
{
    printf("setAdvertisingData()\n\r");
    return BLE_ERROR_NONE;
}

ble_error_t BlueNRG1_Gap::startAdvertising(const GapAdvertisingParams &params)
{   
    printf("startAdvertising()\n\r");     
    return BLE_ERROR_NONE;
}