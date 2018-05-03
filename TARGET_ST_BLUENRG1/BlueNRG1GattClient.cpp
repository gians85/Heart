
#include "BlueNRG1GattClient.h"
#ifdef YOTTA_CFG_MBED_OS
    #include "mbed-drivers/mbed.h"
#else
    #include "mbed.h"
#endif 
#include "BlueNRG1Gap.h"
#include "ble_utils.h"
#include "ble_debug.h"

#include <new>
#include <assert.h>


ble_error_t BlueNRG1GattClient::createGattConnectionClient(Gap::Handle_t connectionHandle)
{
  if(MAX_ACTIVE_CONNECTIONS <= _numConnections) {
    return BLE_ERROR_OPERATION_NOT_PERMITTED;
  }

  for(uint8_t i = 0; i < MAX_ACTIVE_CONNECTIONS; i++) {

    if(_connectionPool[i] == NULL) {
      BlueNRG1GattConnectionClient *gattConnectionClient = new(std::nothrow) BlueNRG1GattConnectionClient(this, connectionHandle);

      if (gattConnectionClient == NULL) {
        return BLE_ERROR_NO_MEM;
      }

      _connectionPool[i] = gattConnectionClient;
      _connectionPool[i]->onServiceDiscoveryTermination(terminationCallback);
      _numConnections++;

      PRINTF("createGattConnectionClient: _connectionPool index=%d\r\n", i);
      PRINTF("createGattConnectionClient: succesfully added new gattConnectionClient (_numConnections=%d)\r\n", _numConnections);
      break;
    }
  }

  return BLE_ERROR_NONE;
}


ble_error_t BlueNRG1GattClient::removeGattConnectionClient(Gap::Handle_t connectionHandle, uint8_t reason)
{
      PRINTF("HAVE TO IMPLEMENT removeGattConnectionClient\r\n");

/*
  PRINTF("removeGattConnectionClient: connectionHandle=%d reason=0x%x\r\n", connectionHandle, reason);

  for (uint8_t i = 0; i < MAX_ACTIVE_CONNECTIONS; i++) {
    PRINTF("removeGattConnectionClient: _connectionPool[%d]->_connectionHandle=%d\r\n", i, _connectionPool[i]->_connectionHandle);

    if(_connectionPool[i]->_connectionHandle == connectionHandle) {
      PRINTF("removeGattConnectionClient: Found gattConnectionClient\r\n");
      delete _connectionPool[i];
      _connectionPool[i] = NULL;

      _numConnections--;
      PRINTF("removeGattConnectionClient: succesfully removed gattConnectionClient (_numConnections=%d)\r\n", _numConnections);

      break;

    } else {
      return BLE_ERROR_INTERNAL_STACK_FAILURE;
    }
  }
*/
  return BLE_ERROR_NONE;
}



BlueNRG1GattConnectionClient * BlueNRG1GattClient::getGattConnectionClient(Gap::Handle_t connectionHandle) {
  PRINTF("getGattConnectionClient\r\n");
  PRINTF("HAD TO IMPLEMENT getGattConnectionClient\r\n");
  
/*
  for (uint8_t i = 0; i < MAX_ACTIVE_CONNECTIONS; i++) {
    PRINTF("getGattConnectionClient: _connectionPool[%d]->_connectionHandle=%d\r\n", i, _connectionPool[i]->_connectionHandle);

    if(_connectionPool[i]->_connectionHandle == connectionHandle) {
      PRINTF("getGattConnectionClient: Found gattConnectionClient\r\n");
      return _connectionPool[i];
    }
  }
*/
  return NULL;
}


void BlueNRG1GattClient::gattProcedureCompleteCB(Gap::Handle_t connectionHandle, uint8_t error_code) {

  if(error_code != BLE_STATUS_SUCCESS) {
    return;
  }

  BlueNRG1GattConnectionClient *gattConnectionClient = getGattConnectionClient(connectionHandle);

  assert(gattConnectionClient != NULL);

  gattConnectionClient->gattProcedureCompleteCB(error_code);
}

void BlueNRG1GattClient::primaryServicesCB(Gap::Handle_t connectionHandle,
                                          uint8_t event_data_length,
                                          uint8_t attribute_data_length,
                                          uint8_t *attribute_data_list) {

  BlueNRG1GattConnectionClient *gattConnectionClient = getGattConnectionClient(connectionHandle);

  assert(gattConnectionClient != NULL);

  gattConnectionClient->primaryServicesCB(event_data_length,
                                          attribute_data_length,
                                          attribute_data_list);
}
    
void BlueNRG1GattClient::primaryServiceCB(Gap::Handle_t connectionHandle,
                                         uint8_t event_data_length,
                                         uint8_t *handles_info_list) {

  BlueNRG1GattConnectionClient *gattConnectionClient = getGattConnectionClient(connectionHandle);

  assert(gattConnectionClient != NULL);

  gattConnectionClient->primaryServiceCB(event_data_length,
                                         handles_info_list);
}
    
ble_error_t BlueNRG1GattClient::findServiceChars(Gap::Handle_t connectionHandle) {

  BlueNRG1GattConnectionClient *gattConnectionClient = getGattConnectionClient(connectionHandle);

  if(gattConnectionClient != NULL) {
    return gattConnectionClient->findServiceChars();
  } else {
    return BLE_ERROR_INTERNAL_STACK_FAILURE;
  }
}
    
void BlueNRG1GattClient::serviceCharsCB(Gap::Handle_t connectionHandle,
                                       uint8_t event_data_length,
                                       uint8_t handle_value_pair_length,
                                       uint8_t *handle_value_pair) {

  BlueNRG1GattConnectionClient *gattConnectionClient = getGattConnectionClient(connectionHandle);

  assert(gattConnectionClient != NULL);

  gattConnectionClient->serviceCharsCB(event_data_length,
                                       handle_value_pair_length,
                                       handle_value_pair);
}
    
void BlueNRG1GattClient::serviceCharByUUIDCB(Gap::Handle_t connectionHandle,
                                            uint8_t event_data_length,
                                            uint16_t attr_handle,
                                            uint8_t *attr_value) {

  BlueNRG1GattConnectionClient *gattConnectionClient = getGattConnectionClient(connectionHandle);

  assert(gattConnectionClient != NULL);

  gattConnectionClient->serviceCharByUUIDCB(event_data_length,
                                            attr_handle,
                                            attr_value);
}

void BlueNRG1GattClient::discAllCharacDescCB(Gap::Handle_t connHandle,
                                            uint8_t event_data_length,
                                            uint8_t format,
                                            uint8_t *handle_uuid_pair) {

  BlueNRG1GattConnectionClient *gattConnectionClient = getGattConnectionClient(connHandle);

  assert(gattConnectionClient != NULL);

  gattConnectionClient->discAllCharacDescCB(event_data_length,
                                            format,
                                            handle_uuid_pair);
}

void BlueNRG1GattClient::charReadCB(Gap::Handle_t connHandle,
                                   uint8_t event_data_length,
                                   uint8_t* attribute_value) {

  BlueNRG1GattConnectionClient *gattConnectionClient = getGattConnectionClient(connHandle);

  assert(gattConnectionClient != NULL);

  gattConnectionClient->charReadCB(event_data_length,
                                   attribute_value);
}

void BlueNRG1GattClient::charWritePrepareCB(Gap::Handle_t connHandle,
                                           uint8_t event_data_length,
                                           uint16_t attribute_handle,
                                           uint16_t offset,
                                           uint8_t *part_attr_value) {

  BlueNRG1GattConnectionClient *gattConnectionClient = getGattConnectionClient(connHandle);

  assert(gattConnectionClient != NULL);

  gattConnectionClient->charWritePrepareCB(event_data_length,
                                           attribute_handle,
                                           offset,
                                           part_attr_value);
}
    
void BlueNRG1GattClient::charWriteExecCB(Gap::Handle_t connHandle) {

  BlueNRG1GattConnectionClient *gattConnectionClient = getGattConnectionClient(connHandle);

  assert(gattConnectionClient != NULL);

  gattConnectionClient->charWriteExecCB();
}

ble_error_t BlueNRG1GattClient::launchServiceDiscovery(Gap::Handle_t                               connectionHandle,
                                                      ServiceDiscovery::ServiceCallback_t         sc,
                                                      ServiceDiscovery::CharacteristicCallback_t  cc,
                                                      const UUID                                 &matchingServiceUUID,
                                                      const UUID                                 &matchingCharacteristicUUIDIn)
{
  PRINTF("BlueNRG1GattClient launchServiceDiscovery\n\r");

  BlueNRG1GattConnectionClient *gattConnectionClient = getGattConnectionClient(connectionHandle);

  if(gattConnectionClient != NULL) {
    return gattConnectionClient->launchServiceDiscovery(sc, cc, matchingServiceUUID, matchingCharacteristicUUIDIn);
  } else {
    return BLE_ERROR_INTERNAL_STACK_FAILURE;
  }
}

ble_error_t BlueNRG1GattClient::discoverServices(Gap::Handle_t                        connectionHandle,
                                                ServiceDiscovery::ServiceCallback_t  callback,
                                                const UUID                          &matchingServiceUUID)
{
  BlueNRG1GattConnectionClient *gattConnectionClient = getGattConnectionClient(connectionHandle);

  if(gattConnectionClient != NULL) {

    return gattConnectionClient->discoverServices(callback, matchingServiceUUID);

  } else {
    return BLE_ERROR_INTERNAL_STACK_FAILURE;
  }
}

ble_error_t BlueNRG1GattClient::discoverServices(Gap::Handle_t                        connectionHandle,
                                                ServiceDiscovery::ServiceCallback_t  callback,
                                                GattAttribute::Handle_t              startHandle,
                                                GattAttribute::Handle_t              endHandle)
{
  BlueNRG1GattConnectionClient *gattConnectionClient = getGattConnectionClient(connectionHandle);

  if(gattConnectionClient != NULL) {

    return gattConnectionClient->discoverServices(callback, startHandle, endHandle);

  } else {
    return BLE_ERROR_INTERNAL_STACK_FAILURE;
  }
}

bool BlueNRG1GattClient::isServiceDiscoveryActive(void) const
{
  bool isSDActive = false;

  for (uint8_t i = 0; i < MAX_ACTIVE_CONNECTIONS; i++) {
    if (_connectionPool[i]) { 
      isSDActive |= _connectionPool[i]->isServiceDiscoveryActive();
    }
  }

  return isSDActive;
}

void BlueNRG1GattClient::terminateServiceDiscovery(void)
{
  for (uint8_t i = 0; i < MAX_ACTIVE_CONNECTIONS; i++) {
    if (_connectionPool[i]) { 
      _connectionPool[i]->terminateServiceDiscovery();
    }
  }
}

void BlueNRG1GattClient::onServiceDiscoveryTermination(ServiceDiscovery::TerminationCallback_t callback) {
  terminationCallback = callback;
  for (uint8_t i = 0; i < MAX_ACTIVE_CONNECTIONS; ++i) {
    if (_connectionPool[i]) { 
      _connectionPool[i]->onServiceDiscoveryTermination(callback);
    }
  }
}

ble_error_t BlueNRG1GattClient::read(Gap::Handle_t connHandle, GattAttribute::Handle_t attributeHandle, uint16_t offset) const
{
  BlueNRG1GattConnectionClient *gattConnectionClient = const_cast<BlueNRG1GattClient*>(this)->getGattConnectionClient(connHandle);

  if(gattConnectionClient != NULL) {

    return gattConnectionClient->read(attributeHandle, offset);

  } else {
    return BLE_ERROR_INTERNAL_STACK_FAILURE;
  }
}

ble_error_t BlueNRG1GattClient::write(GattClient::WriteOp_t    cmd,
                                     Gap::Handle_t            connHandle,
                                     GattAttribute::Handle_t  attributeHandle,
                                     size_t                   length,
                                     const uint8_t           *value) const
{
  BlueNRG1GattConnectionClient *gattConnectionClient = const_cast<BlueNRG1GattClient*>(this)->getGattConnectionClient(connHandle);

  if(gattConnectionClient != NULL) {

    return gattConnectionClient->write(cmd, attributeHandle, length, value);

  } else {
    return BLE_ERROR_INTERNAL_STACK_FAILURE;
  }
}

ble_error_t BlueNRG1GattClient::discoverCharacteristicDescriptors(
        const DiscoveredCharacteristic& characteristic,
        const CharacteristicDescriptorDiscovery::DiscoveryCallback_t& discoveryCallback,
        const CharacteristicDescriptorDiscovery::TerminationCallback_t& terminationCallback)
{
  BlueNRG1GattConnectionClient *gattConnectionClient = getGattConnectionClient(characteristic.getConnectionHandle());

  if(gattConnectionClient != NULL) {

    return gattConnectionClient->discoverCharacteristicDescriptors(characteristic, discoveryCallback, terminationCallback);

  } else {
    return BLE_ERROR_INTERNAL_STACK_FAILURE;
  }
}

/**************************************************************************/
/*!
    @brief  Clear BlueNRG1GattClient's state.

    @returns    ble_error_t

    @retval     BLE_ERROR_NONE
                Everything executed properly
*/
/**************************************************************************/
/*
ble_error_t BlueNRG1GattClient::reset(void)
{
  PRINTF("BlueNRG1GattClient::reset\n");

  for (uint8_t i = 0; i < MAX_ACTIVE_CONNECTIONS; i++) {
    if(_connectionPool[i] != NULL) {
      _connectionPool[i]->reset();

      delete _connectionPool[i];
      _connectionPool[i] = NULL;

      _numConnections--;
    }
  }

  return BLE_ERROR_NONE;
}
*/