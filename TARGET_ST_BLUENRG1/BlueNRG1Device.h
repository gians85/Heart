#ifndef __BLUENRG1DEVICE_H__
#define __BLUENRG1DEVICE_H__

#define BLUENRG
#define DEBUG_BLUENRG_USER

#include "btle.h"

#ifdef YOTTA_CFG_MBED_OS
    #include "mbed-drivers/mbed.h"
#else
    #include "mbed.h"
#endif 

#include "ble/BLE.h"
#include "ble/blecommon.h"
#include "ble/BLEInstanceBase.h"
//#include "ble/generic/GenericGattClient.h"

#include "BlueNRG1Gap.h"
#include "BlueNRG1GattServer.h"
#include "BlueNRG1GattClient.h"


class BlueNRG1Device : public BLEInstanceBase
{
public:
    BlueNRG1Device(void);
    virtual ~BlueNRG1Device(void);
    
    
    virtual void processEvents();
    virtual ble_error_t init(BLE::InstanceID_t instanceID, FunctionPointerWithContext<BLE::InitializationCompleteCallbackContext *> callback);
    virtual bool hasInitialized(void) const {
        return isInitialized;
    }
    virtual ble_error_t shutdown(void);   
    virtual const char *getVersion(void);
    virtual Gap&        getGap();
    virtual const Gap&  getGap() const;
    virtual GattServer& getGattServer();
    virtual const GattServer& getGattServer() const;
    virtual GattClient& getGattClient() {
        return BlueNRG1GattClient::getInstance();
    }
    virtual SecurityManager& getSecurityManager() {
        return *sm;
    }
    virtual const SecurityManager& getSecurityManager() const {
        return *sm;
    }
    virtual void        waitForEvent(void);
    
    
    void reset(void);
    
/*
    uint8_t getUpdaterHardwareVersion(uint8_t *hw_version);
    int updateFirmware(const uint8_t *fw_image, uint32_t fw_size);
    bool dataPresent();
    int32_t spiRead(uint8_t *buffer, uint8_t buff_size);
    int32_t spiWrite(uint8_t* data1, uint8_t* data2, uint8_t Nb_bytes1, uint8_t Nb_bytes2);
    void disable_irq();
    void enable_irq();  */
    
private:
    bool isInitialized;
    SecurityManager *sm;
    
    
public:
    static BlueNRG1Device& Instance(BLE::InstanceID_t instanceId);
    uint8_t ret;
    
    
};

#endif //__BLUENRG1DEVICE_H__
