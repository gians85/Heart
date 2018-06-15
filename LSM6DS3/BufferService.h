/*****************************************************************************
  BLE service that send buffer. Based on Alexander Lea's work
  https://os.mbed.com/questions/6643/GATT-Server-sending-non-numeric-data-as-/                                                                        *
 *****************************************************************************/

#include "ble/BLE.h"


class BufferService
{
public:

    
    const static uint16_t BROADCAST_SERVICE_UUID            = 0x2A67;
    const static uint16_t BROADCAST_CHARACTERISTIC_UUID     = 0x1817;
    
    const static uint16_t LUID_BUFFER_SERVICE   =   GattService::UUID_HUMAN_INTERFACE_DEVICE_SERVICE;
    const static uint16_t LUID_BUFFER_CHARACT   =   0x1817;
    
    
    //StringService(BLEDevice &_ble, uint8_t _command, uint8_t size) :
    BufferService(BLEDevice &_ble, uint8_t *_buffer, uint8_t size) :
        ble(_ble),
        buffer(_buffer),
        

        //modificato
        //stringerCharacteristic(LUID_STRING_CHARACT, &command, size/*sizeof(command)*/, size/*sizeof(command)*/,
        bufferCharacteristic(LUID_BUFFER_CHARACT, buffer, size, size, //sizeof(buff), sizeof(buff),
                                  GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_READ |
                                  // GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE |
                                  GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY) {
       
        static bool serviceAdded = false;
        if (serviceAdded) {
            return;
        }
        
        //GattCharacteristic *charTable[] = {&broadcasterCharacteristic};
        GattCharacteristic *charTable[] = {&bufferCharacteristic};
        //GattService         broadcasterService(BroadcasterService::BROADCAST_SERVICE_UUID, charTable, sizeof(charTable) / sizeof(GattCharacteristic *));
        GattService         bufferService(BufferService::LUID_BUFFER_SERVICE, charTable, sizeof(charTable) / sizeof(GattCharacteristic *));
 
        //ble.addService(broadcasterService);
        ble.addService(bufferService);
        serviceAdded = true;
    }
 

    void sendBuffer (uint8_t *_newbuffer, uint8_t  size) {
    //void sendString(uint8_t *string) {
        //ble.updateCharacteristicValue(broadcasterCharacteristic.getValueAttribute().getHandle(), value, size);
        //uint8_t string[] = {'A','B','c','1','f'};
        buffer = _newbuffer;
        if (ble.getGapState().connected) {        
            ble.updateCharacteristicValue(bufferCharacteristic.getValueAttribute().getHandle(), buffer, size);
        }
    }
    /*void sendString(uint8_t *buff) {
        ble.updateCharacteristicValue(stringerCharacteristic.getValueAttribute().getHandle(), buff, sizeof(buff));
    }*/
 
private:
    BLEDevice           &ble;
    uint8_t             *buffer;    
    GattCharacteristic  bufferCharacteristic;
};