#include "BlueNRG1Device.h"
#include "btle.h"
#include "ble_debug.h"


BlueNRG1Device bluenrg1DeviceInstance;

/**
* BLE-API requires an implementation of the following function in order to
* obtain its transport handle.
*/
BLEInstanceBase *
createBLEInstance(void)
{
    return (&bluenrg1DeviceInstance);
}

BlueNRG1Device::BlueNRG1Device(){}

BlueNRG1Device::~BlueNRG1Device(){}

ble_error_t BlueNRG1Device::init(BLE::InstanceID_t instanceID, FunctionPointerWithContext<BLE::InitializationCompleteCallbackContext *> callback)
{
    if (isInitialized) {
        BLE::InitializationCompleteCallbackContext context = {
            BLE::Instance(instanceID),
            BLE_ERROR_ALREADY_INITIALIZED
        };
        callback.call(&context);
        return BLE_ERROR_ALREADY_INITIALIZED;
    }
    
    // Init the BlueNRG1 stack
	btleInit();

    isInitialized = true;
    BLE::InitializationCompleteCallbackContext context = {
        BLE::Instance(instanceID),
        BLE_ERROR_NONE
    };
    callback.call(&context);
  
    return BLE_ERROR_NONE;
}


void BlueNRG1Device::processEvents() {   
    btle_handler();   //ST
    //PRINTF("!!! Have to implement processEvents()\n\r");
}


/*!
  @brief  Wait for any BLE Event like BLE Connection, Read Request etc.
  @param[in] void
  @returns    char *
*/
void BlueNRG1Device::waitForEvent(void)
{
	bool must_return = false;

	do {
		bluenrg1DeviceInstance.processEvents();

		if(must_return) return;

		__WFE(); /* it is recommended that SEVONPEND in the
			    System Control Register is NOT set */
		must_return = true; /* after returning from WFE we must guarantee
				       that conrol is given back to main loop before next WFE */
	} while(true);

}


/**************************************************************************/
/*!
    @brief  get reference to GAP object
    @param[in] void
    @returns    Gap&
*/
/**************************************************************************/
Gap        &BlueNRG1Device::getGap()
{
    return BlueNRG1Gap::getInstance();
}

const Gap  &BlueNRG1Device::getGap() const
{
    return BlueNRG1Gap::getInstance();
}


/**************************************************************************/
/*!
    @brief  get reference to GATT server object
    @param[in] void
    @returns    GattServer&
*/
/**************************************************************************/
GattServer &BlueNRG1Device::getGattServer()
{
    return BlueNRG1GattServer::getInstance();
}

const GattServer &BlueNRG1Device::getGattServer() const
{
    return BlueNRG1GattServer::getInstance();
}


