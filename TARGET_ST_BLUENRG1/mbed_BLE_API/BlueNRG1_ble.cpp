/* Includes ------------------------------------------------------------------*/
#include "BlueNRG1Gap.h"
#include "BlueNRG1Device.h"
#include "btle.h"

// FIXME: find a better way to get the instance of the BlueNRG device
extern BlueNRG1Device bluenrg1DeviceInstance;


////////////////////////////////////////
// Start of C function wrappers
#ifdef __cplusplus
extern "C" {
#endif

#include "BlueNRG1_ble.h"
#include "ble_debug.h"

void signalEventsToProcess(void)  {
    if(btle_handler_pending == 0) {
        btle_handler_pending = 1;
        bluenrg1DeviceInstance.signalEventsToProcess(BLE::DEFAULT_INSTANCE);
        //BlueNRG1Device::Instance(BLE::DEFAULT_INSTANCE);
    }
}


#ifdef __cplusplus
}
#endif
// End of C function wrappers
////////////////////////////////////////
 