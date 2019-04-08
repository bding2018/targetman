#include "qtarget_graphic_view.h"
#include "target_sqlite.h"
#ifndef QT_NO_PRINTER
#include <QPrinter>
#include <QPrintDialog>
#endif
#ifndef QT_NO_OPENGL
#include <QtOpenGL>
#else
#include <QtWidgets>
#endif


#ifndef QT_NO_WHEELEVENT
void GraphicsView::wheelEvent(QWheelEvent *e)
{
    if (e->modifiers() & Qt::ControlModifier) {

        if (e->delta() > 0)
            view->zoomIn(6);
        else
            view->zoomOut(6);
        e->accept();
    } else {
        QGraphicsView::wheelEvent(e);
    }
}
#endif

QGraphicsScene* GraphicsView::initScene()
{
	QImage image(":/images/qt4logo.png");
	QGraphicsScene* roomScene = new QGraphicsScene;
	const int TOTAL_WIDTH = 2200;
	const int TOTAL_HEGIHT = 1400;
	const int DELT_WIDTH = 110;  //元器件的宽度
	const int DELT_HEIGHT = 70; //元器件的高度


	int    xNumber = 0; //横向编号
	int nitems = 0;
	for (int i = -TOTAL_WIDTH / 2; i < TOTAL_WIDTH / 2; i += DELT_WIDTH) {
		xNumber++;
		int yNumber = 0;//纵向编号
		for (int j = -TOTAL_HEGIHT / 2; j < TOTAL_HEGIHT / 2; j += DELT_HEIGHT)
		{
			yNumber++;
			qreal x = (i + TOTAL_WIDTH / 2) / qreal(TOTAL_WIDTH);
			qreal y = (j + TOTAL_HEGIHT / 2) / qreal(TOTAL_HEGIHT);

			QColor color(image.pixel(int(image.width() * x), int(image.height() * y)));
			QGraphicsItem *item = new QTargetItem(NULL, xNumber, yNumber);
			item->setPos(QPointF(i, j));
			roomScene->addItem(item);
			++nitems;
		}
	}
	DEBUG_PARA2("对象个数", nitems);
	return roomScene;
}
QGraphicsScene* GraphicsView::initScene(QList<TTarget*>& lstTarget,int cols)
{
	QGraphicsScene* roomScene = new QGraphicsScene;
	qreal x = 0, y = 0;
	const int ROW_TARGET_NUM = 4;
	int index = 0, row = 0,col=0;
	if (cols<1 || cols>10)
	{
		cols = ROW_TARGET_NUM;
	}

	for each (TTarget *target in lstTarget)
	{
		row = index / cols;
		col = index % cols;
		QGraphicsItem *item = new QGraphicTarget(target, row, col);
		x = col*(4 + TARGET_WIDTH);
		y = row*(4 + TARGET_HEIGHT);
		item->setPos(QPointF(x, y));
		x += 53;
		roomScene->addItem(item);
		index++;
	}
	return roomScene;
}
QGraphicsScene* GraphicsView::initScene(QList<TTarget*>& lstTarget)
{
	QGraphicsScene* roomScene = new QGraphicsScene;
	const int DELT_WIDTH  = TARGET_WIDTH*1.1;  //靶标的宽度
	const int DELT_HEIGHT = TARGET_HEIGHT*1.1;  //靶标的高度
	const int NUM_LINE    = 4;
	int    xNumber = 0; //横向编号
	int    yNumber = 0;
	int    nitems = lstTarget.count();
	int    i = 0;
	for (i = 0; i < nitems; i ++) 
	{

			yNumber = i / NUM_LINE;
			xNumber = i % NUM_LINE;
			qreal x = xNumber*DELT_WIDTH;
			qreal y = yNumber*DELT_HEIGHT + BUTTON_HEIGHT;
			TTarget* target = lstTarget[i];

			QGraphicsItem *item = new QTargetItem(target, yNumber, xNumber);
			item->setPos(QPointF(x, y));
			roomScene->addItem(item);		
	}

	QStringList lstText;
	lstText << QString::fromLocal8Bit("开始停止");
	lstText << QString::fromLocal8Bit("人员切换");
	lstText << QString::fromLocal8Bit("全部选中");
	lstText << QString::fromLocal8Bit("灯光控制");
	lstText << QString::fromLocal8Bit("模式选择");
	lstText << QString::fromLocal8Bit("训练管理");
	lstText << QString::fromLocal8Bit("关闭靶标");
	lstText << QString::fromLocal8Bit("系统设置");
	qreal x=0, y=0;
	for each (QString var in lstText)
	{
		
		QGraphicsItem *item = new QPixButtonItem(var);
		item->setPos(QPointF(x, y));
		x += 53;
		roomScene->addItem(item);
	}



	DEBUG_PARA2("对象个数", nitems);
	return roomScene;
}
QTargetView::QTargetView(QWidget *parent): QFrame(parent)
{
    setFrameStyle(Sunken | StyledPanel);
    graphicsView = new GraphicsView(this);
    graphicsView->setRenderHint(QPainter::Antialiasing, false);
    graphicsView->setDragMode(QGraphicsView::RubberBandDrag);
    graphicsView->setOptimizationFlags(QGraphicsView::DontSavePainterState);
    graphicsView->setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    graphicsView->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);

    int size = style()->pixelMetric(QStyle::PM_ToolBarIconSize);
    zoomSlider = new QSlider;
    zoomSlider->setMinimum(0);
    zoomSlider->setMaximum(500);
    zoomSlider->setValue(250);
    zoomSlider->setTickPosition(QSlider::TicksRight);

    // Zoom slider layout
    QVBoxLayout *zoomSliderLayout = new QVBoxLayout;
    zoomSliderLayout->addWidget(zoomSlider);

    rotateSlider = new QSlider;
    rotateSlider->setOrientation(Qt::Horizontal);
    rotateSlider->setMinimum(-360);
    rotateSlider->setMaximum(360);
    rotateSlider->setValue(0);
    rotateSlider->setTickPosition(QSlider::TicksBelow);

    // Rotate slider layout
    QHBoxLayout *rotateSliderLayout = new QHBoxLayout;
    rotateSliderLayout->addWidget(rotateSlider);


    QGridLayout *topLayout = new QGridLayout;
    topLayout->addWidget(graphicsView,0, 0);
    topLayout->addLayout(zoomSliderLayout, 0, 1);
    topLayout->addLayout(rotateSliderLayout, 1, 0);
    setLayout(topLayout);


    connect(zoomSlider, SIGNAL(valueChanged(int)), this, SLOT(setupMatrix()));
    connect(rotateSlider, SIGNAL(valueChanged(int)), this, SLOT(setupMatrix()));
    connect(graphicsView->verticalScrollBar(), SIGNAL(valueChanged(int)),
            this, SLOT(setResetButtonEnabled()));
    connect(graphicsView->horizontalScrollBar(), SIGNAL(valueChanged(int)),
            this, SLOT(setResetButtonEnabled()));

	connect(this,SIGNAL(customContextMenuRequested(const QPoint &)),
		   this,SLOT(slotContextMenuRequested(const QPoint&)));


    setupMatrix();
	QGraphicTarget::initTargetPix();
	zoomSlider->hide();
	rotateSlider->hide();
}

QGraphicsView *QTargetView::view() const
{
    return static_cast<QGraphicsView *>(graphicsView);
}

void QTargetView::resetView()
{
    zoomSlider->setValue(250);
    rotateSlider->setValue(0);
    setupMatrix();
    graphicsView->ensureVisible(QRectF(0, 0, 0, 0));
}



void QTargetView::setupMatrix()
{
    qreal scale = qPow(qreal(2), (zoomSlider->value() - 250) / qreal(50));

    QMatrix matrix;
    matrix.scale(scale, scale);
    matrix.rotate(rotateSlider->value());

    graphicsView->setMatrix(matrix);
}
void QTargetView::setPointerMod(bool isDrag)
{

	graphicsView->setDragMode(isDrag ? QGraphicsView::ScrollHandDrag : QGraphicsView::RubberBandDrag);
	graphicsView->setInteractive(!isDrag);
}
void QTargetView::setAntialiasing(bool bFlag)
{
	graphicsView->setRenderHint(QPainter::Antialiasing, bFlag);
}
void QTargetView::setOpenGL(bool bFlag)
{
#ifndef QT_NO_OPENGL
	graphicsView->setViewport(bFlag ? new QGLWidget(QGLFormat(QGL::SampleBuffers)) : new QWidget);
#endif
}


void QTargetView::print()
{
#if !defined(QT_NO_PRINTER) && !defined(QT_NO_PRINTDIALOG)
    QPrinter printer;
    QPrintDialog dialog(&printer, this);
    if (dialog.exec() == QDialog::Accepted) {
        QPainter painter(&printer);
        graphicsView->render(&painter);
    }
#endif
}

void QTargetView::zoomIn(int level)
{
	if (zoomSlider->value() >250)
	{
		qDebug() << QString::fromLocal8Bit("已经到最大级别");
		return;
	}
    zoomSlider->setValue(zoomSlider->value() + level);
}

void QTargetView::zoomOut(int level)
{
	if (zoomSlider->value()<50)
	{
		qDebug() << QString::fromLocal8Bit("已经到最小级别");
		return;
	}
    zoomSlider->setValue(zoomSlider->value() - level);
}

void QTargetView::rotateLeft()
{
    rotateSlider->setValue(rotateSlider->value() - 10);
}

void QTargetView::rotateRight()
{
    rotateSlider->setValue(rotateSlider->value() + 10);
}
QList<TTarget*> QTargetView::getTargetList()
{
	return m_lstTarget;
}
QGraphicsScene* QTargetView::initScene()
{
	QString str;
	QGraphicsScene* scenen = NULL;
		if (m_lstTarget.size() > 0)
		{
			for each (TTarget* t in m_lstTarget)
			{
				delete t;
			}
			m_lstTarget.clear();
		}
		if (graphicsView->scene())
		{
			scenen = graphicsView->scene();
			scenen->clear();
			scenen->deleteLater();
			scenen = NULL;
		}


		
		m_lstTarget = QSqlHelper::queryTarget(str);
		scenen = graphicsView->initScene(m_lstTarget,4);
		if (scenen != NULL)
		{
			graphicsView->setScene(scenen);
		}
	return scenen;
}

QGraphicsScene* QTargetView::initScene(QList<TTarget*>& lstTargets,int cols)
{
	QString str;
	m_lstTarget.clear();
	for each (TTarget* t in lstTargets)
	{
		if (t->m_status.selected)
		{
			m_lstTarget.append(t);
		}
	}

	QGraphicsScene* scenen = NULL;
	if (m_lstTarget.count()>0)
	{
		if (cols>m_lstTarget.count())
		{
			DEBUG_PARA2("cols>m_lstTarget.count()  colse=",cols);
			return NULL;
		}
		scenen = graphicsView->scene();
		if (scenen)
		{
			scenen->deleteLater();
			scenen = NULL;
		}


		scenen = graphicsView->initScene(m_lstTarget,cols);
		if (scenen != NULL)
		{
			graphicsView->setScene(scenen);
		}
		graphicsView->ensureVisible(QRectF(0, 0, 0, 0));

	}
	return scenen;
}

void QTargetView::keyPressEvent(QKeyEvent *keyset)
{
	if (keyset->key() == Qt::Key_Escape){
		setWindowFlags(Qt::SubWindow);
		showNormal();
	}
}

void QTargetView::mouseDoubleClickEvent(QMouseEvent *event)
{
	//if (event->modifiers() & Qt::ControlModifier)
	{
		graphicsView->ensureVisible(QRectF(0, 0, 0, 0));
		if (isFullScreen())
		{
			setWindowFlags(Qt::SubWindow);
			showNormal();
		}else
		{
			setWindowFlags(Qt::Window);
			showFullScreen();
		}
	}
}

void GraphicsView::slotCanvasAction()
{
	centerOn(posMouse);
}
void GraphicsView::contextMenuEvent(QContextMenuEvent *event)
{
	QGraphicsItem *item = itemAt(event->pos());
	DEBUG_PARA1("void QTargetView::slotContextMenuRequested(const QPoint &pos)");
	if (NULL == item)
	{
		static QMenu *pMenu = NULL;
		if (pMenu == NULL)
		{
			pMenu = new QMenu(NULL);

			pMenu->addAction(QString::fromLocal8Bit("居中显示"),this,SLOT(slotCanvasAction()));
			pMenu->addSeparator();
			pMenu->addAction(QString::fromLocal8Bit("缩放重置"));

			QMenu *pChildMenu = new QMenu(pMenu);
			pChildMenu->setTitle(QStringLiteral("画布大小"));
			pChildMenu->addAction(QStringLiteral("背景颜色"));
			pMenu->addMenu(pChildMenu);

			QAction *act1 = new QAction(pChildMenu);
			act1->setText(QStringLiteral("网格显示"));
			act1->setIcon(QIcon("src/Me.png"));
			pChildMenu->addAction(act1);

		}
		posMouse = event->pos();
		pMenu->exec(event->globalPos());
	}else
	{
		QGraphicsView::contextMenuEvent(event);
	}
}