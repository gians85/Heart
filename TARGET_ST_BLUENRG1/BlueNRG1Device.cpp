/* #include "BlueNRG1Device.h"
#include "btle.h"
#include "ble_debug.h"  */
#include "BlueNRG1Device.h"
#include "BlueNRG1Gap.h"
#include "BlueNRG1GattServer.h"

#include "btle.h"
#include "ble_utils.h"

#include "ble_debug.h"
#include "Bluenrg1_ble.h"

extern "C" {
#include "BlueNRG1BLEStack.h"
}

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

/**************************************************************************
  Constructor
**************************************************************************/
BlueNRG1Device::BlueNRG1Device():isInitialized(false){}


/**************************************************************************
  Destructor
**************************************************************************/
BlueNRG1Device::~BlueNRG1Device(){}


/**************************************************************************
   Initialize
**************************************************************************/
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


/*!
    @brief  get GAP version
    @brief Get the BLE stack version information
    @param[in] void
    @returns    char *
    @returns char *
*/
const char *BlueNRG1Device::getVersion(void)
{
    return getVersionString();
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


/**************************************************************************/
/*!
    @brief  shut down the BLE device
    @param[out] error if any
*/
/**************************************************************************/
ble_error_t  BlueNRG1Device::shutdown(void) {
    PRINTF("BlueNRG1Device::reset\n");

    if (!isInitialized) {
        return BLE_ERROR_INITIALIZATION_INCOMPLETE;
    }

    /* Reset the BlueNRG device first */
    //reset();

    /* Shutdown the BLE API and BlueNRG glue code */
    ble_error_t error;

    /* GattServer instance */
    error = BlueNRG1GattServer::getInstance().reset();
    if (error != BLE_ERROR_NONE) {
       return error;
    }

    /* GattClient instance */
    error = BlueNRG1GattClient::getInstance().reset();
    if (error != BLE_ERROR_NONE) {
        return error;
    }

    /* Gap instance */
    error = BlueNRG1Gap::getInstance().reset();
    if (error != BLE_ERROR_NONE) {
        return error;
    }

    isInitialized = false;

    PRINTF("BlueNRG1Device::reset complete\n");
    return BLE_ERROR_NONE;

}


/**************************************************************************
   process events
**************************************************************************/
void BlueNRG1Device::processEvents() {
    btle_handler();
}