class BroadcasterService
{
public:
    /**
    * @param[ref] _ble
    *
    * @param[in]
    */
    
    const static uint16_t BROADCAST_SERVICE_UUID            = 0x2A67;
    const static uint16_t BROADCAST_CHARACTERISTIC_UUID     = 0x1817;
    
    BroadcasterService(BLEDevice &_ble, uint8_t _command) :
        ble(_ble),
        command(_command),
        
        broadcasterCharacteristic(BROADCAST_CHARACTERISTIC_UUID, &command, sizeof(command), sizeof(command),
                                  GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ |
                                  // GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE |
                                  GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY) {
 
       
        static bool serviceAdded = false; /* We should only ever need to add the service once. */
        if (serviceAdded) {
            return;
        }
 
        GattCharacteristic *charTable[] = {&broadcasterCharacteristic};
        GattService         broadcasterService(BroadcasterService::BROADCAST_SERVICE_UUID, charTable, sizeof(charTable) / sizeof(GattCharacteristic *));
 
        ble.addService(broadcasterService);
        serviceAdded = true;
    }
 
    /**
     * @brief
     *
     * @param
     */
    void sendCommand(uint8_t *value, uint16_t  size) {
        ble.updateCharacteristicValue(broadcasterCharacteristic.getValueAttribute().getHandle(), value, size);
    }
 
private:
    BLEDevice           &ble;
    uint8_t             command;    
    GattCharacteristic  broadcasterCharacteristic;
};