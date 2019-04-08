#ifndef UK_GRAPHIC_VIEW_H
#define UK_GRAPHIC_VIEW_H
#include "target_global.h"
#include "qtarget_graphic_item.h"

QT_BEGIN_NAMESPACE
class QLabel;
class QSlider;
class QToolButton;
QT_END_NAMESPACE

class QTargetView;

class GraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
	GraphicsView(QTargetView *v) : QGraphicsView(), view(v) { }
	QGraphicsScene* initScene();
	QGraphicsScene* initScene(QList<TTarget*>& lstTarget);
	QGraphicsScene* initScene(QList<TTarget*>& lstTarget,int cols);
protected:
#ifndef QT_NO_WHEELEVENT
    void wheelEvent(QWheelEvent *) Q_DECL_OVERRIDE;
#endif
	void contextMenuEvent(QContextMenuEvent *event);
private:
	QTargetView *view;
	QPoint     posMouse;
private slots:
	void slotCanvasAction();

};

class QTargetView : public QFrame
{
    Q_OBJECT
public:
	explicit QTargetView(QWidget *parent = 0);

    QGraphicsView *view() const;

public slots:
    void zoomIn(int level = 1);
    void zoomOut(int level = 1);
	void print();
	void rotateLeft();
	void rotateRight();
    void resetView();
    void setupMatrix();
public:
	void setPointerMod(bool isDrag=false);
	void setAntialiasing(bool bFlag);
	void setOpenGL(bool bFlag);	
	QGraphicsScene* initScene();
	QGraphicsScene* initScene(QList<TTarget*>& lstTargets,int cols);
	QList<TTarget*> getTargetList();
	void keyPressEvent(QKeyEvent *keyset);
	void mouseDoubleClickEvent(QMouseEvent *event);
private:
    GraphicsView *graphicsView;   
    QSlider *zoomSlider;
    QSlider *rotateSlider;
	RendererType      m_renderer;              // 当前renderer
	QGraphicsItem    *m_pngItem;               // png文件内容渲染项
	QGraphicsRectItem *m_backgroundItem;        // background渲染
	QGraphicsRectItem *m_outlineItem;           // outline渲染
	QGraphicsRectItem *m_selectRect;            // 选取框
	QList<TTarget*>   m_lstTarget;
};

#endif // UK_GRAPHIC_VIEW_H
