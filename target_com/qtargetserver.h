#ifndef QTARGETSERVER_H
#define QTARGETSERVER_H

#include <QTcpServer>
class MainWindow;
class QTargetClient;
class QTargetServer : public QTcpServer
{
	Q_OBJECT

public:
	QTargetServer(QWidget *parent,int port);
	~QTargetServer();
private:
	QList<QTargetClient*> m_Clients;
	MainWindow*          m_mainWindow;
signals:
	void updateServer(QString,int);
	void updateServerData(QByteArray da);

public slots:
	void slotUpdateClient(QString,int);
	void slotUpdateClientData(QByteArray dat);
	void slotDisconnected(int client_id);
	void slotThreadStarted(void);
	void slotThreadFinished(void);
public:
	virtual void incomingConnection(qintptr handle);
};

#endif // QTARGETSERVER_H
