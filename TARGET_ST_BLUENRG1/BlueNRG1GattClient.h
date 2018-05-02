#ifndef __BLUENRG1GATTCLIENT_H__
#define __BLUENRG1GATTCLIENT_H__

#ifdef YOTTA_CFG_MBED_OS
    #include "mbed-drivers/mbed.h"
#else
    #include "mbed.h"
#endif 
#include "ble/blecommon.h"
//#include "btle.h"
#include "ble/GattClient.h"
#include "ble/DiscoveredService.h"
#include "ble/CharacteristicDescriptorDiscovery.h"
//#include "BlueNRGDiscoveredCharacteristic.h"
//#include "BlueNRGGattConnectionClient.h"

using namespace std;

#define MAX_ACTIVE_CONNECTIONS 7

class BlueNRG1GattClient : public GattClient
{
public:
    static BlueNRG1GattClient &getInstance() {
        static BlueNRG1GattClient m_instance;
        return m_instance;
    }

};

#endif  //__BLUENRG1GATTCLIENT_H__