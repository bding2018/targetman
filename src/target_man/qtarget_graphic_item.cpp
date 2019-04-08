#include "qtarget_graphic_item.h"
#include "mainwindow.h"
#include "suffix_screen.h"
#include "target_browser_delegate.h"
//============================================================================//
//                                  ItemBase                                  //
//============================================================================//

ItemBase::ItemBase(int size, int x, int y) : m_size(size), m_isResizing(false)
{
	setFlag(QGraphicsItem::ItemIsMovable, true);//可移动
	setFlag(QGraphicsItem::ItemIsSelectable, true);//可选择
	setFlag(QGraphicsItem::ItemIsFocusable, true);//可聚焦
	setAcceptHoverEvents(true);//接受鼠标滑过事件
	setPos(x, y);//设置位置
	m_startTime = QTime::currentTime();//当前时间记录
}

ItemBase::~ItemBase()
{

}

QRectF ItemBase::boundingRect() const
{
	//外接矩形框
	return QRectF(-m_size / 2, -m_size / 2, m_size, m_size);
}

void ItemBase::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *)
{
	//选中状态绘制外框和变形控制区
	if (option->state & QStyle::State_Selected)
	{
		painter->setRenderHint(QPainter::Antialiasing, true);
		if (option->state & QStyle::State_HasFocus)
			painter->setPen(Qt::yellow);
		else
			painter->setPen(Qt::white);
		painter->drawRect(boundingRect());

		painter->drawLine(m_size / 2 - 9, m_size / 2, m_size / 2, m_size / 2 - 9);
		painter->drawLine(m_size / 2 - 6, m_size / 2, m_size / 2, m_size / 2 - 6);
		painter->drawLine(m_size / 2 - 3, m_size / 2, m_size / 2, m_size / 2 - 3);

		painter->setRenderHint(QPainter::Antialiasing, false);
	}
}

//响应上下文菜单
void ItemBase::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
	if (!isSelected() && scene())
	{
		scene()->clearSelection();
		setSelected(true);
	}

	QMenu menu;
	QAction *delAction = menu.addAction("Delete"); //删除
	QAction *newAction = menu.addAction("New");    //新建
	QAction *growAction = menu.addAction("Grow");    //放大
	QAction *shrinkAction = menu.addAction("Shrink");   //缩小

	QAction *selectedAction = menu.exec(event->screenPos());

	if (selectedAction == delAction)
		deleteSelectedItems(scene());
	else if (selectedAction == newAction)
		duplicateSelectedItems(scene());
	else if (selectedAction == growAction)
		growSelectedItems(scene());
	else if (selectedAction == shrinkAction)
		shrinkSelectedItems(scene());
}

void ItemBase::duplicateSelectedItems(QGraphicsScene *scene)
{
	if (!scene)
		return;

	QList<QGraphicsItem *> selected;
	selected = scene->selectedItems();

	foreach(QGraphicsItem *item, selected) {
		ItemBase *itemBase = qgraphicsitem_cast<ItemBase *>(item);
		if (itemBase)
			scene->addItem(itemBase->createNew(itemBase->m_size, itemBase->pos().x() + itemBase->m_size, itemBase->pos().y()));
	}
}

void ItemBase::deleteSelectedItems(QGraphicsScene *scene)
{
	if (!scene)
		return;

	QList<QGraphicsItem *> selected;
	selected = scene->selectedItems();

	foreach(QGraphicsItem *item, selected) {
		ItemBase *itemBase = qgraphicsitem_cast<ItemBase *>(item);
		if (itemBase)
			delete itemBase;
	}
}

void ItemBase::growSelectedItems(QGraphicsScene *scene)
{
	if (!scene)
		return;

	QList<QGraphicsItem *> selected;
	selected = scene->selectedItems();

	foreach(QGraphicsItem *item, selected) {
		ItemBase *itemBase = qgraphicsitem_cast<ItemBase *>(item);
		if (itemBase) {
			itemBase->prepareGeometryChange();
			itemBase->m_size *= 2;
			if (itemBase->m_size > MAX_ITEM_SIZE)
				itemBase->m_size = MAX_ITEM_SIZE;
		}
	}
}

void ItemBase::shrinkSelectedItems(QGraphicsScene *scene)
{
	if (!scene)
		return;

	QList<QGraphicsItem *> selected;
	selected = scene->selectedItems();

	foreach(QGraphicsItem *item, selected) {
		ItemBase *itemBase = qgraphicsitem_cast<ItemBase *>(item);
		if (itemBase) {
			itemBase->prepareGeometryChange();
			itemBase->m_size /= 2;
			if (itemBase->m_size < MIN_ITEM_SIZE)
				itemBase->m_size = MIN_ITEM_SIZE;
		}
	}
}

void ItemBase::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	if (m_isResizing) {
		int dx = int(2.0 * event->pos().x());
		int dy = int(2.0 * event->pos().y());
		prepareGeometryChange();
		m_size = (dx > dy ? dx : dy);
		if (m_size < MIN_ITEM_SIZE)
			m_size = MIN_ITEM_SIZE;
		else if (m_size > MAX_ITEM_SIZE)
			m_size = MAX_ITEM_SIZE;
	}
	else {
		QGraphicsItem::mouseMoveEvent(event);
	}
}

void ItemBase::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
	if (m_isResizing || (isInResizeArea(event->pos()) && isSelected()))
		setCursor(Qt::SizeFDiagCursor);
	else
		setCursor(Qt::ArrowCursor);
	QGraphicsItem::hoverMoveEvent(event);
}

void ItemBase::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	static qreal z = 0.0;
	setZValue(z += 1.0);
	if (event->button() == Qt::LeftButton && isInResizeArea(event->pos())) {
		m_isResizing = true;
	}
	else {
		QGraphicsItem::mousePressEvent(event);
	}
}

void ItemBase::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	if (event->button() == Qt::LeftButton && m_isResizing) {
		m_isResizing = false;
	}
	else {
		QGraphicsItem::mouseReleaseEvent(event);
	}
}

void ItemBase::keyPressEvent(QKeyEvent *event)
{
	switch (event->key()) {
	case Qt::Key_Delete:
		deleteSelectedItems(scene());
		break;
	case Qt::Key_Insert:
		duplicateSelectedItems(scene());
		break;
	case Qt::Key_Plus:
		growSelectedItems(scene());
		break;
	case Qt::Key_Minus:
		shrinkSelectedItems(scene());
		break;
	default:
		QGraphicsItem::keyPressEvent(event);
		break;
	}
}

void ItemBase::wheelEvent(QGraphicsSceneWheelEvent *event)
{
	prepareGeometryChange();
	m_size = int(m_size * exp(-event->delta() / 600.0));
	if (m_size > MAX_ITEM_SIZE)
		m_size = MAX_ITEM_SIZE;
	else if (m_size < MIN_ITEM_SIZE)
		m_size = MIN_ITEM_SIZE;
}

int ItemBase::type() const
{
	return Type;
}


bool ItemBase::isInResizeArea(const QPointF &pos)
{
	return (-pos.y() < pos.x() - m_size + 9);
}

//============================================================================//
//                                 CircleItem                                 //
//============================================================================//

CircleItem::CircleItem(int size, int x, int y) : ItemBase(size, x, y)
{
	m_color = QColor::fromHsv(rand() % 360, 255, 255);
}

void CircleItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	int dt = m_startTime.msecsTo(QTime::currentTime());

	qreal r0 = 0.5 * m_size * (1.0 - exp(-0.001 * ((dt + 3800) % 4000)));
	qreal r1 = 0.5 * m_size * (1.0 - exp(-0.001 * ((dt + 0) % 4000)));
	qreal r2 = 0.5 * m_size * (1.0 - exp(-0.001 * ((dt + 1800) % 4000)));
	qreal r3 = 0.5 * m_size * (1.0 - exp(-0.001 * ((dt + 2000) % 4000)));

	if (r0 > r1)
		r0 = 0.0;
	if (r2 > r3)
		r2 = 0.0;

	QPainterPath path;
	//圆圈1
	path.moveTo(r1, 0.0);
	path.arcTo(-r1, -r1, 2 * r1, 2 * r1, 0.0, 360.0);

	//圆圈2
	path.lineTo(r0, 0.0);
	path.arcTo(-r0, -r0, 2 * r0, 2 * r0, 0.0, -360.0);

	//闭合两圈之间的部分
	path.closeSubpath();

	//大环
	path.moveTo(r3, 0.0);
	path.arcTo(-r3, -r3, 2 * r3, 2 * r3, 0.0, 360.0);
	path.lineTo(r0, 0.0);
	path.arcTo(-r2, -r2, 2 * r2, 2 * r2, 0.0, -360.0);
	path.closeSubpath();


	//绘制
	painter->setRenderHint(QPainter::Antialiasing, true);
	painter->setBrush(QBrush(m_color));
	painter->setPen(Qt::NoPen);
	painter->drawPath(path);
	painter->setBrush(Qt::NoBrush);
	painter->setPen(Qt::SolidLine);
	painter->setRenderHint(QPainter::Antialiasing, false);

	ItemBase::paint(painter, option, widget);
}

ItemBase *CircleItem::createNew(int size, int x, int y)
{
	return new CircleItem(size, x, y);
}

//============================================================================//
//                                靶标状态                                   //
//============================================================================//
QPixmap* QTargetItem::m_pix[4];
QTargetItem::QTargetItem(TTarget* pTarget, int row, int col)
{
	m_target = pTarget;
	m_row = row;
	m_col = col;
	memset(m_pix, NULL, 4 * sizeof(QPixmap*));
	m_pix[0] = new QPixmap(":/targets/target1.png");
	m_pix[1] = new QPixmap(":/targets/target2.png");
	m_pix[2] = new QPixmap(":/targets/target3.png");
	m_pix[3] = new QPixmap(":/targets/target4.png");
    //设置对象Z轴坐标
	setZValue(ZVALUE_TARGETITEM);

	//对象可选择，可移动
	//setFlags(ItemIsSelectable | ItemIsMovable);
	setFlags(ItemIsSelectable);

   //响应鼠标滑过事件
	setAcceptHoverEvents(true);

	QString str = QString(QString::fromLocal8Bit("这是第<font color='red'>%1个靶标</font>")).arg(m_row*10+m_col);
	this->setToolTip(str);
}
QRectF QTargetItem::boundingRect() const
{
  return QRectF(0,0,TARGET_WIDTH,TARGET_HEIGHT);
}
QPainterPath QTargetItem::shape() const
{
	QPainterPath path;
	path.addRect(0, 0, TARGET_WIDTH, TARGET_HEIGHT);
	return path;
}

void QTargetItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget)
{
	Q_UNUSED(widget);
	//QColor fillColor = UK_ROOM_COLOR[m_roomInfo->status];

	QColor fillColor(0,255,0,100);

	bool flag = false;

	static QPixmap* pixSelected = NULL;
	if (pixSelected == NULL)
		pixSelected = new QPixmap(":/targets/select.png");

	static QPixmap* pixUnselect = NULL;
	if (pixUnselect == NULL)
		pixUnselect = new  QPixmap(":/targets/unselect.png");
	
	QPixmap* pixNow = pixUnselect;

	if (item->state & QStyle::State_Selected)
	{
		fillColor = fillColor.dark(150);     //选中时加深颜色
		flag = true;
		pixNow = pixSelected;
	}


	if (item->state & QStyle::State_MouseOver)//鼠标划过时颜色变浅
	{
		fillColor = fillColor.light(125);
		flag = true;
	}


	//缩放比例品质
	const qreal lod = item->levelOfDetailFromTransform(painter->worldTransform());
	
	if (lod < 0.125)
	{//最小比例仅仅绘制一个矩形填充
		painter->fillRect(QRectF(14,14,100,100), fillColor);
		return;
	}else if (lod < 0.2)
	{
		//绘制一个矩形填充
		QBrush b = painter->brush();
		painter->setBrush(fillColor);
		painter->drawRect(QRectF(4,4,120,120));
		painter->setBrush(b);
		return;
	}	
	painter->drawPixmap(0, 0, TARGET_WIDTH,TARGET_HEIGHT,*(m_pix[m_row%4]));

	// Draw text
	int yPos  = 22;
	int xPos  = 40;
	int yDelt = 14;
	if (lod >= 1) {//绘制信息
		QFont font("宋体",6);
		font.setStyleStrategy(QFont::ForceOutline);
		painter->setFont(font);
		painter->save();
		font.setBold(true);
		
		painter->scale(0.5, 0.5);//先放大然后开始绘制,防止失真
		int number = m_row*4 + m_col;
		QPen p;
		p.setColor(Qt::black);
		p.setStyle(Qt::SolidLine);
		painter->setPen(p);
		
		painter->drawText(xPos,yPos, QString::number(number));
		painter->drawText(xPos, yPos + yDelt * 1, QString(QString::fromLocal8Bit("第6发")));
		painter->drawText(xPos, yPos + yDelt * 2, QString(QString::fromLocal8Bit("共60发")));
		painter->drawText(xPos, yPos + yDelt * 3, QString(QString::fromLocal8Bit("9.8环")));
		painter->drawText(xPos, yPos + yDelt * 4, QString(QString::fromLocal8Bit("共108.5环")));
		painter->restore();
	}

	if (flag)
	{
		QBrush b = painter->brush();
		painter->setBrush(QBrush(fillColor.dark(item->state & QStyle::State_Sunken ? 120 : 100)));

		painter->drawRect(QRect(0, 0, TARGET_WIDTH, TARGET_HEIGHT));
		painter->setBrush(b);
	}

	if (pixNow)
		painter->drawPixmap(1, 1, ICON_WIDTH, ICON_HEIGHT, *pixNow);


}
void QTargetItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		MainWindow::getInstance()->updateProperty(m_target);
	}
	QGraphicsItem::mousePressEvent(event);
}
void QTargetItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{


	QGraphicsItem::mouseMoveEvent(event);
}
void QTargetItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
	}

	QGraphicsItem::mouseReleaseEvent(event);
}
void QTargetItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
	QGraphicsItem::contextMenuEvent(event);
	QMenu *roomMenu = MainWindow::getInstance()->getContextMenu(m_target);
	if (roomMenu)
	{
		roomMenu->exec(event->screenPos());
	}

}
void QTargetItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
	}
	
	QGraphicsItem::mouseDoubleClickEvent(event);
}

//============================================================================//
//                                靶标按钮                                  //
//============================================================================//
QPixmap* QPixButtonItem::m_pix[4];
QPixButtonItem::QPixButtonItem(const QString&strText)
{
	memset(m_pix, NULL, 4 * sizeof(QPixmap*));
	m_pix[0] = new QPixmap(":/buttons/btn_green.png");
	m_pix[1] = new QPixmap(":/buttons/btn_blue.png");
	m_pix[2] = new QPixmap(":/buttons/btn_sky.png");
	m_pix[3] = new QPixmap(":/buttons/btn_black.png");
	//设置对象Z轴坐标
	setZValue(ZVALUE_TARGETITEM);

	//对象可选择，可移动
	//setFlags(ItemIsSelectable | ItemIsMovable);

	setFlags(ItemIsSelectable);

	//响应鼠标滑过事件
	setAcceptHoverEvents(true);

	m_pressed = false;

	m_text = new QGraphicsTextItem(strText, this);
	QFont f = m_text->font();
	f.setPointSize(7);
	m_text->setFont(f);

	setPixmap(m_pix[0]->scaled(BUTTON_WIDTH, BUTTON_HEIGHT));
}

void QPixButtonItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)m_pressed = true;
	QGraphicsPixmapItem::mousePressEvent(event);

	setPixmap(m_pix[1]->scaled(BUTTON_WIDTH, BUTTON_HEIGHT));
}
void QPixButtonItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	QGraphicsPixmapItem::mouseMoveEvent(event);
}
void QPixButtonItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	qDebug() << "QPixButtonItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)";
	//QGraphicsPixmapItem::mouseReleaseEvent(event);
	setPixmap(m_pix[2]->scaled(BUTTON_WIDTH, BUTTON_HEIGHT));

	if (event->button() == Qt::LeftButton && m_pressed)
	{
		m_pressed = false;
		//QMessageBox::information(MainWindow::getInstance(), QString::fromLocal8Bit("test"), m_text->toPlainText());
		
		static QSuffixScreen* w = NULL;
		if (w == NULL)
		{
			w = new QSuffixScreen(NULL);

		}

		if (w->isHidden())
		{
			w->show();
		}
	}
}
void QPixButtonItem::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
	QGraphicsPixmapItem::contextMenuEvent(event);
}
void QPixButtonItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{

	}
	QGraphicsPixmapItem::mouseDoubleClickEvent(event);
}

void QPixButtonItem::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
	QGraphicsPixmapItem::hoverEnterEvent(event);
	QFont f = m_text->font();
	f.setUnderline(true);
	f.setItalic(true);
	m_text->setFont(f);
	m_pressed = false;
	setPixmap(m_pix[2]->scaled(BUTTON_WIDTH, BUTTON_HEIGHT));
}
void QPixButtonItem::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
	QGraphicsPixmapItem::hoverMoveEvent(event);
}
void QPixButtonItem::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
	m_pressed = false;
	QGraphicsPixmapItem::hoverLeaveEvent(event);
	setPixmap(m_pix[0]->scaled(BUTTON_WIDTH, BUTTON_HEIGHT));
	QFont f = m_text->font();
	f.setUnderline(false);
	f.setItalic(false);
	m_text->setFont(f);
}


//============================================================================//
//                                靶标状态                                   //
//============================================================================//
QPixmap* QGraphicTarget::m_pix[QGraphicTarget::PIX_INDEX_MAX];
QAction   *QGraphicTarget::m_action[ACTION_INDEX_MAX];
void QGraphicTarget::initTargetPix()
{
	memset(m_pix, 0, sizeof(QPixmap*)*PIX_INDEX_MAX);

	m_pix[PIX_INDEX_TARGET_CHEST]   = new QPixmap(":/targets/target_3.png");
	m_pix[PIX_INDEX_TARGET_PART]    = new QPixmap(":/targets/target_1.png");
	m_pix[PIX_INDEX_TARGET_HUMAN]   = new QPixmap(":/targets/target_2.png");
	m_pix[PIX_INDEX_TARGET_HOSTAGE] = new QPixmap(":/targets/target_4.png");
	m_pix[PIX_INDEX_TARGET_PPC]     = new QPixmap(":/targets/target_5.png");

	QStringList pixBigFiles;
	pixBigFiles << "chest.png";
	pixBigFiles << "part.png";
	pixBigFiles << "human.png";
	pixBigFiles << "hostage.png";
	pixBigFiles << "ppc.png";
	int startIndex = PIX_INDEX_TARGET_HOSTAGE;
	for each (QString strFile in pixBigFiles)
	{
		QString absFile = QString("%1/res/%2").arg(QString::fromLocal8Bit(qgetenv("WDH_ROOT"))).arg(strFile);
		if (QFile::exists(absFile))
		{
			++startIndex;
			m_pix[startIndex] = new QPixmap(absFile);
		}
	}


	m_pix[PIX_INDEX_WIFI_OK]         = new QPixmap(":/targets/wif1.png");
	m_pix[PIX_INDEX_WIFI_ERROR]       = new QPixmap(":/targets/wifi0.png");

	m_pix[PIX_INDEX_BATTERY_CHARGING] = new QPixmap(":/battery/battery00.png");
	m_pix[PIX_INDEX_BATTERY_EMPTY] = new QPixmap(":/battery/battery0.png");
	m_pix[PIX_INDEX_BATTERY_LOWER25] = new QPixmap(":/battery/battery1.png");
	m_pix[PIX_INDEX_BATTERY_LOWER50] = new QPixmap(":/battery/battery2.png");
	m_pix[PIX_INDEX_BATTERY_LOWER75] = new QPixmap(":/battery/battery3.png");
	m_pix[PIX_INDEX_BATTERY_LOWER100] = new QPixmap(":/battery/battery4.png");

	m_pix[PIX_INDEX_SELECTED] = new QPixmap(":/targets/select.png");
	m_pix[PIX_INDEX_UNSELECT] = new QPixmap(":/targets/unselect.png");

	memset(m_action, 0,sizeof(QAction*)*ACTION_INDEX_MAX);

}
int QGraphicTarget::type() const
{
	return gitGRAPHIC_TARGET;
}

TTarget* QGraphicTarget::getTarget()
{
	return m_target;
}

QGraphicTarget::QGraphicTarget(TTarget* pTarget, int row, int col) :QGraphicsObject(NULL)
{
	if (pTarget)
	{
		m_target = pTarget;
		pTarget->graphicTarget = this;
	}

	m_row = row;
	m_col = col;
	m_pressed = false;
	//设置对象Z轴坐标
	setZValue(ZVALUE_TARGETITEM);

	//对象可选择，可移动
	//setFlags(ItemIsSelectable | ItemIsMovable);
	setFlags(ItemIsSelectable);

	//响应鼠标滑过事件
	setAcceptHoverEvents(true);

	QString str = QString(QString::fromLocal8Bit("这是第<font color='red'>%1个靶标</font>")).arg(m_row * 10 + m_col);
	this->setToolTip(str);

	signalRunning = new SignalLight(QSizeF(ICON_WIDTH, ICON_WIDTH), QPointF(ICON_WIDTH * 2,3), this);
	signalRunning->setStatus(row%3);
}
QRectF QGraphicTarget::boundingRect() const
{
	return QRectF(0, 0, TARGET_WIDTH, TARGET_HEIGHT);
}
QPainterPath QGraphicTarget::shape() const
{
	QPainterPath path;
	path.addRect(0, 0, TARGET_WIDTH, TARGET_HEIGHT);
	return path;
}

void QGraphicTarget::paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget)
{
	Q_UNUSED(widget);

	QColor fillColor(0, 255, 0, 100);

	bool flag = false;

	QPixmap* pixNow      = m_pix[PIX_INDEX_UNSELECT - m_target->m_status.selected];
	QPixmap* pixBattery  = m_pix[PIX_INDEX_BATTERY_CHARGING + m_target->battery%5];
	QPixmap* pixWifi     = m_pix[PIX_INDEX_WIFI_ERROR - m_target->m_status.online];
	QPixmap* pixTarget   = m_pix[PIX_INDEX_TARGET_PPC];

	if (item->state & QStyle::State_Selected)
	{
		fillColor = fillColor.dark(150);     //选中时加深颜色
		flag = true;
	}


	if (item->state & QStyle::State_MouseOver)//鼠标划过时颜色变浅
	{
		fillColor = fillColor.light(125);
		flag = true;
	}


	//缩放比例品质
	const qreal lod = item->levelOfDetailFromTransform(painter->worldTransform());
	if (lod < 0.125)
	{//最小比例仅仅绘制一个矩形填充
		painter->fillRect(QRectF(14, 14, 100, 100), fillColor);
		return;
	}else if (lod < 0.2)
	{
		//绘制一个矩形填充
		QBrush b = painter->brush();
		painter->setBrush(fillColor);
		painter->drawRect(QRectF(4, 4, 120, 120));
		painter->setBrush(b);
		return;
	}

	int targetType = m_target->shapeType -  QDelegateTarget::MANCONFIG_CLASS - 1;
	int indx  = abs(targetType) % 5;
	pixTarget = m_pix[indx];
	if (lod>3 && m_pix[indx+5]) //放大倍数大于4则使用大图
	{
		pixTarget = m_pix[indx + 5];
	}
	painter->drawPixmap(0, 0, TARGET_WIDTH, TARGET_HEIGHT, *pixTarget);

	// Draw text
	int yPos = 30;
	int xPos = 10;
	int yDelt = 18;
	if (lod >= 1) {//绘制信息
		QFont font("宋体", 8);
		font.setStyleStrategy(QFont::ForceOutline);
		painter->setFont(font);
		painter->save();
		font.setBold(true);

		painter->scale(0.5, 0.5);//先放大然后开始绘制,防止失真
		QPen p;
		p.setColor(Qt::black);
		p.setStyle(Qt::SolidLine);
		painter->setPen(p);

		painter->drawText(xPos, yPos, QString::number(m_target->devNo));
		painter->drawText(xPos, yPos + yDelt * 1, QString(QString::fromLocal8Bit("第6发")));
		painter->drawText(xPos, yPos + yDelt * 2, QString(QString::fromLocal8Bit("共60发")));
		painter->drawText(xPos, yPos + yDelt * 3, QString(QString::fromLocal8Bit("9.8环")));
		painter->drawText(xPos, yPos + yDelt * 4, QString(QString::fromLocal8Bit("共108.5环")));
		painter->drawText(xPos, yPos + yDelt * 5, QString(QString::fromLocal8Bit("队员5")));
		painter->restore();
	}

	if (flag)
	{
		QBrush b = painter->brush();
		painter->setBrush(QBrush(fillColor.dark(item->state & QStyle::State_Sunken ? 120 : 100)));

		painter->drawRect(QRect(0, 0, TARGET_WIDTH, TARGET_HEIGHT));
		painter->setBrush(b);
	}

	if (pixNow)
		painter->drawPixmap(2, 1, ICON_WIDTH, ICON_HEIGHT, *pixNow);

	if (pixBattery)
		painter->drawPixmap(ICON_WIDTH * 3 + 2, 1, ICON_WIDTH*1.6, ICON_HEIGHT, *pixBattery);

	if (pixWifi)
		painter->drawPixmap(ICON_WIDTH * 4.2 + 4, 1, ICON_WIDTH, ICON_HEIGHT, *pixWifi);

	for each (QPointF p in m_target->m_points)
	{
		painter->setPen(Qt::NoPen);
		painter->setBrush(QBrush(Qt::red));
		painter->drawEllipse(p.x() - 1,p.y() - 1,2,2);
	}

	painter->setPen(Qt::NoPen);
	painter->setBrush(QBrush(Qt::yellow));
	painter->drawEllipse(64 - 1, 31 - 1, 2, 2);
}
void QGraphicTarget::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		m_pressed = true;
		//MainWindow::getInstance()->updateProperty(m_target);
	}
	QGraphicsItem::mousePressEvent(event);
}
void QGraphicTarget::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{


	QGraphicsItem::mouseMoveEvent(event);
}
void QGraphicTarget::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		if (m_pressed)
		{
			m_target->m_status.selected = !m_target->m_status.selected;
		}
	}
	m_pressed = false;
	update();
	QGraphicsItem::mouseReleaseEvent(event);
}
void QGraphicTarget::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
	QGraphicsItem::contextMenuEvent(event);
	static QMenu *popMenu = NULL;
	if (popMenu == NULL)
	{
		popMenu = new QMenu(QString::fromLocal8Bit("标靶"));
		int i = 0;
		int num = sizeof(action_title) / sizeof(char*);
		for (i = 0; i < num; i++)
		{

			QString strTitle = QString::fromLocal8Bit(action_title[i]);
			m_action[i] = NULL;
			if (strTitle.isEmpty())
			{
				popMenu->addSeparator();
			}else
			{
				m_action[i] = popMenu->addAction(strTitle,QSuffixScreen::getInstance(), SLOT(slotTargetItemAction()));
			}

		}
	}
	
	if (popMenu)
	{
		QSuffixScreen::getInstance()->setSelectTarget(this);
		popMenu->exec(event->screenPos());
	}



}
void QGraphicTarget::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
	}

	QGraphicsItem::mouseDoubleClickEvent(event);
}


//-----------------------------------信号灯-----------------------------------------------------------------
SignalLight::SignalLight(QSizeF clrSize, QPointF posOfParent, QGraphicsObject* parent) : QGraphicsObject(parent)
{
	animation_group = NULL;
	opacity_animation = NULL;
	rotate_animation = NULL;
	scale_animation = NULL;
	rotate_animation = NULL;
	m_size = clrSize;
	m_color = Qt::red;
	setVisible(true);
	setPos(posOfParent);

}
//设置信号状态
void SignalLight::setStatus(int status)
{
	m_status = status;
	switch (status)
	{
	case STATUS_RUNNING:
		m_color = Qt::red;
		setOpacityAnimation(0.2, 1.0);
		setScaleAnimation(0.5, 1.5);
		break;
	case STATUS_PAUSE:
		m_color = Qt::darkGray;
		clearAnimation(ANISTYLE_SCALE | ANISTYLE_OPACTIY);
		break;
	case STATUS_ERROR:
		clearAnimation(ANISTYLE_SCALE);
		m_color = Qt::yellow;
		break;
	default:
		break;
	}
}
//获取信号状态
int    SignalLight::getStatus()
{
	return m_status;
}



void   SignalLight::setScaleAnimation(qreal scale1, qreal scale2)
{
	if (animation_group == NULL)
		animation_group = new QParallelAnimationGroup(this);


	if (scale_animation == NULL)
	{
		scale_animation = new QPropertyAnimation(this, "scale");
		scale_animation->setStartValue(scale1);
		scale_animation->setEndValue(scale2);
		scale_animation->setEasingCurve(QEasingCurve::SineCurve);
		scale_animation->setDuration(2000);
	}

	if (animation_group->indexOfAnimation(scale_animation) < 0)
		animation_group->addAnimation(scale_animation);

	if (animation_group->state() != QAbstractAnimation::Running)
	{
		animation_group->setLoopCount(-1);
		animation_group->start();
	}
}

void   SignalLight::setOpacityAnimation(qreal opacity1, qreal opacity2)
{
	if (animation_group == NULL)
		animation_group = new QParallelAnimationGroup(this);

	if (opacity_animation == NULL)
	{
		opacity_animation = new QPropertyAnimation(this, "opacity");
		opacity_animation->setStartValue(1);
		opacity_animation->setEndValue(0);
		opacity_animation->setEasingCurve(QEasingCurve::SineCurve);
		opacity_animation->setDuration(1000);
	}

	if (animation_group->indexOfAnimation(opacity_animation) < 0)
		animation_group->addAnimation(opacity_animation);

	if (animation_group->state() != QAbstractAnimation::Running)
	{
		animation_group->setLoopCount(-1);
		animation_group->start();
	}

	if (animation_group->animationCount() == 0)
	{
		animation_group->stop();
	}

}

void   SignalLight::clearAnimation(quint16 style)
{
	if (animation_group)
	{
		if (style & ANISTYLE_SCALE)animation_group->removeAnimation(scale_animation);

		if (style & ANISTYLE_ROTATE)animation_group->removeAnimation(rotate_animation);

		if (style & ANISTYLE_OPACTIY)animation_group->removeAnimation(opacity_animation);
	}
}

void   SignalLight::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
	Q_UNUSED(option);
	Q_UNUSED(widget);
	painter->setPen(Qt::NoPen);
	painter->setBrush(Qt::darkGray);
	painter->drawEllipse(-m_size.width() / 2.0 + 1,//偏移两像素做出阴影效果
		-m_size.height() / 2.0 + 1,
		m_size.width(),
		m_size.height());

	painter->setPen(Qt::NoPen);
	painter->setBrush(QBrush(m_color));
	painter->drawEllipse(-m_size.width() / 2.0,
		-m_size.height() / 2.0,
		m_size.width(),
		m_size.height());
}

QRectF  SignalLight::boundingRect() const
{
	return QRectF(-m_size.width() / 2.0-2, -m_size.height() / 2.0-2, m_size.width()+4, m_size.height()+4);
}
