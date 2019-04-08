#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include "target_global.h"
#include "target_structs.h"
#include <QMainWindow>
class QtProperty;
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
	static MainWindow* getInstance();
private:
    Ui::MainWindow *ui;
	static MainWindow* m_instance;
	TargetSetting  m_target_setting;
	QLabel*       m_status_label[eslCount];

	QAction*      m_target_action[etmCount];
public slots:
	void slotCreateServer();
	void slotTargetItemAction(QAction*);
	void updateServer(QString,int);
//--file menu---
	void on_actionNew_triggered();
	void on_actionOpen_triggered();
	void on_actionSave_triggered();
	void on_actionSaveAs_triggered();
	void on_actionPrint_triggered();

	void on_actionExit_triggered();
	void on_actionRefresh_triggered();

	void on_actionImport_triggered();
	void on_actionExport_triggered();

	void on_actionDataBase_triggered();

	void on_actionMouseSelect_triggered(bool);
	void on_actionMouseDrag_triggered(bool);
	void on_actionAntiAlias_triggered(bool);
	void on_actionOpenGL_triggered(bool);

	void on_actionZoomIn_triggered();
	void on_actionZoomOut_triggered();
	void on_actionRotateClock_triggered();
	void on_actionRotateAnti_triggered();
	void on_actionBgColor_triggered();
	void on_actionBgImage_triggered();
	void on_actionFullScreen_triggered();
	void on_actionResetGraph_triggered();
public:
	bool initConfig(const QString& strFile);
	bool initDB();
	bool initControl();
	bool iniConnects();
	bool initGraphic();
	bool initProperty();
	QWidget* initProperty(QWidget* parent);
	bool init();
	bool   showStatusMessage(QString& msg,int pos);
	bool   showFadeMessage(QString& msg,int  ms);
	QMenu* getContextMenu(TTarget* target);
	bool   updateProperty(TTarget* target);
private:
	virtual void timerEvent(QTimerEvent *);
	virtual void customEvent(QEvent *);


/***************************property manager*********************************/
public Q_SLOTS:
	void valueChanged(QtProperty *property, double value);
	void valueChanged(QtProperty *property, int value);
	void valueChanged(QtProperty *property, const QString &value);
	void valueChanged(QtProperty *property, const QColor &value);
	void valueChanged(QtProperty *property, const QFont &value);
	void valueChanged(QtProperty *property, const QPoint &value);
	void valueChanged(QtProperty *property, const QSize &value);
	void valueChanged(QtProperty *property, const QPointF &value);
	void valueChanged(QtProperty *property, const QSizeF &value);
private:
	class QtDoublePropertyManager *doubleManager; //浮点属性
	class QtStringPropertyManager *stringManager; //整型属性
	class QtColorPropertyManager  *colorManager;  //颜色属性
	class QtFontPropertyManager   *fontManager;   //字体属性
	class QtPointPropertyManager  *pointManager;  //点属性
	class QtPointFPropertyManager  *pointFManager;  //点属性(浮点)
	class QtSizePropertyManager   *sizeManager;   //尺寸属性
	class QtSizeFPropertyManager   *sizeFManager;   //尺寸属性(浮点)
	class QtEnumPropertyManager   *enumManager;  //枚举类型
	class QtTreePropertyBrowser   *propertyEditor; //文字属性

	QMap<QtProperty *, QString>   propertyToId;   //属性与ID映射
	QMap<QString, QtProperty *>   idToProperty;   //ID与属性映射
	QMap<QString, bool>          idToExpanded;   //属性是否已经展开

	//更新属性标签展开状态
	void updateExpandState();

	//重置属性框项目
	void resetProperty();

	//追加一条属性
	void addProperty(QtProperty *property, const QString &id);
	/*****************************************************************************/
	QWidget* customWidgetItem(QWidget* p, int id);
};

#endif // MAINWINDOW_H
