#include "btle.h"
#include "ble/Gap.h"
#include "ble/GapEvents.h"
#include "BlueNRG1Gap.h"
#include "BlueNRG1GattServer.h"
#include "BlueNRG1GattClient.h"
#include "ble_utils.h"

extern "C" {
#include <stdio.h>
#include "BlueNRG1BLEStack.h"
#include "ble_hal.h"
#include "ble_debug.h"   
}

#define BLE_LED_DEBUG

uint16_t g_gap_service_handle = 0;
uint16_t g_appearance_char_handle = 0;
uint16_t g_device_name_char_handle = 0;
uint16_t g_preferred_connection_parameters_char_handle = 0;

/* Private variables ---------------------------------------------------------*/
volatile uint8_t set_connectable = 1;
//static char versionString[32];




/**************************************************************************
   Construction of BlueNRG_Stack_Init_params
 **************************************************************************/
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



/**************************************************************************
    @brief  Init the BTLE stack with the specified role
    @returns void
**************************************************************************/
void btleInit(void)
{
    PRINTF("btleInit>>\r\n");

    uint8_t ret;
    /*uint8_t BTLE_Stack_version_major;
    uint8_t BTLE_Stack_version_minor;
    uint8_t BTLE_Stack_version_patch;*/
    uint16_t service_handle, dev_name_char_handle, appearance_char_handle;

    /* BlueNRG-1 stack init */
    //ret = BlueNRG_Stack_Initialization();
    ret = BlueNRG_Stack_Initialization(&BlueNRG_Stack_Init_params);
    if (ret != BLE_STATUS_SUCCESS) {
        PRINTF("Error in BlueNRG_Stack_Initialization() 0x%02x\r\n", ret);
    }

    /* get the BlueNRG HW and FW versions */
    /* ret = getBlueNRGStackVersion(&BTLE_Stack_version_major,
                                 &BTLE_Stack_version_minor,
                                 &BTLE_Stack_version_patch);
    if (ret != BLE_STATUS_SUCCESS) {
        PRINTF("Error in getBlueNRGStackVersion() 0x%02x\r\n", ret);
    } */

    /* set BLE version string */
    /*
    setVersionString(BTLE_Stack_version_major,
                     BTLE_Stack_version_minor,
                     BTLE_Stack_version_patch);
    */

    /* Set the TX power -2 dBm */
    aci_hal_set_tx_power_level(1, 4);
    if (ret != BLE_STATUS_SUCCESS) {
      PRINTF("Error in aci_hal_set_tx_power_level() 0x%04x\r\n", ret);
    }

    /* GATT Init */
    ret = aci_gatt_init();
    if(ret != BLE_STATUS_SUCCESS){
        PRINTF("aci_gatt_init() failed: 0x%02x\r\n", ret);
    }

    /* GAP Init */
    ret = aci_gap_init(GAP_PERIPHERAL_ROLE|GAP_CENTRAL_ROLE|GAP_OBSERVER_ROLE,
                       0, // disable privacy
                       0x18,
                       &service_handle,
                       &dev_name_char_handle,
                       &appearance_char_handle);

    // read the default static address and inject it into the GAP object
    {
        Gap::Address_t BLE_address_BE = { 0 };
        uint8_t data_len_out;
        aci_hal_read_config_data(CONFIG_DATA_RANDOM_ADDRESS, &data_len_out, BLE_address_BE);
        // FIXME error handling of this function
        BlueNRG1Gap::getInstance().setAddress(BLEProtocol::AddressType::RANDOM_STATIC, BLE_address_BE);
    }

    if(ret != BLE_STATUS_SUCCESS){
        PRINTF("aci_gap_init() failed: 0x%02x\r\n", ret);
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
    if (ret != BLE_STATUS_SUCCESS) {
        PRINTF("aci_gap_set_authentication_requirement()failed: 0x%02x\r\n", ret);
    }
    
    //aci_hal_set_tx_power_level(1,4);

    g_gap_service_handle = service_handle;
    g_appearance_char_handle = appearance_char_handle;
    g_device_name_char_handle = dev_name_char_handle;

    signalEventsToProcess();
    // update the peripheral preferred conenction parameters handle
    // This value is hardcoded at the moment.
    g_preferred_connection_parameters_char_handle = 10;

    return;
}



/**************************************************************************
    @brief  mbedOS
    @param[in]  void
    @returns
 **************************************************************************/
int btle_handler_pending = 0;

void btle_handler(void)
{
    btle_handler_pending = 0;
    BlueNRG1Gap::getInstance().Process();
    BTLE_StackTick();
    
}

tBleStatus btleStartRadioScan(uint8_t scan_type,
                              uint16_t scan_interval,
                              uint16_t scan_window,
                              uint8_t own_address_type)
{
  tBleStatus ret;

  PRINTF("scan_interval=%d scan_window=%d\n\r", scan_interval, scan_window);
  PRINTF("scan_type=%d own_address_type=%d\n\r", scan_type, own_address_type);
  /* FIXME: to be cecked
   * 0x00 Accept all advertisement packets (it is allowed only if controller privacy is enabled).
   * bluenrg1_gap_aci.h
   */
  uint8_t scanning_filter_policy = 0;
  ret = aci_gap_start_observation_proc(scan_interval,
                                       scan_window,
                                       scan_type,
                                       own_address_type,
                                       0,  // 1 to filter duplicates
                                       scanning_filter_policy);

  return ret;

}


void Attribute_Modified_CB(uint16_t Connection_Handle,
                           uint16_t Attr_Handle,
                           uint16_t Offset,
                           uint16_t Attr_Data_Length,
                           uint8_t Attr_Data[])
{
    
    //Extract the GattCharacteristic from p_characteristics[] and find the properties mask
    GattCharacteristic *p_char = BlueNRG1GattServer::getInstance().getCharacteristicFromHandle(Attr_Handle);
    if(p_char!=NULL) {
        GattAttribute::Handle_t charHandle = p_char->getValueAttribute().getHandle()-BlueNRG1GattServer::CHAR_VALUE_HANDLE;
        BlueNRG1GattServer::HandleEnum_t currentHandle = BlueNRG1GattServer::CHAR_HANDLE;
        PRINTF("CharHandle %d, length: %d, Data: %d\n\r", charHandle, Attr_Data_Length, Attr_Data[0]);
        PRINTF("getProperties 0x%x\n\r",p_char->getProperties());

        if(Attr_Handle == charHandle+BlueNRG1GattServer::CHAR_VALUE_HANDLE) {
            currentHandle = BlueNRG1GattServer::CHAR_VALUE_HANDLE;
        }

        if(Attr_Handle == charHandle+BlueNRG1GattServer::CHAR_DESC_HANDLE) {
            currentHandle = BlueNRG1GattServer::CHAR_DESC_HANDLE;
        }
        PRINTF("currentHandle %d\n\r", currentHandle);
        if((p_char->getProperties() &
            (GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_NOTIFY | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_INDICATE)) &&
            currentHandle == BlueNRG1GattServer::CHAR_DESC_HANDLE) {

            GattAttribute::Handle_t charDescHandle = p_char->getValueAttribute().getHandle()+1;

            PRINTF("*****NOTIFICATION CASE\n\r");
            //Now Check if data written in Enable or Disable
            if(Attr_Data[0]==1) {
                PRINTF("Notify ENABLED\n\r");
                BlueNRG1GattServer::getInstance().HCIEvent(GattServerEvents::GATT_EVENT_UPDATES_ENABLED, charDescHandle);
            } else {
                PRINTF("Notify DISABLED\n\r");
                BlueNRG1GattServer::getInstance().HCIEvent(GattServerEvents::GATT_EVENT_UPDATES_DISABLED, charDescHandle);
            }
            return;
        }

        //Check if attr handle property is WRITEABLE, in the case generate GATT_EVENT_DATA_WRITTEN Event
        if((p_char->getProperties() &
            (GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE_WITHOUT_RESPONSE | GattCharacteristic::BLE_GATT_CHAR_PROPERTIES_WRITE)) &&
            currentHandle == BlueNRG1GattServer::CHAR_VALUE_HANDLE) {

            PRINTF("*****WRITE CASE\n\r");

            GattWriteCallbackParams writeParams;
            writeParams.connHandle = Connection_Handle;
            writeParams.handle = p_char->getValueAttribute().getHandle();
            writeParams.writeOp = GattWriteCallbackParams::OP_WRITE_REQ;
            writeParams.len = Attr_Data_Length;
            writeParams.data = Attr_Data;
            writeParams.offset = Offset;

            //BlueNRGGattServer::getInstance().handleEvent(GattServerEvents::GATT_EVENT_DATA_WRITTEN, attr_handle);
            //Write the actual Data to the Attr Handle? (uint8_1[])att_data contains the data
            if ((p_char->getValueAttribute().getValuePtr() != NULL) && (p_char->getValueAttribute().getLength() > 0)) {
                BlueNRG1GattServer::getInstance().write(
                    p_char->getValueAttribute().getHandle(),
                    Attr_Data,
                    Attr_Data_Length,
                    false
                );
            }

            BlueNRG1GattServer::getInstance().HCIDataWrittenEvent(&writeParams);
        } else {
            PRINTF("*****WRITE DESCRIPTOR CASE\n\r");

            GattWriteCallbackParams writeParams;
            writeParams.connHandle = Connection_Handle;
            writeParams.handle = Attr_Handle;
            writeParams.writeOp = GattWriteCallbackParams::OP_WRITE_REQ;
            writeParams.len = Attr_Data_Length;
            writeParams.data = Attr_Data;
            writeParams.offset = Offset;

            BlueNRG1GattServer::getInstance().HCIDataWrittenEvent(&writeParams);
        }
    }
}


/******************************************************************************/
/*                 BlueNRG-1 Peripherals Interrupt Handlers                   */
/******************************************************************************/
extern "C" {
    void Blue_Handler(void){
        // Call RAL_Isr
        RAL_Isr();
    }
}

/******************************************************************************/
/*                 BlueNRG-1 BLE Events Callbacks                             */
/******************************************************************************/
#ifdef __cplusplus
extern "C" {
#endif

uint16_t connection_handle = 0;

/*******************************************************************************
 * Function Name  : hci_disconnection_complete_event.
 * Description    : This event occurs when a connection is terminated.
 * Input          : See file bluenrg1_events.h
 * Output         : See file bluenrg1_events.h
 * Return         : See file bluenrg1_events.h
 *******************************************************************************/
void hci_disconnection_complete_event(uint8_t Status,
                                      uint16_t Connection_Handle,
                                      uint8_t Reason)
{
#ifdef BLE_LED_DEBUG
    GPIO_WriteBit(GPIO_Pin_14, Bit_RESET);
#endif
    PRINTF("hci_disconnection_complete_event\r\n");
    connection_handle =0;
    
    if(BlueNRG1Gap::getInstance().getGapRole() == Gap::CENTRAL) {
        BlueNRG1GattClient::getInstance().removeGattConnectionClient(Connection_Handle, Reason);
    }
    BlueNRG1Gap::getInstance().processDisconnectionEvent(Connection_Handle, (Gap::DisconnectionReason_t)Reason);
    
}

/*******************************************************************************
 * Function Name  : hci_le_connection_complete_event.
 * Description    : This event indicates that a new connection has been created.
 * Input          : See file bluenrg1_events.h
 * Output         : See file bluenrg1_events.h
 * Return         : See file bluenrg1_events.h
 *******************************************************************************/
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
#ifdef BLE_LED_DEBUG
    GPIO_WriteBit(GPIO_Pin_14, Bit_SET);
#endif
    PRINTF("hci_le_connection_complete_event\r\n");
    
    connection_handle = Connection_Handle;

    Gap::Address_t ownAddr;
    Gap::AddressType_t ownAddrType;
    BlueNRG1Gap::getInstance().getAddress(&ownAddrType, ownAddr);

    Gap::AddressType_t peerAddrType = BLEProtocol::AddressType::RANDOM_STATIC;
    Gap::Role_t role;

    BlueNRG1Gap::getInstance().setConnectionHandle(Connection_Handle);
    BlueNRG1Gap::ConnectionParams_t connectionParams = {
                            /* minConnectionInterval = */ Conn_Interval,
                            /* maxConnectionInterval = */ Conn_Interval,
                            /* slaveLatency = */ Conn_Latency,
                            /* connectionSupervisionTimeout = */ Supervision_Timeout
                        };

    BlueNRG1Gap::getInstance().setConnectionInterval(Conn_Interval);

    switch (Peer_Address_Type) {
        case PUBLIC_ADDR:
            peerAddrType = BLEProtocol::AddressType::PUBLIC;
            break;
        case STATIC_RANDOM_ADDR:
            peerAddrType = BLEProtocol::AddressType::RANDOM_STATIC;
            break;
        case RESOLVABLE_PRIVATE_ADDR:
            peerAddrType = BLEProtocol::AddressType::RANDOM_PRIVATE_RESOLVABLE;
            break;
        case NON_RESOLVABLE_PRIVATE_ADDR:
            peerAddrType = BLEProtocol::AddressType::RANDOM_PRIVATE_NON_RESOLVABLE;
            break;
    }

    switch (Role) {
	case 0: //master
            role = Gap::CENTRAL;
            BlueNRG1GattClient::getInstance().createGattConnectionClient(Connection_Handle);
            break;
	case 1:
            role = Gap::PERIPHERAL;
            break;
	default:
            role = Gap::PERIPHERAL;
            break;
    }

    BlueNRG1Gap::getInstance().setGapRole(role);

    BlueNRG1Gap::getInstance().processConnectionEvent(Connection_Handle,
                                                     role,
                                                     peerAddrType,
                                                     Peer_Address,
                                                     ownAddrType,
                                                     ownAddr,
                                                     &connectionParams);
    

}


/*******************************************************************************
 * Function Name  : hci_le_advertising_report_event.
 * Description    : This event indicates that a Bluetooth device or multiple
 *                  Bluetooth devices have responded to an active scan
 *                  or received some information during a passive scan.
 * Input          : See file bluenrg1_events.h
 * Output         : See file bluenrg1_events.h
 * Return         : See file bluenrg1_events.h
 *******************************************************************************/
void hci_le_advertising_report_event(uint8_t Num_Reports,
                                     Advertising_Report_t Advertising_Report[])
{
    PRINTF("hci_le_advertising_report_event\r\n");

    for (int i = 0; i < Num_Reports; i++) {
        BlueNRG1Gap::getInstance().Discovery_CB(BlueNRG1Gap::DEVICE_FOUND,
                                               Advertising_Report[i].Event_Type,
                                               Advertising_Report[i].Address_Type,
                                               Advertising_Report[i].Address,
                                               &Advertising_Report[i].Length_Data,
                                               Advertising_Report[i].Data,
                                               &Advertising_Report[i].RSSI);
    }
}

/*******************************************************************************
 * Function Name  : aci_gatt_write_permit_req_event.
 * Description    : This event is given when a write request is received
 *                  by the server from the client.
 * Input          : See file bluenrg1_events.h
 * Output         : See file bluenrg1_events.h
 * Return         : See file bluenrg1_events.h
 *******************************************************************************/
void aci_gatt_write_permit_req_event(uint16_t Connection_Handle,
                                     uint16_t Attribute_Handle,
                                     uint8_t Data_Length,
                                     uint8_t Data[])
{
    PRINTF("aci_gatt_write_permit_req_event\r\n");

    // ask the local server if the write operation is authorized
    uint8_t err_code = BlueNRG1GattServer::getInstance().Write_Request_CB(
                            Connection_Handle,
                            Attribute_Handle,
                            Data_Length,
                            Data
                       );
    uint8_t write_status = err_code == 0 ? 0 : 1;

    // reply to the shield
    aci_gatt_write_resp(
                        Connection_Handle,
                        Attribute_Handle,
                        write_status,
                        err_code,
                        Data_Length,
                        Data
                       );
}

/*******************************************************************************
 * Function Name  : aci_gatt_read_permit_req_event.
 * Description    : This event is given when a read request is received
 *                  by the server from the client.
 * Input          : See file bluenrg1_events.h
 * Output         : See file bluenrg1_events.h
 * Return         : See file bluenrg1_events.h
 *******************************************************************************/
void aci_gatt_read_permit_req_event(uint16_t Connection_Handle,
                                    uint16_t Attribute_Handle,
                                    uint16_t Offset)
{
    PRINTF("aci_gatt_read_permit_req_event\r\n");

    BlueNRG1GattServer::getInstance().Read_Request_CB(Attribute_Handle);
}

/*******************************************************************************
 * Function Name  : aci_gatt_attribute_modified_event.
 * Description    : This event is given when a read request is received
 *                  by the server from the client.
 * Input          : See file bluenrg1_events.h
 * Output         : See file bluenrg1_events.h
 * Return         : See file bluenrg1_events.h
 *******************************************************************************/
void aci_gatt_attribute_modified_event(uint16_t Connection_Handle,
                                       uint16_t Attr_Handle,
                                       uint16_t Offset,
                                       uint16_t Attr_Data_Length,
                                       uint8_t Attr_Data[])
{
    PRINTF("aci_gatt_attribute_modified_event\r\n");

    Attribute_Modified_CB(Connection_Handle,
                          Attr_Handle,
                          Offset,
                          Attr_Data_Length,
                          Attr_Data);
}

/*******************************************************************************
 * Function Name  : aci_att_read_by_group_type_resp_event.
 * Description    : This event is generated in response to
 *                  a Read By Group Type Request.
 * Input          : See file bluenrg1_events.h
 * Output         : See file bluenrg1_events.h
 * Return         : See file bluenrg1_events.h
 *******************************************************************************/
void aci_att_read_by_group_type_resp_event(uint16_t Connection_Handle,
                                           uint8_t Attribute_Data_Length,
                                           uint8_t Data_Length,
                                           uint8_t Attribute_Data_List[])
{
    PRINTF("aci_att_read_by_group_type_resp_event\r\n");

    BlueNRG1GattClient::getInstance().primaryServicesCB(Connection_Handle,
                                                       Attribute_Data_Length,
                                                       Data_Length,
                                                       Attribute_Data_List);
}

/*******************************************************************************
 * Function Name  : aci_att_read_by_type_resp_event.
 * Description    : This event is generated in response to
 *                  a Read By Type Request.
 * Input          : See file bluenrg1_events.h
 * Output         : See file bluenrg1_events.h
 * Return         : See file bluenrg1_events.h
 *******************************************************************************/
void aci_att_read_by_type_resp_event(uint16_t Connection_Handle,
                                     uint8_t Handle_Value_Pair_Length,
                                     uint8_t Data_Length,
                                     uint8_t Handle_Value_Pair_Data[])
{
    PRINTF("aci_att_read_by_type_resp_event\r\n");

    BlueNRG1GattClient::getInstance().serviceCharsCB(Connection_Handle,
                                                    Data_Length,
                                                    Handle_Value_Pair_Length,
                                                    Handle_Value_Pair_Data);
}

/*******************************************************************************
 * Function Name  : aci_att_read_resp_event.
 * Description    : This event is generated in response to
 *                  a Read Request.
 * Input          : See file bluenrg1_events.h
 * Output         : See file bluenrg1_events.h
 * Return         : See file bluenrg1_events.h
 *******************************************************************************/
void aci_att_read_resp_event(uint16_t Connection_Handle,
                             uint8_t Event_Data_Length,
                             uint8_t Attribute_Value[])
{
    PRINTF("aci_att_read_resp_event\r\n");

    BlueNRG1GattClient::getInstance().charReadCB(Connection_Handle,
                                                Event_Data_Length,
                                                Attribute_Value);
}

/*******************************************************************************
 * Function Name  : aci_att_exec_write_resp_event.
 * Description    : This event is generated in response to an
 *                  Execute Write Request.
 * Input          : See file bluenrg1_events.h
 * Output         : See file bluenrg1_events.h
 * Return         : See file bluenrg1_events.h
 *******************************************************************************/
void aci_att_exec_write_resp_event(uint16_t Connection_Handle)
{
    PRINTF("aci_att_exec_write_resp_event\r\n");

    BlueNRG1GattClient::getInstance().charWriteExecCB(Connection_Handle);
}

/*******************************************************************************
 * Function Name  : aci_att_prepare_write_resp_event.
 * Description    : This event is generated in response to a
 *                  Prepare Write Request.
 * Input          : See file bluenrg1_events.h
 * Output         : See file bluenrg1_events.h
 * Return         : See file bluenrg1_events.h
 *******************************************************************************/
void aci_att_prepare_write_resp_event(uint16_t Connection_Handle,
                                      uint16_t Attribute_Handle,
                                      uint16_t Offset,
                                      uint8_t Part_Attribute_Value_Length,
                                      uint8_t Part_Attribute_Value[])
{
    PRINTF("aci_att_prepare_write_resp_event\r\n");

    BlueNRG1GattClient::getInstance().charWritePrepareCB(Connection_Handle,
                                                        Part_Attribute_Value_Length,
                                                        Attribute_Handle,
                                                        Offset,
                                                        Part_Attribute_Value);
}

/*******************************************************************************
 * Function Name  : aci_gatt_disc_read_char_by_uuid_resp_event.
 * Description    : This event can be generated during a
 *                  "Discover Characteristics By UUID" procedure or a
 *                  "Read using Characteristic UUID" procedure.
 * Input          : See file bluenrg1_events.h
 * Output         : See file bluenrg1_events.h
 * Return         : See file bluenrg1_events.h
 *******************************************************************************/
void aci_gatt_disc_read_char_by_uuid_resp_event(uint16_t Connection_Handle,
                                                uint16_t Attribute_Handle,
                                                uint8_t Attribute_Value_Length,
                                                uint8_t Attribute_Value[])
{
    PRINTF("aci_gatt_disc_read_char_by_uuid_resp_event\r\n");

    BlueNRG1GattClient::getInstance().serviceCharByUUIDCB(Connection_Handle,
                                                         Attribute_Value_Length,
                                                         Attribute_Handle,
                                                         Attribute_Value);
}

/*******************************************************************************
 * Function Name  : aci_att_find_by_type_value_resp_event.
 * Description    : This event is generated in response to a
 *                  aci_att_find_by_type_value_req.
 * Input          : See file bluenrg1_events.h
 * Output         : See file bluenrg1_events.h
 * Return         : See file bluenrg1_events.h
 *******************************************************************************/
void aci_att_find_by_type_value_resp_event(uint16_t Connection_Handle,
                                           uint8_t Num_of_Handle_Pair,
                                           Attribute_Group_Handle_Pair_t Attribute_Group_Handle_Pair[])
{
    PRINTF("aci_att_find_by_type_value_resp_event\r\n");

    BlueNRG1GattClient::getInstance().primaryServiceCB(Connection_Handle,
                                                      Num_of_Handle_Pair,
                                                      (uint8_t *)Attribute_Group_Handle_Pair);
}

/*******************************************************************************
 * Function Name  : aci_gatt_proc_complete_event.
 * Description    : This event is generated when a GATT client procedure
 *                  completes either with error or successfully.
 * Input          : See file bluenrg1_events.h
 * Output         : See file bluenrg1_events.h
 * Return         : See file bluenrg1_events.h
 *******************************************************************************/
void aci_gatt_proc_complete_event(uint16_t Connection_Handle,
                                  uint8_t Error_Code)
{
    PRINTF("aci_gatt_proc_complete_event Error_Code=%d\r\n", Error_Code);

    BlueNRG1GattClient::getInstance().gattProcedureCompleteCB(Connection_Handle, Error_Code);
}

/*******************************************************************************
 * Function Name  : aci_att_find_info_resp_event.
 * Description    : This event is generated in response to a
 *                  Find Information Request.
 * Input          : See file bluenrg1_events.h
 * Output         : See file bluenrg1_events.h
 * Return         : See file bluenrg1_events.h
 *******************************************************************************/
void aci_att_find_info_resp_event(uint16_t Connection_Handle,
                                  uint8_t Format,
                                  uint8_t Event_Data_Length,
                                  uint8_t Handle_UUID_Pair[])
{
    PRINTF("aci_att_find_info_resp_event\r\n");

    BlueNRG1GattClient::getInstance().discAllCharacDescCB(Connection_Handle,
                                                         Event_Data_Length,
                                                         Format,
                                                         Handle_UUID_Pair);
}

/*******************************************************************************
 * Function Name  : aci_l2cap_connection_update_req_event.
 * Description    : This event is sent by the GAP to the upper layers when a
 *                  procedure previously started has been terminated
 *                  by the upper layer or has completed for any other reason.
 * Input          : See file bluenrg1_events.h
 * Output         : See file bluenrg1_events.h
 * Return         : See file bluenrg1_events.h
 *******************************************************************************/
void aci_l2cap_connection_update_req_event(uint16_t Connection_Handle,
                                           uint8_t Identifier,
                                           uint16_t L2CAP_Length,
                                           uint16_t Interval_Min,
                                           uint16_t Interval_Max,
                                           uint16_t Slave_Latency,
                                           uint16_t Timeout_Multiplier)
{
    PRINTF("aci_l2cap_connection_update_req_event\r\n");

    // we assume the application accepts the request from the slave
    aci_l2cap_connection_parameter_update_resp(Connection_Handle,
                                               Interval_Min,
                                               Interval_Max,
                                               Slave_Latency,
                                               Timeout_Multiplier,
                                               CONN_L1, CONN_L2,
                                               Identifier,
                                               0x0000);
}

/*******************************************************************************
 * Function Name  : aci_gap_proc_complete_event.
 * Description    : The event is given by the L2CAP layer when a
 *                  connection update request is received from the slave.
 * Input          : See file bluenrg1_events.h
 * Output         : See file bluenrg1_events.h
 * Return         : See file bluenrg1_events.h
 *******************************************************************************/
void aci_gap_proc_complete_event(uint8_t Procedure_Code,
                                 uint8_t Status,
                                 uint8_t Data_Length,
                                 uint8_t Data[])
{

    PRINTF("aci_gap_proc_complete_event (Procedure_Code=0x%02X)\r\n", Procedure_Code);

    switch(Procedure_Code) {
        case GAP_OBSERVATION_PROC:

          BlueNRG1Gap::getInstance().Discovery_CB(BlueNRG1Gap::DISCOVERY_COMPLETE, 0, 0, NULL, NULL, NULL, NULL);
          break;
    }
}

#ifdef __cplusplus
}
#endif
