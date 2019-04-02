
#include "stdlib.h"
#include <iostream>
#include "tp_communication_if.h"

int  cmd_serial = 0;
char rx[1024] = {0};


char pp_get_device(char type)
{   
    char device = 0;
    switch(type)
    {
       case DB_QD:
          device = INDIVIDUAL_SOLDIER;
          break;
       case DB_JD_QD:
          device = INDIVIDUAL_PRECIS_SOLDIER;
          break;
       case BX_QD:
          device = PORTABLE_INDI_SOLDIER;
          break;
       case BX_CZ:
          device = PORTABLE_INDI_SOLDIER_SIDE;
          break;
       case BX_YB:
          device = PORTABLE_INDI_SOLDIER_SWING;
          break;

       default:
          device = 0xff;
          break;
    }
    return device;

}


char pp_get_device_type(char device)
{
	char type = 0;
	switch (device)
	{
	case INDIVIDUAL_SOLDIER:
		type = DB_QD;
		break;
	case INDIVIDUAL_PRECIS_SOLDIER:
		type = DB_JD_QD;
		break;
	case PORTABLE_INDI_SOLDIER:
		type = BX_QD;
		break;
	case PORTABLE_INDI_SOLDIER_SIDE:
		type = BX_CZ;
		break;
	case PORTABLE_INDI_SOLDIER_SWING:
		type = BX_YB;
		break;

	default:
		type = 0xff;
		break;
	}
	return type;

}


char pp_get_checsum(char* tx, int lenth)
{
    char checsum = 0;
    int i = 0;
    for(i = 0; i < lenth; i++)
    {
        checsum += tx[i];
    }
    return checsum;
}
int pp_cfg_format(tg_ctrl_cfg* cmd, char* data)
{
    int rec =0;

    //printf("pp_cfg_format  pp_cfg_format %02x  \n", cmd->op_type);
    switch(cmd->op_type)
    {
       case TG_CMD_CFG_DEVTYPE:
          data[0] = (cmd->data & 0xff);
          data[1] = (cmd->data2 & 0xff);
          data[2] = 0;
          data[3] = 0;
          break;
       case TG_CMD_CFG_DEVNUMB:
           data[0] = (cmd->data & 0xff);
           data[1] = ((cmd->data >> 8) & 0xff);  //user id
           data[2] = (cmd->data2 & 0xff);
           data[3] = ((cmd->data2 >> 8) & 0xff);   //dev id
          break;
       case TG_CMD_CFG_DEVCMU:
           data[0] = (cmd->data & 0xff);           
           data[1] = 0;
           if(cmd->data == TG_CM_CYCLE_SEND)
           {
               data[1] = (cmd->data2 & 0xff);
           }
           data[2] = 0;
           data[3] = 0;
          break;
       case TG_CMD_CFG_MODE:
           data[0] = (cmd->data & 0xff);
           data[1] = 0;
           data[2] = 0;
           data[3] = 0;
          break;
       case TG_CMD_CTL_RESET:
           data[0] = 0;  //
           data[1] = 0;
           data[2] = 0;
           data[3] = 0;
          break;
       case TG_CMD_CTL_UPDOWN:
           data[0] = ((cmd->data << 4) & 0xf0)|(cmd->data2 & 0x0f);
           data[1] = 0;
           data[2] = 0;
           data[3] = 0;
          break;
       case TG_CMD_CTL_SWING:
           data[0] = ((cmd->data >> 8) & 0xf0)|((cmd->data & 0x0f));
           data[1] = ((cmd->data2 >> 8) & 0xf0)|((cmd->data2 & 0x0f));;
           data[2] = cmd->times;
           data[3] = 0;
          break;
       case TG_CMD_CTL_LIGHT:
           data[0] = (cmd->data & 0xff);
           data[1] = 0;
           data[2] = 0;
           data[3] = 0;
          break;
       case TG_CMD_CTL_HIT_NUM:
           data[0] = (cmd->data & 0xff);
           data[1] = ((cmd->data >> 8) & 0xff);  //
           data[2] = 0;
           data[3] = 0;
          break;

       default:
          rec = -1;
          break;
    }
    return rec;
}
void pp_cfg_comm_format(tg_ctrl_cfg* cmd, contrl_cmd* data)
{
	data->head = 0x55;
	data->lenth = 0x0f;
	data->cmd_serial = (cmd_serial & 0xff);
	data->device = 0x01;  //轻武器靶标系列
	data->sub_dev = pp_get_device(cmd->dev_type);
	data->user_id_h = (cmd->userId & 0xff00) >> 8;   //
	data->user_id_l = cmd->userId & 0xff;   //
	data->dev_id_h = (cmd->targetId & 0xff00) >> 8;  // 
	data->dev_id_l = cmd->targetId & 0xff;  // 
	data->cmd_type = cmd->op_type;    
    return ;
}

/* 精度靶 :  工作模式、参数对应关系、状态信息(位置信息、运行信息)、设备信息(电量、故障信息)*/
/* 战术靶 :  工作模式、参数对应关系、状态信息(位置信息、运行信息)、设备信息(电量、故障信息)*/
/*命令类型-- 配置、控制、查询 */
/*********************************************************************
* func:        target_cfg_command
* Purpose:     靶标配置接口
* parameter:   targetId  设备编号；
*              dev_type  设备类型:
*                            1     单兵起倒靶 Individual soldier 
*                            2     单兵精度起倒靶  
*                            3     便携式起倒靶 Individual precision reverse target
*                            4     便携式侧转靶 Portable  side  
*                            5     便携式摇摆靶
*              op_type : 
*                          #define TG_CMD_CFG_DEVTYPE            0x30  //  配置设备类型
*                          #define TG_CMD_CFG_DEVNUMB            0x31 //  配置设备号
*                          #define TG_CMD_CFG_DEVCMU             0x32 // 配置通信模式
*                          #define TG_CMD_CFG_MODE               0x50 //  （命中后是否倒等）
*              data:    参数数值
* return:      success: 0；  error: -1;  timeout: -2;
**********************************************************************/
int  target_cfg_command(tg_ctrl_cfg* cmd,char* buff)
{
    int rec = 0;
    contrl_cmd* cmd_data = (contrl_cmd*)buff;

    pp_cfg_comm_format(cmd, cmd_data);
    rec = pp_cfg_format(cmd, cmd_data->ctl_data);
    if(rec < 0)
    {
        printf("pp_cfg_format cfg error %d  \n", rec);
        return -1;
    }
    cmd_data->checksum = (char)pp_get_checsum(buff, cmd_data->lenth - 1);

     // send 
     
    cmd_serial++;

    return 15;
}

/*********************************************************************
* func:        target_query_command
* Purpose:     靶标配置接口
* parameter:   targetId  设备编号；
*              dev_type  设备类型:
*                            1     单兵起倒靶 Individual soldier 
*                            2     单兵精度起倒靶  
*                            3     便携式起倒靶 Individual precision reverse target
*                            4     便携式侧转靶 Portable  side  
*                            5     便携式摇摆靶
*              op_type : 
*                          #define TG_CMD_CTL_RESET              0x80 //  reset  
*                          #define TG_CMD_CTL_UPDOWN             0x81 // 起倒/侧转控制
*                          #define TG_CMD_CTL_SWING              0x50 // 摇摆靶控制
*                          #define TG_CMD_CTL_LIGHT              0xa0 // 灯光控制
*                          #define TG_CMD_CTL_HIT_NUM            0x50 // 设置命中弹数
*              data:    坐标值:经纬度、分 定位与否及经纬度标识  BIT7： 0，表示未定位；1，表示已定位  BIT1：0，表示北纬；1，表示南纬 BIT0：0，表示东经；1，表示西经
*                       状态:  dev_status
* return:      success: 0；  error: -1;  timeout: -2;
**********************************************************************/
int  target_ctrl_command(tg_ctrl_cfg* cmd,char* buff)
{
    int rec =0;
    contrl_cmd* cmd_data = (contrl_cmd*)buff;

    pp_cfg_comm_format(cmd, cmd_data);
    
    rec = pp_cfg_format(cmd, cmd_data->ctl_data);
    if(rec < 0)
    {
        
        printf("pp_cfg_format ctrl error %d  \n", rec);
        return -1;
    }
    cmd_data->checksum = (char)pp_get_checsum(buff, cmd_data->lenth - 1);

     // send 
     
    cmd_serial++;

    return 15;

}


/*********************************************************************
* func:        target_ctrl_command
* Purpose:     靶标配置接口
* parameter:   targetId  设备编号；
*              dev_type  设备类型:
*                            1     单兵起倒靶 Individual soldier 
*                            2     单兵精度起倒靶  
*                            3     便携式起倒靶 Individual precision reverse target
*                            4     便携式侧转靶 Portable  side  
*                            5     便携式摇摆靶
*              op_type : 
*                       #define TG_CMD_QUERY_STATUS           0x01 //  设备状态（故障、起、倒、命中弹数等）查询
*                       #define TG_CMD_QUERY_POSISION         0x02  //  定位坐标查询
*              data:    参数数值
* return:      success: 0；  error: -1;  timeout: -2;
**********************************************************************/
int  target_query_command(target_query* cmd,char* buff)
{
    int rec             =  0;
    contrl_cmd* cmd_data  = (contrl_cmd*)buff;

    pp_cfg_comm_format((tg_ctrl_cfg*)cmd, cmd_data);
    cmd_data->checksum = (char)pp_get_checsum(buff, cmd_data->lenth - 1);

     // send 
     
    cmd_serial++;
	return 15;

}



int  target_query_return_msg(char* msg, int msg_len, target_query* info)
{
	int rec = 0;
	target_data* paket = NULL;
	pp_data*     status = NULL;

	if (msg_len < 18)
	{
		return -2;
	}
	paket = (target_data*)msg;

	info->targetId = (paket->dev_id_h << 8) | paket->dev_id_l;
	info->userId = (paket->user_id_h << 8) | paket->user_id_l;;
	info->dev_type = (dev_type_e)pp_get_device_type(paket->sub_dev);
	info->op_type = paket->cmd_type;

	if (paket->cmd_type == TG_CMD_QUERY_POSISION)
	{
		memcpy(&(info->data.ps_info), paket->data, sizeof(positon_s));
	}
	else if (paket->cmd_type == TG_CMD_QUERY_STATUS)
	{
		status = (pp_data*)paket->data;
		info->data.stat_info.fault = (status->fault_h << 8 | status->fault_l);
		info->data.stat_info.run_state = status->run_state;
		info->data.stat_info.battery = status->battery;
		info->data.stat_info.hit_total = (status->total_times_h << 8 | status->total_times_l);
		info->data.stat_info.cycle_hits = status->cycle_times;
		memcpy(info->data.stat_info.hit, status->data, status->cycle_times);
	}
	else
	{
		printf("pp  query msg error %d  \n", paket->cmd_type);
		return -1;
	}


	return 0;

}

void printf_tx_buffer(int lenth)
{
	int i = 0;
	printf("\n");
	for (i = 0; i < lenth; i++)
	{
		printf(" %02x", 0x0f);
		if ((i != 0) && (i % 8 == 0))printf("\n");
	}
	return;
}

#ifdef CS_PP_IF
int hanshaking_pkt_tx(void* data, char* tx)
{
    tp_handsh_info* cdtIfo = (tp_handsh_info*)tx;

    cdtIfo->type = htons(HANDSHAKING_DATA);
    cdtIfo->tg_number = htons(sys_pp_info.tg_number);
    cdtIfo->crc = htons(CRC16_USB(tx, sizeof(tp_handsh_info)-2));

    data = data;
    
    return 0;

}

int target_state_pkt_tx(void* data, void* tx)
{
    /*run  stop  position + - */
    int tmp = 0;
    tp_status_info* st_info = (tp_status_info*)tx;

    st_info->type = htons(TARGET_STATE);
    st_info->tg_number = htons(sys_pp_info.tg_number);
    if(data == NULL)
    {
        st_info->car_pos = htons(sys_pp_info.pstion);
        st_info->tp_pos = htons(sys_pp_info.tgStatus);
        st_info->status = htons(sys_pp_info.run);
    }
    else
    {
        tmp = *((int*)data);
        st_info->tp_pos = htons(2);
        st_info->status = htons(tmp);
    }
    st_info->crc = htons(CRC16_USB(tx, sizeof(tp_status_info)-2));
    return 0;

}
int target_battery_pkt_tx(void* data, void* tx)
{
    unsigned short alarm = *((unsigned short*)data);
    //alarm 
    tp_comm_data* almInfo = (tp_comm_data*)tx;

    almInfo->type = htons(BATTRY_STAT);
    almInfo->tg_number = htons(sys_pp_info.tg_number);
    almInfo->data = htons(alarm);
    almInfo->crc = htons(CRC16_USB(tx, sizeof(tp_comm_data)-2));

    return 0;
}

int target_alarm_pkt_tx(void* data, void* tx)
{
    unsigned short alarm = *((unsigned short*)data);
    //alarm 
    tp_comm_data* almInfo = (tp_comm_data*)tx;

    almInfo->type = htons(TARGET_ALARM);
    almInfo->tg_number = htons(sys_pp_info.tg_number);
    almInfo->data = htons(alarm);
    almInfo->crc = htons(CRC16_USB(tx, sizeof(tp_comm_data)-2));

    return 0;
}
int pp_sys_heart(void* data, void* tx)
{
    //heart 
    tp_comm_data* almInfo = (tp_comm_data*)tx;

    almInfo->type = htons(SYS_HEART);
    almInfo->tg_number = htons(sys_pp_info.tg_number);
    almInfo->data = htons(0x1234);
    almInfo->crc = htons(CRC16_USB(tx, sizeof(tp_comm_data)-2));

    return 0;
}

int get_coordinate_pkt_tx(void* data, char* tx)
{/*弹点数据*/
   cdt_data* cdt = (cdt_data*)data;
   tp_pp_cdtInfo* cdtIfo = (tp_pp_cdtInfo*)tx;
   UINT32 temp = 0;
   //memcpy(&tx->data.cdtInfo, cdt, sizeof(cdt_data)*DATA_GROUP_MAX);
   
   cdtIfo->type = htons(COORDINATE_DATA);
   cdtIfo->tg_number = htons(sys_pp_info.tg_number);
   cdtIfo->x = htonl(cdt[1].x);
   cdtIfo->y = htonl(cdt[1].y);
   cdtIfo->p_numb = htons(sys_pp_info.cur_num);
   cdtIfo->crc = htons(CRC16_USB(tx, sizeof(tp_pp_cdtInfo)-2));
   //gettimeofday(&(tx->data.cdtInfo.time), NULL);

   printf("send cdt tg %d x %d  y%d p_num %d crc %d\n",
     sys_pp_info.tg_number, cdt[1].x, cdt[1].y,sys_pp_info.cur_num,
     cdtIfo->crc);
   
   //cdt[i].time.tv_sec = current_time0.tv_sec;
   //cdt[i].time.tv_usec = current_time0.tv_usec;
   return 0;
}


int get_temprature_pkt_tx(void* data, char* tx)
{
    /*temprature*/
    tp_comm_data* tmInfo = (tp_comm_data*)tx;
    UINT16 tempra = 0;

    if(sys_pp_info.temperature < 0)
    {
        tempra =  (fabs(sys_pp_info.temperature) * 100);
        tempra |= 0x8000; 
    }
    else{
      tempra = sys_pp_info.temperature * 100;
    }

    tmInfo->type = htons(TEMPERATURE_DATA);
    tmInfo->tg_number = htons(sys_pp_info.tg_number);
    tmInfo->data = htons(tempra);
    tmInfo->crc = htons(CRC16_USB(tx, sizeof(tp_comm_data) - 2));

    return 0;

}

int get_berfor_coord_pkt_tx(void* data, char* tx)
{/*弹点数据*/
   tp_pp_cdtInfo_get* cdt = (tp_pp_cdtInfo_get*)data;
   tp_pp_cdtInfo* cdtIfo = (tp_pp_cdtInfo*)tx;
   
   UINT32 sendNum = ntohs(cdt->p_numb);
   
   cdt_data* cdtDate = sys_pp_info.cdtInfo;
   UINT32 curNum = sys_pp_info.cur_num;
   //memcpy(&tx->data.cdtInfo, cdt, sizeof(cdt_data)*DATA_GROUP_MAX);

   printf("get_berfor_coord_pkt_tx 111 cdt tg %d  numb %d  crc %d \n",
     curNum, sendNum, cdtIfo->crc);

   
   cdtIfo->type = htons(COORDINATE_DATA);
   cdtIfo->tg_number = htons(sys_pp_info.tg_number);
   cdtIfo->p_numb = htons(sendNum);

   if(sendNum > curNum)
   {
       sendNum = curNum;
   }
   
   cdtIfo->x = htonl(cdtDate[sendNum].x);
   cdtIfo->y = htonl(cdtDate[sendNum].y);
   cdtIfo->p_numb = htons(sendNum);
   cdtIfo->crc = htons(CRC16_USB(tx, sizeof(tp_pp_cdtInfo)-2));
   //gettimeofday(&(tx->data.cdtInfo.time), NULL);

   printf("get_berfor_coord_pkt_tx  cdt tg %d  numb %d  crc %d \n",
     cdtIfo->tg_number, cdtIfo->p_numb, cdtIfo->crc);
   
   //cdt[i].time.tv_sec = current_time0.tv_sec;
   //cdt[i].time.tv_usec = current_time0.tv_usec;
   return 0;
}


int pp_packet_send(void)
{
    int send_size;
    send_size = sizeof(tp_pp_packetInfo);

  //  printf("pp_packet_send  %d \n", sys_pp_info.ctrlMod);

    
    if(sys_pp_info.ctrlMod == CTRL_MODE_M)
    {

    
        //printf("pp_packet_send main ip  %x \n", ntohl(server_address.sin_addr.s_addr));
        if(sendto(pp_Socket, (char *) &pkt_tx, send_size, 0,(struct sockaddr *)&server_address, sizeof(pc_address)) == 0 ) 
        {
            printf("Failed in  send NTP reply to the SNTP client\n");
            return -2;
        }
    }
    else
    {
    
        //printf("pp_packet_send sub  ip   %x \n", ntohl(server_address2.sin_addr.s_addr));
        if(sendto(pp_Socket, (char *) &pkt_tx, send_size, 0,(struct sockaddr *)&server_address2, sizeof(pc_address)) == 0 ) 
        {
            printf("Failed in  send NTP reply to the SNTP client\n");
            return -2;
        }
    }
    return 0;
}



int pp_data_send_format( UINT16 type, void* data)
{
   int rec = 0;
   
   memset(&pkt_tx, 0,1500);
   switch(type)
   {
       case HANDSHAKING_DATA:
           rec = hanshaking_pkt_tx(data, &pkt_tx);
           break;
       case TARGET_STATE:
           rec = target_state_pkt_tx(data, &pkt_tx);
           break;
       case TARGET_ALARM:
           rec = target_alarm_pkt_tx(data, &pkt_tx);
           break;
       
       case COORDINATE_DATA:
           rec = get_coordinate_pkt_tx(data, &pkt_tx);
           break;
       case TEMPERATURE_DATA:
           rec = get_temprature_pkt_tx(data, &pkt_tx);
           break;
       case COORDINATE_DATA_GET:
           rec = get_berfor_coord_pkt_tx(data, &pkt_tx);
           break;
           
       case SYS_HEART:
           rec = pp_sys_heart(data, &pkt_tx);
           break;
           
       case BATTRY_STAT:
           rec = target_battery_pkt_tx(data, &pkt_tx);
           break;
       default:

           return -2;

   }
   if(pp_packet_send() < 0)
    {
      printf("send data error  %d \n", type);
   }
   //printf("send data ok  %d \n", type);
   return 0;
}




int config_info_get(char* rx, tp_pp_info *sysInfo)
{
    unsigned short  crc;   
    tp_comm_data* pp_ack = (tp_comm_data* )rx;

    //chec rx info
    if(sysInfo->tg_number != ntohs(pp_ack->tg_number))
    {
        return -1;
    }

    memcpy(&pkt_tx, &pkt_rx, sizeof(tp_pp_packetInfo));
    pp_packet_send();
    return 0;
}

int config_id_proc(char* rx, tp_pp_info *sysInfo)
{
    int rec = 0;
    unsigned short  crc;   
    tp_comm_data* pp_ack = (tp_comm_data* )rx;
    UINT32 tg_number = 0;
    UINT32 tg_new_id = 0;
    char* cfg_file = "config.cfg";
    char value[64];

    
    tg_number = ntohs(pp_ack->tg_number);
    tg_new_id = ntohs(pp_ack->data);

    crc = CRC16_USB(rx, sizeof(tp_comm_data) - 2);

    
    //printf("config_id_proc id set crc  %d  crc_p %d \n", crc, (pp_ack->crc));
    if(crc != pp_ack->crc)
    {   // send crc error
        pp_ack->data =  0xffff;  // ack
        memcpy(&pkt_tx, rx, sizeof(tp_comm_data));
        pp_packet_send();
        return -1;
    }
    if(sysInfo->tg_number != tg_number)
    {
        pp_ack->data =  0xfffe;  // ack
        memcpy(&pkt_tx, rx, sizeof(tp_comm_data));
        pp_packet_send();
        return -2;
    }
    
    sysInfo->tg_number = tg_new_id;
    //get
    
    memset(value, 0, sizeof(value));
    sprintf(value,"%d", tg_new_id);    
    rec = write_conf_value(key[4], value,cfg_file);  //pp_number
    printf(" key %s   value %s rec %d\n",key[4], value, rec);

    //test  
    pp_ack->data =  0xffff;  // ack
    memcpy(&pkt_tx, rx, sizeof(tp_comm_data));
    pp_packet_send();
    return 0;

}

int hanshaking_info_proc(char* rx, unsigned int sip, tp_pp_info *sysInfo)
{
    unsigned short  crc; 
    unsigned short hanskAlm = HAND_ERROR_ALM;
    tp_pp_hansk* hansk = (tp_pp_hansk*)rx;

    crc = CRC16_USB(rx, sizeof(tp_pp_hansk) - 2);
    if(crc != hansk->crc)
    {   // send crc error
        
    }
    // 
    printf("hand  sip  %x \n", ntohl(sip));
   // print("hand sera:", hansk->seria);
   // print("pp   sera:", sys_pp_info.seria0);

    if(sys_pp_info.tg_number != ntohs(hansk->tg_number))
    {
        printf("hanshaking_info_proc error  tg_number tf: %d  rv :%d \n",
               sys_pp_info.tg_number ,ntohs(hansk->tg_number));
        return 0;
    }
    if(memcmp(hansk->seria, sys_pp_info.seria0, SERIA_DATA_LEN) != 0)
    { // error
    
         printf("hanshaking_info_proc error \n");
         pp_data_send_format(TARGET_ALARM, &hanskAlm);
         sys_pp_info.hand = False;
         return 0;
    }
    #if 1
    if(sip == server_ip)
    {
        sys_pp_info.ctrlMod = CTRL_MODE_M;
    }
    else
    {
        printf("hand  sip  %x ,change to sub mode \n", sip);
        sys_pp_info.ctrlMod = CTRL_MODE_S;
        server_address2.sin_addr.s_addr = htonl(sip);
    }
    #endif
    
    if(sys_pp_info.tgStatus == ERROR_TG)
    { // error
    
         printf("Position sensor abnormality \n");
         hanskAlm = SENSOR_ALARM;
         pp_data_send_format(TARGET_ALARM, &hanskAlm);
         //sys_pp_info.hand = False;
         //return 0;
    }
    if(sys_pp_info.alarmFlag & ALARM_FLG_TMPR)
    {
        hanskAlm = TEMPERATURE_ALARM;
        pp_data_send_format(TARGET_ALARM, &hanskAlm);
    }
    
    if(sys_pp_info.alarmFlag & ALARM_FLG_MOTO)
    {
        hanskAlm = HAND_MOTOR_ALM;
        pp_data_send_format(TARGET_ALARM, &hanskAlm);
    }
    // decode serial code --> two code
    

    // tow code check

    sys_pp_info.hand = True;
    
    return 0;
}

int pp_ack_info_proc(char* rx, tp_pp_info *sysInfo)
{
    unsigned short  crc;   
    tp_comm_data* pp_ack = (tp_comm_data* )rx;

    UINT32 tg_number = 0;
    UINT32 pp_number = 0;
    tg_number = ntohs(pp_ack->tg_number);
    pp_number = ntohs(pp_ack->data);
    
    printf("pp_ack_info_proc tg_number %d pp %d  tg-sys %d \n", tg_number, pp_number,sysInfo->tg_number);
    //chec rx info
    if(sysInfo->tg_number != tg_number)
    {
        return -1;
    }
    
    sysInfo->cdtInfo[pp_number].sendOk = 1;
    //sysInfo->cdtInfo[pp_number].valid = 1;

    return 0;
}


int pp_sw_contrl(char* rx, unsigned int sip, tp_pp_info *sysInfo)
{
    unsigned short  crc;       
    unsigned short  ctrlData;   
    int gpio;
    int state = 0;
    tp_comm_data* pp_light = (tp_comm_data* )rx;

    if(sys_pp_info.hand != True)
    {
        memset(&pp_light->data, 0xff, 2);
        memcpy(&pkt_tx, rx, sizeof(tp_comm_data));
        pp_packet_send();
        return 0;
    }
    
    if(sip == server_ip)
    {
        if(sys_pp_info.ctrlMod == CTRL_MODE_S)
        {
            return 0;
        }
    }
    else
    {
        if(sys_pp_info.ctrlMod == CTRL_MODE_M)
        {
            return 0;
        }
    }
    
    printf("sw ctl tg %d  ctrl %d \n", ntohs(pp_light->tg_number), ntohs(pp_light->data));
    //chec rx info 
    
    crc = CRC16_USB(rx, sizeof(tp_comm_data) - 2);

    
    printf("sw crc  %d  crc_p %d \n", crc, ntohs(pp_light->crc));
    if(crc != pp_light->crc)
    {   // send crc error
        
    }

    
    printf("sw ctl tg %d  tgsys %d   %d \n", ntohs(pp_light->tg_number), sysInfo->tg_number, ntohs(pp_light->data));
    ctrlData = ntohs(pp_light->data);
    if(ntohs(pp_light->tg_number) == sysInfo->tg_number)
    {
        
        gpio = open("/dev/gpio",O_RDWR);
        
        gpio_set_value(gpio,GPIO2_IO18_SW,1);  
        usleep(200000);
        gpio_set_value(gpio,GPIO2_IO18_SW,0);  
        close(gpio);
    }
    else
    {

    }
    
    memset(&pp_light->data, 0, 2);
    memcpy(&pkt_tx, rx, sizeof(tp_comm_data));
    pp_packet_send();
    return 0;
}

int pp_light_contrl(char* rx, unsigned int sip, tp_pp_info *sysInfo)
{
    unsigned short  crc;       
    unsigned short  ctrlData;   
    int gpio;
    int state = 0;
    tp_comm_data* pp_light = (tp_comm_data* )rx;

    if(sys_pp_info.hand != True)
    {
        memset(&pp_light->data, 0xff, 2);
        memcpy(&pkt_tx, rx, sizeof(tp_comm_data));
        
        printf("hand != True  light ctl tg %d  ctrl %d \n", ntohs(pp_light->tg_number), ntohs(pp_light->data));
        pp_packet_send();
        return 0;
    }
    
    if(sip == server_ip)
    {
        if(sys_pp_info.ctrlMod == CTRL_MODE_S)
        {
            return 0;
        }
    }
    else
    {
        if(sys_pp_info.ctrlMod == CTRL_MODE_M)
        {
            return 0;
        }
    }
    
    printf("light ctl tg %d  ctrl %d \n", ntohs(pp_light->tg_number), ntohs(pp_light->data));
    //chec rx info 
    
    crc = CRC16_USB(rx, sizeof(tp_comm_data) - 2);

    
    printf("light crc  %d  crc_p %d \n", crc, ntohs(pp_light->crc));
    if(crc != pp_light->crc)
    {   // send crc error
        
    }

    
    printf("light ctl tg %d  tgsys %d   %d \n", ntohs(pp_light->tg_number), sysInfo->tg_number, ntohs(pp_light->data));
    ctrlData = ntohs(pp_light->data);
    if(ntohs(pp_light->tg_number) == sysInfo->tg_number)
    {
        
        gpio = open("/dev/gpio",O_RDWR);
        if(ctrlData == 1)
        {
            state = 0;                      
        }
        if(ctrlData == 0)
        {
            state = 1;
        }

        
        gpio_set_value(gpio,GPIO2_IO19_LIGHT,state);  
        close(gpio);
    }
    else
    {

    }
    
    memset(&pp_light->data, 0, 2);
    memcpy(&pkt_tx, rx, sizeof(tp_comm_data));
    pp_packet_send();
    return 0;
}

int target_state_info_proc(tp_pp_packetInfo* rx, tp_pp_info *sysInfo)
{
    /*侧、正*/
    tp_status_get * st_get = (tp_status_get*)rx;


    if(ntohs(st_get->tg_number) != sysInfo->tg_number)
    {
        return -1;
    }
    pp_data_send_format(TARGET_STATE, st_get);
    return 0;
}

int target_run_info_proc(tp_pp_packetInfo* rx, unsigned int sip, tp_pp_info *sysInfo)
{
    /*run  stop*/
    int rv = 0;
    unsigned short  ctrlData;   
    UINT8 ctrl = FPGA_REG_RESET;
    tp_comm_data* runInfo = (tp_comm_data*)rx;

    int fd;   // test

    if(sys_pp_info.hand != True)
    {
        memset(&runInfo->data, 0xff, 2);
        memcpy(&pkt_tx, rx, sizeof(tp_comm_data));
        
        printf("hand != True  run tg %d  ctrl %d \n", ntohs(runInfo->tg_number), ntohs(runInfo->data));
        pp_packet_send();
        return 0;
    }

    if(sip == server_ip)
    {
        if(sys_pp_info.ctrlMod == CTRL_MODE_S)
        {
            return 0;
        }
    }
    else
    {
        if(sys_pp_info.ctrlMod == CTRL_MODE_M)
        {
            return 0;
        }
    }
    
    ctrlData = ntohs(runInfo->data);
        
    printf("target run status set  %d \n", ntohs(runInfo->data));
    if((runInfo == NULL)||(sysInfo == NULL))
    {
        runInfo->data = 0xffff;
        memcpy(&pkt_tx, rx, sizeof(tp_comm_data));
        pp_packet_send();
        return -1;
    }
    if(ctrlData == 2)//stop
    {
         sysInfo->run = 0;


         
         //test counters 
         fd = open("/dev/gpio",O_RDWR);
         counter_set_value(fd);
         close(fd);
         //

    }
    else if(ctrlData == 1)
    {    
        //弹点记录清零复位
        memset(sysInfo->cdtInfo, 0, 4094);
        sysInfo->cur_num = 0;
        
        rv = spi_write_proc(spi_addr[FPGA_REG_CTRL], 1, ctrl);
        if(rv != 0)
        {
             printf("spi write ctrl cmd error %d \n", rv);
        }
        sysInfo->run = 1;
    }
    else if(ctrlData == 16)
    {  // power off

    }
    else
    {

    }

    
    memset(&runInfo->data, 0, 2);
    memcpy(&pkt_tx, rx, sizeof(tp_comm_data));
    pp_packet_send();
    return 0;
}
int target_car_position_proc(tp_pp_packetInfo* rx, tp_pp_info *sysInfo)
{
    /*靶车运动控制*/


}

int get_coordinate_data_proc(char* rx, unsigned int sip, tp_pp_info *sysInfo)
{
    /*读取前n发弹点数据*/
    tp_pp_cdtInfo_get *cdtGet = (tp_pp_cdtInfo_get*)rx; 

    if(sys_pp_info.hand != True)
    {
        memset(&cdtGet->p_numb, 0xff, 2);
        memcpy(&pkt_tx, rx, sizeof(tp_comm_data));
        
        printf("hand != True  mode tg %d  ctrl %d \n", ntohs(cdtGet->tg_number), ntohs(cdtGet->p_numb));
        pp_packet_send();
        return 0;
    }

    
    if(ntohs(cdtGet->tg_number) != sysInfo->tg_number)
    {
    
         printf("get_coordinate_data_proc  %d   s %\n", ntohs(cdtGet->tg_number), sysInfo->tg_number);
       // return -1;
    }
    pp_data_send_format(COORDINATE_DATA_GET, cdtGet);


    printf("get_coordinate_data_proc %d  \n",  ntohs(cdtGet->p_numb));

    return 0;
}

int get_temprature_data_proc(tp_pp_packetInfo* rx, tp_pp_info *sysInfo)
{
    /*读取temprature数据*/
    tp_comm_data *tm_get = (tp_comm_data*)rx;
    if(tm_get->tg_number != sysInfo->tg_number)
    {
        return -1;
    }
    pp_data_send_format(TEMPERATURE_DATA, tm_get);

    return 0;
}

int set_target_mode_proc(tp_pp_packetInfo* rx, unsigned int sip, tp_pp_info *sysInfo)
{
    /*mode  */
    unsigned short  crc;   
    tp_mode_info* pp_mode = (tp_mode_info* )rx;

    UINT32 tg_number = 0;
    UINT32 mode = 0;
    tg_number = ntohs(pp_mode->tg_number);
    mode = ntohs(pp_mode->mode);
    
    printf("pp_ack_info_proc tpye %d tg_number %d pp mode %d  xb %d  yb %d  ts %d \n", 
          sysInfo->type, tg_number, mode, ntohs(pp_mode->xb),ntohs(pp_mode->yb),ntohs(pp_mode->times));

    if(sys_pp_info.hand != True)
    {
        memset(&pp_mode->mode, 0xff, 2);
        memcpy(&pkt_tx, rx, sizeof(tp_comm_data));
        
        printf("hand != True  mode tg %d  ctrl %d \n", ntohs(pp_mode->tg_number), ntohs(pp_mode->mode));
        pp_packet_send();
        return 0;
    }
    if(sip == server_ip)
    {
        if(sys_pp_info.ctrlMod == CTRL_MODE_S)
        {
            return 0;
        }
    }
    else
    {
        if(sys_pp_info.ctrlMod == CTRL_MODE_M)
        {
            return 0;
        }
    }

    //chec rx info
    if(sysInfo->tg_number != tg_number)
    {
        pp_mode->mode = 0xffff;
        memcpy(&pkt_tx, rx, sizeof(tp_mode_info));
        pp_packet_send();
        return -1;
    }
    if(sysInfo->type == TP_GD)
    {
        if(mode == 17)  //sigle mode
        {
            printf("hand  sip  %x ,change to sub mode \n", sip);
            sys_pp_info.ctrlMod = CTRL_MODE_S;
            server_address2.sin_addr.s_addr = htonl(sip);
        }
        else if(mode == 16)  // net mode
        {
            sys_pp_info.ctrlMod = CTRL_MODE_M;
        }
        else
            {}
    }
    else if(sysInfo->type == TP_CZ)
    {
        if(mode == GENERA)
        {
            sysInfo->tgMode.mode = mode;
        }
        else if(mode == FAST)
        {
            sysInfo->tgMode.mode = mode;
            sysInfo->tgMode.xb = ntohs(pp_mode->xb);
            sysInfo->tgMode.yb = ntohs(pp_mode->yb);
            sysInfo->tgMode.times = ntohs(pp_mode->times);
        }
        
        else if(mode == 17)  //sigle mode
        {
            printf("hand  sip  %x ,change to sub mode \n", sip);
            sys_pp_info.ctrlMod = CTRL_MODE_S;
            server_address2.sin_addr.s_addr = htonl(sip);
        }
        else if(mode == 16)  // net mode
        {
            sys_pp_info.ctrlMod = CTRL_MODE_M;
        }
        else
            {}
    }
    else if(sysInfo->type == TP_XG)
    {
        if(mode == GENERA)
        {
            sysInfo->tgMode.mode = mode;
            sysInfo->tgMode.interval = ntohs(pp_mode->interval);
        }
        else if(mode == FAST)
        {
            sysInfo->tgMode.mode = mode;
            sysInfo->tgMode.xb = ntohs(pp_mode->xb);
            sysInfo->tgMode.yb = ntohs(pp_mode->yb);
            sysInfo->tgMode.times = ntohs(pp_mode->times);
            sysInfo->tgMode.interval = ntohs(pp_mode->interval);
        }
        else if(mode == PPC)
        {
            sysInfo->tgMode.mode = mode;
            sysInfo->tgMode.xb = ntohs(pp_mode->xb);
            sysInfo->tgMode.yb = ntohs(pp_mode->yb);
            sysInfo->tgMode.times = ntohs(pp_mode->times);
            sysInfo->tgMode.interval = ntohs(pp_mode->interval);
            sysInfo->tgMode.stage = ntohs(pp_mode->stage);
        }
        
        else if(mode == 17)  //sigle mode
        {
            printf("hand  sip  %x ,change to sub mode \n", sip);
            sys_pp_info.ctrlMod = CTRL_MODE_S;
            server_address2.sin_addr.s_addr = htonl(sip);
        }
        else if(mode == 16)  // net mode
        {
            sys_pp_info.ctrlMod = CTRL_MODE_M;
        }
        else
            {}
    }
    else
        {}
    
    memset(&pp_mode->mode, 0, 2);
    memcpy(&pkt_tx, rx, sizeof(tp_mode_info));
    pp_packet_send();
    return 0;
}


int set_target_loct_proc(tp_pp_packetInfo* rx, unsigned int sip, tp_pp_info *sysInfo)
{
    int gpio;
    int state = 0;
    int run = 0;
    unsigned short  crc;   
    tp_comm_data* pp_mode = (tp_comm_data* )rx;

    UINT32 tg_number = 0;
    UINT32 mode = 0;
    tg_number = ntohs(pp_mode->tg_number);
    mode = ntohs(pp_mode->data);
    
    printf("set_target_loct_proc tg_number %d pp mode %d \n",tg_number, mode);
    
    if(sys_pp_info.hand != True)
    {
        memset(&pp_mode->data, 0xff, 2);
        memcpy(&pkt_tx, rx, sizeof(tp_comm_data));
        
        printf("hand != True  mode tg %d  ctrl %d \n", ntohs(pp_mode->tg_number), ntohs(pp_mode->data));
        pp_packet_send();
        return 0;
    }
    if(sip == server_ip)
    {
        if(sys_pp_info.ctrlMod == CTRL_MODE_S)
        {
            return 0;
        }
    }
    else
    {
        if(sys_pp_info.ctrlMod == CTRL_MODE_M)
        {
            return 0;
        }
    }

    //chec rx info
    if(sysInfo->tg_number != tg_number)
    {
        return -1;
    }

    sysInfo->tgMode.face = mode;

    
    memset(&pp_mode->data, 0, 2);
    memcpy(&pkt_tx, rx, sizeof(tp_comm_data));
    pp_packet_send();
    
    return 0;
#if 0    
    gpio = open("/dev/gpio",O_RDWR);

    state = gpio_get_value(gpio,GPIO4_IO25_SS0); 
    state |= (gpio_get_value(gpio,GPIO4_IO25_SS0) << 1);
    if(mode == P_TARGET) //zheng
    {

    }
    else if(mode == S_TARGET)  //ce
    {

    }
    else if(mode == C_TARGET)// f z
    {

    }
    else
    {
    }
    close(gpio);

    return 0;
#endif
}


int get_target_status_proc(tp_pp_packetInfo* rx, tp_pp_info *sysInfo)
{
    /*  读取靶标状态信息，运行状态开始／结束，　靶标正／侧，靶车位置 */


}
int target_network_proc(char* rx, tp_pp_info *sysInfo)
{
    /*  IP mac set */
    int rec = 0;
    char value[64];
    char* cfg_file = "config.cfg";
    UINT32 ipadd;
    UINT16 type = 0;
    tp_pp_netif* ifCfg = (tp_pp_netif*)rx;

    ipadd = ntohl(ifCfg->pp_ip);
    type = ntohl(ifCfg->if_type);
    //set ip mac
    //printf("sys if set  ip :%x   %x  sys_pp_info.ipadd %x  \n", ifCfg->pp_ip, ipadd, sys_pp_info.ipadd);
    
    printf("sys if set  mac:  %x:%x:%x:%x:%x:%x\n", ifCfg->mac[0],
        ifCfg->mac[1],ifCfg->mac[2],ifCfg->mac[3],ifCfg->mac[4],ifCfg->mac[5]);
    // write to cfg_file
    //strcpy(value, "192.168.3.7");
    
    memset(value, 0, sizeof(value));
    sprintf(value,"%d.%d.%d.%d", (ipadd >> 24)& 0xff, (ipadd >> 16)& 0xff,
                (ipadd >> 8)& 0xff, (ipadd)& 0xff);
    if(sys_pp_info.ipadd != ipadd)
    {
        rec = write_conf_value(key[0], value,cfg_file);
        printf(" key %s   value %s \n",key[0], value);
    }
    
    ipadd = ntohl(ifCfg->mian_ip);

    
    //printf("sys if set  ip :%x   %x  server_ip %x  \n", ifCfg->mian_ip, ipadd, server_ip);
    if(server_ip != ipadd)
    {
        memset(value, 0, sizeof(value));
        sprintf(value,"%d.%d.%d.%d", (ipadd >> 24)& 0xff, (ipadd >> 16)& 0xff,
                (ipadd >> 8)& 0xff, (ipadd)& 0xff);
        rec = write_conf_value(key[3], value,cfg_file);
        printf(" key %s   value %s \n",key[3], value);
    }    
    ipadd = ntohl(ifCfg->sub_ip);
    
    //printf("sys if set  ip :%x   %x  server2_ip %x  \n", ifCfg->sub_ip, ipadd, server2_ip);
    if(server2_ip != ipadd)
    {
        memset(value, 0, sizeof(value));
        sprintf(value,"%d.%d.%d.%d", (ipadd >> 24)& 0xff, (ipadd >> 16)& 0xff,
                (ipadd >> 8)& 0xff, (ipadd)& 0xff);
        rec = write_conf_value(key[8], value,cfg_file);
        printf(" key %s   value %s \n",key[8], value);
    }
    
   // memset(value, 0, sizeof(value));
    //strcpy(value, "00:00:12:11:00:11");
   // sprintf(value,"%02x:%02x:%02x:%02x:%02x:%02x", ifCfg->mac[0],
   //     ifCfg->mac[1],ifCfg->mac[2],ifCfg->mac[3],ifCfg->mac[4],ifCfg->mac[5]);    
   // rec = write_conf_value(key[1], value,cfg_file);
   // printf(" key %s   value %s \n",key[1], value);

    memset(&ifCfg->pp_ip, 0, sizeof(int));
    memcpy(&pkt_tx, rx, sizeof(tp_pp_netif));
    pp_packet_send();

    return 0;
}
int target_wifi_set(char* rx, tp_pp_info *sysInfo)
{
    /*  IP mac set */
    int rec = 0;
    char value[64];
    char* name = NULL;
    char* wifKey = NULL;
    char  keyLen = 0;
    char* cfg_file = "config.cfg";
    tp_pp_wifi* wifi = (tp_pp_wifi*)rx;

    printf(" target_wifi_set  %s %d   \n",key[5], wifi->lenth);


    if(ntohl(wifi->tg_number) != sysInfo->tg_number)
    {
        //check tg_number
    }
    name = rx + (sizeof(tp_pp_wifi));
    keyLen = *(name + wifi->lenth);
    wifKey = name + wifi->lenth + sizeof(char);

    if((wifi->lenth > WIFI_DATA_LEN)||(keyLen > WIFI_DATA_LEN))
    {

    }
    if((wifi->lenth <= 0 )||(keyLen <= 0))
    {

    }
    // write to cfg_file
    
    memset(value, 0, sizeof(value));
    name[wifi->lenth] = "\0";
    strncpy(value, name,wifi->lenth);

    
    //printf(" key %s   value %s \n",key[5], value);
    rec = write_conf_value(key[5], value,cfg_file);
    printf(" key %s   value %s \n",key[5], value);

    memset(value, 0, sizeof(value));
    wifKey[keyLen] = "\0";
    strncpy(value, wifKey, keyLen);

    
   // printf(" key %s   value %s  %d \n",key[6], value, keyLen);
    rec = write_conf_value(key[6], value,cfg_file);
    printf(" key %s   value %s \n",key[6], value);
    
    wifi->lenth =  0;  // ack
    memset(&wifi->lenth, 0, wifi->lenth);
    memcpy(&pkt_tx, rx, sizeof(tp_pp_wifi));
    pp_packet_send();

    return 0;
}

int test_proc(tp_pp_packetInfo* rx, tp_pp_info *sysInfo)
{
    char* test_file = "test.txt";
    char  value[32] = {0};
    int   data[3] = {0};
    char  key[32] = {0};
    int i = 0, ji = 0;
    int rec = 0;
    double deltaT1 = 0;
    double deltaT2 = 0;
    double x = 0;
    double y = 0;
    
    int gpio;
    int s0, s1, s2,s3 ,s4;
    

    gpio = open("/dev/gpio",O_RDWR);
    s0 = gpio_get_value(gpio,GPIO4_IO25_SS0);  
    s1 = gpio_get_value(gpio,GPIO4_IO26_SS1); 
    s2 = gpio_get_value(gpio,GPIO4_IO27_SS2);  
    s3 = gpio_get_value(gpio,GPIO1_IO03_BATT2); 
    s4 = gpio_get_value(gpio,GPIO1_IO04_BATT3); 
    
    close(gpio);
    printf("test_proc  %d  %d  %d   %d  %d\n",s0, s1, s2,s3 ,s4);
  //  sysInfo->run = 1;

    
  //  sysInfo->tgMode.mode = FAST;
 //   sysInfo->tgMode.xb = 5;
 //   sysInfo->tgMode.yb = 4;
 //   sysInfo->tgMode.times = 3;
 //   sysInfo->tgMode.face = P_TARGET;

    return 0;

    if(rx->data.commData[0] == 0x1010)//stop
    {
        for(i = 1; i < rx->data.commData[1]; i++)
        {
            for(ji = 1; ji <3; ji++)
            {
                sprintf(key, "data%d%d", i,ji);
                rec = read_conf_value(key, value,test_file);
                data[ji] = atoi(value);
                printf("read data %d %d  value %s  %d \n", i, ji, value, data[ji]);
            }
            deltaT1 = (double)(data[1] - data[0]);
            deltaT2 = (double)(data[1] - data[2]);
            printf("pp xxxxx  delta1 %f delta2 %f \n", deltaT1, deltaT2);
            deltaT1 = deltaT1/US2S;
            deltaT2 = deltaT2/US2S;
            //caculateCoordinate(25, deltaT1, deltaT2, &x, &y);
            printf("pp %d   x = %f  y = %f  delta1 %f delta2 %f \n", i, x, y, deltaT1, deltaT2);
        }

       
    }
    if(rx->data.commData[0] == 0x3030)//stop
    {
       pp_data_read_proc();
    }
    if(rx->data.commData[0] == 0x0000)
    {
        pp_data_read_proc();
    }
    //if(ntohs(rx->tg_number) == 0x0001)
    {
        printf("uart  test---io");
        gpio = open("/dev/gpio",O_RDWR);
        if(gpio < 0){
            perror("open");
        }
        gpio_set_value(gpio,7,1);  //sd1-cmd  gpio2.16
        usleep(15000);
        gpio_set_value(gpio,7,0);  //sd1-cmd  gpio2.16
        usleep(5000);
        gpio_set_value(gpio,7,1);  //sd1-cmd  gpio2.16
        close(gpio);
    }
    return 0;
}

int w1_fd_file_find(char *w1_file)
{
   UINT8  flg = 0;
   UCHAR  file[128] = {0};
   DIR * dp = NULL;
   struct dirent *filename = NULL;
   INT32  size = 0;
   
   memset(file, 0, sizeof(file));
   sprintf(file, "%s", W1_PATH);
   
   dp = opendir(file);
   if (!dp)
   {       
       printf("open directory error\n");       
       return flg;
   }

   while(filename=readdir(dp))
   {       
      // printf("filename:%-10s\td_info:%ld\t d_reclen:%us\n",         
      //     filename->d_name,filename->d_ino,filename->d_reclen); 
       size = strlen(filename->d_name);
       if(size > strlen(W1_FILE))
       {
            if(strncmp(filename->d_name, W1_FILE, strlen(W1_FILE)) == 0)
            {
                flg++;
                break;
            }
       }
       
   }
   closedir(dp);

   if(flg == 0)
   {
       return -1;
   }
   sprintf(w1_file, "%s%s%s", W1_PATH, filename->d_name, W1_SLAVE);
   
   printf("csd check %s    %s   end flg %d \n",w1_file, filename->d_name,  flg);
   return flg;   
}

#define SEC_TO_USEC 1000000
void tp_run_times(unsigned short time)
{
    struct timeval tt1;
    struct timeval tt2;
    struct timeval tt3;
    int  tmp = 0;
        
    gettimeofday(&tt1, NULL);
    while(1)
    {
         usleep(50000);
         gettimeofday(&tt2, NULL);

         tt3.tv_sec = tt2.tv_sec - tt1.tv_sec;
         tt3.tv_usec = tt2.tv_usec - tt1.tv_usec;
         tmp = (tt3.tv_sec*SEC_TO_USEC) + tt3.tv_usec;
         if(tmp >= time * SEC_TO_USEC)
         {
              return;
         }
    }
    
    return;
}

int pp_packet_proc(char* buffer, UINT32 sip, tp_pp_info *sysInfo)
{
     int rec = 0;
     memcpy(&pkt_rx, buffer,sizeof(pkt_rx));
     memset(&pkt_tx, 0, sizeof(pkt_tx));

     UINT16 type;
     int timeout = 0;
     unsigned short hanskAlm = 0;

     while(motorRun == 1)
     {
          usleep(10000);
          timeout++;
          if(timeout >60)
          {
          
              printf("motor run timeout msg no process\n");
              hanskAlm = SENSOR_ALARM;
              pp_data_send_format(TARGET_ALARM, &hanskAlm);
              return -2;
          }
     }

     type = ntohs(pkt_rx.type);
     if(dbg_flg)
     {
         printf("pp rcv packet %x    %x\n", pkt_rx.type, type);
     }
     
     switch(type)
     {
        case CONFIG_INFO_GET:
            rec = config_info_get(&pkt_rx, sysInfo);
            break;
        case CONFIG_TG_ID:
            rec = config_id_proc(&pkt_rx, sysInfo);
            break;

        case HANDSHAKING_DATA:
            rec = hanshaking_info_proc(&pkt_rx, sip, sysInfo);
            break;
        case TARGET_STATE:
            rec = target_state_info_proc(&pkt_rx, sysInfo);
            break;
        case TARGET_RUN_SET:
            rec = target_run_info_proc(&pkt_rx, sip, sysInfo);
            break;
        case TARGET_CAR_POSITION_SET:
            rec = target_car_position_proc(&pkt_rx, sysInfo);
            break;
        
        case COORDINATE_DATA_GET:
            rec = get_coordinate_data_proc(&pkt_rx, sip, sysInfo);
            break;
        case TEMPERATURE_DATA:
            rec = get_temprature_data_proc(&pkt_rx, sysInfo);
            break;
        case TARGET_MODE:
            rec = set_target_mode_proc(&pkt_rx, sip, sysInfo);
            break;

        case TARGET_LOCT:
            rec = set_target_loct_proc(&pkt_rx, sip, sysInfo);
            break;
            
        case TARGET_IF:
            rec = target_network_proc(&pkt_rx, sysInfo);
            break;
        case SEND_ACK:
            rec = pp_ack_info_proc(&pkt_rx, sysInfo);
            break;

        case LIGHT_CTL:
            rec = pp_light_contrl(&pkt_rx, sip, sysInfo);
            break;
        case POWR_SW_CTL:
            rec = pp_sw_contrl(&pkt_rx, sip, sysInfo);
            break;
            
        case PP_WLAN_SET:
            rec = target_wifi_set(&pkt_rx, sysInfo);
            break;

        case PP_TEST_TYPE:
            rec = test_proc(&pkt_rx, sysInfo);
            break;
        default:
            
            break;
           // return -1;
     }

     //debg
     //pp_packet_send();     

     return 0;

}

#endif

