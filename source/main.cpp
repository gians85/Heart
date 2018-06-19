#include "mbed.h"
#include "ble/BLE.h"
#include "ble/services/HealthThermometerService.h" 
#include "LSM6DS3/IMU.h"
#include "LSM6DS3/BufferService.h"


/* Pheripherals Objects */
DigitalOut led1(LED1, 0);
DigitalIn  button(PUSH1);
Serial     pc(USBTX, USBRX);
SPI        spi(SPI_MOSI, SPI_MISO, SPI_SCK, SPI_CS); // mosi, miso, sclk
IMU        imu(&pc, &spi);


/* BLE Services */
static HealthThermometerService*   thermometerService;
BufferService*                uartService;

/* Device Proprieties */
const static char       DEVICE_NAME[] = "ST_BlueNRG1";
static const uint16_t   uuid16_list[] = {GattService::UUID_HEALTH_THERMOMETER_SERVICE,//GattService::UUID_ENVIRONMENTAL_SERVICE,
                                         GattService::UUID_HUMAN_INTERFACE_DEVICE_SERVICE};

/* Initial State Proprieties */
static float     currentTemperature = 20;
uint8_t  size = 24;
uint8_t  buffer[24] = {};

/* Disconnection Method */
void disconnectionCallback(const Gap::DisconnectionCallbackParams_t *params)
{
    BLE::Instance(BLE::DEFAULT_INSTANCE).gap().startAdvertising(); // restart advertising
}

/* BLE Setup */
void bleInitComplete(BLE::InitializationCompleteCallbackContext *params)
{
    BLE &ble          = params->ble;
    ble_error_t error = params->error;
    
    /* Check BLE errors */
    if (error != BLE_ERROR_NONE) {
        return;
    }
    
    /* Define diconnection action */
    ble.gap().onDisconnection(disconnectionCallback);
    
    /* Setup primary service. */
    thermometerService = new HealthThermometerService(ble, currentTemperature, HealthThermometerService::LOCATION_BODY);    
    
    /* Setup primary service. */  
    uartService = new BufferService(ble, buffer, size);   
    
    /* Setup advertising. */
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::BREDR_NOT_SUPPORTED | GapAdvertisingData::LE_GENERAL_DISCOVERABLE);
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LIST_16BIT_SERVICE_IDS, (uint8_t *)uuid16_list, sizeof(uuid16_list));
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::JOYSTICK);
    ble.gap().accumulateAdvertisingPayload(GapAdvertisingData::COMPLETE_LOCAL_NAME, (uint8_t *)DEVICE_NAME, sizeof(DEVICE_NAME));
    ble.gap().setAdvertisingType(GapAdvertisingParams::ADV_CONNECTABLE_UNDIRECTED);
    ble.gap().setAdvertisingInterval(1000); /* 1000ms */
    ble.gap().startAdvertising();
}


/* MAIN */
int main(void)
{  
    /* Local Variables */
    int16_t raw_acc[3];
    char buff[24];
    float a = 0.061;
        
    /* BLE Init */
    BLE& ble = BLE::Instance(BLE::DEFAULT_INSTANCE);
    ble.init(bleInitComplete);
    
    /* IMU Config */
    imu.configIMU();

    /* SpinWait for initialization to complete. This is necessary because the
     * BLE object is used in the main loop below. */
    while (ble.hasInitialized()  == false) { /* spin loop */ }
    
    
    /* Main Loop */
    while (1)
    {
        /* getting temperature */
        currentTemperature = imu.readTempIMU();
        
        /* getting accelerations */
        imu.readAccIMU(raw_acc);
        sprintf(buff, "%.0f %.0f %.0f    ", raw_acc[0]*a, raw_acc[1]*a, raw_acc[2]*a);

        /* sendig temperature if device is connected*/
        if ( ble.getGapState().connected ) {
            thermometerService->updateTemperature(currentTemperature);
        } 
        ble.waitForEvent();
        
        /* sending accelerations if device is connected*/
        if ( ble.getGapState().connected ) {
            uartService->sendBuffer((uint8_t*)buff, size);
        } 
        ble.waitForEvent();
        
        /* blinkning activity led */
        if ( ble.getGapState().connected ) {
            led1 = !led1;
        }
        else
            led1 = 0;
        ble.waitForEvent();
        
        /* delay */
        wait_ms(100);
    }
}


