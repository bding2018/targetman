#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "target_sqlite.h"
#include "target_browser.h"
MainWindow* MainWindow::m_instance = NULL;
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
	m_instance = this;
    ui->setupUi(this);

	memset(m_status_label,0,sizeof(QLabel*)*eslCount);
	QActionGroup* actGroupMouse = new QActionGroup(this);
	actGroupMouse->setExclusive(true);
	actGroupMouse->addAction(ui->actionMouseDrag);
	actGroupMouse->addAction(ui->actionMouseSelect);
}

MainWindow::~MainWindow()
{
    delete ui;
}


MainWindow* MainWindow::getInstance()
{
	return m_instance;
}

QMenu* MainWindow::getContextMenu(TTarget* target)
{
	static QMenu *roomMen = NULL;
	if (roomMen == NULL)
	{
		roomMen = new QMenu(QString::fromLocal8Bit("±ê°Ð"));
		int i = 0;
		int num = sizeof(action_title) / sizeof(char*);
		for ( i = 0; i < num; i++)
		{

			QString strTitle = QString::fromLocal8Bit(action_title[i]);
			m_target_action[i] = NULL;
			if (strTitle.isEmpty())
			{
				roomMen->addSeparator();
			}else
			{
				m_target_action[i] = roomMen->addAction(strTitle, this, SLOT(slotTargetItemAction(QAction*)));
			}
	
		}
	
	}
	return roomMen;
}

bool MainWindow::showStatusMessage(QString& msg, int pos)
{
	if (pos >=0 && pos<eslCount)
	{
		m_status_label[pos]->setText(msg);
	}
	return true;
}
bool MainWindow::showFadeMessage(QString& msg, int  ms)
{
	statusBar()->showMessage(msg, ms);
	return true;
}



void MainWindow::on_actionExit_triggered()
{
	DEBUG_PARA1("on_actionExit_triggered");
	qApp->quit();
}

void MainWindow::on_actionNew_triggered()
{
	DEBUG_PARA1("on_actionNew_triggered");
}
void MainWindow::on_actionOpen_triggered()
{
	DEBUG_PARA1("on_actionOpen_triggered");
}
void MainWindow::on_actionSave_triggered()
{
	DEBUG_PARA1("on_actionSave_triggered");
}
void MainWindow::on_actionSaveAs_triggered()
{
	DEBUG_PARA1("on_actionSaveAs_triggered");
}
void MainWindow::on_actionPrint_triggered()
{
	DEBUG_PARA1("on_actionPrint_triggered");
	ui->targetFrame->print();
}

void MainWindow::on_actionRefresh_triggered()
{
	DEBUG_PARA1("on_actionRefresh_triggered");
}

void MainWindow::on_actionImport_triggered()
{
	DEBUG_PARA1("on_actionImport_triggered");
}

void MainWindow::on_actionExport_triggered()
{
	DEBUG_PARA1("on_actionExport_triggered");
}

void MainWindow::on_actionDataBase_triggered()
{
	DEBUG_PARA1("on_actionDataBase_triggered")
	static Browser* browser = NULL;
	if (browser == NULL)
	{
		browser = new Browser(this);
	}
	if (browser)
	{
		browser->exec();
	}
}

void MainWindow::on_actionMouseSelect_triggered(bool flag)
{
	ui->targetFrame->setPointerMod(!flag);
}
void MainWindow::on_actionMouseDrag_triggered(bool flag)
{
	ui->targetFrame->setPointerMod(flag);
}

void MainWindow::on_actionAntiAlias_triggered(bool flag)
{

}
void MainWindow::on_actionOpenGL_triggered(bool flag)
{
	ui->targetFrame->setOpenGL(flag);
}
void MainWindow::on_actionZoomIn_triggered()
{
	ui->targetFrame->zoomIn();
}
void MainWindow::on_actionZoomOut_triggered()
{
	ui->targetFrame->zoomOut();
}
void MainWindow::on_actionRotateClock_triggered()
{
	ui->targetFrame->rotateRight();
}
void MainWindow::on_actionRotateAnti_triggered()
{
	ui->targetFrame->rotateLeft();
}
void MainWindow::on_actionBgColor_triggered()
{
	//QColor initColor = m_backgroundItem->brush().color();
	QSize   pixSize(16, 16);
	QPixmap pixOld = ui->actionBgColor->icon().pixmap(pixSize);
	static QColor initColor = Qt::black;

	QColor color = QColorDialog::getColor(initColor,	this, "color dialog");
	QPainter painter(&pixOld);
	painter.fillRect(1, 12, 14, 4, color);
	ui->actionBgColor->setIcon(pixOld);

	//m_backgroundItem->setBrush(c);
}
void MainWindow::on_actionBgImage_triggered()
{
	QString strFile = QString("%1/res/background.png").arg(QString(qgetenv(APPLICATION_ROOT)));
	QString strCaption("open config file");
	QString strFilter("Image files (*.png *.png *.jpg *.gif *.bmp);;SVG files (*.svg *.yml)");
	strFile = QFileDialog::getOpenFileName(this, strCaption, strFile, strFilter);
	if (!strFile.isEmpty())
	{
		QString strCSS  = QString("border-image: url(%1);").arg(strFile);
		ui->targetFrame->setStyleSheet(strCSS);
	}

}
void MainWindow::on_actionFullScreen_triggered()
{
	ui->targetFrame->setWindowFlags(Qt::Window);
	ui->targetFrame->showFullScreen();




}
void MainWindow::on_actionResetGraph_triggered()
{
	ui->targetFrame->resetView();
}

