#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "target_sqlite.h"
#include "qtpropertymanager.h"
#include "qteditorfactory.h"
#include "qttreepropertybrowser.h"
#include "qtbuttonpropertybrowser.h"
#include "qtgroupboxpropertybrowser.h"
bool MainWindow::init()
{
	QString str = QString("%1/etc/target.ini").arg(qgetenv(APPLICATION_ROOT).data());
	if (!initConfig(str))
	{
		DEBUG_PARA2("fail to read the config file", str);
		return false;
	}

	if (!initDB())
	{
		DEBUG_PARA2("fail to init database", m_target_setting.db_url);
		return false;
	}

	if (!initGraphic())
	{
		DEBUG_PARA1("fail to init graphic view");
		return false;
	}

	if (!initControl())
	{
		DEBUG_PARA1("fail to init control");
		return false;
	}

	if (!initProperty(ui->tabProperty))
	{
		DEBUG_PARA1("fail to init property window");
		return false;
	}

	str = QString::fromLocal8Bit("管理员:<font color='red'><strong>王时辰</strong></font>");
	showStatusMessage(str, eslAdmin);
	str = QString::fromLocal8Bit("肖静静 <font color='red'>182.5环 第19发</font>");
	showStatusMessage(str, eslSoldier);
	str = QString::fromLocal8Bit("10号靶 <font color='green'>7.2环 第4发</font>");
	showStatusMessage(str, eslTarget);
	str = QString::fromLocal8Bit("状态:<font color='red'>训练中</font>");
	showStatusMessage(str, eslInfor);
	return true;
}
/************************************************************************/
/* name:             initProperty
/* desc:             创建属性对话框
/* para:             QWidget* parent[父窗体]
/* return            QWidget* [成功: propertyEditor] [失败:NULL]
/* author:           bding
/* date:             2017-06-25
/************************************************************************/
QWidget* MainWindow::initProperty(QWidget* parent)
{
	doubleManager = new QtDoublePropertyManager(this);
	stringManager = new QtStringPropertyManager(this);
	colorManager = new QtColorPropertyManager(this);
	fontManager = new QtFontPropertyManager(this);
	pointManager = new QtPointPropertyManager(this);
	sizeManager = new QtSizePropertyManager(this);
	enumManager = new QtEnumPropertyManager(this);

	connect(doubleManager, SIGNAL(valueChanged(QtProperty *, double)),
		this, SLOT(valueChanged(QtProperty *, double)));
	connect(stringManager, SIGNAL(valueChanged(QtProperty *, const QString &)),
		this, SLOT(valueChanged(QtProperty *, const QString &)));
	connect(colorManager, SIGNAL(valueChanged(QtProperty *, const QColor &)),
		this, SLOT(valueChanged(QtProperty *, const QColor &)));
	connect(fontManager, SIGNAL(valueChanged(QtProperty *, const QFont &)),
		this, SLOT(valueChanged(QtProperty *, const QFont &)));
	connect(pointManager, SIGNAL(valueChanged(QtProperty *, const QPoint &)),
		this, SLOT(valueChanged(QtProperty *, const QPoint &)));
	connect(sizeManager, SIGNAL(valueChanged(QtProperty *, const QSize &)),
		this, SLOT(valueChanged(QtProperty *, const QSize &)));

	QtDoubleSpinBoxFactory *doubleSpinBoxFactory = new QtDoubleSpinBoxFactory(this);
	QtCheckBoxFactory *checkBoxFactory = new QtCheckBoxFactory(this);
	QtSpinBoxFactory *spinBoxFactory = new QtSpinBoxFactory(this);
	QtLineEditFactory *lineEditFactory = new QtLineEditFactory(this);
	QtEnumEditorFactory *comboBoxFactory = new QtEnumEditorFactory(this);

	propertyEditor = new QtTreePropertyBrowser(parent);
	propertyEditor->setFactoryForManager(doubleManager, doubleSpinBoxFactory);
	propertyEditor->setFactoryForManager(stringManager, lineEditFactory);
	propertyEditor->setFactoryForManager(colorManager->subIntPropertyManager(), spinBoxFactory);
	propertyEditor->setFactoryForManager(fontManager->subIntPropertyManager(), spinBoxFactory);
	propertyEditor->setFactoryForManager(fontManager->subBoolPropertyManager(), checkBoxFactory);
	propertyEditor->setFactoryForManager(fontManager->subEnumPropertyManager(), comboBoxFactory);
	propertyEditor->setFactoryForManager(pointManager->subIntPropertyManager(), spinBoxFactory);
	propertyEditor->setFactoryForManager(sizeManager->subIntPropertyManager(), spinBoxFactory);


	QGridLayout* gparentLayout = new QGridLayout(parent);
	gparentLayout->setMargin(1);
	gparentLayout->setSpacing(1);
	gparentLayout->addWidget(propertyEditor);

	QtProperty *property = NULL;
	property = doubleManager->addProperty(tr("Opacity"));
	doubleManager->setRange(property, 0, 1);
	doubleManager->setValue(property,0.1);
	doubleManager->setSingleStep(property, 0.1);
	addProperty(property, QLatin1String("Opacity"));

	return propertyEditor;
}
bool  MainWindow::initProperty()
{
	QtBoolPropertyManager *boolManager = new QtBoolPropertyManager(ui->tabProperty);
	QtIntPropertyManager *intManager = new QtIntPropertyManager(ui->tabProperty);
	QtStringPropertyManager *stringManager = new QtStringPropertyManager(ui->tabProperty);
	QtSizePropertyManager *sizeManager = new QtSizePropertyManager(ui->tabProperty);
	QtRectPropertyManager *rectManager = new QtRectPropertyManager(ui->tabProperty);
	QtSizePolicyPropertyManager *sizePolicyManager = new QtSizePolicyPropertyManager(ui->tabProperty);
	QtEnumPropertyManager *enumManager = new QtEnumPropertyManager(ui->tabProperty);
	QtGroupPropertyManager *groupManager = new QtGroupPropertyManager(ui->tabProperty);

	QtProperty *item0 = groupManager->addProperty("QObject");

	QtProperty *item1 = stringManager->addProperty("objectName");
	item0->addSubProperty(item1);

	QtProperty *item2 = boolManager->addProperty("enabled");
	item0->addSubProperty(item2);

	QtProperty *item3 = rectManager->addProperty("geometry");
	item0->addSubProperty(item3);

	QtProperty *item4 = sizePolicyManager->addProperty("sizePolicy");
	item0->addSubProperty(item4);

	QtProperty *item5 = sizeManager->addProperty("sizeIncrement");
	item0->addSubProperty(item5);

	QtProperty *item7 = boolManager->addProperty("mouseTracking");
	item0->addSubProperty(item7);

	QtProperty *item8 = enumManager->addProperty("direction");
	QStringList enumNames;
	enumNames << "Up" << "Right" << "Down" << "Left";
	enumManager->setEnumNames(item8, enumNames);
	QMap<int, QIcon> enumIcons;
	enumIcons[0] = QIcon(":/property/up.png");
	enumIcons[1] = QIcon(":/property/right.png");
	enumIcons[2] = QIcon(":/property/down.png");
	enumIcons[3] = QIcon(":/property/left.png");
	enumManager->setEnumIcons(item8, enumIcons);
	item0->addSubProperty(item8);

	QtProperty *item9 = intManager->addProperty("value");
	intManager->setRange(item9, -100, 100);
	item0->addSubProperty(item9);

	QtCheckBoxFactory *checkBoxFactory = new QtCheckBoxFactory(ui->tabProperty);
	QtSpinBoxFactory *spinBoxFactory = new QtSpinBoxFactory(ui->tabProperty);
	QtSliderFactory *sliderFactory = new QtSliderFactory(ui->tabProperty);
	QtScrollBarFactory *scrollBarFactory = new QtScrollBarFactory(ui->tabProperty);
	QtLineEditFactory *lineEditFactory = new QtLineEditFactory(ui->tabProperty);
	QtEnumEditorFactory *comboBoxFactory = new QtEnumEditorFactory(ui->tabProperty);

	QtAbstractPropertyBrowser *editor1 = new QtTreePropertyBrowser();
	editor1->setFactoryForManager(boolManager, checkBoxFactory);
	editor1->setFactoryForManager(intManager, spinBoxFactory);
	editor1->setFactoryForManager(stringManager, lineEditFactory);
	editor1->setFactoryForManager(sizeManager->subIntPropertyManager(), spinBoxFactory);
	editor1->setFactoryForManager(rectManager->subIntPropertyManager(), spinBoxFactory);
	editor1->setFactoryForManager(sizePolicyManager->subIntPropertyManager(), spinBoxFactory);
	editor1->setFactoryForManager(sizePolicyManager->subEnumPropertyManager(), comboBoxFactory);
	editor1->setFactoryForManager(enumManager, comboBoxFactory);

	editor1->addProperty(item0);

	QtAbstractPropertyBrowser *editor2 = new QtTreePropertyBrowser();
	editor2->addProperty(item0);

	QtAbstractPropertyBrowser *editor3 = new QtGroupBoxPropertyBrowser();
	editor3->setFactoryForManager(boolManager, checkBoxFactory);
	editor3->setFactoryForManager(intManager, spinBoxFactory);
	editor3->setFactoryForManager(stringManager, lineEditFactory);
	editor3->setFactoryForManager(sizeManager->subIntPropertyManager(), spinBoxFactory);
	editor3->setFactoryForManager(rectManager->subIntPropertyManager(), spinBoxFactory);
	editor3->setFactoryForManager(sizePolicyManager->subIntPropertyManager(), spinBoxFactory);
	editor3->setFactoryForManager(sizePolicyManager->subEnumPropertyManager(), comboBoxFactory);
	editor3->setFactoryForManager(enumManager, comboBoxFactory);

	editor3->addProperty(item0);

	QScrollArea *scroll3 = new QScrollArea();
	scroll3->setWidgetResizable(true);
	scroll3->setWidget(editor3);

	QtAbstractPropertyBrowser *editor4 = new QtGroupBoxPropertyBrowser();
	editor4->setFactoryForManager(boolManager, checkBoxFactory);
	editor4->setFactoryForManager(intManager, scrollBarFactory);
	editor4->setFactoryForManager(stringManager, lineEditFactory);
	editor4->setFactoryForManager(sizeManager->subIntPropertyManager(), spinBoxFactory);
	editor4->setFactoryForManager(rectManager->subIntPropertyManager(), spinBoxFactory);
	editor4->setFactoryForManager(sizePolicyManager->subIntPropertyManager(), sliderFactory);
	editor4->setFactoryForManager(sizePolicyManager->subEnumPropertyManager(), comboBoxFactory);
	editor4->setFactoryForManager(enumManager, comboBoxFactory);

	editor4->addProperty(item0);

	QScrollArea *scroll4 = new QScrollArea();
	scroll4->setWidgetResizable(true);
	scroll4->setWidget(editor4);

	QtAbstractPropertyBrowser *editor5 = new QtButtonPropertyBrowser();
	editor5->setFactoryForManager(boolManager, checkBoxFactory);
	editor5->setFactoryForManager(intManager, scrollBarFactory);
	editor5->setFactoryForManager(stringManager, lineEditFactory);
	editor5->setFactoryForManager(sizeManager->subIntPropertyManager(), spinBoxFactory);
	editor5->setFactoryForManager(rectManager->subIntPropertyManager(), spinBoxFactory);
	editor5->setFactoryForManager(sizePolicyManager->subIntPropertyManager(), sliderFactory);
	editor5->setFactoryForManager(sizePolicyManager->subEnumPropertyManager(), comboBoxFactory);
	editor5->setFactoryForManager(enumManager, comboBoxFactory);

	editor5->addProperty(item0);

	QScrollArea *scroll5 = new QScrollArea();
	scroll5->setWidgetResizable(true);
	scroll5->setWidget(editor5);

	QGridLayout *layout = new QGridLayout(ui->tabProperty);
	QLabel *label1 = new QLabel("Editable Tree Property Browser");
	QLabel *label2 = new QLabel("Read Only Tree Property Browser, editor factories are not set");
	QLabel *label3 = new QLabel("Group Box Property Browser");
	QLabel *label4 = new QLabel("Group Box Property Browser with different editor factories");
	QLabel *label5 = new QLabel("Button Property Browser");
	label1->setWordWrap(true);
	label2->setWordWrap(true);
	label3->setWordWrap(true);
	label4->setWordWrap(true);
	label5->setWordWrap(true);
	label1->setFrameShadow(QFrame::Sunken);
	label2->setFrameShadow(QFrame::Sunken);
	label3->setFrameShadow(QFrame::Sunken);
	label4->setFrameShadow(QFrame::Sunken);
	label5->setFrameShadow(QFrame::Sunken);
	label1->setFrameShape(QFrame::Panel);
	label2->setFrameShape(QFrame::Panel);
	label3->setFrameShape(QFrame::Panel);
	label4->setFrameShape(QFrame::Panel);
	label5->setFrameShape(QFrame::Panel);
	label1->setAlignment(Qt::AlignCenter);
	label2->setAlignment(Qt::AlignCenter);
	label3->setAlignment(Qt::AlignCenter);
	label4->setAlignment(Qt::AlignCenter);
	label5->setAlignment(Qt::AlignCenter);

	layout->addWidget(label1, 0, 0);
	layout->addWidget(label2, 0, 1);
	layout->addWidget(label3, 0, 2);
	layout->addWidget(label4, 0, 3);
	layout->addWidget(label5, 0, 4);
	layout->addWidget(editor1, 1, 0);
	layout->addWidget(editor2, 1, 1);
	layout->addWidget(scroll3, 1, 2);
	layout->addWidget(scroll4, 1, 3);
	layout->addWidget(scroll5, 1, 4);
	return true;
}
/************************************************************************
* 名    称:  resetProperty

* 参    数:  无

* 返	   回:  无

* 功能描述: 重置属性框项目
/************************************************************************/
void MainWindow::resetProperty()
{
	QMap<QtProperty *, QString>::ConstIterator itProp = propertyToId.constBegin();
	while (itProp != propertyToId.constEnd())
	{
		delete itProp.key();
		itProp++;
	}
	propertyToId.clear();
	idToProperty.clear();
}
/************************************************************************
* 名    称:  updateExpandState

* 参    数:  无

* 返	   回:  无

* 功能描述: 更新属性框状态
/************************************************************************/
void MainWindow::updateExpandState()
{
	//所有顶层属性
	QList<QtBrowserItem *> list = propertyEditor->topLevelItems();
	QListIterator<QtBrowserItem *> it(list);

	//扫描所有属性
	while (it.hasNext())
	{
		QtBrowserItem *item = it.next();
		QtProperty *prop = item->property();
		idToExpanded[propertyToId[prop]] = propertyEditor->isExpanded(item);
	}
}

/************************************************************************
* 名    称:  addProperty

* 参    数:  [in]QtProperty *property 追加一条属性
[in]const QString &id    属性字符串id

* 返	   回:  无

* 功能描述: 追加一条属性
/************************************************************************/
void MainWindow::addProperty(QtProperty *property, const QString &id)
{
	propertyToId[property] = id;
	idToProperty[id] = property;
	QtBrowserItem *item = propertyEditor->addProperty(property);
	if (idToExpanded.contains(id))
		propertyEditor->setExpanded(item, idToExpanded[id]);
}


bool MainWindow::initConfig(const QString& strFile)
{
	DEBUG_PARA1("initConfig");
	QSettings iniFile(strFile, QSettings::IniFormat);

	iniFile.beginGroup("system");
	m_target_setting.sys_caption = iniFile.value("caption").toString();
	m_target_setting.sys_about     = iniFile.value("about").toString();
	m_target_setting.sys_version   = iniFile.value("version").toString();
	m_target_setting.sys_background = iniFile.value("background").toString();
	iniFile.endGroup();

	iniFile.beginGroup("database");
	m_target_setting.db_driver = iniFile.value("driver").toString();
	m_target_setting.db_usr = iniFile.value("usr").toString();
	m_target_setting.db_pwd = iniFile.value("pwd").toString();
	m_target_setting.db_url = iniFile.value("url").toString();
	iniFile.endGroup();

	iniFile.beginGroup("tcp");
	m_target_setting.tcp_port = iniFile.value("port").toInt();
	m_target_setting.tcp_maxclient = iniFile.value("max").toInt();
	m_target_setting.tcp_timeout = iniFile.value("timeout").toInt();
	iniFile.endGroup();

	iniFile.beginGroup("com");
	m_target_setting.com_databit = iniFile.value("port").toInt();
	m_target_setting.com_baudrate = iniFile.value("baudrate").toInt();
	m_target_setting.com_name = iniFile.value("name").toString();
	m_target_setting.com_direction = iniFile.value("direction").toInt();
	m_target_setting.com_flowcontrol = iniFile.value("flowcontrol").toInt();
	m_target_setting.com_parity = iniFile.value("parity").toInt();
	m_target_setting.com_stopbit = iniFile.value("stopbit").toInt();
	iniFile.endGroup();

	return true;
}
bool MainWindow::initDB()
{
	DEBUG_PARA1("initDB");
	if (!QFile::exists(m_target_setting.db_url))
	{
		DEBUG_PARA2("配置文件中的数据库文件不存在", m_target_setting.db_url);
		return false;
	}

	if (m_target_setting.db_driver == QString("QSQLITE"))
	{
		QSqlError err = QSqlHelper::addConnection(m_target_setting.db_driver, m_target_setting.db_url);
		if (err.type() != QSqlError::NoError)
		{
			qDebug() << QObject::tr("fail to open sqlite database")
				<< m_target_setting.db_url
				<< QObject::tr(":") << err;
			return false;
		}
	}
	else
	{
		DEBUG_PARA2("不支持的数据库驱动程序", m_target_setting.db_driver);
		return false;
	}
	return true;
}
QWidget* MainWindow::customWidgetItem(QWidget* p, int id)
{
	QWidget *widget = new QWidget(p);
	QWidget *mainArea = new QWidget(widget);
	QWidget *fileinfoWidget = new QWidget(mainArea);

	//显示区域的布局
	QVBoxLayout *verLayout = new QVBoxLayout();
	QHBoxLayout *horLayout = new QHBoxLayout();
	QHBoxLayout *fileinfoLayout = new QHBoxLayout();

	//主要控件
	QLabel *idLabel = new QLabel(widget);
	QLabel *textLabel = new QLabel(mainArea);
	QLabel *speedLabel = new QLabel(mainArea);
	QProgressBar *progressbar = new QProgressBar(mainArea);
	QPushButton *giveupButton = new QPushButton(widget);
	QLabel *tipLabel1 = new QLabel(widget);
	QLabel *tipLabel2 = new QLabel(widget);
	QLabel *filesizeLabel = new QLabel(widget);

	//设置不同控件的样式
	idLabel->setFixedSize(30, 30);
	idLabel->setStyleSheet("background:red; border-radius:15px;color:white;");
	idLabel->setText("test");
	idLabel->setAlignment(Qt::AlignCenter);

	giveupButton->setToolTip(QStringLiteral("取消下载"));
	giveupButton->setFixedSize(24, 24);
	QIcon icon3(":/file/ftp.png");
	giveupButton->setIcon(icon3);
	giveupButton->setIconSize(QSize(16, 16));

	giveupButton->setProperty("index", QString::number(id - 1));
	QFont tempFont(QStringLiteral("宋体"), 9, 0);

	textLabel->setText("file list");
	textLabel->setFont(tempFont);
	textLabel->setFixedHeight(16);
	progressbar->setValue(45);
	progressbar->setFixedHeight(16);
	progressbar->setFont(tempFont);
	speedLabel->setFixedSize(150, 16);
	speedLabel->setAlignment(Qt::AlignRight);
	speedLabel->setFont(tempFont);

	filesizeLabel->setText("145KB/290KB");
	filesizeLabel->setFixedWidth(120);
	filesizeLabel->setAlignment(Qt::AlignCenter);

	tipLabel1->setText(QStringLiteral("    未下载 "));
	tipLabel1->setFixedWidth(80);
	tipLabel1->setAlignment(Qt::AlignCenter);

	tipLabel2->setFixedSize(28, 28);

	fileinfoLayout->setContentsMargins(0, 0, 0, 0);
	fileinfoLayout->setSpacing(10);
	fileinfoLayout->addWidget(textLabel);
	fileinfoLayout->addWidget(speedLabel);
	fileinfoWidget->setLayout(fileinfoLayout);

	verLayout->setContentsMargins(0, 0, 0, 0);
	verLayout->setMargin(0);
	verLayout->setSpacing(5);
	verLayout->addWidget(fileinfoWidget);
	verLayout->addWidget(progressbar);
	mainArea->setLayout(verLayout);

	horLayout->setContentsMargins(0, 0, 0, 0);
	horLayout->setSpacing(10);
	horLayout->addWidget(idLabel);
	horLayout->addWidget(mainArea);
	horLayout->addWidget(filesizeLabel);
	horLayout->addWidget(tipLabel1);
	horLayout->addWidget(tipLabel2);
	horLayout->addWidget(giveupButton);
	widget->setLayout(horLayout);

	return widget;
}
bool MainWindow::initControl()
{
	DEBUG_PARA1("initControl");
		//QLabel *locationLabel;
	QLabel* locationLabel = new QLabel("July");
	locationLabel->setAlignment(Qt::AlignCenter);
	locationLabel->setMinimumSize(locationLabel->sizeHint());


	//QLabel *aixLabel;
	/*QLabel* aixLabel = new QLabel("\"CTRL + H\" for help");
	statusBar()->setStyleSheet(QString("QStatusBar::item{border: 1px}")); // 设置不显示label的边框
	statusBar()->setSizeGripEnabled(true); //设置是否显示右边的大小控制点
	statusBar()->addWidget(locationLabel);
	statusBar()->addWidget(aixLabel, 1);*/

	int i = 0;
	for (i = 0; i < eslCount;i++)
	{
		m_status_label[i] = new QLabel("label", this);
		statusBar()->addPermanentWidget(m_status_label[i]); //显示永久信息
	}
	statusBar()->showMessage("Status is here...", 3000);   // 显示临时信息，时间3秒钟.


	QIcon  Icon0 = QIcon(":/images/target_on.png");
	QIcon  Icon1 = QIcon(":/images/target_off.png");
	QList<TTarget*> lst = ui->targetFrame->getTargetList();
	for (i = 0; i < lst.count(); i++)
   {
		// ui->listWidget->addItem(QString("%1").arg(i));        //只简单的添加文本数据的
		// QListWidgetItem* Item = new QListWidgetItem(QString("%1").arg(i));    //通过QListWidgetItem 添加文本数据的
		TTarget* t = lst[i];
		QListWidgetItem * IconItem = NULL;
		if ((i+1)%5==0)
			IconItem = new QListWidgetItem(Icon0, QString::fromLocal8Bit("%1[on]").arg(t->tName), ui->listTarget);//通过 QListWidgetItem添加文本以及Icon数据的
		else
			IconItem = new QListWidgetItem(Icon1, QString::fromLocal8Bit("%1[off]").arg(t->tName), ui->listTarget);//通过 QListWidgetItem添加文本以及Icon数据的

		ui->listTarget->addItem(IconItem);
		IconItem->setToolTip(QString::fromLocal8Bit("tooltip message %1").arg(i));
	}

	QListWidgetItem *item = new QListWidgetItem();
	QSize size = item->sizeHint();
	item->setSizeHint(QSize(size.width(), 56));
	ui->listTarget->addItem(item);
	QWidget* widget = customWidgetItem(this, 45);
	widget->setSizeIncrement(size.width(), 56);
	ui->listTarget->setItemWidget(item, widget);

	return true;
}
bool MainWindow::iniConnects()
{
	DEBUG_PARA1("iniConnects");
	return true;
}
bool MainWindow::initGraphic()
{
	if (QFile::exists(m_target_setting.sys_background))
	{
		QString strCSS = QString("border-image: url(%1);").arg(m_target_setting.sys_background);
		ui->targetFrame->setStyleSheet(strCSS);
	}

	DEBUG_PARA1("initGraphic");
	ui->targetFrame->initScene();
	return true;
}