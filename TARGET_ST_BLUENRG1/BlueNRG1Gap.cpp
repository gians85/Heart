#include "BlueNRG1Gap.h"
#include "ble_debug.h"
#include "ble_payload.h"
#include "ble_utils.h"

extern "C" {
#include "BlueNRG1BLEStack.h"
}


/*BASIC IMPLEMENTATION*/
ble_error_t BlueNRG1Gap::setAdvertisingData(const GapAdvertisingData &advData, const GapAdvertisingData &scanResponse)
{
    PRINTF("BlueNRG1Gap::setAdvertisingData\n\r");
    /* Make sure we don't exceed the advertising payload length */
    if (advData.getPayloadLen() > GAP_ADVERTISING_DATA_MAX_PAYLOAD) {
        PRINTF("Exceeded the advertising payload length\n\r");
        return BLE_ERROR_BUFFER_OVERFLOW;
    }

    /* Make sure we have a payload! */
    if (advData.getPayloadLen() != 0) {
        PayloadPtr loadPtr(advData.getPayload(), advData.getPayloadLen());

        /* Align the GAP Service Appearance Char value coherently
           This setting is duplicate (see below GapAdvertisingData::APPEARANCE)
           since BLE API has an overloaded function for appearance
        */
        STORE_LE_16(deviceAppearance, advData.getAppearance());
        setAppearance((GapAdvertisingData::Appearance)(deviceAppearance[1]<<8|deviceAppearance[0]));


        for(uint8_t index=0; index<loadPtr.getPayloadUnitCount(); index++) {
            loadPtr.getUnitAtIndex(index);

            PRINTF("adData[%d].length=%d\n\r", index,(uint8_t)(*loadPtr.getUnitAtIndex(index).getLenPtr()));
            PRINTF("adData[%d].AdType=0x%x\n\r", index,(uint8_t)(*loadPtr.getUnitAtIndex(index).getAdTypePtr()));

            switch(*loadPtr.getUnitAtIndex(index).getAdTypePtr()) {
            /**< TX Power Level (in dBm) */
            case GapAdvertisingData::TX_POWER_LEVEL:
                {
                PRINTF("Advertising type: TX_POWER_LEVEL\n\r");
                int8_t enHighPower = 0;
                int8_t paLevel = 0;

                int8_t dbm = *loadPtr.getUnitAtIndex(index).getDataPtr();
                tBleStatus ret = getHighPowerAndPALevelValue(dbm, enHighPower, paLevel);
#ifdef DEBUG
                PRINTF("dbm=%d, ret=%d\n\r", dbm, ret);
                PRINTF("enHighPower=%d, paLevel=%d\n\r", enHighPower, paLevel);
#endif
                if(ret == BLE_STATUS_SUCCESS) {
                  aci_hal_set_tx_power_level(enHighPower, paLevel);
                }
                break;
                }
            /**< Appearance */
            case GapAdvertisingData::APPEARANCE:
                {
                PRINTF("Advertising type: APPEARANCE\n\r");

                GapAdvertisingData::Appearance appearanceP;
                memcpy(deviceAppearance, loadPtr.getUnitAtIndex(index).getDataPtr(), 2);

                PRINTF("input: deviceAppearance= 0x%x 0x%x\n\r", deviceAppearance[1], deviceAppearance[0]);

                appearanceP = (GapAdvertisingData::Appearance)(deviceAppearance[1]<<8|deviceAppearance[0]);
                /* Align the GAP Service Appearance Char value coherently */
                setAppearance(appearanceP);
                break;
                }

            } // end switch

        } //end for

    }

    // update the advertising data in the shield if advertising is running
    if (state.advertising == 1) {
        //tBleStatus ret = hci_le_set_scan_resp_data(scanResponse.getPayloadLen(), scanResponse.getPayload());
        tBleStatus ret = hci_le_set_scan_response_data(scanResponse.getPayloadLen(), (uint8_t*)scanResponse.getPayload());

        if(BLE_STATUS_SUCCESS != ret) {
            PRINTF(" error while setting scan response data (ret=0x%x)\n", ret);
            switch (ret) {
                case BLE_STATUS_TIMEOUT:
                    return BLE_STACK_BUSY;
                default:
                    return BLE_ERROR_UNSPECIFIED;
            }
        }

        ret = hci_le_set_advertising_data(advData.getPayloadLen(), (uint8_t*)advData.getPayload());
        if (ret) {
            PRINTF("error while setting the payload\r\n");
            return BLE_ERROR_UNSPECIFIED;
        }
    }

    _advData = advData;
    _scanResponse = scanResponse;

    return BLE_ERROR_NONE;
}

ble_error_t BlueNRG1Gap::startAdvertising(const GapAdvertisingParams &params)
{   
    PRINTF("startAdvertising()\n\r");     
    return BLE_ERROR_NONE;
}

