#ifndef __BLUENRG1_GATTSERVER_H__
#define __BLUENRG1_GATTSERVER_H__

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

class BlueNRG1_GattServer : public GattServer
{
public:
    static BlueNRG1_GattServer &getInstance() {
        static BlueNRG1_GattServer m_instance;
        return m_instance;
    }
    
};

#endif //__BLUENRG1_GATTSERVER_H__