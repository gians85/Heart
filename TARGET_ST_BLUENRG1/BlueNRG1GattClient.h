#ifndef __BLUENRG1GATTCLIENT_H__
#define __BLUENRG1GATTCLIENT_H__

#ifdef YOTTA_CFG_MBED_OS
    #include "mbed-drivers/mbed.h"
#else
    #include "mbed.h"
#endif 
#include "ble/blecommon.h"
#include "btle.h"
#include "ble/GattClient.h"
#include "ble/DiscoveredService.h"
#include "ble/CharacteristicDescriptorDiscovery.h"
#include "BlueNRG1DiscoveredCharacteristic.h"
#include "BlueNRG1GattConnectionClient.h"

using namespace std;

#define MAX_ACTIVE_CONNECTIONS 7

class BlueNRG1GattClient : public GattClient
{
public:
    static BlueNRG1GattClient &getInstance() {
        static BlueNRG1GattClient m_instance;
        return m_instance;
    }

    ble_error_t createGattConnectionClient(Gap::Handle_t connectionHandle);
    ble_error_t removeGattConnectionClient(Gap::Handle_t connectionHandle, uint8_t reason);
    
    /* Functions that must be implemented from GattClient */
    virtual ble_error_t launchServiceDiscovery(Gap::Handle_t                               connectionHandle,
                                               ServiceDiscovery::ServiceCallback_t         sc                           = NULL,
                                               ServiceDiscovery::CharacteristicCallback_t  cc                           = NULL,
                                               const UUID                                 &matchingServiceUUID          = UUID::ShortUUIDBytes_t(BLE_UUID_UNKNOWN),
                                               const UUID                                 &matchingCharacteristicUUIDIn = UUID::ShortUUIDBytes_t(BLE_UUID_UNKNOWN));

    virtual ble_error_t discoverServices(Gap::Handle_t                        connectionHandle,
                                         ServiceDiscovery::ServiceCallback_t  callback,
                                         const UUID                          &matchingServiceUUID = UUID::ShortUUIDBytes_t(BLE_UUID_UNKNOWN));

    virtual ble_error_t discoverServices(Gap::Handle_t                        connectionHandle,
                                         ServiceDiscovery::ServiceCallback_t  callback,
                                         GattAttribute::Handle_t              startHandle,
                                         GattAttribute::Handle_t              endHandle);

    virtual bool isServiceDiscoveryActive(void) const;
    virtual void terminateServiceDiscovery(void);
    virtual void onServiceDiscoveryTermination(ServiceDiscovery::TerminationCallback_t callback);
    virtual ble_error_t read(Gap::Handle_t connHandle, GattAttribute::Handle_t attributeHandle, uint16_t offset) const;
    virtual ble_error_t write(GattClient::WriteOp_t    cmd,
                              Gap::Handle_t            connHandle,
                              GattAttribute::Handle_t  attributeHandle,
                              size_t                   length,
                              const uint8_t           *value) const;
    virtual ble_error_t discoverCharacteristicDescriptors(
        const DiscoveredCharacteristic& characteristic,
        const CharacteristicDescriptorDiscovery::DiscoveryCallback_t& discoveryCallback,
        const CharacteristicDescriptorDiscovery::TerminationCallback_t& terminationCallback);

    virtual ble_error_t reset(void);

    void gattProcedureCompleteCB(Gap::Handle_t connectionHandle, uint8_t error_code);

    void primaryServicesCB(Gap::Handle_t connectionHandle,
                           uint8_t event_data_length,
                           uint8_t attribute_data_length,
                           uint8_t *attribute_data_list);
    
    void primaryServiceCB(Gap::Handle_t connectionHandle,
                          uint8_t event_data_length,
                          uint8_t *handles_info_list);
    
    ble_error_t findServiceChars(Gap::Handle_t connectionHandle);
    
    void serviceCharsCB(Gap::Handle_t connectionHandle,
                        uint8_t event_data_length,
                        uint8_t handle_value_pair_length,
                        uint8_t *handle_value_pair);
    
    void serviceCharByUUIDCB(Gap::Handle_t connectionHandle,
                             uint8_t event_data_length,
                             uint16_t attr_handle,
                             uint8_t *attr_value);

    void discAllCharacDescCB(Gap::Handle_t connHandle,
                             uint8_t event_data_length,
                             uint8_t format,
                             uint8_t *handle_uuid_pair);

    void charReadCB(Gap::Handle_t connHandle,
                    uint8_t event_data_length,
                    uint8_t* attribute_value);

    void charWritePrepareCB(Gap::Handle_t connHandle,
                            uint8_t event_data_length,
                            uint16_t attribute_handle,
                            uint16_t offset,
                            uint8_t *part_attr_value);
    
    void charWriteExecCB(Gap::Handle_t connHandle);


protected:

    BlueNRG1GattClient(): _connectionPool() {};

    ServiceDiscovery::TerminationCallback_t terminationCallback;

private:

  BlueNRG1GattClient(BlueNRG1GattClient const &);
  void operator=(BlueNRG1GattClient const &);

  BlueNRG1GattConnectionClient *_connectionPool[MAX_ACTIVE_CONNECTIONS];
  uint8_t _numConnections;

  BlueNRG1GattConnectionClient * getGattConnectionClient(Gap::Handle_t connectionHandle);

};

#endif /* __BLUENRG1GATTCLIENT_H__ */