#ifndef __BLUENRG1GATTSERVER_H__
#define __BLUENRG1GATTSERVER_H__

#ifdef YOTTA_CFG_MBED_OS
    #include "mbed-drivers/mbed.h"
#else
    #include "mbed.h"
#endif 
#include "ble/blecommon.h"
#include "btle.h"
#include "ble/GattService.h"
#include "ble/GattServer.h"
#include <vector>
#include <map>

#define BLE_TOTAL_CHARACTERISTICS 10

using namespace std;

class BlueNRG1GattServer : public GattServer
{
public:
    static BlueNRG1GattServer &getInstance() {
        static BlueNRG1GattServer m_instance;
        return m_instance;
    }

    enum HandleEnum_t {
        CHAR_HANDLE = 0,
        CHAR_VALUE_HANDLE,
        CHAR_DESC_HANDLE
    };

    /* Functions that must be implemented from GattServer */
    virtual ble_error_t addService(GattService &);
    virtual ble_error_t read(GattAttribute::Handle_t attributeHandle, uint8_t buffer[], uint16_t *lengthP);
    virtual ble_error_t read(Gap::Handle_t connectionHandle, GattAttribute::Handle_t attributeHandle, uint8_t buffer[], uint16_t *lengthP);
    virtual ble_error_t write(GattAttribute::Handle_t, const uint8_t[], uint16_t, bool localOnly = false);
    virtual ble_error_t write(Gap::Handle_t connectionHandle, GattAttribute::Handle_t, const uint8_t[], uint16_t, bool localOnly = false);
    virtual ble_error_t initializeGATTDatabase(void);

    virtual bool isOnDataReadAvailable() const {
        return true;
    }

    virtual ble_error_t reset(void);

    /* BlueNRG Functions */
    void eventCallback(void);
    //void hwCallback(void *pckt);
    ble_error_t Read_Request_CB(uint16_t attributeHandle);
    uint8_t Write_Request_CB(
        uint16_t connection_handle, uint16_t attr_handle,
        uint8_t data_length, const uint8_t* data
    );
    GattCharacteristic* getCharacteristicFromHandle(uint16_t charHandle);
    void HCIDataWrittenEvent(const GattWriteCallbackParams *params);
    void HCIDataReadEvent(const GattReadCallbackParams *params);
    void HCIEvent(GattServerEvents::gattEvent_e type, uint16_t charHandle);
    void HCIDataSentEvent(unsigned count);

private:

    // compute the number of attribute record needed by a service
    static uint16_t computeAttributesRecord(GattService& service);


    static const int MAX_SERVICE_COUNT = 10;
    uint8_t serviceCount;
    uint8_t characteristicCount;
    uint16_t servHandle, charHandle;

    std::map<uint16_t, uint16_t> bleCharHandleMap;  // 1st argument is characteristic, 2nd argument is service
    GattCharacteristic *p_characteristics[BLE_TOTAL_CHARACTERISTICS];
    uint16_t bleCharacteristicHandles[BLE_TOTAL_CHARACTERISTICS];

    BlueNRG1GattServer() {
        serviceCount = 0;
        characteristicCount = 0;
    };

    BlueNRG1GattServer(BlueNRG1GattServer const &);
    void operator=(BlueNRG1GattServer const &);

    static const int CHAR_DESC_TYPE_16_BIT=0x01;
    static const int CHAR_DESC_TYPE_128_BIT=0x02;
    static const int CHAR_DESC_SECURITY_PERMISSION=0x00;
    static const int CHAR_DESC_ACCESS_PERMISSION=0x03;
    static const int CHAR_ATTRIBUTE_LEN_IS_FIXED=0x00;
};

#endif /* __BLUENRG1GATTSERVER_H__ */






/*
#ifndef __BLUENRG1GATTSERVER_H__
#define __BLUENRG1GATTSERVER_H__

#ifdef YOTTA_CFG_MBED_OS
    #include "mbed-drivers/mbed.h"
#else
    #include "mbed.h"
#endif 
#include "ble/blecommon.h"
//#include "btle.h"
#include "ble/GattService.h"
#include "ble/GattServer.h"
#include <vector>
#include <map>

#define BLE_TOTAL_CHARACTERISTICS 10

using namespace std;

class BlueNRG1GattServer : public GattServer
{
public:
    static BlueNRG1GattServer &getInstance() {
        static BlueNRG1GattServer m_instance;
        return m_instance;
    }
    
};

#endif //__BLUENRG1GATTSERVER_H__

*/