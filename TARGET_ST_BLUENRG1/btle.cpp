#include "btle.h"

extern "C" {
#include <stdio.h>
//#include <stdint.h>
#include "BlueNRG1BLEStack.h"
#include "ble_debug.h"
}

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
