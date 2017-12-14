#include "vibcurve.hpp"

#include <QDialog>
#include <QMessageBox>

#include <QwtExtra/editablecurve.hpp>
#include <qwt_symbol.h>

#include <assert.h>

#include "ispc/ModuleVIB.h"

#include <ctx_reg_precisions.h>
#include <felix_hw_info.h>
#include "vib.hpp"

#define VIB_CURVE_MAX 1.0f
#define VIB_CURVE_MIN 0.0f

/*
 * private
 */

void VIBCurve::init()
{
    Ui::VIBCurve::setupUi(this);

	// Set starting points for identity curve
    QVector<QPointF> iden;
	iden.push_back(QPointF(0.0f, 0.0f));
    iden.push_back(QPointF(1.0f, 1.0f));

	// Set starting points for setting curve
	QVector<QPointF> sett;
	sett.push_back(QPointF(0.0f, 0.0f));
    sett.push_back(QPointF(0.0f, 0.0f));
	sett.push_back(QPointF(1.0f, 1.0f));
	sett.push_back(QPointF(1.0f, 1.0f));

	QwtPlotCurve *Saturation;

	// Set style for setting curve
	settPen.setColor(Qt::green);
	settPen.setWidth(3);
	settPen.setStyle(Qt::DashLine);
	settSymbol = new QwtSymbol();
	settSymbol->setStyle(QwtSymbol::Ellipse);
	settSymbol->setBrush(Qt::green);
	settSymbol->setPen(Qt::green, 1);
	settSymbol->setSize(10, 10);

	// Add all curves to plot
    idC = curve->addCurve(iden, tr("Identity"));
	settC = curve->addCurve(sett, tr("Saturation curve"));

	// Set editables
	curve->setEditable(idC, false, false);
	curve->setEditable(settC, true, true);

	// Set setting curve style
	curve->curve(settC)->setPen(settPen);
	curve->curve(settC)->setSymbol(settSymbol);

	Saturation = curve->curve(settC);

	// Set axis style
    curve->setAxisScale(QwtPlot::xBottom, 0.0f, 1.0f, 0.1f);
    curve->setAxisMaxMinor(QwtPlot::xBottom, 5);
	curve->setAxisTitle(QwtPlot::xBottom, "Saturation IN");
	curve->setAxisTitle(QwtPlot::yLeft, "Saturation OUT");

	connect(curve, SIGNAL(pointMoved(int, int, QPointF)), this, SLOT(pointMoved(int, int, QPointF)));
    
	// Draw curves
    curve->replot();
}

/*
 * public
 */
	
VIBCurve::VIBCurve(QWidget *parent): QWidget(parent)
{
    init();
}

/*
 * public slots
 */

void VIBCurve::receivePoints(double p1x, double p1y, double p2x, double p2y)
{
	// Get setting curve data
	QVector<QPointF> *settCdata = curve->data(settC);

	// Check if new points are ok
	// If not reset spinBoxes to old values
	if(p1x >= p2x || p1y >= p2y)
	{
		emit updateSpins(settCdata->at(1).x(), settCdata->at(1).y(), settCdata->at(2).x(), settCdata->at(2).y());
		return;
	}

	// Set new point data
	settCdata->clear();
	settCdata->push_back(QPointF(0.0f, 0.0f));
    settCdata->push_back(QPointF(p1x, p1y));
	settCdata->push_back(QPointF(p2x, p2y));
	settCdata->push_back(QPointF(1.0f, 1.0f));
	curve->curve(settC)->setSamples(*settCdata);

	// Draw curves
	curve->replot();
}

void VIBCurve::pointMoved(int curveIdx, int point, QPointF prevPosition)
{
	QVector<QPointF> *data = curve->data(curveIdx);
	if(curveIdx != settC)
		return;

	// If points are wrong revert changes
	if(point == 0 || point == data->size() - 1)
	{
		data->replace(point, prevPosition);
        curve->curve(curveIdx)->setSamples(*data);
        curve->replot();
        return;
	}

	// Get setting curve data
    double currH = data->at(point).x();
	double currV = data->at(point).y();
	double prevH = data->at(point - 1).x();
	double prevV = data->at(point - 1).y();
	double nextH = data->at(point + 1).x();
	double nextV = data->at(point + 1).y();

	// Check if new points are ok
	// If not return to old values
	if (currH >= VIB_SAT_POINT_MAX || currH <= VIB_SAT_POINT_MIN || currV >= VIB_SAT_POINT_MAX || currV <= VIB_SAT_POINT_MIN
		|| prevH >= currH || prevV >= currV || currH >= nextH || currV >= nextV)
    {
        data->replace(point, prevPosition);
        curve->curve(curveIdx)->setSamples(*data);
        curve->replot();
        return;
    }

	// Update spinBoxes with new values
	emit updateSpins(data->at(1).x(), data->at(1).y(), data->at(2).x(), data->at(2).y());
}

QVector<QPointF> VIBCurve::getGains()
{
	// Remove all below when vibC will be brought back
	QVector<QPointF> *data = curve->data(settC);

	// Calculate vibrancy curve points
	double points[32];
	ISPC::ModuleVIB::saturationCurve(data->at(1).x(), data->at(1).y(), data->at(2).x(), data->at(2).y(), points);

	QVector<QPointF> ret;
	for(int i = 0; i < MIE_SATMULT_NUMPOINTS; i++)
	{
		ret.push_back(QPointF(1/double(MIE_SATMULT_NUMPOINTS - 1), points[i]));
	}

	return ret;
}



