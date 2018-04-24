#ifndef __BLUENRG1GAP_H__
#define __BLUENRG1GAP_H__

#ifdef YOTTA_CFG_MBED_OS
    #include "mbed-drivers/mbed.h"
#else
    #include "mbed.h"
#endif 

#include "ble/blecommon.h"
#include "btle.h"
#include "ble/GapAdvertisingParams.h"
#include "ble/GapAdvertisingData.h"
#include "ble/Gap.h"

#define BLE_CONN_HANDLE_INVALID 0x0
#define BDADDR_SIZE 6

#define BLUENRG_GAP_ADV_INTERVAL_MIN (0x0020)
#define BLUENRG_GAP_ADV_INTERVAL_MAX (0x4000)
#define BLUENRG_GAP_ADV_NONCON_INTERVAL_MIN (0x00A0)

// Scanning and Connection Params used by Central for creating connection
#define GAP_OBSERVATION_PROC (0x80)

#define SCAN_P         (0x0010)
#define SCAN_L         (0x0010)
#define SUPERV_TIMEOUT (0xC80)
#define CONN_P(x)      ((int)((x)/1.25f))
#define CONN_L(x)      ((int)((x)/0.625f))
#define CONN_P1        ((int)(_advParams.getInterval()+5)/1.25f)//(0x4C)//(0x6C)
#define CONN_P2        ((int)(_advParams.getInterval()+5)/1.25f)//(0x4C)//(0x6C)
#define CONN_L1        (0x0008)
#define CONN_L2        (0x0008)
#define GUARD_INT      5 //msec
#define MIN_INT_CONN   0x0006 //=>7.5msec
#define MAX_INT_CONN   0x0C80 //=>4000msec
#define DEF_INT_CONN   0x0140 //=>400msec (default value for connection interval)

/**************************************************************************/

/**************************************************************************/
class BlueNRG1Gap : public Gap
{
public:
    static BlueNRG1Gap &getInstance() {
        static BlueNRG1Gap m_instance;
        return m_instance;
    }
    
    virtual ble_error_t setAdvertisingData(const GapAdvertisingData &, const GapAdvertisingData &);
    virtual ble_error_t startAdvertising(const GapAdvertisingParams &);
    
    // ADV timeout handling
    Timeout& getAdvTimeout(void) {
        return advTimeout;
    }
    void setAdvToFlag(void);
    
    void Process(void);
    
    // SCAN timeout handling
    Timeout scanTimeout;
    bool ScanToFlag;
    
    ble_error_t getAddress(BLEProtocol::AddressType_t *, BLEProtocol::AddressBytes_t );

private:
    uint8_t deviceAppearance[2];
    GapAdvertisingData _advData;
    GapAdvertisingData _scanResponse;
    void setAdvParameters(void);
    //uint16_t scanInterval;
    //uint16_t scanWindow;
    uint16_t advInterval;
    //uint16_t slaveConnIntervMin;
    //uint16_t slaveConnIntervMax;
    uint16_t conn_min_interval;
    uint16_t conn_max_interval;
    //void setAdvParameters(void);
    AddressType_t addr_type;
    
    // ADV timeout handling
    Timeout advTimeout;
    bool AdvToFlag;
    
    
    uint8_t bdaddr[BDADDR_SIZE];
    

    
};



#endif //__BLUENRG1GAP_H__