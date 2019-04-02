#ifndef  TARGET_STRUCTS_H
#define  TARGET_STRUCTS_H
#include "target_constants.h"
#include "qdatetime.h"
#include "qpoint.h"
#include "qlist.h"
struct TARGET_STATUS
{
	quint32  ready : 1;//准备好
	quint32  hidden : 1;//隐藏
	quint32  invalid : 1;//无效的
	quint32  selected : 1;//选择了
	quint32  online : 1;//通讯在线
	quint32  broken : 1;//损坏
	quint32  shooting : 1;//射击中
	quint32  completed : 1;//射击完成
	quint32  preparing : 1;//准备中
	quint32  back1 : 1;//已清洁
	quint32  back2 : 1;//正在保洁
	quint32  back3 : 1;//告警
	quint32  back4 : 1;//呼叫
	quint32  back5 : 1;//请求清理
	quint32  back6 : 1;//房间有人
};

struct SOLDIER_STATUS
{
	quint32  absent      : 1;//缺席
	quint32  preparing   : 1;//预备
	quint32  waiting     : 1;//等待
	quint32  training    : 1;//训练中
	quint32  regSucce    : 1;//登录成功
	quint32  regFailed   : 1;//认证失败
	quint32  illegal     : 1;//非法的
	quint32  canceled    : 1;//取消的
};
struct  TObjTable
{
	int tableid;
	int tableIndex;
	int valid;
	QString tableName;
	QString tableAlias;
};

struct TObjField
{
	int tableid;
	int fieldid;
	int valid;
	QString fieldName;
	QString fieldAlias;
	int type;
	//char fieldName[STRLEN_NAME_32];
	//char fieldAlias[STRLEN_ALIAS_48];
};

struct TTarget
{
	int tid;//数据库id
	char name[STRLEN_NAME_32];//名称
	QString tName;
	QString tMemo;
	int  type;   //类别定义靶形状类
	int  subtype;//子类别定义靶功能类
	int  prevTarget;//前一个
	int  netxTarget;//后一个靶标
	int  gallery;  //靶场
	int  devNo;    //通讯装置编号
	int  orderNo;  //排列序号
	int  targetNo; //靶标编号
	int  shootNo;  //当前射击次数
	int  shootNum; //射击总数
	int  userId;  //用户ID
	int  status;

	int  portNo;     //靶标的端口
	QString strIPAddr;//靶标的IP

	unsigned short fault;     // 故障信息  ALARM_FLG_XXX定义信息
	unsigned short run_state;  // 运行状态  静止：起、倒、未知 运动：起过程中，倒过程中  ，摇摆靶时为摇摆位置的角度
	unsigned short battery;    //电池电量
	unsigned short hit_total;  // 总命中数
	unsigned short cycle_hits;  // 一个查询周期的命中数
	QList<QPointF>  m_points;

	int  curDirection;//命中方位
	float  totalPoints;//总环数
	float  currentPoints;//当前环数
	float  xPos;//坐标x
	float  yPos;//坐标y
	float  thelta;//倾斜角度
	TARGET_STATUS     m_status;
	class QGraphicTarget* graphicTarget;
};

struct TGallery
{
	int tid;
	char name[STRLEN_NAME_32];
	int devNo;
	int objectType;
};

struct TMilitaryUnit
{
	int tid;
	char name[STRLEN_NAME_32];
	int devNo;
	int objectType;
};

struct TAdministrator
{
	int tid;
	char name[STRLEN_NAME_32];
	int devNo;
	int objectType;
};

struct TSoldier
{
	int tid;
	char name[STRLEN_NAME_32];
	int devNo;
	int objectType;
};

struct  TSoldierShot
{
	int  tid;
	char name[STRLEN_NAME_32];
	int  soldier;
	int  train;
	float points;
	float xPos;
	float yPos;
	int   direction;
};

struct TSoldierTrain
{
	int   tid;
	char  name[STRLEN_NAME_32];
	int   soldier;
	float totalPoints;
	int   shoots;
	int   status;
	int   admin;
	int   startTime;
	int   endTime;
	int   valid;
};

struct TargetSetting
{
	QString db_driver;
	QString db_url;
	QString db_usr;
	QString db_pwd;
	
	QString sys_caption;
	QString sys_version;
	QString sys_about;
	QString sys_background;
	
	int     tcp_port;
	int     tcp_maxclient;
	int     tcp_timeout;
	int     tcp_interval;

	QString com_name;
	int     com_databit;
	int     com_baudrate;
	int     com_flowcontrol;
	int     com_stopbit;
	int     com_direction;
	int     com_parity;

	QString   train_soldier_name;
	QString   train_soldier_id;
	QString   train_id;
	QDateTime tarin_start;
	QDateTime train_end;
};

struct TARGET_COMMAND
{
	int      cmdType;
	int      dataArr[4];
	int      timeout;//超时时间
	TTarget*  target;
	QDateTime tmStamp;
};


#define DEBUG_PARA1(x) qDebug()<<"<"<<__FILE__<<","<<__LINE__<<">"<<QString::fromLocal8Bit(x);
#define DEBUG_PARA2(x,y) qDebug()<<"<"<<__FILE__<<","<<__LINE__<<">"<<QString::fromLocal8Bit(x)<<":"<<y;
#endif