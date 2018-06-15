/*****************************************************************************
  BLE service that send strings. Based on Alexander Lea's work
  https://os.mbed.com/questions/6643/GATT-Server-sending-non-numeric-data-as-/                                                                        *
 *****************************************************************************/

#include "ble/BLE.h"


class StringService
{
public:

    
    const static uint16_t BROADCAST_SERVICE_UUID            = 0x2A67;
    const static uint16_t BROADCAST_CHARACTERISTIC_UUID     = 0x1817;
    
    const static uint16_t LUID_STRING_SERVICE   =   GattService::UUID_HUMAN_INTERFACE_DEVICE_SERVICE;
    const static uint16_t LUID_STRING_CHARACT   =   0x1817;
    
    
    //StringService(BLEDevice &_ble, uint8_t _command, uint8_t size) :
    StringService(BLEDevice &_ble, uint8_t *_buffer, uint8_t size) :
        ble(_ble),
        buffer(_buffer),
        

        //modificato
        //stringerCharacteristic(LUID_STRING_CHARACT, &command, size/*sizeof(command)*/, size/*sizeof(command)*/,
        stringerCharacteristic(LUID_STRING_CHARACT, buffer, size, size, //sizeof(buff), sizeof(buff),
                                  GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ |
                                  // GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE |
                                  GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY) {
       
        static bool serviceAdded = false;
        if (serviceAdded) {
            return;
        }
        
        //GattCharacteristic *charTable[] = {&broadcasterCharacteristic};
        GattCharacteristic *charTable[] = {&stringerCharacteristic};
        //GattService         broadcasterService(BroadcasterService::BROADCAST_SERVICE_UUID, charTable, sizeof(charTable) / sizeof(GattCharacteristic *));
        GattService         stringerService(StringService::LUID_STRING_SERVICE, charTable, sizeof(charTable) / sizeof(GattCharacteristic *));
 
        //ble.addService(broadcasterService);
        ble.addService(stringerService);
        serviceAdded = true;
    }
 

    void sendString (uint8_t *_newbuffer, uint8_t  size) {
    //void sendString(uint8_t *string) {
        //ble.updateCharacteristicValue(broadcasterCharacteristic.getValueAttribute().getHandle(), value, size);
        //uint8_t string[] = {'A','B','c','1','f'};
        buffer = _newbuffer;
        ble.updateCharacteristicValue(stringerCharacteristic.getValueAttribute().getHandle(), buffer, size);
    }
    /*void sendString(uint8_t *buff) {
        ble.updateCharacteristicValue(stringerCharacteristic.getValueAttribute().getHandle(), buff, sizeof(buff));
    }*/
 
private:
    BLEDevice           &ble;
    uint8_t             *buffer;    
    GattCharacteristic  stringerCharacteristic;
};