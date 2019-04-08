#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "target_sqlite.h"
#include "qtpropertymanager.h"
#include "qteditorfactory.h"
#include "qttreepropertybrowser.h"
void MainWindow::slotCreateServer()
{

}
void MainWindow::updateServer(QString, int)
{

}

void MainWindow::slotTargetItemAction(QAction* action)
{
	DEBUG_PARA1("slotTargetItemAction(QAction* action)");
	if (m_target_action[etmCount])
	{
	}
}


void MainWindow::valueChanged(QtProperty *property, double value)
{
	if (!propertyToId.contains(property))
		return;

	/*
	if (!curItem)return;

	QString id = propertyToId[property];
	if (id == QString("Opacity"))
	{
		curItem->setOpacity(value);
	}else if (id == QString("Angle"))
	{
		DiagramItem* dItem = (DiagramItem*)curItem;
		dItem->setAngle(value);
	}*/

}

void MainWindow::valueChanged(QtProperty *property, int value)
{
	if (!propertyToId.contains(property))
		return;

	/*if (!curItem)return;

	QString id = propertyToId[property];
	if (curItem->type() == DiagramItem::Type)
	{
		DiagramItem* dItem = (DiagramItem*)curItem;
		if (id == QLatin1String("scale"))
		{
			dItem->setScale(STANDARD_SCALE_ARRAY[value]);
			MainWindow::instance()->getYAMLFile().defScale = STANDARD_SCALE_ARRAY[value];//¼ÇÒäÄ¬ÈÏÖµ
		}
		else if (id == QLatin1String("tagtype"))
		{
			dItem->setTagClass((TAG_PONT_TYPE)value);
		}
		else if (id == QLatin1String("layer"))
		{
			dItem->setLayerNo(value);
			scene->slotSetLayer(sYAMLFile.FLAG_LAYER, curItem);
		}

		dItem->update();
	}
	else if (curItem->type() == gitSensArea)
	{
		PolySensArea* pItem = (PolySensArea*)curItem;
		if (id == QLatin1String("scale"))
		{
			pItem->setScale(STANDARD_SCALE_ARRAY[value]);
		}
		else if (id == QLatin1String("areatype"))
		{
			pItem->setAreaTyppe((SENS_AREA_TYPE)value);
		}
		else if (id == QLatin1String("layer"))
		{
			pItem->setLayerNo(value);
			scene->slotSetLayer(sYAMLFile.FLAG_LAYER, curItem);
		}
		pItem->update();
	}
	else if (curItem->type() == dtsRectangle)
	{
		ShapeRect* pItem = (ShapeRect*)curItem;
		QPen pen = pItem->pen();
		QBrush brsh = pItem->brush();
		if (id == QLatin1String("stroke style"))
		{
			pen.setStyle((Qt::PenStyle)value);
		}
		else if (id == QLatin1String("brush style"))
		{
			brsh.setStyle((Qt::BrushStyle)value);
		}
		else if (id == QLatin1String("stroke width"))
		{
			pen.setWidth(value);

		}
		pItem->setBrush(brsh);
		pItem->setPen(pen);
		pItem->update();
	}*/

}

void MainWindow::valueChanged(QtProperty *property, const QString &value)
{
	if (!propertyToId.contains(property))
		return;

	/*QString id = propertyToId[property];
	if (curItem->type() == DiagramItem::Type)
	{
		DiagramItem* dItem = (DiagramItem*)curItem;
		if (id == QLatin1String("text"))
		{
			dItem->setText(value);
		}
		dItem->update();
	}*/

}

void MainWindow::valueChanged(QtProperty *property, const QColor &value)
{
	if (!propertyToId.contains(property))
		return;

	/*QString id = propertyToId[property];
	if (curItem->type() == dtsRectangle)
	{
		ShapeRect* rect = (ShapeRect*)curItem;
		if (id == QLatin1String("brush color"))
		{
			QBrush brsh = rect->brush();
			brsh.setColor(value);
			rect->setBrush(brsh);
		}
		else	    if (id == QLatin1String("stroke color"))
		{
			QPen pen = rect->pen();
			pen.setColor(value);
			rect->setPen(pen);
		}
		rect->update();
	}*/
}

void MainWindow::valueChanged(QtProperty *property, const QFont &value)
{
	if (!propertyToId.contains(property))
		return;

}

void MainWindow::valueChanged(QtProperty *property, const QPoint &value)
{
	if (!propertyToId.contains(property))
		return;
}

void MainWindow::valueChanged(QtProperty *property, const QSize &value)
{
	if (!propertyToId.contains(property))
		return;

}

void MainWindow::valueChanged(QtProperty *property, const QPointF &value)
{
	if (!propertyToId.contains(property))
		return;

	/*if (!curItem)return;

	QString id = propertyToId[property];
	if (curItem->type() == DiagramItem::Type)
	{
		DiagramItem* dItem = (DiagramItem*)curItem;
		if (id == QLatin1String("GeoPos"))
		{
			dItem->setGeoPosition(value);
		}
		dItem->update();
	}
	else if (curItem->type() == gitSensArea)
	{
		PolySensArea* pItem = (PolySensArea*)curItem;

		if (id == QLatin1String("GeoPos"))
		{
			pItem->setGeoPosition(value);
		}
		else
		{
			QPolygonF poly = pItem->polygon();
			int i = 0;
			for (i = 0; i < poly.count(); i++)
			{
				QString vertex = QString("Vertex%1").arg(i + 1);
				if (id == vertex)
				{
					poly[i] = scene->GeographToMap(value) - pItem->pos();
					pItem->setPoints(poly);
					break;
				}
			}
		}
	}*/
}
void MainWindow::valueChanged(QtProperty *property, const QSizeF &value)
{

}