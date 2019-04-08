#ifndef UK_GRAPHIC_ROOM_H
#define UK_GRAPHIC_ROOM_H

#include "target_structs.h"
#include "target_global.h"


//图元基类
class ItemBase : public QGraphicsItem
{
public:
	enum { Type = UserType + 1 };
	ItemBase(int size, int x, int y);
	virtual ~ItemBase();
	virtual QRectF boundingRect() const Q_DECL_OVERRIDE;
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) Q_DECL_OVERRIDE;
protected:
	virtual ItemBase *createNew(int size, int x, int y) = 0;
	virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) Q_DECL_OVERRIDE;
	virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
	virtual void hoverMoveEvent(QGraphicsSceneHoverEvent *event) Q_DECL_OVERRIDE;
	virtual void mousePressEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
	virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
	virtual void keyPressEvent(QKeyEvent *event) Q_DECL_OVERRIDE;
	virtual void wheelEvent(QGraphicsSceneWheelEvent *event) Q_DECL_OVERRIDE;
	virtual int type() const Q_DECL_OVERRIDE;
	bool isInResizeArea(const QPointF &pos);

	static void duplicateSelectedItems(QGraphicsScene *scene);
	static void deleteSelectedItems(QGraphicsScene *scene);
	static void growSelectedItems(QGraphicsScene *scene);
	static void shrinkSelectedItems(QGraphicsScene *scene);

	int       m_size;
	QTime m_startTime;
	bool m_isResizing;
};
//============================================================================//
//                                 发光圆圈                                   //
//============================================================================//
class CircleItem : public ItemBase
{
public:
	CircleItem(int size, int x, int y);
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) Q_DECL_OVERRIDE;
protected:
	virtual ItemBase *createNew(int size, int x, int y) Q_DECL_OVERRIDE;

	QColor m_color;
};
//============================================================================//
//                               靶标                                         //
//============================================================================//
class QTargetItem : public QGraphicsItem
{
public:
	QTargetItem(TTarget* pTarget, int row, int col);
	QRectF boundingRect() const Q_DECL_OVERRIDE;
	QPainterPath shape()  const Q_DECL_OVERRIDE;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget) Q_DECL_OVERRIDE;

protected:
	void mousePressEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
	void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) Q_DECL_OVERRIDE;
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)  Q_DECL_OVERRIDE;
private:
	QPointF         m_pos;
	QSizeF          m_size;
	QColor          m_pen_color;
	QColor          m_brush_color;
	static QPixmap         *m_pix[4];
	TTarget         *m_target;

	int             m_row;
	int             m_col;
};

//============================================================================//
//                               按钮                                         //
//============================================================================//
class QPixButtonItem : public QGraphicsPixmapItem
{
public:
	QPixButtonItem(const QString&strText);
protected:
	void mousePressEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
	void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) Q_DECL_OVERRIDE;
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)  Q_DECL_OVERRIDE;

	void hoverEnterEvent(QGraphicsSceneHoverEvent *event) Q_DECL_OVERRIDE;
	void hoverMoveEvent(QGraphicsSceneHoverEvent *event)  Q_DECL_OVERRIDE;
	void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) Q_DECL_OVERRIDE;
private:
	QPointF         m_pos;
	QSizeF          m_size;
	QColor          m_pen_color;
	QColor          m_brush_color;
	QGraphicsTextItem *m_text;
	bool             m_pressed;
	static QPixmap    *m_pix[4];
};

//信号灯
class SignalLight :public QGraphicsObject
{
public:
	SignalLight(QSizeF clrSize, QPointF posOfParent, QGraphicsObject* parent);

	int    type() const Q_DECL_OVERRIDE{ return 4; }
	QRectF  boundingRect() const Q_DECL_OVERRIDE;
	void    paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) Q_DECL_OVERRIDE;

	enum  { ANISTYLE_ROTATE = 4, ANISTYLE_SCALE = 2, ANISTYLE_OPACTIY = 1, ANISTYLE_NONE = 0 };
	void   setScaleAnimation(qreal scale1, qreal scale2);
	void   setOpacityAnimation(qreal opacity1, qreal opacity2);
	void   clearAnimation(quint16 style);

	enum{ STATUS_RUNNING = 0, STATUS_ERROR = 1, STATUS_PAUSE = 2, STATUS_UNKNOWN = 3 };
	void    setStatus(int status);
	int     getStatus();

private:
	int     m_status;
	QSizeF  m_size;
	QColor  m_color;
	quint16 m_style;
	bool    m_animation;
	QParallelAnimationGroup *animation_group;
	QPropertyAnimation*     scale_animation;
	QPropertyAnimation*     opacity_animation;
	QPropertyAnimation*     rotate_animation;
};


//============================================================================//
//                               靶标                                         //
//============================================================================//
class QGraphicTarget : public QGraphicsObject
{
public:
	static void initTargetPix();
	QGraphicTarget(TTarget* pTarget, int row, int col);
	QRectF boundingRect() const Q_DECL_OVERRIDE;
	QPainterPath shape()  const Q_DECL_OVERRIDE;
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *item, QWidget *widget) Q_DECL_OVERRIDE;
	TTarget* getTarget();
	virtual int type() const;
protected:
	void mousePressEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event) Q_DECL_OVERRIDE;
	void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) Q_DECL_OVERRIDE;
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)  Q_DECL_OVERRIDE;
private:
	QPointF         m_pos;
	QSizeF          m_size;
	QColor          m_pen_color;
	QColor          m_brush_color;
	enum{PIX_INDEX_TARGET_CHEST    = 0,//胸环靶
		 PIX_INDEX_TARGET_PPC     = 1,//PPC靶
		 PIX_INDEX_TARGET_PART    = 2,//部位靶
		 PIX_INDEX_TARGET_HUMAN   = 3,//人型靶
		 PIX_INDEX_TARGET_HOSTAGE = 4,//人质靶
		 PIX_INDEX_WIFI_OK   =10,
		 PIX_INDEX_WIFI_ERROR=11,
		 PIX_INDEX_BATTERY_CHARGING=12,
		 PIX_INDEX_BATTERY_EMPTY=14,
		 PIX_INDEX_BATTERY_LOWER25=15,
		 PIX_INDEX_BATTERY_LOWER50 =16,
		 PIX_INDEX_BATTERY_LOWER75 =17,
		 PIX_INDEX_BATTERY_LOWER100=18,
		 PIX_INDEX_CONTROL=21,
		 PIX_INDEX_SELECTED=22,
		 PIX_INDEX_UNSELECT=23,
		 PIX_INDEX_MAX = 32
	};
	static QPixmap   *m_pix[PIX_INDEX_MAX];

	enum{ ACTION_INDEX_MAX = 32 };
	static QAction   *m_action[ACTION_INDEX_MAX];
	TTarget         *m_target;
	SignalLight     *signalRunning;

	int             m_row;
	int             m_col;
	bool            m_pressed;
};

#endif //
