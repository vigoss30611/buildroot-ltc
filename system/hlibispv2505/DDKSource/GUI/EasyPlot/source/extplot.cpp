#include "extplot.h"
#include "plot.h"

#include <QGridLayout>
#include <QComboBox>
#include <QSpacerItem>
#include <QtCore/qmath.h>

#define VIEW_OPTION_JOIN "Joined View"
#define VIEW_OPTION_SPLIT "Split View"
#define VIEW_LAYOUT_OPTION_VERT "Vertical"
#define VIEW_LAYOUT_OPTION_HORIZ "Horizontal"
#define VIEW_LAYOUT_OPTION_GRID "Grid"

EasyPlot::ExtPlot::ExtPlot(QWidget *parent) : QWidget(parent)
{
	_pMainLayout = NULL;
	_pControlLayout = NULL;
	_pPlotLayout = NULL;
	_pViewOption = NULL;
	_pViewLayoutOption = NULL;
	_pSpacer = NULL;

	init();
}

EasyPlot::ExtPlot::~ExtPlot()
{
}

void EasyPlot::ExtPlot::addCurve(const QString &name, QVector<QPointF> data)
{
	if (_SplitPlot.find(name) != _SplitPlot.end() || data.size() < 2)
		return;

	_pJoinPlot->addCurve(name, data);

	Plot *newplot = new Plot();
	newplot->addCurve(name, data);
	_SplitPlot.insert(name, newplot);

	QObject::connect(newplot, SIGNAL(pointChanged(QString, unsigned, QPointF, QPointF)),
		this, SIGNAL(pointChanged(QString, unsigned, QPointF, QPointF)));

	changeView();
}

void EasyPlot::ExtPlot::updateCurve(const QString &name, QVector<QPointF> data)
{
	if (_pJoinPlot)
		_pJoinPlot->updateCurve(name, data);

	QMap<QString, Plot *>::iterator it = _SplitPlot.find(name);
	if (it != _SplitPlot.end())
		(*it)->updateCurve(name, data);
}

QVector<QPointF> EasyPlot::ExtPlot::data(const QString &name)
{
	if (_pJoinPlot)
		return _pJoinPlot->data(name);

	//    QMap<QString, Plot *>::iterator it = _SplitPlot.find(name);
	//    if(it != _SplitPlot.end())
	//        return (*it)->data(name);

	return QVector<QPointF>();
}

void EasyPlot::ExtPlot::setCurvePen(const QString &name, const QPen &pen)
{
	if (_pJoinPlot)
		_pJoinPlot->setCurvePen(name, pen);

	QMap<QString, Plot *>::iterator it = _SplitPlot.find(name);
	if (it != _SplitPlot.end())
		(*it)->setCurvePen(name, pen);
}

void EasyPlot::ExtPlot::setCurveType(const QString &name, CurveType type)
{
	if (_pJoinPlot)
		_pJoinPlot->setCurveType(name, type);

	QMap<QString, Plot *>::iterator it = _SplitPlot.find(name);
	if (it != _SplitPlot.end())
		(*it)->setCurveType(name, type);
}

void EasyPlot::ExtPlot::setCurveEditable(const QString &name, bool option)
{
	if (_pJoinPlot)
		_pJoinPlot->setCurveEditable(name, option);

	QMap<QString, Plot *>::iterator it = _SplitPlot.find(name);
	if (it != _SplitPlot.end())
		(*it)->setCurveEditable(name, option);
}

void EasyPlot::ExtPlot::setCurveZValue(const QString &name, qreal z)
{
	if (_pJoinPlot)
		_pJoinPlot->setCurveZValue(name, z);

	QMap<QString, Plot *>::iterator it = _SplitPlot.find(name);
	if (it != _SplitPlot.end())
		(*it)->setCurveZValue(name, z);
}

void EasyPlot::ExtPlot::setCurveVisible(const QString &name, bool visible)
{
	if (!_pJoinPlot || !_pViewOption)
		return;

	_pJoinPlot->updateCurve(name, _zeroData);

	QMap<QString, Plot *>::iterator it = _SplitPlot.find(name);
	if (it != _SplitPlot.end())
	{
		(*it)->setVisible(visible && _pViewOption->currentText() == QStringLiteral(VIEW_OPTION_SPLIT));
	}
}

void EasyPlot::ExtPlot::setMarkerType(const QString &name, MarkerType type)
{
	if (_pJoinPlot)
		_pJoinPlot->setMarkerType(name, type);

	QMap<QString, Plot *>::iterator it = _SplitPlot.find(name);
	if (it != _SplitPlot.end())
		(*it)->setMarkerType(name, type);
}

void EasyPlot::ExtPlot::setMarkerSize(const QString &name, QSize size)
{
	if (_pJoinPlot)
		_pJoinPlot->setMarkerSize(name, size);

	QMap<QString, Plot *>::iterator it = _SplitPlot.find(name);
	if (it != _SplitPlot.end())
		(*it)->setMarkerSize(name, size);
}

void EasyPlot::ExtPlot::setMarkerPen(const QString &name, QPen pen)
{
	if (_pJoinPlot)
		_pJoinPlot->setMarkerPen(name, pen);

	QMap<QString, Plot *>::iterator it = _SplitPlot.find(name);
	if (it != _SplitPlot.end())
		(*it)->setMarkerPen(name, pen);
}

void EasyPlot::ExtPlot::setMarkerEditPen(const QString &name, QPen pen)
{
	if (_pJoinPlot)
		_pJoinPlot->setMarkerEditPen(name, pen);

	QMap<QString, Plot *>::iterator it;
	for (it = _SplitPlot.begin(); it != _SplitPlot.end(); it++)
		(*it)->setMarkerEditPen(name, pen);
}

void EasyPlot::ExtPlot::setPositionInfoColor(const QColor &color)
{
	if (_pJoinPlot)
		_pJoinPlot->setPositionInfoColor(color);

	QMap<QString, Plot *>::iterator it;
	for (it = _SplitPlot.begin(); it != _SplitPlot.end(); it++)
		(*it)->setPositionInfoColor(color);
}

void EasyPlot::ExtPlot::setTitleText(const QString &text)
{
	if (_pJoinPlot)
		_pJoinPlot->setTitleText(text);

	QMap<QString, Plot *>::iterator it;
	for (it = _SplitPlot.begin(); it != _SplitPlot.end(); it++)
		(*it)->setTitleText(text);
}

void EasyPlot::ExtPlot::setTitleFont(const QFont &font)
{
	if (_pJoinPlot)
		_pJoinPlot->setTitleFont(font);

	QMap<QString, Plot *>::iterator it;
	for (it = _SplitPlot.begin(); it != _SplitPlot.end(); it++)
		(*it)->setTitleFont(font);
}

void EasyPlot::ExtPlot::setTitleColor(const QColor &color)
{
	if (_pJoinPlot)
		_pJoinPlot->setTitleColor(color);

	QMap<QString, Plot *>::iterator it;
	for (it = _SplitPlot.begin(); it != _SplitPlot.end(); it++)
		(*it)->setTitleColor(color);
}

void EasyPlot::ExtPlot::setTitleVisible(bool visible)
{
	if (_pJoinPlot)
		_pJoinPlot->setTitleVisible(visible);

	QMap<QString, Plot *>::iterator it;
	for (it = _SplitPlot.begin(); it != _SplitPlot.end(); it++)
		(*it)->setTitleVisible(visible);
}

void EasyPlot::ExtPlot::setAxisXTitleText(const QString &text)
{
	if (_pJoinPlot)
		_pJoinPlot->setAxisXTitleText(text);

	QMap<QString, Plot *>::iterator it;
	for (it = _SplitPlot.begin(); it != _SplitPlot.end(); it++)
		(*it)->setAxisXTitleText(text);
}

void EasyPlot::ExtPlot::setAxisXTitleFont(const QFont &font)
{
	if (_pJoinPlot)
		_pJoinPlot->setAxisXTitleFont(font);

	QMap<QString, Plot *>::iterator it;
	for (it = _SplitPlot.begin(); it != _SplitPlot.end(); it++)
		(*it)->setAxisXTitleFont(font);
}

void EasyPlot::ExtPlot::setAxisXTitleColor(const QColor &color)
{
	if (_pJoinPlot)
		_pJoinPlot->setAxisXTitleColor(color);

	QMap<QString, Plot *>::iterator it;
	for (it = _SplitPlot.begin(); it != _SplitPlot.end(); it++)
		(*it)->setAxisXTitleColor(color);
}

void EasyPlot::ExtPlot::setAxisXTitleVisible(bool visible)
{
	if (_pJoinPlot)
		_pJoinPlot->setAxisXTitleVisible(visible);

	QMap<QString, Plot *>::iterator it;
	for (it = _SplitPlot.begin(); it != _SplitPlot.end(); it++)
		(*it)->setAxisXTitleVisible(visible);
}

void EasyPlot::ExtPlot::setAxisXPen(const QPen &pen)
{
	if (_pJoinPlot)
		_pJoinPlot->setAxisXPen(pen);

	QMap<QString, Plot *>::iterator it;
	for (it = _SplitPlot.begin(); it != _SplitPlot.end(); it++)
		(*it)->setAxisXPen(pen);
}

void EasyPlot::ExtPlot::setAxisXVisible(bool visible)
{
	if (_pJoinPlot)
		_pJoinPlot->setAxisXVisible(visible);

	QMap<QString, Plot *>::iterator it;
	for (it = _SplitPlot.begin(); it != _SplitPlot.end(); it++)
		(*it)->setAxisXVisible(visible);
}

void EasyPlot::ExtPlot::setAxisXMinRange(float min, float max)
{
	if (_pJoinPlot)
		_pJoinPlot->setAxisXMinRange(min, max);

	QMap<QString, Plot *>::iterator it;
	for (it = _SplitPlot.begin(); it != _SplitPlot.end(); it++)
		(*it)->setAxisXMinRange(min, max);
}

void EasyPlot::ExtPlot::setAxisXLinesVisible(bool visible)
{
	if (_pJoinPlot)
		_pJoinPlot->setAxisXLinesVisible(visible);

	QMap<QString, Plot *>::iterator it;
	for (it = _SplitPlot.begin(); it != _SplitPlot.end(); it++)
		(*it)->setAxisXLinesVisible(visible);
}

void EasyPlot::ExtPlot::setAxisXLinesPen(QPen pen)
{
	if (_pJoinPlot)
		_pJoinPlot->setAxisXLinesPen(pen);

	QMap<QString, Plot *>::iterator it;
	for (it = _SplitPlot.begin(); it != _SplitPlot.end(); it++)
		(*it)->setAxisXLinesPen(pen);
}

void EasyPlot::ExtPlot::setAxisYTitleText(const QString &text)
{
	if (_pJoinPlot)
		_pJoinPlot->setAxisYTitleText(text);

	QMap<QString, Plot *>::iterator it;
	for (it = _SplitPlot.begin(); it != _SplitPlot.end(); it++)
		(*it)->setAxisYTitleText(text);
}

void EasyPlot::ExtPlot::setAxisYTitleFont(const QFont &font)
{
	if (_pJoinPlot)
		_pJoinPlot->setAxisYTitleFont(font);

	QMap<QString, Plot *>::iterator it;
	for (it = _SplitPlot.begin(); it != _SplitPlot.end(); it++)
		(*it)->setAxisYTitleFont(font);
}

void EasyPlot::ExtPlot::setAxisYTitleColor(const QColor &color)
{
	if (_pJoinPlot)
		_pJoinPlot->setAxisYTitleColor(color);

	QMap<QString, Plot *>::iterator it;
	for (it = _SplitPlot.begin(); it != _SplitPlot.end(); it++)
		(*it)->setAxisYTitleColor(color);
}

void EasyPlot::ExtPlot::setAxisYTitleVisible(bool visible)
{
	if (_pJoinPlot)
		_pJoinPlot->setAxisYTitleVisible(visible);

	QMap<QString, Plot *>::iterator it;
	for (it = _SplitPlot.begin(); it != _SplitPlot.end(); it++)
		(*it)->setAxisYTitleVisible(visible);
}

void EasyPlot::ExtPlot::setAxisYPen(const QPen &pen)
{
	if (_pJoinPlot)
		_pJoinPlot->setAxisYPen(pen);

	QMap<QString, Plot *>::iterator it;
	for (it = _SplitPlot.begin(); it != _SplitPlot.end(); it++)
		(*it)->setAxisYPen(pen);
}

void EasyPlot::ExtPlot::setAxisYVisible(bool visible)
{
	if (_pJoinPlot)
		_pJoinPlot->setAxisYVisible(visible);

	QMap<QString, Plot *>::iterator it;
	for (it = _SplitPlot.begin(); it != _SplitPlot.end(); it++)
		(*it)->setAxisYVisible(visible);
}

void EasyPlot::ExtPlot::setAxisYMinRange(float min, float max)
{
	if (_pJoinPlot)
		_pJoinPlot->setAxisYMinRange(min, max);

	QMap<QString, Plot *>::iterator it;
	for (it = _SplitPlot.begin(); it != _SplitPlot.end(); it++)
		(*it)->setAxisYMinRange(min, max);
}

void EasyPlot::ExtPlot::setAxisYLinesVisible(bool visible)
{
	if (_pJoinPlot)
		_pJoinPlot->setAxisYLinesVisible(visible);

	QMap<QString, Plot *>::iterator it;
	for (it = _SplitPlot.begin(); it != _SplitPlot.end(); it++)
		(*it)->setAxisYLinesVisible(visible);
}

void EasyPlot::ExtPlot::setAxisYLinesPen(QPen pen)
{
	if (_pJoinPlot)
		_pJoinPlot->setAxisYLinesPen(pen);

	QMap<QString, Plot *>::iterator it;
	for (it = _SplitPlot.begin(); it != _SplitPlot.end(); it++)
		(*it)->setAxisYLinesPen(pen);
}

void EasyPlot::ExtPlot::setLegendPen(const QPen &pen)
{
	if (_pJoinPlot)
		_pJoinPlot->setLegendPen(pen);

	QMap<QString, Plot *>::iterator it;
	for (it = _SplitPlot.begin(); it != _SplitPlot.end(); it++)
		(*it)->setLegendPen(pen);
}

void EasyPlot::ExtPlot::setLegendVisible(bool visible)
{
	if (_pJoinPlot)
		_pJoinPlot->setLegendVisible(visible);

	QMap<QString, Plot *>::iterator it;
	for (it = _SplitPlot.begin(); it != _SplitPlot.end(); it++)
		(*it)->setLegendVisible(visible);
}

void EasyPlot::ExtPlot::setViewType(ViewType type)
{
	_pViewOption->setCurrentIndex(type);
}

void EasyPlot::ExtPlot::setSplitViewType(SplitViewTpye type)
{
	_pViewOption->setCurrentIndex(SplitView);
	_pViewLayoutOption->setCurrentIndex(type);
}

void EasyPlot::ExtPlot::init()
{
	_zeroData.push_back(QPointF(0, 0));
	_zeroData.push_back(QPointF(0, 0));

	_pMainLayout = new QGridLayout();
	setLayout(_pMainLayout);

	_pControlLayout = new QGridLayout();
	_pMainLayout->addLayout(_pControlLayout, 0, 0);

	_pViewOption = new QComboBox();
	_pViewOption->addItem(QStringLiteral(VIEW_OPTION_JOIN));
	_pViewOption->addItem(QStringLiteral(VIEW_OPTION_SPLIT));
	_pControlLayout->addWidget(_pViewOption, 0, 0);

	_pViewLayoutOption = new QComboBox();
	_pViewLayoutOption->addItem(QStringLiteral(VIEW_LAYOUT_OPTION_VERT));
	_pViewLayoutOption->addItem(QStringLiteral(VIEW_LAYOUT_OPTION_HORIZ));
	_pViewLayoutOption->addItem(QStringLiteral(VIEW_LAYOUT_OPTION_GRID));
	_pViewLayoutOption->setEnabled(false);
	_pControlLayout->addWidget(_pViewLayoutOption, 0, 1);

	_pSpacer = new QSpacerItem(1, 1, QSizePolicy::Expanding);
	_pControlLayout->addItem(_pSpacer, 0, 2);

	_pPlotLayout = new QGridLayout();
	_pMainLayout->addLayout(_pPlotLayout, 1, 0);

	_pJoinPlot = new Plot();
	_pPlotLayout->addWidget(_pJoinPlot);

	QObject::connect(_pJoinPlot, SIGNAL(pointChanged(QString, unsigned, QPointF, QPointF)),
		this, SIGNAL(pointChanged(QString, unsigned, QPointF, QPointF)));

	QObject::connect(_pViewOption, SIGNAL(currentIndexChanged(int)),
		this, SLOT(changeView()));
	QObject::connect(_pViewLayoutOption, SIGNAL(currentIndexChanged(int)),
		this, SLOT(changeView()));

}

void EasyPlot::ExtPlot::resetLayouts()
{
	if (!_pMainLayout || !_pPlotLayout)
		return;

	for (int i = 0; i < _pPlotLayout->count(); i++)
		_pPlotLayout->removeItem(_pPlotLayout->itemAt(i));

	delete _pPlotLayout;

	for (int i = 0; i < _pMainLayout->count(); i++)
		_pMainLayout->removeItem(_pMainLayout->itemAt(i));

	delete _pMainLayout;

	_pMainLayout = new QGridLayout();
	setLayout(_pMainLayout);

	_pMainLayout->addLayout(_pControlLayout, 0, 0);

	_pPlotLayout = new QGridLayout();
	_pMainLayout->addLayout(_pPlotLayout, 1, 0);
}

void EasyPlot::ExtPlot::changeView()
{
	if (!_pPlotLayout || !_pViewOption || !_pViewLayoutOption ||
		!_pJoinPlot)
		return;

	bool viewOptionChanged = (((QComboBox *)sender()) == _pViewOption);
	bool viewLayoutOptionChanged = (((QComboBox *)sender()) == _pViewLayoutOption);

	if (viewOptionChanged)
	{
		resetLayouts();

		if (_pViewOption->currentText() == QStringLiteral(VIEW_OPTION_JOIN))
		{
			_pViewLayoutOption->setEnabled(false);

			QMap<QString, Plot *>::iterator it;
			for (it = _SplitPlot.begin(); it != _SplitPlot.end(); it++)
				(*it)->setVisible(false);

			_pPlotLayout->addWidget(_pJoinPlot, 0, 0, 1, _pPlotLayout->columnCount());
			_pJoinPlot->setVisible(true);
		}
		else if (_pViewOption->currentText() == QStringLiteral(VIEW_OPTION_SPLIT))
		{
			_pViewLayoutOption->setEnabled(true);

			_pJoinPlot->setVisible(false);

			QMap<QString, Plot *>::iterator it;
			for (it = _SplitPlot.begin(); it != _SplitPlot.end(); it++)
				(*it)->setVisible(true);

			if (_pViewLayoutOption->currentText() == QStringLiteral(VIEW_LAYOUT_OPTION_VERT))
			{
				QMap<QString, Plot *>::iterator it;
				for (it = _SplitPlot.begin(); it != _SplitPlot.end(); it++)
					_pPlotLayout->addWidget(*it, _pPlotLayout->rowCount(), 0, 1, 1);
			}
			else if (_pViewLayoutOption->currentText() == QStringLiteral(VIEW_LAYOUT_OPTION_HORIZ))
			{
				QMap<QString, Plot *>::iterator it;
				for (it = _SplitPlot.begin(); it != _SplitPlot.end(); it++)
					_pPlotLayout->addWidget(*it, 0, _pPlotLayout->columnCount(), 1, 1);
			}
			else if (_pViewLayoutOption->currentText() == QStringLiteral(VIEW_LAYOUT_OPTION_GRID))
			{
				int maxW = qCeil(qSqrt(_SplitPlot.size()));
				int x = 0;
				int y = 0;
				QMap<QString, Plot *>::iterator it;
				for (it = _SplitPlot.begin(); it != _SplitPlot.end(); it++)
				{
					if (x + 1 < maxW)
					{
						_pPlotLayout->addWidget(*it, y, x, 1, 1);
						x++;
					}
					else
					{
						_pPlotLayout->addWidget(*it, y, x, 1, 1);
						x = 0;
						y++;
					}
				}
			}
		}
	}
	else if (viewLayoutOptionChanged)
	{
		resetLayouts();

		if (_pViewLayoutOption->currentText() == QStringLiteral(VIEW_LAYOUT_OPTION_VERT))
		{
			QMap<QString, Plot *>::iterator it;
			for (it = _SplitPlot.begin(); it != _SplitPlot.end(); it++)
				_pPlotLayout->addWidget(*it, _pPlotLayout->rowCount(), 0, 1, 1);
		}
		else if (_pViewLayoutOption->currentText() == QStringLiteral(VIEW_LAYOUT_OPTION_HORIZ))
		{
			QMap<QString, Plot *>::iterator it;
			for (it = _SplitPlot.begin(); it != _SplitPlot.end(); it++)
				_pPlotLayout->addWidget(*it, 0, _pPlotLayout->columnCount(), 1, 1);
		}
		else if (_pViewLayoutOption->currentText() == QStringLiteral(VIEW_LAYOUT_OPTION_GRID))
		{
			int maxW = qCeil(qSqrt(_SplitPlot.size()));
			int x = 0;
			int y = 0;
			QMap<QString, Plot *>::iterator it;
			for (it = _SplitPlot.begin(); it != _SplitPlot.end(); it++)
			{
				if (x + 1 < maxW)
				{
					_pPlotLayout->addWidget(*it, y, x, 1, 1);
					x++;
				}
				else
				{
					_pPlotLayout->addWidget(*it, y, x, 1, 1);
					x = 0;
					y++;
				}
			}
		}
	}
}
