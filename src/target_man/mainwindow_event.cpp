#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "target_sqlite.h"
#include "qtpropertymanager.h"
#include "qteditorfactory.h"
#include "qttreepropertybrowser.h"
void MainWindow::timerEvent(QTimerEvent *evt)
{

}

void MainWindow::customEvent(QEvent *evt)
{
	DEBUG_PARA1("customEvent(QEvent *evt)");
}

bool   MainWindow::updateProperty(TTarget* target)
{
	updateExpandState();

	QMap<QtProperty *, QString>::ConstIterator itProp = propertyToId.constBegin();
	while (itProp != propertyToId.constEnd())
	{
		delete itProp.key();
		itProp++;
	}
	propertyToId.clear();
	idToProperty.clear();

	QtProperty *property;

	//Í¸Ã÷¶È
	property = doubleManager->addProperty(tr("Opacity"));
	doubleManager->setRange(property, 0, 1);
	doubleManager->setValue(property, 0.5);
	doubleManager->setSingleStep(property, 0.1);
	addProperty(property, QLatin1String("Opacity"));

	//±Ê´¥ÑÕÉ«

		//±Ê´¥ÑÕÉ«
		property = colorManager->addProperty(tr("stroke color"));
		colorManager->setValue(property,Qt::blue);
		addProperty(property, QLatin1String("stroke color"));

		//±Ê´¥·ç¸ñ
		property = enumManager->addProperty(tr("stroke style"));
		QStringList lstNames;
		QMap<int, QIcon> mapIcons;
		for (int i = 0; i < LINE_STYLE_NUM; i++)
		{
			lstNames.append(sLineStyle[i]);
			QIcon icon(QString(":/img/style/%1.png").arg(sLineStyle[i]));
			mapIcons.insert(i, icon);

		}
		enumManager->setEnumIcons(property, mapIcons);
		enumManager->setEnumNames(property, lstNames);
		enumManager->setValue(property, 5);
		addProperty(property, QLatin1String("stroke style"));

		//±Ê´¥¿í¶È
		property = enumManager->addProperty(tr("stroke width"));
		lstNames.clear();
		mapIcons.clear();
		for (int i = 0; i < LINE_WIDTH_NUM; i++)
		{
			lstNames.append(lstWidth[i]);
			QIcon icon(QString(":/img/style/%1.bmp").arg(lstWidth[i]));
			mapIcons.insert(i, icon);
		}
		enumManager->setEnumIcons(property, mapIcons);
		enumManager->setEnumNames(property, lstNames);
		enumManager->setValue(property, 3);
		addProperty(property, QLatin1String("stroke width"));

		//Ìî³äÑÕÉ«
		property = colorManager->addProperty(tr("brush color"));
		colorManager->setValue(property, Qt::red);
		addProperty(property, QLatin1String("brush color"));

		//Ìî³ä·ç¸ñ
		property = enumManager->addProperty(tr("brush style"));

		lstNames.clear();
		mapIcons.clear();
		for (int i = 0; i < 5; i++)
		{
			lstNames.append(QString("style%1").arg(i+1));
			QIcon icon(QString(":/icon24/user_blue.png"));
			mapIcons.insert(i, icon);

		}
		enumManager->setEnumIcons(property, mapIcons);
		enumManager->setEnumNames(property, lstNames);
		enumManager->setValue(property, 5);
		addProperty(property, QLatin1String("brush style"));

	return true;
}
