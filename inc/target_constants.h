#ifndef  TARGET_CONSTANTS_H
#define  TARGET_CONSTANTS_H
#include "qnamespace.h"
#include "qstring.h"
#include "qpoint.h"
enum ENUM_TREE_LEVEL
{
	TREE_ITEM_LEVEL_ROOT = 0,
	TREE_ITEM_LEVEL_TAG   = 1,
	TREE_ITEM_LEVEL_DEVICE = 2,
	TREE_ITEM_LEVEL_METER = 3,
	TREE_ITEM_LEVEL_ACTION = 4,
	TREE_ITEM_LEVEL_VOLTAGE = 5,
	TREE_ITEM_LEVEL_BAY = 6,
	ROLE_OBJECT_NAME = Qt::UserRole + 2,
	ROLE_OBJECT_ID   = Qt::UserRole + 3,
	ROLE_OBJECT_ALIAS= Qt::UserRole + 4,
	TREE_ROLE_LEVEL = 1,
	/*TARGET_ROLE_OBJID   = 2,
	TARGET_ROLE_OBJNAME = 3,
	TARGET_ROLE_OBJALIAS= 4,*/
	RECORD_UNINITIALIZED = 0,
	RECORD_UNCOMPLETED = 1,
	RECORD_COMPLETED = 2,
};

enum ENUM_STR_LEN
{
	STRLEN_NAME_32 = 32,
	STRLEN_MEMO_128= 128,
	STRLEN_ALIAS_48=  48,
	BUFFER_SIZE1024=1024,
	BUFFER_SIZE256 =256,
	BUFFER_SIZE8192=8192
};

enum ENUM_ITEM_SIZE
{
	MAX_ITEM_SIZE = 32,
	MIN_ITEM_SIZE = 16,
	TARGET_WIDTH  = 96,
	TARGET_HEIGHT = 64,
	ICON_WIDTH    = 5,
	ICON_HEIGHT   = 5,
	BUTTON_WIDTH  = 48,
	BUTTON_HEIGHT = 20,
	MAX_TARGET_COUNT=256
};

enum ENUM_ZVALUE
{
	ZVALUE_TARGETITEM=0
};
enum ENUM_FIELD_FLAG
{
    FIELD_FLAG_VISIBLE = 0x01,
	FIELD_FLAG_VALID   = 0x02,
	FIELD_FLAG_DELETED = 0x04,
	FIELD_FLAG_READONLY= 0x08,
	FIELD_FLAG_RESIZE  = 0x10
};

static const char* NUMBER_ARR[] = {"1","2"};

enum RendererType { Native, OpenGL, Image };

#define APPLICATION_ROOT "WDH_ROOT"

enum ENUM_STATUS_LABEL
{
	eslAdmin   = 0,//管理员信息
	eslTarget  = 1,//靶标信息
	eslSoldier = 2,//学员信息
	eslInfor   = 3,//汇总信息
	eslCount   = 4 //数目
};
static const char* action_title[] = { "连接", "断开", "显靶", "隐靶", "复位",
								"",
								"读取状态","弹点设置", "距离设置", "方式设置", "辅屏显示",
								"", 
								"前进", "后退", "转弯","停止","","灯光控制" };
enum ENUM_TARGETITEM_MENUITEM
{	
	etmConnect=0,
	etmDisconnect=1,
	etmShowTarget=2,
	etmHideTarget=3,
	etmResetTarget = 4,
	etmSeperator4=5,
	etmReadStatus=6,
	etmResetBullet=7,
	etmSetDistance=8,
	etmActionStytel=9,
	etmScreen=10,
	etmSeperator10 = 11,
	etmCarForward = 12,
	etmCarBackward = 13,
	etmCarTurn = 14,
	etmCarStop = 15,
	etmSeperator16=16,
	etmLightControl=17,
	etmCount = 24 //数目
};


#define  BRUSH_STYLE_NUM 9
static Qt::BrushStyle bPatternStyle[BRUSH_STYLE_NUM] =
{
	Qt::NoBrush,
	Qt::SolidPattern,
	Qt::Dense1Pattern,
	Qt::Dense2Pattern,
	Qt::Dense3Pattern,
	Qt::Dense4Pattern,
	Qt::Dense5Pattern,
	Qt::Dense6Pattern,
	Qt::DiagCrossPattern,

};
static QString sPatternStyle[BRUSH_STYLE_NUM] = { "nobrush",
"solidpattern",
"dense1pattern",
"dense2pattern",
"dense3pattern",
"dense4pattern",
"dense5pattern",
"dense6pattern",
"diagcrosspattern" };

#define  LINE_STYLE_NUM 6
static  QString sLineStyle[LINE_STYLE_NUM] = { "nopen", "solid", "dash", "dot", "dashdot", "dashdotdot" };
static  Qt::PenStyle pLineStyle[LINE_STYLE_NUM] = {
	Qt::NoPen,
	Qt::SolidLine,
	Qt::DashLine,
	Qt::DotLine,
	Qt::DashDotLine,
	Qt::DashDotDotLine
};

#define  LINE_WIDTH_NUM 9
static  QString lstWidth[LINE_WIDTH_NUM] = { "nopen", "LineWidth1", "LineWidth2", "LineWidth3", "LineWidth4", "LineWidth5", "LineWidth6", "LineWidth7", "LineWidth8" };
static  int lineWidth[LINE_WIDTH_NUM] = { 0, 1, 2, 3, 4, 5, 6, 7, 8 };


enum CASCADE_TREE_ITEM
{
	ctiMaxCount = 100,
	ctiTrains = 0, //训练分支	
	ctiPlans = 1, //计划分支
	ctiActionSeries = 2, //动作序列总分支
	ctiActionItems = 3, //动作总
	ctiStages = 4,
	ctiTargets = 5,
	ctiSoldiers = 6,
	ctiShoots = 7,
	ctiDepartments = 8, //部门分支
	ctiConfigItem = 9,
	ctiConfigs = 10,//所有配置


	ctiTrain = ctiTrains + 1000, //训练分支
	ctiDepartment = ctiDepartments + 1000, //部门分支
	ctiPlan = ctiPlans + 1000, //计划分支
	ctiActionSerial = ctiActionSeries + 1000,
	ctiActionItem = ctiActionItems + 1000, //动作序列分支
	ctiStage = ctiStages+1000,
	ctiTarget = ctiTargets+1000,
	ctiSoldier = ctiSoldiers+1000,
	ctiShoot = ctiShoots+1000,
	ctiConfigGroup = ctiConfigs + 100,
	ctiCustom     = 99
};

const float   PI = 3.1416f;
const QPointF TARGET_CENTER = QPointF(64,32);
const qreal   TARGET_RADIUS = 28.0f;

enum GRAPHIC_ITEM_TYPE{
	Type = 1,
	UserType = 65536,
	gitGRAPHIC_TARGET = UserType + 1
};

enum  TARGET_CONFIG_STATUS
{
	tcsVisible=1,
	tcsValid=2,
	tcsWork=4
};
#endif