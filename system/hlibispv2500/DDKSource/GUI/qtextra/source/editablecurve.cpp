/**
******************************************************************************
@file editablecurve.cpp

@brief Implementation of EditableCurve

@copyright Imagination Technologies Ltd. All Rights Reserved.

@license Strictly Confidential.
No part of this software, either material or conceptual may be copied or
distributed, transmitted, transcribed, stored in a retrieval system or
translated into any human or computer language in any form by any means,
electronic, mechanical, manual or other-wise, or disclosed to third
parties without the express written permission of
Imagination Technologies Limited,
Unit 8, HomePark Industrial Estate,
King's Langley, Hertfordshire,
WD4 8LZ, U.K.

******************************************************************************/
#include "QwtExtra/editablecurve.hpp"

#include <QWidget>
#include <QMouseEvent>
#include <QtCore/QObject>
#include <QtCore/QEvent>

#include <qwt_plot_curve.h>
#include <qwt_plot.h>
#include <qwt_plot_picker.h>

#include <assert.h>

using namespace QwtExtra;

// Curve

EditableCurve::Curve::Curve(const QVector<QPointF>data, bool editHoriz, bool editVert)
	: data(data)
{
	editable[0] = editHoriz;
	editable[1] = editVert;

	curve.setSamples(data);
}

void EditableCurve::Curve::move(int point, const QPointF &neo)
{
	if ( !isEditable() ) return; // nothing to update

	if ( point >= data.size() ) return; // not a valid point

	QPointF v = data.at(point);

	if ( editable[0] ) v.setX(neo.x());
	if ( editable[1] ) v.setY(neo.y());

	data.replace(point, v);

	curve.setSamples(data);
}

// public
#include <qwt_picker_machine.h>

EditableCurve::EditableCurve(QWidget *parent)
	: QwtPlot(parent), tracker(this->canvas())
{
	_precision = 10;
	
	tracker.setStateMachine(new QwtPickerClickPointMachine);
	tracker.setTrackerMode(QwtPicker::AlwaysOn);
	
	connect(&tracker, SIGNAL(appended(const QPoint&)), this, SLOT(mouseClicked(const QPoint&)));
}

EditableCurve::~EditableCurve()
{
	for ( int i = 0 ; i < curves.size() ; i++ )
	{
		Curve *n = curves.at(i);
		delete n;
	}
	curves.clear();
}

int EditableCurve::nCurves() const
{
	return curves.size();
}
	
int EditableCurve::addCurve(const QVector<QPointF> &data, const QString &name)
{
	EditableCurve::Curve *neo = new EditableCurve::Curve(data);
    int c = curves.size();

	neo->curve.setTitle(name);
	curves.push_back(neo);

	neo->curve.attach(this);

	replot();

    return c;
}

void EditableCurve::removeCurve(int curve)
{
	if ( curve >= curves.size() ) return;

	EditableCurve::Curve *neo = curves.at(curve);

	neo->curve.detach();
	delete neo;
	curves.remove(curve);

	replot();
}

QwtPlotCurve* EditableCurve::curve(int curve)
{
	if ( curve >= curves.size() ) return NULL;
	return &(curves.at(curve)->curve);
}

QVector<QPointF>* EditableCurve::data(int curve)
{
	if ( curve >= curves.size() ) return NULL;
	return &(curves.at(curve)->data);
}

bool EditableCurve::editable(int curve, int axis)
{
	if ( curve >= curves.size() || axis >= 2 ) return false;
	return curves.at(curve)->editable[axis];
}

void EditableCurve::setEditable(int curve, int axis, bool value)
{
	if ( curve >= curves.size() || axis >= 2 ) return;

	curves.at(curve)->editable[axis] = value;
}

void EditableCurve::setEditable(int curve, bool horiz, bool vert)
{
	if ( curve >= curves.size() ) return;

	curves.at(curve)->editable[0] = horiz;
	curves.at(curve)->editable[1] = vert;
}

void EditableCurve::setPrecision(double prec)
{
	_precision = prec;
}

double EditableCurve::precision() const 
{
	return _precision;
}

void EditableCurve::cancelSelection()
{
	selectedCurve = -1;
	selectedPoint = -1;
}

// protected

void EditableCurve::mouseMoveEvent(QMouseEvent *event)
{
	QPoint pos = event->globalPos() - this->canvas()->mapToGlobal(QPoint(0, 0)); // Global position eliminates error caused by axis and titles
	QPointF neo, prev;

	if ( selectedCurve == -1 ) 
	{
		QwtPlot::mouseMoveEvent(event);
		return;
	}
	assert(selectedPoint != -1);

	prev = curves.at(selectedCurve)->data.at(selectedPoint);

	neo.setX( invTransform(curves.at(selectedCurve)->curve.xAxis(), pos.x()) );
	neo.setY( invTransform(curves.at(selectedCurve)->curve.yAxis(), pos.y()) );
	curves.at(selectedCurve)->move(selectedPoint, neo);

	replot();
	Q_EMIT pointMoved(selectedCurve, selectedPoint, prev);
}

// public slots

void EditableCurve::mouseClicked(const QPoint& pos)
{
	//std::cout << __FUNCTION__ << " " << pos.x() << ":" << pos.y() << std::endl;

	// stolen code from the examples of Qwt
	int curveIdx = -1;
    double dist = 10e10;
    int index = -1;

    const QwtPlotItemList& itmList = itemList();
    for ( QwtPlotItemIterator it = itmList.begin(); it != itmList.end(); ++it )
    {
        if ( ( *it )->rtti() == QwtPlotItem::Rtti_PlotCurve )
        {
            QwtPlotCurve *c = static_cast<QwtPlotCurve *>( *it );

            double d = 0;
            int idx = c->closestPoint( pos, &d );
            if ( d < dist ) // find the closet point
            {
				for ( int i = 0 ; i < curves.size() ; i++ )
				{
					if ( c == &(curves.at(i)->curve) )
					{
						curveIdx = i;
						break;
					}
				}

				if ( curves.at(curveIdx)->isEditable() )
				{
					index = idx;
					dist = d;
				}
            } // if closer
        } // if a curve
    }

	selectedCurve = -1;
	selectedPoint = -1;

    if ( index >= 0 && dist < _precision ) // 10 pixels tolerance
    {
		selectedPoint = index;
		selectedCurve = curveIdx;
		Q_EMIT pointSelected(curveIdx, index);
    }
}
