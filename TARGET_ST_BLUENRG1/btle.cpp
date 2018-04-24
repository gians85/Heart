#include "btle.h"

extern "C" {
#include <stdio.h>
//#include <stdint.h>
#include "BlueNRG1BLEStack.h"
#include "ble_debug.h"
}
    
#include "BlueNRG1Gap.h"

uint8_t ret;
uint8_t * ptr;
unsigned int * ptr2;


/* FLASH reserved to store all the security database information and
 * and the server database information */
ALIGN(4)
SECTION(".noinit.stacklib_flash_data")
NOLOAD(const uint32_t stacklib_flash_data[TOTAL_FLASH_BUFFER_SIZE(FLASH_SEC_DB_SIZE, FLASH_SERVER_DB_SIZE)>>2]);

/* FLASH reserved to store: security root keys, static random address, 
 * public address */
ALIGN(4)
SECTION(".noinit.stacklib_stored_device_id_data")
NOLOAD(const uint8_t stacklib_stored_device_id_data[56]); //As recommended by Program Guide

/* RAM reserved to manage all the data stack according the number of links,
 * number of services, number of attributes and attribute value length
 */
NO_INIT(uint32_t dyn_alloc_a[TOTAL_BUFFER_SIZE(NUM_LINKS,NUM_GATT_ATTRIBUTES,NUM_GATT_SERVICES,ATT_VALUE_ARRAY_SIZE,PCKT_COUNT)>>2]);


/* This structure contains memory and low level hardware configuration data for the device */
const BlueNRG_Stack_Initialization_t BlueNRG_Stack_Init_params = {
    (uint8_t*)stacklib_flash_data,
    FLASH_SEC_DB_SIZE,
    FLASH_SERVER_DB_SIZE,
    (uint8_t*)stacklib_stored_device_id_data,
    (uint8_t*)dyn_alloc_a,
    TOTAL_BUFFER_SIZE(NUM_LINKS,NUM_GATT_ATTRIBUTES,NUM_GATT_SERVICES,ATT_VALUE_ARRAY_SIZE,PCKT_COUNT),
    NUM_GATT_ATTRIBUTES,
    NUM_GATT_SERVICES,
    ATT_VALUE_ARRAY_SIZE,
    NUM_LINKS,
    0,
    PREPARE_WRITE_LIST_SIZE, /* [New parameter added on BLE stack v2.x] */
    PCKT_COUNT,              /* [New parameter added on BLE stack v2.x] */
    MAX_ATT_MTU,             /* [New parameter added on BLE stack v2.x] */
    CONFIG_TABLE,
};



void BlueNRG1_stackInit(){
    ret = BlueNRG_Stack_Initialization(&BlueNRG_Stack_Init_params);
    if (ret != BLE_STATUS_SUCCESS) {
        PRINTF("Error in BlueNRG_Stack_Initialization() 0x%02x\r\n", ret);
        while(1);
    }
}


uint8_t BlueNRG1_deviceInit(){
    PRINTF("Sensor_DeviceInit()\r\n");
    uint8_t bdaddr[] = {0x12, 0x34, 0x00, 0xE1, 0x80, 0x02};
    uint16_t service_handle, dev_name_char_handle, appearance_char_handle;
    uint8_t device_name[] = {'B', 'l', 'u', 'e', 'N', 'R', 'G'};

    /* Set the device public address */
    ret = aci_hal_write_config_data(CONFIG_DATA_PUBADDR_OFFSET, CONFIG_DATA_PUBADDR_LEN,
                                  bdaddr);
    if(ret != BLE_STATUS_SUCCESS) {
        PRINTF("aci_hal_write_config_data() failed: 0x%02x\r\n", ret);
        return ret;
    }

    /* Set the TX power -2 dBm */
    aci_hal_set_tx_power_level(1, 4);


    /* GATT Init */
    ret = aci_gatt_init();
    if (ret != BLE_STATUS_SUCCESS) {
        PRINTF("aci_gatt_init() failed: 0x%02x\r\n", ret);
        return ret;
    }

    /* GAP Init */
    ret = aci_gap_init(GAP_PERIPHERAL_ROLE, PRIVACY_DISABLED, sizeof(device_name), &service_handle, &dev_name_char_handle, &appearance_char_handle);
    if (ret != BLE_STATUS_SUCCESS) {
        PRINTF("aci_gap_init() failed: 0x%02x\r\n", ret);
        return ret;
    }

    /* Update device name */
    ret = aci_gatt_update_char_value(service_handle, dev_name_char_handle, 0, sizeof(device_name), device_name);
    if(ret != BLE_STATUS_SUCCESS) {
        PRINTF("aci_gatt_update_char_value() failed: 0x%02x\r\n", ret);
        return ret;
    }

    /* BLE Security v4.2 is supported: BLE stack FW version >= 2.x (new API prototype) */
    ret = aci_gap_set_authentication_requirement(BONDING,
                                               MITM_PROTECTION_REQUIRED,
                                               SC_IS_SUPPORTED,
                                               KEYPRESS_IS_NOT_SUPPORTED,
                                               7,
                                               16,
                                               USE_FIXED_PIN_FOR_PAIRING,
                                               123456,
                                               0x00);
    if(ret != BLE_STATUS_SUCCESS) {
        PRINTF("aci_gap_set_authentication_requirement()failed: 0x%02x\r\n", ret);
        return ret;
    }

    PRINTF("BLE Stack Initialized with SUCCESS\r\n");

    /* Add services and Characteristics */


    return BLE_STATUS_SUCCESS;
}



/*void btle_handler(void)
{
    btle_handler_pending = 0;
    BlueNRG1Gap::getInstance().Process();
    //HCI_HandleSPI();
    //HCI_Process();
}*/



/**************************************************************************
    @brief  mbedOS
    @param[in]  void
    @returns
 **************************************************************************/
int btle_handler_pending = 0;



/******************************************************************************/
/*                 BlueNRG-1 Peripherals Interrupt Handlers                   */
/******************************************************************************/
extern "C" {
    void Blue_Handler(void){
        // Call RAL_Isr
        RAL_Isr();
    }
}



void hci_le_connection_complete_event(uint8_t Status,
                                      uint16_t Connection_Handle,
                                      uint8_t Role,
                                      uint8_t Peer_Address_Type,
                                      uint8_t Peer_Address[6],
                                      uint16_t Conn_Interval,
                                      uint16_t Conn_Latency,
                                      uint16_t Supervision_Timeout,
                                      uint8_t Master_Clock_Accuracy)
{
    PRINTF("hci_le_connection_complete_event\n");
}
