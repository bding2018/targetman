#ifndef QTARGETCLIENT_H
#define QTARGETCLIENT_H
#include "target_global.h"
#include "target_structs.h"
class MainWindow;

class QTargetClient : public QTcpSocket
{
	Q_OBJECT

public:
	QTargetClient(QObject *parent,TTarget* pTarget);
	~QTargetClient();
 	TTarget* m_target;
signals:
	void sigUpdateClient(QString,int);
	void sigUpdateClientData(QByteArray da);
	void sigDisconnected(TTarget*);
	void sigConnected(TTarget*);
	void sigError(TTarget*, SocketError);
	void sigChanged(TTarget*, SocketState);
public slots:

   //数据接收
	void slotDisconnected();

    //连接断开
	void slotDataReceived();

	//连接建立
	void slotConnected();   

	//状态改变
	void slotStateChanged(SocketState);

	//发生错误
	void slotError(SocketError);

};

#endif // QTARGETCLIENT_H
