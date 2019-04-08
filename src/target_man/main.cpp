#include "mainwindow.h"
#include <QApplication>
#include "suffix_screen.h"

QDialog* test360Dialog()
{
	static QSuffixScreen* w = NULL;
	if (w == NULL)
	{
		w = new QSuffixScreen(NULL);

	}

	return w;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
	QByteArray ba = qgetenv("WDH_ROOT");
	if (QString::fromLocal8Bit(ba).trimmed().isEmpty())
	{
		QMessageBox::information(NULL, QString::fromLocal8Bit("注意"), QString::fromLocal8Bit("没有添加环境变量 WDH_ROOT "));
		return -1;
	}
	QString strPath = QDir::currentPath();
	QString strPlugin = strPath + QString("/plugins");

	QString strSqlDriver = strPath + QString("/sqldrivers");
	QString strSqlite    = strPath + QString("/sqldrivers/qsqlite.dll");

	QDir dirPlugin(strPlugin);
	if (dirPlugin.exists())
	QApplication::addLibraryPath(strPlugin);

	if (QFile::exists(strSqlite))
	QPluginLoader loader(strSqlite);

    /*MainWindow w;
	if (!w.init())
	{
		return -1;
	}
	w.showMaximized();*/
	QDialog* dlg = test360Dialog();
	dlg->show();
    return a.exec();
}
