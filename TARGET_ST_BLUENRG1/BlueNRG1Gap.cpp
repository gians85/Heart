#include "BlueNRG1Gap.h"
#include "ble_debug.h"
#include "ble_payload.h"
#include "ble_utils.h"

extern "C" {
#include "BlueNRG1BLEStack.h"
}

/*
 * Utility to process GAP specific events (e.g., Advertising timeout)
 */
void BlueNRG1Gap::Process(void)
{
    if(AdvToFlag) {
        AdvToFlag = false;
        stopAdvertising();
    }

    if(ScanToFlag) {
        ScanToFlag = false;
        stopScan();
    }
}


/*
 * Utility to set ADV timeout flag
 */
void BlueNRG1Gap::setAdvToFlag(void) {
    AdvToFlag = true;
    //signalEventsToProcess();
    PRINTF("!!! HAVE TO IMPLEMENT signalEventsToProcess()\r\n");
}

/*
 * ADV timeout callback
 */
#ifdef AST_FOR_MBED_OS
static void advTimeoutCB(void)
{
    BlueNRGGap::getInstance().stopAdvertising();
}
#else
static void advTimeoutCB(void)
{
    BlueNRG1Gap::getInstance().setAdvToFlag();

    Timeout& t = BlueNRG1Gap::getInstance().getAdvTimeout();
    t.detach(); /* disable the callback from the timeout */
}
#endif /* AST_FOR_MBED_OS */


ble_error_t BlueNRG1Gap::setAdvertisingData(const GapAdvertisingData &advData, const GapAdvertisingData &scanResponse)
{
    /*
    PRINTF("BlueNRG1Gap::setAdvertisingData\n\r");
    return BLE_ERROR_NONE;
    */
    
    
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
    /*return BLE_ERROR_NONE; */
    
    tBleStatus ret;
    int err;

    /* Make sure we support the advertising type */
    if (params.getAdvertisingType() == GapAdvertisingParams::ADV_CONNECTABLE_DIRECTED) {
        /* ToDo: This requires a proper security implementation, etc. */
        return BLE_ERROR_NOT_IMPLEMENTED;
    }

    /* Check interval range */
    if (params.getAdvertisingType() == GapAdvertisingParams::ADV_NON_CONNECTABLE_UNDIRECTED) {
        /* Min delay is slightly longer for unconnectable devices */
        if ((params.getIntervalInADVUnits() < GapAdvertisingParams::GAP_ADV_PARAMS_INTERVAL_MIN_NONCON) ||
                (params.getIntervalInADVUnits() > GapAdvertisingParams::GAP_ADV_PARAMS_INTERVAL_MAX)) {
            return BLE_ERROR_PARAM_OUT_OF_RANGE;
        }
    } else {
        if ((params.getIntervalInADVUnits() < GapAdvertisingParams::GAP_ADV_PARAMS_INTERVAL_MIN) ||
                (params.getIntervalInADVUnits() > GapAdvertisingParams::GAP_ADV_PARAMS_INTERVAL_MAX)) {
            return BLE_ERROR_PARAM_OUT_OF_RANGE;
        }
    }

    /* Check timeout is zero for Connectable Directed */
    if ((params.getAdvertisingType() == GapAdvertisingParams::ADV_CONNECTABLE_DIRECTED) && (params.getTimeout() != 0)) {
        /* Timeout must be 0 with this type, although we'll never get here */
        /* since this isn't implemented yet anyway */
        return BLE_ERROR_PARAM_OUT_OF_RANGE;
    }

    /* Check timeout for other advertising types */
    if ((params.getAdvertisingType() != GapAdvertisingParams::ADV_CONNECTABLE_DIRECTED) &&
            (params.getTimeout() > GapAdvertisingParams::GAP_ADV_PARAMS_TIMEOUT_MAX)) {
        return BLE_ERROR_PARAM_OUT_OF_RANGE;
    }

    /*
     * Advertising filter policy setting
     * FIXME: the Security Manager should be implemented
     */
    AdvertisingPolicyMode_t mode = getAdvertisingPolicyMode();
    if(mode != ADV_POLICY_IGNORE_WHITELIST) {
        ret = aci_gap_configure_whitelist();
        if(ret != BLE_STATUS_SUCCESS) {
          PRINTF("aci_gap_configure_whitelist ret=0x%x\n\r", ret);
          return BLE_ERROR_OPERATION_NOT_PERMITTED;
        }
    }

    uint8_t advFilterPolicy = NO_WHITE_LIST_USE;
    switch(mode) {
        case ADV_POLICY_FILTER_SCAN_REQS:
            advFilterPolicy = WHITE_LIST_FOR_ONLY_SCAN;
            break;
        case ADV_POLICY_FILTER_CONN_REQS:
            advFilterPolicy = WHITE_LIST_FOR_ONLY_CONN;
            break;
        case ADV_POLICY_FILTER_ALL_REQS:
            advFilterPolicy = WHITE_LIST_FOR_ALL;
            break;
        default:
            advFilterPolicy = NO_WHITE_LIST_USE;
            break;
    }

    /* Check the ADV type before setting scan response data */
    if (params.getAdvertisingType() == GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED ||
        params.getAdvertisingType() == GapAdvertisingParams::ADV_SCANNABLE_UNDIRECTED) {

        /* set scan response data */
        PRINTF(" setting scan response data (_scanResponseLen=%u)\n\r", _scanResponse.getPayloadLen());
        //ret = hci_le_set_scan_resp_data(_scanResponse.getPayloadLen(), _scanResponse.getPayload());
        ret = hci_le_set_scan_response_data(_scanResponse.getPayloadLen(), (uint8_t*)_scanResponse.getPayload());

        if(BLE_STATUS_SUCCESS!=ret) {
            PRINTF(" error while setting scan response data (ret=0x%x)\n\r", ret);
            switch (ret) {
              case BLE_STATUS_TIMEOUT:
                return BLE_STACK_BUSY;
              default:
                return BLE_ERROR_UNSPECIFIED;
            }
        }
    } else {
        //hci_le_set_scan_resp_data(0, NULL);
        hci_le_set_scan_response_data(0, NULL);
    }

    setAdvParameters();
    PRINTF("advInterval=%d advType=%d\n\r", advInterval, params.getAdvertisingType());

    err = hci_le_set_advertising_data(_advData.getPayloadLen(), (uint8_t*)_advData.getPayload());

    if (err) {
        PRINTF("error while setting the payload\r\n");
        return BLE_ERROR_UNSPECIFIED;
    }

    tBDAddr dummy_addr = { 0 };
    uint16_t advIntervalMin = advInterval == GapAdvertisingParams::GAP_ADV_PARAMS_INTERVAL_MAX ? advInterval - 1 : advInterval;
    uint16_t advIntervalMax = advIntervalMin + 1;

    err = hci_le_set_advertising_parameters(
        advIntervalMin,
        advIntervalMax,
        params.getAdvertisingType(),
    	addr_type,
        0x00,
        dummy_addr,
        /* all channels */ 7,
    	advFilterPolicy
    );

    if (err) {
        PRINTF("impossible to set advertising parameters\n\r");
        PRINTF("advInterval min: %u, advInterval max: %u\n\r", advInterval, advInterval + 1);
        PRINTF("advType: %u, advFilterPolicy: %u\n\r", params.getAdvertisingType(), advFilterPolicy);
        return BLE_ERROR_INVALID_PARAM;
    }

    err = hci_le_set_advertise_enable(0x01);
    if (err) {
        PRINTF("impossible to start advertising\n\r");
        return BLE_ERROR_UNSPECIFIED;
    }

    if(params.getTimeout() != 0) {
        PRINTF("!!! attaching adv to!!!\n\r");
#ifdef AST_FOR_MBED_OS
        minar::Scheduler::postCallback(advTimeoutCB).delay(minar::milliseconds(params.getTimeout() * 1000));
#else
        advTimeout.attach(advTimeoutCB, params.getTimeout());
#endif
    }

    return BLE_ERROR_NONE;
    
    
}

/**************************************************************************/
/*!
    @brief  Set advertising parameters according to the current state
            Parameters value is set taking into account guidelines of the BlueNRG
            time slots allocation
*/
/**************************************************************************/
void BlueNRG1Gap::setAdvParameters(void)
{
    uint32_t advIntMS;
    
    if(state.connected == 1) {
        advIntMS = (conn_min_interval*1.25)-GUARD_INT;
        advInterval = _advParams.MSEC_TO_ADVERTISEMENT_DURATION_UNITS(advIntMS);
        
        PRINTF("conn_min_interval is equal to %u\r\n", conn_min_interval);
    } else {
        advInterval = _advParams.getIntervalInADVUnits();
    }
}


ble_error_t BlueNRG1Gap::getAddress(BLEProtocol::AddressType_t *typeP, BLEProtocol::AddressBytes_t address)
{
    uint8_t bdaddr[BDADDR_SIZE];
    uint8_t data_len_out;

    // precondition, check that pointers in input are valid
    if (typeP == NULL || address == NULL) {
        return BLE_ERROR_INVALID_PARAM;
    }

    if (addr_type == BLEProtocol::AddressType::PUBLIC) {
        tBleStatus ret = aci_hal_read_config_data(CONFIG_DATA_PUBADDR_OFFSET, &data_len_out, bdaddr);
        if(ret != BLE_STATUS_SUCCESS || data_len_out != BDADDR_SIZE) {
            return BLE_ERROR_UNSPECIFIED;
        }
    } else if (addr_type == BLEProtocol::AddressType::RANDOM_STATIC) {
        // FIXME hci_read_bd_addr and
        // aci_hal_read_config_data CONFIG_DATA_RANDOM_ADDRESS_IDB05A1
        // does not work, use the address stored in class data member
        memcpy(bdaddr, this->bdaddr, sizeof(bdaddr));
    } else {
        // FIXME: should be implemented with privacy features
        // BLEProtocol::AddressType::RANDOM_PRIVATE_NON_RESOLVABLE
        // BLEProtocol::AddressType::RANDOM_PRIVATE_RESOLVABLE
        return BLE_ERROR_NOT_IMPLEMENTED;
    }

    *typeP = addr_type;
    memcpy(address, bdaddr, BDADDR_SIZE);

    return BLE_ERROR_NONE;
}

