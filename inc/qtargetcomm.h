#ifndef QTARGETCOMM_H
#define QTARGETCOMM_H

#include "target_constants.h"
#include "target_structs.h"
#include "target_sql_define.h"
#include "tp_communication_if.h"
#include "target_global.h"
#include <iostream>


const UINT16 UDP_PEER_PORT = 2234;
const UINT16 UDP_BIND_PORT = 8204;
const UINT16 SCAN_INTERVAL = 400;
const UINT16 CURRENT_USERID= 1;
#define  UDP_PEER_HOST "127.0.0.1"
class QUdpSocket;
class QTargetSerial;
class QTargetServer;
class QTargetClient;
using namespace std;
class TARGET_COM_EXPORT QTargetComm :public QUdpSocket
{
	Q_OBJECT
public:
	QTargetComm(QObject* parent);
	~QTargetComm();

	void test(int style);

	int push_command(TARGET_COMMAND& cmd);

	int init(ushort portNo);
	
	int pause(bool flag);
	
	int query(TTarget* target, int cmdId);
	
	int config(TTarget* target, int cmdId,int* data);
	
	int control(TTarget* target, int cmdId,int* data);

	int sendBuff(TTarget* target, char* buff, int len);

	int connectTarget(TTarget* target);

	int disconnectTarget(TTarget* tareget);
	
	enum UDP_COMM_STATUS
	{
		ucsNone = 0,
		ucsRunning=1,
		ucsPaused=2,
		ucsTerminated=4,
		ucsWaiting=8,
		ucsTimeout=16,
		ucsReturnedOK=32
	};

	enum UDP_COMM_COMMAND
	{
		uccNone=0,
		uccQuery=1,
		uccConfig=2,
		uccControl=3
	};
private:
	QList<TARGET_COMMAND>  m_commandList;
	TARGET_COMMAND        m_current_cmd;
	class QTimer         *m_timer;
	QThread    serialThread;
	QThread    tcpServerThread;

	TargetSetting  *m_target_setting;
	QTargetSerial  *m_serial;
	QTargetServer  *m_tcpServer;
	QList<QTargetClient*>  m_target_clients;
	UINT16        m_status;
	QMutex        mutex;
private slots:
	void readPendingDatagrams();

	void slotPolling();

	void slotResponseTimeout();

	void on_actionSerial_triggered(bool bFlag);

	void on_actionTcpServer_triggered(bool bFlag);

	void slotUpdateClientData(QByteArray ba);

	void slotClientConnected(TTarget*);
	void slotClientError(TTarget*,SocketError);
	void slotClientDisconnected(TTarget*);
	void slotClientChanged(TTarget*,SocketState);
signals:
	void sigUpdateTarget(TTarget*);
};

#endif // QTARGETCOMM_H
