#include "histogram.hpp"

#include <qlayout.h>

#define HIST_RED_NAME "Red"
#define HIST_GREEN_NAME "Green"
#define HIST_GREEN2_NAME "Green2"
#define HIST_BLUE_NAME "Blue"

//
// Public Functions
//

VisionLive::Histogram::Histogram(QWidget *parent): QWidget(parent)
{
	retranslate();

	_pPlot = NULL;
	
	initHistogram();
}

VisionLive::Histogram::~Histogram()
{
	if (_pPlot)
	{
		delete _pPlot;
	}
}

void VisionLive::Histogram::setPlotColors(QColor color1, QColor color2, QColor color3, QColor color4)
{
	if (!_pPlot)
	{
		return;
	}

	_pPlot->setCurvePen(QStringLiteral(HIST_RED_NAME), QPen(QBrush(color1), 3, Qt::SolidLine));
	_pPlot->setCurvePen(QStringLiteral(HIST_GREEN_NAME), QPen(QBrush(color2), 3, Qt::SolidLine));
	_pPlot->setCurvePen(QStringLiteral(HIST_GREEN2_NAME), QPen(QBrush(color3), 3, Qt::SolidLine));
	_pPlot->setCurvePen(QStringLiteral(HIST_BLUE_NAME), QPen(QBrush(color4), 3, Qt::SolidLine));
}

//
// Private Functions
//

void VisionLive::Histogram::initHistogram()
{
	_pLayout = new QVBoxLayout();
	setLayout(_pLayout);

	zeroData.push_back(QPointF(0, 0));
	zeroData.push_back(QPointF(0, 0));

	_pPlot = new EasyPlot::ExtPlot();
	_pLayout->addWidget(_pPlot);

	_pPlot->addCurve(QStringLiteral(HIST_RED_NAME), zeroData);
	_pPlot->setCurveZValue(QStringLiteral(HIST_RED_NAME), 1);
	_pPlot->setCurvePen(QStringLiteral(HIST_RED_NAME), QPen(QBrush(Qt::red), 3, Qt::SolidLine));
	_pPlot->setCurveType(QStringLiteral(HIST_RED_NAME), EasyPlot::Bar);

	_pPlot->addCurve(QStringLiteral(HIST_GREEN_NAME), zeroData);
	_pPlot->setCurveZValue(QStringLiteral(HIST_GREEN_NAME), 2);
	_pPlot->setCurvePen(QStringLiteral(HIST_GREEN_NAME), QPen(QBrush(Qt::green), 3, Qt::SolidLine));
	_pPlot->setCurveType(QStringLiteral(HIST_GREEN_NAME), EasyPlot::Bar);

	_pPlot->addCurve(QStringLiteral(HIST_GREEN2_NAME), zeroData);
	_pPlot->setCurveZValue(QStringLiteral(HIST_GREEN2_NAME), 3);
	_pPlot->setCurvePen(QStringLiteral(HIST_GREEN2_NAME), QPen(QBrush(Qt::darkGreen), 3, Qt::SolidLine));
	_pPlot->setCurveType(QStringLiteral(HIST_GREEN2_NAME), EasyPlot::Bar);

	_pPlot->addCurve(QStringLiteral(HIST_BLUE_NAME), zeroData);
	_pPlot->setCurveZValue(QStringLiteral(HIST_BLUE_NAME), 4);
	_pPlot->setCurvePen(QStringLiteral(HIST_BLUE_NAME), QPen(QBrush(Qt::blue), 3, Qt::SolidLine));
	_pPlot->setCurveType(QStringLiteral(HIST_BLUE_NAME), EasyPlot::Bar);

	_pPlot->setTitleVisible(false);
	_pPlot->setAxisXTitleVisible(false);
	_pPlot->setAxisYTitleVisible(false);
	_pPlot->setAxisYVisible(false);
	_pPlot->setLegendVisible(false);
	_pPlot->setAxisXPen(QPen(QBrush(Qt::white), 1));
	_pPlot->setPositionInfoColor(Qt::white);
}

void VisionLive::Histogram::retranslate()
{
}

//
// Public Slots
//

void VisionLive::Histogram::histogramReceived(QVector<QVector<QPointF> > histData)
{
	if(!_pPlot)
	{
		return;
	}
	
	int channels = histData.size();
	if(channels == 4)
	{
		_pPlot->updateCurve(QStringLiteral(HIST_RED_NAME), histData[0]);
		_pPlot->updateCurve(QStringLiteral(HIST_GREEN_NAME), histData[1]);
		_pPlot->setCurveVisible(QStringLiteral(HIST_GREEN2_NAME), true);
		_pPlot->updateCurve(QStringLiteral(HIST_GREEN2_NAME), histData[2]);
		_pPlot->updateCurve(QStringLiteral(HIST_BLUE_NAME), histData[3]);
	}
	else if(channels == 3)
	{
		_pPlot->updateCurve(QStringLiteral(HIST_RED_NAME), histData[0]);
		_pPlot->updateCurve(QStringLiteral(HIST_GREEN_NAME), histData[1]);
		_pPlot->setCurveVisible(QStringLiteral(HIST_GREEN2_NAME), false);
		_pPlot->updateCurve(QStringLiteral(HIST_BLUE_NAME), histData[2]);
	}
}