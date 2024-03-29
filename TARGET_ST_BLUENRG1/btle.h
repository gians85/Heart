#ifndef __BTLE_H__
#define __BTLE_H__

//#include <stdint.h>
//#include "compiler.h"


#ifndef _SENSORDEMO_CONFIG_H_
#define _SENSORDEMO_CONFIG_H_

//#include "compiler.h"

/* This file contains all the information needed to init the BlueNRG-1 stack.
 * These constants and variables are used from the BlueNRG-1 stack to reserve RAM and FLASH
 * according the application requests
 */


/* Default number of link */
#define MIN_NUM_LINK            1
/* Default number of GAP and GATT services */
#define DEFAULT_NUM_GATT_SERVICES   2
/* Default number of GAP and GATT attributes */
#define DEFAULT_NUM_GATT_ATTRIBUTES 9

#if defined (ST_OTA_LOWER_APPLICATION) || defined (ST_OTA_HIGHER_APPLICATION)
/* Number of services requests from the Sensor demo */
#define NUM_APP_GATT_SERVICES (2 + 1) /* 2 Sensor services + 1 OTA service */

/* Number of attributes requests from the chat demo */
#define NUM_APP_GATT_ATTRIBUTES (15 + 9) /* 15 attributes x BLE Sensor demo services characteristics + 9 for OTA Service characteristics */

#define SENSOR_MAX_ATT_SIZE (20)

#else /* NO OTA Service is required */

/* Number of services requests from the sensor demo */
#define NUM_APP_GATT_SERVICES 2

/* Number of attributes requests from the sensor demo */
#define NUM_APP_GATT_ATTRIBUTES 15

#define SENSOR_MAX_ATT_SIZE (6)
#endif

/* Number of links needed for the demo: 1
 * Only 1 the default
 */
#define NUM_LINKS               (MIN_NUM_LINK)

/* Maximum number of attribute records that can be added to the first application service: acceleration service */
#define MAX_NUMBER_ATTRIBUTES_RECORDS_SERVICE_1 (7)

/* Maximum number of attribute records that can be added to the second application service: environmental service */
#define MAX_NUMBER_ATTRIBUTES_RECORDS_SERVICE_2 (10)

/* Number of GATT attributes needed for the sensor demo. */
#define NUM_GATT_ATTRIBUTES     (DEFAULT_NUM_GATT_ATTRIBUTES + NUM_APP_GATT_ATTRIBUTES)

/* Number of GATT services needed for the sensor demo. */
#define NUM_GATT_SERVICES       (DEFAULT_NUM_GATT_SERVICES + NUM_APP_GATT_SERVICES)

/* Array size for the attribute value for OTA service */
#if defined (ST_OTA_LOWER_APPLICATION) || defined (ST_OTA_HIGHER_APPLICATION)
#define OTA_ATT_VALUE_ARRAY_SIZE (119)    /* OTA service is used: 4 characteristics (1 notify property) */
#else
#define OTA_ATT_VALUE_ARRAY_SIZE (0)       /* No OTA service is used */
#endif

/* Array size for the attribute value */
#ifndef SENSOR_EMULATION
#define ATT_VALUE_ARRAY_SIZE    (43 + 106  + OTA_ATT_VALUE_ARRAY_SIZE) //(GATT + GAP) = 43 (Device Name: BlueNRG) + Acceleration (Acceleration (27) + Free Fall (21) characteristics) +  Environmental Sensor (Temperature (28), Pressure (29)  characteristics)  Services
#else
#define ATT_VALUE_ARRAY_SIZE    (43 + 106 + 28 + OTA_ATT_VALUE_ARRAY_SIZE) //(GATT + GAP) = 43 (Device Name: BlueNRG) + Acceleration (Acceleration (27) + Free Fall (21) characteristics) +  Environmental Sensor (Temperature (28) , Pressure (29), Humidity (28) characteristics)  Services
#endif

/* Flash security database size */
#define FLASH_SEC_DB_SIZE       (0x400)

/* Flash server database size */
#define FLASH_SERVER_DB_SIZE    (0x400)

/* Set supported max value for ATT_MTU enabled by the application. Allowed values in range: [23:158] [New parameter added on BLE stack v2.x] */
#define MAX_ATT_MTU             (DEFAULT_ATT_MTU)

/* Set supported max value for attribute size: it is the biggest attribute size enabled by the application */
#define MAX_ATT_SIZE            (SENSOR_MAX_ATT_SIZE)

/* Set the minumum number of prepare write requests needed for a long write procedure for a characteristic with len > 20bytes:
 *
 * It returns 0 for characteristics with len <= 20bytes
 *
 * NOTE: If prepare write requests are used for a characteristic (reliable write on multiple characteristics), then
 * this value should be set to the number of prepare write needed by the application.
 *
 *  [New parameter added on BLE stack v2.x]
*/
#define PREPARE_WRITE_LIST_SIZE PREP_WRITE_X_ATT(MAX_ATT_SIZE)

/* Additional number of memory blocks  to be added to the minimum */
#define OPT_MBLOCKS		(6) /* 6:  for reaching the max throughput: ~220kbps (same as BLE stack 1.x) */

/* Set the minimum number of memory blocks for packet allocation [New parameter added on BLE stack v2.x] */
#define MIN_PCKT_COUNT          PCKT_MBLOCKS_C(PREPARE_WRITE_LIST_SIZE, MAX_ATT_MTU, NUM_LINKS)

/* Set the total number of memory blocks for packet allocation [New parameter added on BLE stack v2.x] */
#define PCKT_COUNT             (MIN_PCKT_COUNT + OPT_MBLOCKS)

/* RAM reserved to manage all the data stack according the number of links,
 * number of services, number of attributes and attribute value length
 */
NO_INIT(uint32_t dyn_alloc_a[TOTAL_BUFFER_SIZE(NUM_LINKS,NUM_GATT_ATTRIBUTES,NUM_GATT_SERVICES,ATT_VALUE_ARRAY_SIZE,PCKT_COUNT)>>2]);

/* FLASH reserved to store all the security database information and
 * and the server database information
 */
ALIGN(4)
SECTION(".noinit.stacklib_flash_data")
NOLOAD(const uint32_t stacklib_flash_data[TOTAL_FLASH_BUFFER_SIZE(FLASH_SEC_DB_SIZE, FLASH_SERVER_DB_SIZE)>>2]);
/* FLASH reserved to store: security root keys, static random address, public address */
ALIGN(4)
SECTION(".noinit.stacklib_stored_device_id_data")
NOLOAD(const uint8_t stacklib_stored_device_id_data[56]);


/* Maximum duration of the connection event */
#define MAX_CONN_EVENT_LENGTH 0xFFFFFFFF

/* Sleep clock accuracy */
#if (LS_SOURCE == LS_SOURCE_INTERNAL_RO)

/* Sleep clock accuracy in Slave mode */
#define SLAVE_SLEEP_CLOCK_ACCURACY 500

/* Sleep clock accuracy in Master mode */
#define MASTER_SLEEP_CLOCK_ACCURACY MASTER_SCA_500ppm

#else

/* Sleep clock accuracy in Slave mode */
#define SLAVE_SLEEP_CLOCK_ACCURACY 100

/* Sleep clock accuracy in Master mode */
#define MASTER_SLEEP_CLOCK_ACCURACY MASTER_SCA_100ppm

#endif

/* Low Speed Oscillator source */
#if (LS_SOURCE == LS_SOURCE_INTERNAL_RO)
#define LOW_SPEED_SOURCE  1 // Internal RO
#else
#define LOW_SPEED_SOURCE  0 // External 32 KHz
#endif

/* High Speed start up time */
#define HS_STARTUP_TIME 0x0107 // 642 us

/* Low level hardware configuration data for the device */
#define CONFIG_TABLE            \
{                               \
  NULL,          \
  MAX_CONN_EVENT_LENGTH,        \
  SLAVE_SLEEP_CLOCK_ACCURACY,   \
  MASTER_SLEEP_CLOCK_ACCURACY,  \
  LOW_SPEED_SOURCE,             \
  HS_STARTUP_TIME               \
}

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


#endif