#include "power_modbus.h"


#pragma warning(disable:4786 4305 4101 4800)

Crc*     modbus_functions::crc= new Crc(16,0X8005, 1,0XFFFF,0);//初始化校验

FBYTE4   modbus_functions::pAnalog_Value[MAX_DEV_NUM*MAX_ANALOG_PER_DEV];

FBYTE4   modbus_functions::pPara_Value[MAX_DEV_NUM*MAX_PARA_PER_DEV];

UBYTE1   modbus_functions::pStatus_Value[MAX_DEV_NUM*MAX_STATUS_PER_DEV];

UBYTE4   modbus_functions::pTime[MAX_DEV_NUM];

UBYTE1   modbus_functions::dev_state[MAX_DEV_NUM];

map<int,int> modbus_functions::id_to_status_sub;

map<int,int> modbus_functions::id_to_analog_sub;

map<int,int> modbus_functions::id_to_para_sub;

map<int,int> modbus_functions::control_to_analog_sub;

int   modbus_functions::status_id[MAX_STATUS_PER_DEV];
int   modbus_functions::analog_id[MAX_ANALOG_PER_DEV];
int   modbus_functions::para_id[MAX_PARA_PER_DEV];


BYTE4  modbus_functions::to_int(UBYTE1* buff)
{
	BYTE4  intval = 0; 
	UBYTE1 *p =(UBYTE1*)&intval;
	*p     = buff[1];
	*(p+1) = buff[0]; 
	*(p+2) = buff[3];
	*(p+3) = buff[2];
	return intval;
}
int  modbus_functions::to_buff(BYTE4 intval,UBYTE1* buff)
{
	UBYTE1* p = (UBYTE1*)&intval;
	buff[1] = *(p);
	buff[0] = *(p+1);
	buff[3] = *(p+2);
	buff[2] = *(p+3);
	
	return 4;
}
//双字高位在前，四字节权重[1 0 3 2]
FBYTE4 modbus_functions::to_float(UBYTE1* buff)
{
	FBYTE4  fval = 0; 
	UBYTE1 *p =(UBYTE1*)&fval;
	*(p+0) = buff[3];
	*(p+1) = buff[2];
	*(p+2) = buff[1];	
	*(p+3) = buff[0]; 
	return fval;
}

int  modbus_functions::to_buff(FBYTE4 fval,UBYTE1* buff)
{
	UBYTE1* p = (UBYTE1*)&fval;
	buff[3] = *(p+0);
	buff[2] = *(p+1);
	buff[1] = *(p+2);
	buff[0] = *(p+3);

	return 4;
}
//单字 高位在前，低位在后[1 0]
UBYTE2 modbus_functions::to_word(UBYTE1* buff)
{
	UBYTE2   wval = buff[0]*256 + buff[1];
	return wval;
}
//单字 高位在前，低位在后
int  modbus_functions::to_buff(UBYTE2 wval,UBYTE1* buff)
{
	buff[1] = wval%256;
	wval    = wval/256;
	buff[0] = wval%256;
	return 2;
}

int modbus_functions::status_to_buff(UBYTE2 wval,UBYTE1* buff)
{	
	buff[0] = wval%256;//低8位遥信在前
	wval    = wval/256;
	buff[1] = wval%256;//高8位遥信在后
	return 2;
}

bool modbus_functions::buff_to_status(UBYTE1* buff,int bit)
{	
	UBYTE1 tmp = 1;
	UBYTE1 val = 0;
	if (bit>8)
	{
		val = buff[1];
		bit -= 8;
	}else
	{
		val = buff[0];
	}

	while(bit>0)
	{
		tmp= (tmp<<1);
		bit-- ;
	}
	return (tmp & val);
}



int modbus_functions::to_buff(tm& curtime,UBYTE1* buff)
{

	buff[0] = curtime.tm_sec;
	buff[1] = curtime.tm_min;
	buff[2] = curtime.tm_hour;
	buff[3] = curtime.tm_wday;
	buff[4] = curtime.tm_mday;
	buff[5] = curtime.tm_mon + 1;//1月从0开始
	buff[6] = curtime.tm_year%100;
	return 7;
}

time_t modbus_functions::to_time(UBYTE1*buff)
{
	tm curtime;
	curtime.tm_year  = buff[6]+ 100;//年
	curtime.tm_mon   = buff[5]- 1;  //1月从0
	curtime.tm_mday  = buff[4];       //日
	curtime.tm_wday  = buff[3];       //日
	curtime.tm_hour  = buff[2];       //时
	curtime.tm_min   = buff[1];       //分
	curtime.tm_sec   = buff[0];       //秒
	time_t t = mktime(&curtime);
	return t;
}

UBYTE1 modbus_functions::BCD_TO_HEX(UBYTE1 bcdByte)
{
	UBYTE1 dec=0,sig=0;
	dec = ((bcdByte>>4)&1) * 10;
	sig = (bcdByte & 0x0f);
	return (sig + dec);
}

UBYTE1 modbus_functions::HEX_TO_BCD(UBYTE1 btHex)
{
	UBYTE1 dec=0,sig=0;
	dec = btHex/10;
	sig = btHex%10;
	return (sig + (dec<<4));
}
UBYTE1 modbus_functions::BCD_TO_HEX(UBYTE1* bcdByte,UBYTE1 nLen)
{
	UBYTE1 i = 0;
	for (i=0;i<nLen;i++)
	{
		bcdByte[i] = BCD_TO_HEX(bcdByte[i]);
	}
	return nLen;
}

UBYTE1 modbus_functions::HEX_TO_BCD(UBYTE1 *btHex,UBYTE1 nLen)
{
	UBYTE1 i = 0;
	for (i=0;i<nLen;i++)
	{
		btHex[i] = HEX_TO_BCD(btHex[i]);
	}
	return nLen;
}


int modbus_functions::set_crc(UBYTE1* pAnswer,int len)
{
	UBYTE2 ret=crc->getcrc((const char*)pAnswer,len);
	pAnswer[len++] = ret%256;
	pAnswer[len++] = ret/256;
	return 2;
}
/*读取定值*/
int modbus_functions::onReadPara(UBYTE1 *pAsk,UBYTE1* pAnswer)
{
	int    pos  = 0;
	pAnswer[POS_DEV_ID] = pAsk[POS_DEV_ID];
	pAnswer[POS_FC]     = pAsk[POS_FC];
	pAnswer[POS_BYTES]  = 0x08;
	pos                 = POS_BYTES + 1;

	UBYTE2 register_addr = to_word(pAsk+POS_FC+1);
	int sub = get_para_sub(pAsk[POS_DEV_ID],register_addr);
	float para_val = pPara_Value[sub];
	float time_val = pPara_Value[sub + 1];

	pos +=to_buff(para_val,pAnswer+pos);
	pos +=to_buff(time_val,pAnswer+pos);
	pos +=set_crc(pAnswer,pos);
	return pos;
}

/*读取定值*/
int modbus_functions::onReadPara(UBYTE1 *pAsk,UBYTE1* pAnswer,FBYTE4 val1,FBYTE4 val2)
{
	int    pos  = 0;
	pAnswer[POS_DEV_ID] = pAsk[POS_DEV_ID];
	pAnswer[POS_FC]     = pAsk[POS_FC];
	pAnswer[POS_BYTES]  = 0x08;
	pos                 = POS_BYTES + 1;
	pos +=to_buff(val1,pAnswer+pos);
	pos +=to_buff(val2,pAnswer+pos);
	pos +=set_crc(pAnswer,pos);
	return pos;
}

int modbus_functions::toReadPara(UBYTE1 *pAsk,UBYTE2 devID,UBYTE2 register_addr)
{
	int    pos         = 0;
	pos               += to_buff(devID, pAsk);//装置编号                 
	pAsk[pos] = FC_READ_PARA;           //功能码
	pos++;

	pos               +=to_buff(register_addr,pAsk + pos);  //寄存器地址

	UBYTE2 register_num = 2;             //寄存器个数
	pos        += to_buff(register_num,pAsk + pos);

	pAsk[pos] = (register_num*2)%256;//字节数
	pos++;

	pos        +=set_crc(pAsk,pos);
	return pos;
}
//设置整型参数
int modbus_functions::toSetPara(UBYTE1 *pAsk,UBYTE2 devID,UBYTE2 paraId,BYTE4 paraValue)
{
	int    pos         = 0;
	pos += to_buff(devID, pAsk);//装置编号                  //1+1byte 装置编号
    pAsk[POS_FC]       = FC_SET_PARA;              //2+1byte 功能码
	to_buff(paraId,pAsk + POS_REGISTER);           //3+2byte 寄存器地址即参数ID
	UBYTE2 register_num      = 2;
	to_buff(register_num,pAsk + POS_REGISTER_NUM);  //5+2    寄存器个数
	pAsk[POS_SET_PARA_BYTES] = (register_num*2)%256;//7+1    读取字节数
	pos = POS_SET_PARA_BYTES+1;
	pos +=to_buff(paraValue,pAsk+pos);               //8 + 4  整型参数值
	pos +=set_crc(pAsk,pos);                         //12+ 2  校验码(共12字节)
	return pos;
}

int modbus_functions::toSetPara(UBYTE1 *pAsk,UBYTE2 devID,UBYTE2 register_addr, FBYTE4 val1,FBYTE4 val2)
{
	int    pos         = 0;
	pos += to_buff(devID, pAsk);//装置编号  
    pAsk[pos]       = FC_SET_PARA;          //功能码
	pos++;

	pos += to_buff(register_addr,pAsk + pos);//寄存器地址
	UBYTE2 register_num      = 4;
	to_buff(register_num,pAsk + POS_REGISTER_NUM);//寄存器个数×2 = 8个字节
	pAsk[POS_SET_PARA_BYTES] = (register_num*2)%256;
	pos = POS_SET_PARA_BYTES+1;
	pos +=to_buff(val1,pAsk+pos);//浮点参数1
	pos +=to_buff(val2,pAsk+pos);//浮点参数2
	pos +=set_crc(pAsk,pos);     //校验码
	return pos;
}

int modbus_functions::onSetPara(UBYTE1 *pAsk,UBYTE1* pAnswer,FBYTE4* val1,FBYTE4* val2)
{
	int    pos                 = 0;
	pAnswer[POS_DEV_ID]        = pAsk[POS_DEV_ID];
	pAnswer[POS_FC]            = pAsk[POS_FC];
	pAnswer[POS_REGISTER]      = pAsk[POS_REGISTER];
	pAnswer[POS_REGISTER+1]    = pAsk[POS_REGISTER+1];
	pAnswer[POS_REGISTER_NUM]  = pAsk[POS_REGISTER_NUM];
	pAnswer[POS_REGISTER_NUM+1]= pAsk[POS_REGISTER_NUM+1];
	pos = POS_SET_PARA_BYTES + 1;
	//----------------------------
	*val1    = to_float(pAsk + pos);
	*val2    = to_float(pAsk + pos + 4);
	//----------------------------
	pos = POS_REGISTER_NUM + 2;
	pos +=set_crc(pAnswer,pos);
	return pos;
}
int modbus_functions::toExitParaSet(UBYTE1 *pAsk,UBYTE2 devId)
{
	int    pos         = 0;
	pAsk[POS_DEV_ID]   = devId;                    //1+1byte 装置编号
    pAsk[POS_FC]       = FC_EXIT_SET;              //2+1byte 功能码
	UBYTE2 register_addr = REGISTER_PARA_EXIT;
	to_buff(register_addr,pAsk + POS_REGISTER);    //3-4 byte 寄存器地址即参数ID
	pos = POS_REGISTER + 2;
	pos +=set_crc(pAsk,pos);                       //5-6  校验码(共6字节)
	return pos;
}
int modbus_functions::onSetPara(UBYTE1 *pAsk,UBYTE1* pAnswer)
{
	int    pos                 = 0;
	pAnswer[POS_DEV_ID]        = pAsk[POS_DEV_ID]; //装置编号 1字节
	pAnswer[POS_FC]            = pAsk[POS_FC];     //功能码   1字节
	pAnswer[POS_REGISTER]      = pAsk[POS_REGISTER];//寄存器  起始地址2字节
	pAnswer[POS_REGISTER+1]    = pAsk[POS_REGISTER+1];
	pAnswer[POS_REGISTER_NUM]  = pAsk[POS_REGISTER_NUM]; //寄存器个数 2字节
	pAnswer[POS_REGISTER_NUM+1]= pAsk[POS_REGISTER_NUM+1];
	
	pos = POS_SET_PARA_BYTES + 1;
    //----------------------------
	UBYTE2 register_addr = to_word(pAsk+POS_FC+1);
	int sub = get_para_sub(pAsk[POS_DEV_ID],register_addr);
	pPara_Value[sub]    = to_float(pAnswer + pos);
	pPara_Value[sub + 1]= to_float(pAnswer + pos + 4);
    //----------------------------
	pos = POS_REGISTER_NUM + 2;
	pos +=set_crc(pAnswer,pos);
	return pos;
}

int modbus_functions::toReadCoil(UBYTE1 *pAsk, UBYTE2 devID, UBYTE2 register_addr)
{
   return 0;
}
int modbus_functions::onReadCoil(UBYTE1 *pAsk,UBYTE1* pAnswer)
{
   return 0;
}

int modbus_functions::toWriteCoil(UBYTE1 *pAsk,UBYTE2 devID,UBYTE2 register_addr)
{
	return 0;
}
int modbus_functions::onWriteCoil(UBYTE1 *pAsk,UBYTE1* pAnswer)
{
	return 0;
}

//读取状态量

int modbus_functions::toReadStatus(UBYTE1 *pAsk,UBYTE2 devID,UBYTE2 register_addr)
{
	int    pos         = 0;
	pAsk[POS_DEV_ID]   = devID;
    pAsk[POS_FC]       = FC_READ_STATUS;
	to_buff(register_addr,pAsk + POS_REGISTER);
	UBYTE2 register_num = REGISTER_NUM;	
	pos = POS_REGISTER_NUM ;
	pos +=to_buff(register_num,pAsk + pos);
	pos +=set_crc(pAsk,pos);
	return pos;
}
int modbus_functions::onReadStatus(UBYTE1 *pAsk,UBYTE1* pAnswer)
{
	int    pos                 = 0;
	pAnswer[POS_DEV_ID]        = pAsk[POS_DEV_ID];
	pAnswer[POS_FC]            = pAsk[POS_FC];
	UBYTE2 register_addr = to_word(pAsk + POS_FC + 1);
	UBYTE2 register_num  = to_word(pAsk + POS_FC + 3);

	pos +=set_crc(pAnswer,pos);
	return pos;
}
//读取遥测
int modbus_functions::toReadAnalog(UBYTE1 *pAsk,UBYTE2 devID,UBYTE2 register_addr,UBYTE2 register_num)
{
	int    pos              = 0;
	pos += to_buff(devID,pAsk);//0-  装置地址       
	pAsk[pos] = FC_READ_ANALOG;//1-2  功能码
	pos ++;

	pos += to_buff(register_addr,pAsk + pos);//2-3 寄存器地址
	pos += to_buff(register_num ,pAsk + pos);//4-5 寄存器数目
	pos += set_crc(pAsk,pos);                //6-7 校验码
	return pos;
}
int modbus_functions::onReadAnalog(UBYTE1 *pAsk,UBYTE1* pAnswer)
{
	int    pos                 = 0;
	return pos;
}
//响应遥控
int modbus_functions::toTeleControl(UBYTE1 *pAsk,UBYTE2 devID,UBYTE2 register_addr)
{
	int    pos              = 0;
	pAsk[POS_DEV_ID]        = devID;            //装置编号
	pAsk[POS_FC]            = FC_TELE_CONTROL;  //功能码
	pos = POS_FC + 1;
	
	if(register_addr == 0x01)
	{	
    	pos += to_buff((UBYTE2)STATUS_BASE,pAsk + pos);   //两字节控合地址
		//pos += to_buff((UBYTE2)CON_CB_CLOSE_VALUE,pAsk  + pos);         //两字节遥控合值
	}else if(register_addr == 0x02)
	{
    	pos += to_buff((UBYTE2)STATUS_BASE,pAsk + pos);   //两字节控分地址
		//pos += to_buff((UBYTE2)CON_CB_OPEN_VALUE,pAsk  + pos);         //两字节遥控分值
	}else
	{
		return 0;
	}
	pos += set_crc(pAsk,pos);
	return pos;
}
int modbus_functions::onTeleControl(UBYTE1 *pAsk,UBYTE1* pAnswer)
{
	int    pos                 = 0;
	pAnswer[POS_DEV_ID]        = pAsk[POS_DEV_ID];
	pAnswer[POS_FC]            = pAsk[POS_FC];
	UBYTE2 control_addr        = to_word(pAsk + POS_FC + 1);
	UBYTE2 control_val         = to_word(pAsk + POS_FC + 3);
	pos  = POS_FC + 1;
    pos += to_buff(control_addr,pAnswer+pos);
	pos += to_buff(control_val,pAnswer+pos);
	pAnswer[pos] = 1;
    pos ++;
	pos +=set_crc(pAnswer,pos);
	return pos;
}
//读取设备时间
int modbus_functions::toReadTime(UBYTE1 *pAsk,UBYTE2 devId)
{
	int pos          = 0;
	pAsk[POS_DEV_ID] = devId%256;
	pAsk[POS_FC]     = FC_READ_TIME;
	UBYTE2 register_addr  = REGISTER_PARA_TIME;
	pos  = POS_FC + 1;
	pos += to_buff(register_addr,pAsk + pos);
	UBYTE2 register_num   = 3;
	pos += to_buff(register_num,pAsk + pos);
	pos += set_crc(pAsk,pos);
	return pos;
}
//读取装置时间
int modbus_functions::onReadTime(UBYTE1 *pAsk,UBYTE1* pAnswer)
{
	int pos = 0;
	pAnswer[POS_DEV_ID] = pAsk[POS_DEV_ID];
	pAnswer[POS_FC]     = pAsk[POS_FC];
	pAnswer[POS_FC + 1] = pAsk[POS_FC + 1];
	pAnswer[POS_FC + 2] = pAsk[POS_FC + 2];

	pos = POS_FC + 3;
	pos += to_buff(to_word(pAsk+pos),pAnswer + pos);
	pos += to_buff(to_word(pAsk+pos),pAnswer + pos);
	pos += to_buff(to_word(pAsk+pos),pAnswer + pos);

	time_t tNow = time(NULL);
	tm local_tm =*localtime(&tNow);
	pos += to_buff(local_tm,pAnswer + pos);
	pos +=set_crc(pAnswer,pos);
	return pos;
}
//自动校准
int modbus_functions::toAutoAdjust(UBYTE1 *pAsk,UBYTE2 devId,UBYTE2 paraId)
{
	int pos = 0;
	pAsk[POS_DEV_ID] = devId%256;
	pAsk[POS_FC]     = FC_ADJUST_PARA;
	UBYTE2 register_addr  = paraId;
	pos  = POS_FC + 1;
	pos += to_buff(register_addr,pAsk + pos);
	pos  = POS_REGISTER + 2;
	pos += set_crc(pAsk,pos);
	return pos;	
}
//对时
int modbus_functions::toAdjustTime(UBYTE1 *pAsk,UBYTE2 devId)
{
	int pos = 0;
	pAsk[POS_DEV_ID] = devId%256;
	pAsk[POS_FC]     = FC_SET_TIME;
	UBYTE2 register_addr  = REGISTER_PARA_TIME;
	pos  = POS_FC + 1;
	pos += to_buff(register_addr,pAsk + pos);
	pAsk[pos] = 0x07;
	pos       = POS_TIME_SET;

	time_t tNow = time(NULL);
	tm local_tm =*localtime(&tNow);
	pos += to_buff(local_tm,pAsk + pos);
	pos += set_crc(pAsk,pos);
	return pos;
}
//对时响应
int modbus_functions::onAdjustTime(UBYTE1 *pAsk,UBYTE1* pAnswer)
{
	int pos = 0;
	pAnswer[POS_DEV_ID] = 	pAsk[POS_DEV_ID];
	pAnswer[POS_FC]     = 	pAsk[POS_FC]    ;  
	pAnswer[POS_FC + 1] = 	pAsk[POS_FC + 1];
	pAnswer[POS_FC + 2] = 	pAsk[POS_FC + 2];
	pos = POS_FC + 3;
	pos += to_buff(to_word(pAsk+pos),pAnswer + pos);
	pos += to_buff(to_word(pAsk+pos),pAnswer + pos);
	pos += to_buff(to_word(pAsk+pos),pAnswer + pos);
	
	time_t t = to_time(pAsk + pos);
	tm local_tm_set =*localtime(&t);
	time_t tNow = time(NULL);
	tm local_tm =*localtime(&tNow);
	pos += to_buff(local_tm,pAnswer + pos);
	pos += set_crc(pAnswer,pos);
	return pos;
}
//读取时间记录
int modbus_functions::toReadEvent(UBYTE1 *pAsk,UBYTE2 devId)
{
	int pos = 0;
	pAsk[POS_DEV_ID] = devId%256;
	pAsk[POS_FC]     = FC_READ_EVENT;
	pAsk[POS_FC + 1] = 0x07;
	pAsk[POS_FC + 2] = 0x06;
	UBYTE2 fileNo = 0x0007;
	pos = POS_FC + 3;
	pos += to_buff(fileNo,pAsk + pos);
	UBYTE2 register_addr = 0x0000;
	pos += to_buff(register_addr,pAsk + pos);
	UBYTE2 register_num = 0x0004;
	pos += to_buff(register_num,pAsk + pos);
	pos +=set_crc(pAsk,pos);
	return pos;
}

int modbus_functions::onReadEvent(UBYTE1 *pAsk,UBYTE1* pAnswer)
{
	int pos = 0;
	pAnswer[POS_DEV_ID] = 	pAsk[POS_DEV_ID];
	pAnswer[POS_FC]     = 	pAsk[POS_FC]    ;  
	pAnswer[POS_FC + 1] = 	pAsk[POS_FC + 1];
	pAnswer[POS_FC + 2] = 	pAsk[POS_FC + 2];
	pos = POS_FC + 3;
	pos += to_buff(to_word(pAsk+pos),pAnswer + pos);
	pos += to_buff(to_word(pAsk+pos),pAnswer + pos);
	pos += to_buff(to_word(pAsk+pos),pAnswer + pos);
	
	time_t t = to_time(pAsk + pos);
	tm local_tm_set =*localtime(&t);
	tm local_tm_cur =*localtime(NULL);
	pos += to_buff(local_tm_cur,pAnswer + pos);
	pos += set_crc(pAnswer,pos);
	return pos;
}

int modbus_functions::get_status_sub(UBYTE2 devId,int register_addr)
{
    int ret = devId*MAX_STATUS_PER_DEV + id_to_status_sub[register_addr];
	return ret;
}

int modbus_functions::get_analog_sub(UBYTE2 devId, int register_addr)
{
	int ret = devId*MAX_ANALOG_PER_DEV + id_to_analog_sub[register_addr];
	return ret;
}

int modbus_functions::get_para_sub(UBYTE2 devId, int register_addr)
{
	int ret = devId*MAX_PARA_PER_DEV + id_to_para_sub[register_addr];
	return ret;
}
//初始化
int modbus_functions::modbus_init()
{
	memset(pAnalog_Value,0,sizeof(FBYTE4)*MAX_DEV_NUM*MAX_ANALOG_PER_DEV);
	memset(pPara_Value,0,sizeof(FBYTE4)*MAX_DEV_NUM*MAX_PARA_PER_DEV);
	memset(pStatus_Value,0,sizeof(UBYTE1)*MAX_DEV_NUM*MAX_STATUS_PER_DEV);
	memset(pTime,0,sizeof(time_t)*MAX_DEV_NUM);
	memset(dev_state,0,sizeof(UBYTE1)*MAX_DEV_NUM);
	memset(status_id,0,sizeof(int)*MAX_STATUS_PER_DEV);
	memset(analog_id,0,sizeof(int)*MAX_ANALOG_PER_DEV);
	memset(para_id,  0,sizeof(int)*MAX_PARA_PER_DEV);
	int i = 0,sub = 0;
	
	//状态量

	return 0;
}

int    modbus_functions::parsebuffer(UBYTE1* buff,int register_addr,int register_num,list<modbus_item>& lstItems)
{
	int start    = POS_BYTES + 1;
	int item_num = register_num / 2;
	for (int i = 0;i<item_num;i++)
	{
		modbus_item item;
		item.id    = register_addr + i*4;
		item.value = to_float(buff + item.id);
		lstItems.push_back(item);
	}
	
	return 0;
}

int modbus_functions::toSetParas(UBYTE1* pAsk,tDEVPARAM& devParam)
{
	int pos              = 0;
	pAsk[POS_DEV_ID]      = (UBYTE1)devParam.devAddr;
	pAsk[POS_FC]          = FC_SET_PARAS;
	UBYTE2 register_addr  = REGISTER_PARAS_ALL;
	pos          = POS_FC + 1;
	pos         += to_buff(register_addr,pAsk + pos);
	size_t nSize = sizeof(tDEVPARAM);
	memcpy(pAsk + pos,&devParam,nSize);
	pos +=nSize;
	pos += set_crc(pAsk,pos);
	return pos;
}
int modbus_functions::onSetParas(UBYTE1 *pAsk,UBYTE1* pAnswer)
{
	int pos                  = 0;
	pAnswer[POS_DEV_ID]      = pAsk[POS_DEV_ID];
	pAnswer[POS_FC]          = pAsk[POS_FC];
	pAnswer[POS_FC+1]        = pAsk[POS_FC+1];
	pAnswer[POS_FC+2]        = pAsk[POS_FC+2];
	size_t nSize = sizeof(tDEVPARAM);
	tDEVPARAM paras;
	pos = POS_FC + 3;
	memcpy(&paras,pAsk + pos, nSize);
	pos += set_crc(pAsk,pos);
	return pos;
}