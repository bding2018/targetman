#if !defined(AFX_POWER_MODBUS__C9BEE721_A8E7_44C8_9802_107CA28C518F__INCLUDED_)
#define AFX_POWER_MODBUS__C9BEE721_A8E7_44C8_9802_107CA28C518F__INCLUDED_
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "power_crc.h"
#include <set>
#include <map>
#include <vector>
#include <list>
using namespace std;

#ifdef WIN32
#pragma warning(disable:4786)
#endif
enum LOCATION
{
	LOCATION_ON = 0x70,//是否定位
	LOCATION_LONGITUDE = 0x01,//经度
	LOCATION_LATITUDE = 0x02,//维度
};
enum FAULT_INFORMATION
{
	FAULT_TARGET_BOARD_D0 = 1,//导电靶板故障
	FAULT_OPTIMAL_SENSOR_D1 = 2,//光电传感器故障
	FAULT_CURRENT_SAMPLE_D2 = 4,//电流采样电路故障
	FAULT_POWER_CICUIT_D3 = 8,//驱动电路故障
	FAULT_MOTOR_BLOCK_D4 = 16,//电机堵转
	FAULT_MOTORS_ENSOR_D5 = 32//电机传感器故障
};
enum  MOVE_STATUS
{
	MOVE_STOP_D15 = 0xF000, //静止OR运动
	MOVE_STAND_UP = 0x0001, //	0 未知	1 起 	2 倒
	MOVE_LAYDOWN =  0x0002, //D0~D3:用D0~D3表示起到状态

};
enum REGISTER_TARGET
{
	TARGET_LOCATION = 0x0001,

	TARGET_LONGITUDE_DEGREE = 0x0002,
	TARGET_LONGITUDE_MINUTE = 0x0003,
	TARGET_LONGITUDE_MINUTE_DECIMAL = 0x0004,

	TARGET_ALTITUDE_DEGREE = 0x0005,
	TARGET_ALTITUDE_MINUTE = 0x0006,
	TARGET_ALTITUDE_MINUTE_DECIMAL = 0x0007,
	TARGET_FAULT_INFORMATION = 0x0008,

	TARGET_MOVE_STATUS = 0x0009, //运动状态
	TARGET_BATTERY = 0x000A,//电池电量
	TARGET_HIT_BULLETS = 0x000B,//命中弹数
	TARGET_HIT_POINTS = 0x000C,//命中环数

	TARGET_DEVICETYPE = 0x0101,//	设备类型
	TARGET_DEVICENO   = 0x0102,//	设备编号
	TARGET_DEVICEMODE = 0x0103,//	通信模式
	TARGET_DEVICENO1 = 0x0104,//	设备号1
	TARGET_DEVICENO2 = 0x0105,//	设备号2
	TARGET_PARAMETER = 0x0106,//	参数
	TARGET_PARA      = 0x0107,//	待补充

	TARGET_RESET = 0x0201,//复位
	TARGET_POSTURE_CONTROL = 0x0202,//起倒	起、倒，速度（5级）
	TARGET_LIGHT_CONTROL   = 0x0203,//灯光控制	亮、灭，亮度（5级）
	TARGET_HIT_SETTING     = 0x0204 //命中弹数设置	清零或设置指定弹数
};

enum REGISTER_CAR
{
	CAR_SPEED = 0x0001,//速度2个字节
	CAR_POSITION = 0x0002,//位置2个字节
	CAR_MODE = 0x0003,//模式2个字节
	CAR_ERROR_CODE = 0x0004,//错误2个字节
	CAR_INITILIZE = 0x0101,//初始化	2个字节
	CAR_BREAKER  = 0x0102,//刹车	2个字节
	CAR_CURVE_ANGLE = 0x0103,//曲线行驶 - 角度	2个字节
	CAR_CURVE_RADIUS = 0x0104,//曲线行驶 - 转弯半径	2个字节
	CAR_LINE_DISTANCE = 0x0105,//直线行驶 - 行驶距离	2个字节
	CAR_LINE_TAP = 0x0106,//直线行驶 - 速度档位	2个字节
	CAR_RELEASE = 0x0107,//释放	2个字节
	CAR_PARK = 0x0108,//驻车	2个字节
	CAR_SPEED_LEFTWHEEL = 0x0109,//持续行驶 - 左轮速度	2个字节
	CAR_SPEED_RIGHTWHEEL = 0x010A,//持续行驶 - 右轮速度	2个字节
	CAR_TURNTAIL_ALTITUDE = 0x010B,//掉头 - 幅度	2个字节
	CAR_TURNTAIL_TAP      = 0x010C//掉头 - 速度档位	2个字节
};
//主要定值寄存器开始地址： 
//1.保护定值读取
#define REGISTER_PARA_BAUD     0x0060   //波特率
#define REGISTER_PARA_DATABITS 0x0064   //数据位
#define REGISTER_PARA_STOPBITS 0x0068   //停止位
#define REGISTER_PARA_PARITY   0X006C   //校验方式
#define REGISTER_PARA_Ua       0X0070   //Ua变比
#define REGISTER_PARA_Ub       0X0074   //Ub变比
#define REGISTER_PARA_Uc       0X0078   //Uc变比
#define REGISTER_PARA_Ia       0X007C   //Ia变比
#define REGISTER_PARA_Ib       0X0080   //Ib变比
#define REGISTER_PARA_Ic       0X0084   //Ic变比
#define REGISTER_PARA_I0       0X0088   //I0变比
#define REGISTER_PARA_Pa       0X008C   //Pa变比
#define REGISTER_PARA_Pb       0X0090   //Pb变比
#define REGISTER_PARA_Pc       0X0094   //Pc变比
#define REGISTER_PARA_NETWORK  0x0098   //网络选择
#define REGISTER_PARA_PT       0x009C   //额定电压[PT变比1 220]
#define REGISTER_PARA_CT       0x00A0   //额定电压[CT变比100:5=20 200:5=40 300:5=60 600:5=120]
#define REGISTER_PARA_TIME     0x00A4   //时间
#define REGISTER_PARA_PWD      0x00A8   //密码
#define REGISTER_PARA_ADDR     0x00AC   //地址
#define REGISTER_PARA_EXIT     0x00B0   //退出
#define REGISTER_PARA_CONTROL  0x00B4   //遥控
#define REGISTER_PARAS_FLOAT   0x00B6   //设置浮点数主机序、网络序
#define REGISTER_PARA_PROCTIME 0x00B8   //设置软件时间
#define REGISTER_PARA_PROCVER  0x00BA   //设置软件版本
#define REGISTER_AMP_ADJUST    0x0051   //幅值自动校准
#define REGISTER_ANG_ADJUST    0x0052   //相位自动校准
#define REGISTER_PARAS_ALL     0x0053   //设置所有参数

//10.功能码
#define FC_READ_PARA    0x80 //读取参数
#define FC_SET_PARA     0x81 //设置参数
#define FC_READ_TIME    0X90 //读取装置时间
#define FC_SET_TIME     0x91 //设置装置时间
#define FC_READ_SETTING 0x04 //读取定值
#define FC_SET_SETTING  0x10 //写定值
#define FC_READ_COIL    0x01 //读取线圈状态
#define FC_READ_STATUS  0x02 //读取遥信
#define FC_READ_ANALOG  0x03 //读取遥测
#define FC_TELE_CONTROL 0x05 //遥控
#define FC_READ_EVENT   0x16 //读取事件
#define FC_EXIT_SET     0x82 //退出设置
#define FC_ERROR_RAISE  0x83 //报告故障 
#define FC_ADJUST_PARA  0x93 //装置校准
#define FC_SET_PARAS    0x94 //设置所有参数      


#define  ERROR_BASE 40
#define  ERROR_READ_PARA   (ERROR_BASE + 1)
#define  ERROR_READ_TIME   (ERROR_BASE + 2)
#define  ERROR_READ_ANALOG (ERROR_BASE + 3)
#define  ERROR_WRITE_PARA  (ERROR_BASE + 4)
#define  ERROR_WRITE_TIME  (ERROR_BASE + 5)

#define STATUS_BASE      10
#define STATUS_INDEX_DI1 1
#define STATUS_INDEX_DI2 2
#define STATUS_INDEX_DI3 3

//缓冲区位置
#define  POS_DEV_ID   0
#define  POS_FC       1
#define  POS_BYTES    2
#define  POS_REGISTER 2
#define  POS_REGISTER_NUM   4
#define  POS_SET_PARA_BYTES 6
#define  POS_TIME_SET 5

#define  MAX_ANALOG_PER_DEV 64
#define  MAX_STATUS_PER_DEV 32
#define  MAX_PARA_PER_DEV   16
#define  MAX_CONTROL_PER_DEV 8
#define  MAX_DEV_NUM         10

#define  CLOSE_DEV 0X00FF
#define  OPEN_DEV  0x00DD

const UBYTE1 ACT_DEV       = 0X01;
const UBYTE1 START_ADDRESS = 0x02;
const UBYTE1 FUNCTION_CODE = FC_READ_PARA;
const UBYTE2 REGISTER_NUM  = 0X0004;
const FBYTE4 trip_val      = 0.05f;
const FBYTE4 trip_time     = 0.02f;

const UBYTE1 STR_LEN       = 32;
const UBYTE1 CONFIG_NUM    = 32;
const UBYTE1 MEASURE_NUM   = 64;
class config_item
{
	UBYTE4 itemNo;          //显示编号
	UBYTE4 itemCode;        //数据编码
	UBYTE4 itemSub;         //数据去下标
	BYTE1  itemChn[STR_LEN];//中文标识
	BYTE1  itemEng[STR_LEN];//英文标识
	BYTE4  oldValue;        //旧值
	BYTE4  curValue;        //当前值
	BYTE4  maxVal;          //最大值
	BYTE4  minVal;          //最小值
	UBYTE1 changed;         //数据变化
	UBYTE1 readOnly;        //只读属性
};

class meas_item
{
	UBYTE4 itemNo;          //显示编号
	UBYTE4 itemCode;        //数据编码
	UBYTE4 itemSub;         //数据去下标
	BYTE1  itemChn[STR_LEN];//中文标识
	BYTE1  itemEng[STR_LEN];//英文标识
	BYTE4  oldValue;        //旧值
	BYTE4  curValue;        //当前值
	BYTE4  maxVal;          //最大值
	BYTE4  minVal;          //最小值
	BYTE4  lastUpdate;      //最后一次更新时间
	BYTE4  regAddr;         //寄存器地址
	BYTE4  regBytes;        //字节数
	UBYTE1  devDataType;    //采集端类型
	UBYTE1  stnDataType;    //厂站端类型
	UBYTE1  modbusType;     //modbus数据类型
	UBYTE1  convertType;    //转换类型
	UBYTE1 readWrite;       //读写类型
};

class scanblock
{
	BYTE1 modbusStation[STR_LEN];            //modbus端口名称
	config_item configs[CONFIG_NUM];         //配置项目
	meas_item   measures[MEASURE_NUM];       //采集项目
};

struct modbus_item
{
	UBYTE2 id;
	char   desc[16];
	FBYTE4 value;
	UBYTE1 type;
	UBYTE1 bitidx;
	bool   bVal;
};

//保持4字节对齐
typedef struct 
{
	unsigned char startCode;			//offset=4

	unsigned int posEPT;//此值需要每隔1s存到eeprom里去，上电后加载进来 单位kwh*100
	unsigned int negEPT;//此值需要每隔1s存到eeprom里去，上电后加载进来 单位kwh*100
	unsigned short pt;
	unsigned short ct;
	unsigned int rsvd[4];		     //offset = 32

	unsigned char  datbit;
	unsigned char  stopbit;
	unsigned char  parity;			 //offset = 32

	unsigned short band;			 
	unsigned short devAddr;			 //36

	unsigned short ua;				 //40
	unsigned short ub;

	unsigned short uc;
	unsigned short ia;			     //44

	unsigned short ib;
	unsigned short ic;				//48

	unsigned short i0;
	unsigned short pha;			   //52

	unsigned short phb;
	unsigned short phc;			  //56

	unsigned short dc;			 
	unsigned char  flt;          //浮点数格式,1:主机序 2：网络序
	unsigned char  sum;			 //60
}tDEVPARAM;

class modbus_functions
{
 public:
	  static int modbus_init();

	  static int toReadPara(UBYTE1 *pAsk,UBYTE2 devID, UBYTE2 register_addr);
      static int onReadPara(UBYTE1 *pAsk,UBYTE1* pAnswer);
	  static int onReadPara(UBYTE1 *pAsk,UBYTE1* pAnswer,FBYTE4 val1,FBYTE4 val2);

	  static int toSetPara(UBYTE1 *pAsk,UBYTE2 devID, UBYTE2 paraId, BYTE4 paraValue);
	  static int toSetPara(UBYTE1 *pAsk,UBYTE2 devID, UBYTE2 register_addr, FBYTE4 trip_val, FBYTE4 trip_time);
      static int onSetPara(UBYTE1 *pAsk,UBYTE1* pAnswer);
	  static int onSetPara(UBYTE1 *pAsk,UBYTE1* pAnswer,FBYTE4* val1,FBYTE4* val2);

	  static int toReadStatus(UBYTE1 *pAsk,UBYTE2 devID,UBYTE2 register_addr);
      static int onReadStatus(UBYTE1 *pAsk,UBYTE1* pAnswer);

	  static int toReadCoil(UBYTE1 *pAsk,UBYTE2 devID,UBYTE2 register_addr);	
	  static int onReadCoil(UBYTE1 *pAsk,UBYTE1* pAnswer);

	  static int toWriteCoil(UBYTE1 *pAsk,UBYTE2 devID,UBYTE2 register_addr);
	  static int onWriteCoil(UBYTE1 *pAsk,UBYTE1* pAnswer);

	  static int toReadAnalog(UBYTE1 *pAsk,UBYTE2 devID,UBYTE2 register_addr,UBYTE2 register_num);
      static int onReadAnalog(UBYTE1 *pAsk,UBYTE1* pAnswer);

	  static int toTeleControl(UBYTE1 *pAsk,UBYTE2 devID,UBYTE2 register_addr);
      static int onTeleControl(UBYTE1 *pAsk,UBYTE1* pAnswer);

	  static int toReadTime(UBYTE1 *pAsk,UBYTE2 devId);
      static int onReadTime(UBYTE1 *pAsk,UBYTE1* pAnswer);

	  static int toAdjustTime(UBYTE1 *pAsk,UBYTE2 devId);
	  static int toAutoAdjust(UBYTE1 *pAsk, UBYTE2 devId, UBYTE2 paraId);
	  static int toExitParaSet(UBYTE1 *pAsk, UBYTE2 devId);
      static int onAdjustTime(UBYTE1 *pAsk,UBYTE1* pAnswer);

	  static int toReadEvent(UBYTE1 *pAsk, UBYTE2 devId);
      static int onReadEvent(UBYTE1 *pAsk,UBYTE1* pAnswer);

	  static int toSetParas(UBYTE1* pAsk,tDEVPARAM& devParam);
	  static int onSetParas(UBYTE1 *pAsk,UBYTE1* pAnswer);

	  static int get_status_sub(UBYTE2 devId, int register_addr);

	  static int get_analog_sub(UBYTE2 devId, int register_addr);

	  static int get_para_sub(UBYTE2 devId, int register_addr);

	  static FBYTE4 to_float(UBYTE1* buff);

	  static BYTE4  to_int(UBYTE1* buff);

	  static int    to_buff(FBYTE4,UBYTE1* buff);

	  static int    to_buff(BYTE4,UBYTE1* buff);

	  static UBYTE2 to_word(UBYTE1* buff);

	  static int    to_buff(UBYTE2,UBYTE1* buff);

	  static int    status_to_buff(UBYTE2,UBYTE1* buff);
	  static bool   buff_to_status(UBYTE1* buff,int bit);
      static int    set_crc(UBYTE1* pAnswer,int len);

	  static int    to_buff(tm&,UBYTE1* buff);

	  static int    parsebuffer(UBYTE1* buff,int register_addr,int register_num,list<modbus_item>& lstItems);

	  static time_t to_time(UBYTE1*buff);
	  
	  static UBYTE1   BCD_TO_HEX(UBYTE1 bcdByte);
	  static UBYTE1   HEX_TO_BCD(UBYTE1 btHex);
	  static UBYTE1   BCD_TO_HEX(UBYTE1* bcdByte,UBYTE1 nLen);
	  static UBYTE1   HEX_TO_BCD(UBYTE1* btHex,  UBYTE1 nLen);


	  static FBYTE4   pAnalog_Value[MAX_DEV_NUM*MAX_ANALOG_PER_DEV];
	  static FBYTE4   pPara_Value[MAX_DEV_NUM*MAX_PARA_PER_DEV];
	  static UBYTE1   pStatus_Value[MAX_DEV_NUM*MAX_STATUS_PER_DEV];
	  static UBYTE4   pTime[MAX_DEV_NUM];
	  static UBYTE1   dev_state[MAX_DEV_NUM];
	 
	  static map<int,int> id_to_status_sub;
      static int status_id[MAX_STATUS_PER_DEV];

	  static map<int,int> id_to_analog_sub;
      static int analog_id[MAX_ANALOG_PER_DEV];

	  static map<int,int> id_to_para_sub;
      static int para_id[MAX_PARA_PER_DEV];

	  static map<int,int> control_to_analog_sub;

	  static Crc* crc;
};


#endif