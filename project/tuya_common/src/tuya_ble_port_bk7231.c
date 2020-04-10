
#include "rwip_config.h"
#include "tuya_ble_type.h"
#include "tuya_ble_port.h"
#include "tuya_ble_type.h"
#include "ble_api.h"
#include "ble_pub.h"
#include "uart_pub.h"
#include "app_task.h"
#include "mcu_ps_pub.h"
#include "application.h"

#define BLE_OPEN         1

uint16_t ntf_enable;

#if defined(TY_BT_MOD) && TY_BT_MOD == 1
bk_attm_desc_t btl_att_db[6] =
{
	//  Service Declaration
	[0]        =   {0x1910,  BK_PERM_SET(RD, ENABLE), 0, 0},
	
	//  Level Characteristic Declaration
	[1]        =   {0x2803,   BK_PERM_SET(RD, ENABLE), 0, 0},
	//  Level Characteristic Value
	[2]        =   {0x2b11,   BK_PERM_SET(WRITE_REQ, ENABLE), BK_PERM_SET(RI, ENABLE) , 20},
	
	[3]        =   {0x2803,   BK_PERM_SET(RD, ENABLE), 0, 0},
	//  Level Characteristic Value
	[4]        =   {0x2b10,   BK_PERM_SET(NTF, ENABLE) , BK_PERM_SET(RI, ENABLE) , 20},

	//  Level Characteristic - Client Characteristic Configuration Descriptor

	[5]        =   {0x2902,  BK_PERM_SET(RD, ENABLE)|BK_PERM_SET(WRITE_REQ, ENABLE), 0, 0},
};

TY_BT_MSG_CB ty_bt_msg_cb;
uint8_t ble_stack_init_ok = 0;

void ble_write_callback(write_req_t *write_req)
{
    tuya_ble_data_buf_t data;

    if(write_req->att_idx == 5)
    {
        ntf_enable = (write_req->value[0]) | (write_req->value[1] << 8);
    }
    else
    {
        data.data = write_req->value;
        data.len = write_req->len;

        bk_printf("%s len:%d\r\n", __FUNCTION__, data.len);
        if(ty_bt_msg_cb!=NULL)
            ty_bt_msg_cb(0, TY_BT_EVENT_RX_DATA, &data);
    }
}

uint8_t ble_read_callback(read_req_t *read_req)
{
    if(read_req->att_idx == 5)
    {
        read_req->value = ntf_enable;
    }
    return 2;
}

void appm_update_param(struct gapc_conn_param *conn_param);

void ble_update_parma(void)
{
	struct gapc_conn_param param;

	param.intv_min = 78;
	param.intv_max = 80;
	param.latency = 0;
	param.time_out = 200;
	
    bk_printf("STACK appm_update_param\r\n");
	appm_update_param(&param);
}

unsigned char ble_att_flag = 0;
void ble_event_callback(ble_event_t event, void *param)
{
    switch(event)
    {
        case BLE_STACK_OK:
        {
            bk_printf("STACK INIT OK\r\n");
            struct bk_ble_db_cfg ble_db_cfg;
            ble_db_cfg.att_db = btl_att_db;
            ble_db_cfg.att_db_nb = 6;
            ble_db_cfg.prf_task_id = 0;
            ble_db_cfg.start_hdl = 0;
            ble_db_cfg.svc_perm = 0;
            ble_db_cfg.uuid = 0x1910;

            bk_ble_create_db(&ble_db_cfg);
        }
        break;
        case BLE_STACK_FAIL:
            bk_printf("STACK INIT FAIL\r\n");
        break;

        case BLE_CONNECT:
            bk_printf("BLE CONNECT\r\n");

			ble_update_parma();
		
            mcu_prevent_set(MCU_PS_BLE_FROBID);
            if(ty_bt_msg_cb!=NULL)
                ty_bt_msg_cb(0, TY_BT_EVENT_CONNECTED ,NULL);
        break;

        case BLE_DISCONNECT:
        {
            bk_printf("BLE DISCONNECT\r\n");
			ble_att_flag = 0;
            mcu_prevent_clear(MCU_PS_BLE_FROBID);
            if(ty_bt_msg_cb!=NULL)
                ty_bt_msg_cb(0, TY_BT_EVENT_DISCONNECTED ,NULL);

#if 1
            //该代码为了解决蓝牙配网的临时解决方案。
            typedef BYTE_T GW_WF_NWC_STAT_T;
#define GWNS_LOWPOWER           0   // current in low power mode
#define GWNS_UNCFG_SMC          1   // current in smart-cfg mode
#define GWNS_UNCFG_AP           2   // current in ap-cfg mode
#define GWNS_TY_SMARTCFG        3   // already get ssid && passwd from smart-cfg mode
#define GWNS_TY_AP              4   // already get ssid && passwd from ap-cfg mode
#define GWNS_WECHAT_AK          5   // already get ssid && passwd from wechat mode
#define GWNS_OTHER_CFG          6   // already get ssid && passwd from other mode.for example:uart、bluetooth、qrcode and so on
#define GWNS_NO_NEED_CFG        7   // no need config wifi


            GW_WF_NWC_STAT_T nc_tp;
            OPERATE_RET  ret = gw_get_nctp_stat(&nc_tp);
            if(OPRT_OK == ret) {
                if(nc_tp == GWNS_UNCFG_SMC || nc_tp == GWNS_UNCFG_AP) {
                    bk_printf("****************tuya_bt_start_adv****************");
                    tuya_bt_start_adv();
                }
            }
#endif

        }
        break;

        case BLE_MTU_CHANGE:
            bk_printf("BLE_MTU_CHANGE:%d\r\n", *(uint16_t *)param);
        break;
        case BLE_CFG_NOTIFY:
            bk_printf("BLE_CFG_NOTIFY:%d\r\n", *(uint16_t *)param);
        break;
        case BLE_CFG_INDICATE:
            bk_printf("BLE_CFG_INDICATE:%d\r\n", *(uint16_t *)param);
        break;
        case BLE_TX_DONE:
            bk_printf("BLE_TX_DONE\r\n");
			ble_att_flag = 0;
        break;
        case BLE_GEN_DH_KEY:
        {
            bk_printf("BLE_GEN_DH_KEY\r\n");
            bk_printf("key_len:%d\r\n", ((struct ble_gen_dh_key_ind *)param)->len);
            for(int i = 0; i < ((struct ble_gen_dh_key_ind *)param)->len; i++)
            {
                bk_printf("%02x ", ((struct ble_gen_dh_key_ind *)param)->result[i]);
            }
            bk_printf("\r\n");
        }    
        break;
        case BLE_GET_KEY:
        {
            bk_printf("BLE_GET_KEY\r\n");
            bk_printf("pri_len:%d\r\n", ((struct ble_get_key_ind *)param)->pri_len);
            for(int i = 0; i < ((struct ble_get_key_ind *)param)->pri_len; i++)
            {
                bk_printf("%02x ", ((struct ble_get_key_ind *)param)->pri_key[i]);
            }
            bk_printf("\r\n");
        }    
        break;
        case BLE_CREATE_DB_OK:
        {
            bk_printf("CREATE DB SUCCESS\r\n");
            ble_stack_init_ok = 1;
            if(ty_bt_msg_cb!=NULL)
                ty_bt_msg_cb(0, TY_BT_EVENT_ADV_READY ,NULL);
        }
        break;
        default:
            bk_printf("UNKNOW EVENT\r\n");
        break;
    }
}

void ble_recv_adv_callback(uint8_t *buf, uint8_t len)
{
#if (BLE_APP_CLIENT)
    uint8_t find = 0;

    find = appm_adv_data_decode(len, buf, NULL, 0);

    if(find)
    {
        extern int *scan_check_result;
        if(scan_check_result)
        {
            *scan_check_result = 2;
            bk_printf("scan_check_result\r\n");
        }
    }
#endif
}

OPERATE_RET tuya_before_netcfg_cb(VOID)
{    
#if BLE_OPEN
    bk_printf("!!!!!!!!!!tuya_before_netcfg_cb\r\n");
	appm_start_advertising();
#endif
	return OPRT_OK;
}

OPERATE_RET tuya_after_netcfg_cb(VOID)
{ 
#if BLE_OPEN
	uint8_t app_status = 0;
	app_status = appm_get_app_status();
	bk_printf("!!!!!!!!!!tuya_after_netcfg_cb:%d\r\n", app_status);
	if(app_status == 3)
	{
		appm_stop_advertising();
	}
	if(app_status == 9)
	{
		appm_disconnect(0x13);
	}
	
#endif
	return OPRT_OK;
}

OPERATE_RET tuya_bt_port_init(ty_bt_param_t *p)
{
#if BLE_OPEN
    ble_err_t status;
    bk_printf("!!!!!!!!!!tuya_bt_port_init\r\n");

    if((p!=NULL)&&(p->cb!=NULL))
        ty_bt_msg_cb = p->cb;
    else
        bk_printf("!!!!!!!!!!tuya_bt_port_init, p is null\r\n");

    ble_activate(NULL);
    ble_set_write_cb(ble_write_callback);
    ble_set_read_cb(ble_read_callback);
    ble_set_event_cb(ble_event_callback);
    ble_set_recv_adv_cb(ble_recv_adv_callback);

#endif        
    return OPRT_OK;
}

OPERATE_RET tuya_bt_port_deinit(void)
{
#if BLE_OPEN   
	bk_printf("!!!!!!!!!!tuya_bt_port_deinit\r\n");    
	if(appm_stop_advertising() == ERR_SUCCESS)    
	{        
		rtos_delay_milliseconds(100);    
	}
#endif
	return OPRT_OK;
}

OPERATE_RET tuya_bt_gap_disconnect(void)
{
#if BLE_OPEN
    bk_printf("!!!!!!!!!!tuya_bt_gap_disconnect\r\n");
    appm_disconnect(0x13);
#endif
    return OPRT_OK;
}


OPERATE_RET tuya_bt_send(BYTE_T *data, UINT8_T len)
{
#if BLE_OPEN
    bk_printf("!!!!!!!!!!tuya_bt_send\r\n");
	while(ble_att_flag == 1){
		rtos_delay_milliseconds(1);
	}
	ble_att_flag = 1;

    bk_ble_send_ntf_value(len, data, 0, 4);
#endif
    return OPRT_OK;
}

OPERATE_RET tuya_bt_reset_adv(tuya_ble_data_buf_t *adv, tuya_ble_data_buf_t *scan_resp)
{
#if BLE_OPEN
    bk_printf("!!!!!!!!!!tuya_bt_reset_adv\r\n");
    /*if (kernel_state_get(TASK_APP) == APPM_ADVERTISING)
    {
        appm_stop_advertising();
        rtos_delay_milliseconds(100);
    }*/

    memset(&adv_info, 0x00, sizeof(adv_info));

    adv_info.channel_map = 7;
    adv_info.interval_min = 160;
    adv_info.interval_max = 160;

    memcpy(adv_info.advData, adv->data, adv->len);
    adv_info.advDataLen = adv->len;

    memcpy(adv_info.respData, scan_resp->data, scan_resp->len);
    adv_info.respDataLen = scan_resp->len;
    
    //appm_start_advertising();
#endif
    return OPRT_OK;
}

OPERATE_RET tuya_bt_get_rssi(SCHAR_T *rssi)
{
#if BLE_OPEN
    bk_printf("!!!!!!!!!!tuya_bt_get_rssi\r\n");
#endif
    return OPRT_OK;
}

OPERATE_RET tuya_bt_start_adv(void)
{
#if BLE_OPEN
    bk_printf("!!!!!!!!!!tuya_bt_start_adv\r\n");
    if(appm_start_advertising()==OPRT_OK)
        return OPRT_OK;
    else
        return OPRT_COM_ERROR;
#endif
    return OPRT_OK;
}

OPERATE_RET tuya_bt_stop_adv(void)
{
#if BLE_OPEN
    bk_printf("!!!!!!!!!!tuya_bt_stop_adv\r\n");
    if(appm_stop_advertising()==OPRT_OK)
        return OPRT_OK;
    else
        return OPRT_COM_ERROR;
#endif
    return OPRT_OK;
}

OPERATE_RET tuya_bt_assign_scan(IN OUT ty_bt_scan_info_t *info)
{
#if BLE_OPEN
    bk_printf("!!!!!!!!!!tuya_bt_assign_scan\r\n");
#endif
    return OPRT_OK;
}

#endif
