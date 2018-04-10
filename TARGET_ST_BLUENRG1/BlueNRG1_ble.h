#ifndef __BLUENRG1_BLE_H__
#define __BLUENRG1_BLE_H__

#include "ble/BLE.h"
#include "ble/blecommon.h"
#include "ble/BLEInstanceBase.h"
#include "ble/generic/GenericGattClient.h"

#include "BlueNRG1_Gap.h"
#include "BlueNRG1_GattServer.h"
#include "BlueNRG1_SecurityManager.h"
#include "BlueNRG1_GattClient.h"

//#include "btle.h"

class BlueNRG1_ble : public BLEInstanceBase
{
public:
    BlueNRG1_ble(void);
    virtual ~BlueNRG1_ble(void);
    
    
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
        return BlueNRG1_GattClient::getInstance();
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
    static BlueNRG1_ble& Instance(BLE::InstanceID_t instanceId);
    uint8_t ret;
    
    
/* public:
    BlueNRG1_ble(void);
    virtual ~BlueNRG1_ble(void);
    
    
public:
    static BlueNRG1_ble& Instance(BLE::InstanceID_t instanceId);

private:
    bool              initialized;
    BLE::InstanceID_t instanceID;
    
    virtual void processEvents();
    virtual ble_error_t init();
*/    
/*
    virtual ble_error_t init(BLE::InstanceID_t instanceID, FunctionPointerWithContext<BLE::InitializationCompleteCallbackContext *> callback);
    virtual bool        hasInitialized(void) const {
        return initialized;
    }
    virtual ble_error_t shutdown(void);
    virtual const char *getVersion(void);

//    /**
//     * Accessors to GAP. This function checks whether gapInstance points to an
//     * object. If if does not, then the gapInstance is updated to
//     * &_getInstance before returning.
//     *
//     * @return  A reference to GattServer.
//     *
//     * @note  Unlike the GattClient, GattServer and SecurityManager, Gap is
//     *        always needed in a BLE application. Therefore it is allocated
//     *        statically.
//     *
    virtual Gap &getGap() {
        return gapInstance;
    };

//    /**
//     * Accessors to GATT Server. This function checks whether a GattServer
//     * object was previously instantiated. If such object does not exist, then
//     * it is created before returning.
//     *
//     * @return  A reference to GattServer.
//     *
    virtual GattServer &getGattServer() {
        if (gattServerInstance == NULL) {
            gattServerInstance = new BlueNRG1_GattServer();
        }
        return *gattServerInstance;
    };

//    /**
//     * Accessors to GATT Client. This function checks whether a GattClient
//     * object was previously instantiated. If such object does not exist, then
//     * it is created before returning.
//     *
//     * @return  A reference to GattClient.
//     *
    virtual GattClient &getGattClient() {
        return gattClient;
    }

//    /**
//     * Accessors to Security Manager. This function checks whether a SecurityManager
//     * object was previously instantiated. If such object does not exist, then
//     * it is created before returning.
//     *
//     * @return  A reference to GattServer.
//     *
    virtual nRF5xSecurityManager &getSecurityManager() {
        if (securityManagerInstance == NULL) {
            securityManagerInstance = new BlueNRG1_SecurityManager();
        }
        return *securityManagerInstance;
    }

//    /**
//     * Accessors to GAP. This function checks whether gapInstance points to an
//     * object. If if does not, then the gapInstance is updated to
//     * &_getInstance before returning.
//     *
//     * @return  A const reference to GattServer.
//     *
//     * @note  Unlike the GattClient, GattServer and SecurityManager, Gap is
//     *        always needed in a BLE application. Therefore it is allocated
//     *        statically.
//     *
//     * @note  The accessor is able to modify the object's state because the
//     *        internal pointer has been declared mutable.
//     *
    virtual const BlueNRG1_xGap &getGap() const  {
        return gapInstance;
    };

//    /**
//     * Accessors to GATT Server. This function checks whether a GattServer
//     * object was previously instantiated. If such object does not exist, then
//     * it is created before returning.
//     *
//     * @return  A const reference to GattServer.
//     *
//     * @note  The accessor is able to modify the object's state because the
//     *        internal pointer has been declared mutable.
//     *
    virtual const BlueNRG1_GattServer &getGattServer() const {
        if (gattServerInstance == NULL) {
            gattServerInstance = new BlueNRG1_GattServer();
        }
        return *gattServerInstance;
    };

//    /**
//     * Accessors to Security Manager. This function checks whether a SecurityManager
//     * object was previously instantiated. If such object does not exist, then
//     * it is created before returning.
//     *
//     * @return  A const reference to GattServer.
//     *
//     * @note  The accessor is able to modify the object's state because the
//     *        internal pointer has been declared mutable.
//     *
    virtual const BlueNRG1_SecurityManager &getSecurityManager() const {
        if (securityManagerInstance == NULL) {
            securityManagerInstance = new BlueNRG1_SecurityManager();
        }
        return *securityManagerInstance;
    }

    virtual void waitForEvent(void);

    virtual void processEvents();

public:
    static BlueNRG1_ble& Instance(BLE::InstanceID_t instanceId);

private:
    bool              initialized;
    BLE::InstanceID_t instanceID;

private:
    mutable BlueNRG1_Gap gapInstance; // /**< Gap instance whose reference is returned from a call to
//                                   * getGap(). Unlike the GattClient, GattServer and
//                                   * SecurityManager, Gap is always needed in a BLE application. *

private:
    mutable BlueNRG1_GattServer      *gattServerInstance;     // /**< Pointer to the GattServer object instance.
//                                                            *   If NULL, then GattServer has not been initialized.
//                                                            *   The pointer has been declared as 'mutable' so that
//                                                            *   it can be assigned inside a 'const' function. *
    ble::generic::GenericGattClient gattClient;

    mutable BlueNRG1_SecurityManager *securityManagerInstance; // /**< Pointer to the SecurityManager object instance.
//                                                            *   If NULL, then SecurityManager has not been initialized.
//                                                            *   The pointer has been declared as 'mutable' so that
//                                                            *   it can be assigned inside a 'const' function. *

*/
};

#endif //__BLUENRG1_BLE_H__
