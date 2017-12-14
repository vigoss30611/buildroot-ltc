/********************************************************************************
@file gmawidget.cpp

@brief GMAWidget class implementation

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
#include "gma/widget.hpp"
#include <qwt_symbol.h>
#include <QInputDialog>
#include "QtExtra/textparser.hpp"
#include <QMessageBox>
#include <QDebug>
#include <felix_hw_info.h>

#define GMA_LUT_INT 8
#define GMA_LUT_FRAC 4
#define GMA_LUT_SIGNRED 0

/*
 * EditPoints
 */

//
// public
//

EditPoints::EditPoints(QwtExtra::EditableCurve *curve, int id, QString name): _curve(curve), _id(id)
{
	setWindowTitle(name);

	_mainLayout = new QGridLayout();
	setLayout(_mainLayout);

	_import = new QPushButton("Import values");
	_mainLayout->addWidget(_import, 0, 0, 1, 2);

	_data = _curve->data(_id);

	int j = 0;
	int k = 0;
	for(int i = 0; i < _data->size(); i++)
	{
		if(i > 0 && i%10 == 0) 
		{
			j++;
			k = 0;
		}
		_values.push_back(new QDoubleSpinBox(this));
		_mainLayout->addWidget(_values[i], j+1, k);
		_values[i]->setButtonSymbols(QAbstractSpinBox::NoButtons);
		_values[i]->setMaximum(GMA_REG_MAX_VAL);
		_values[i]->setValue(_data[0][i].y());
		connect(_values[i], SIGNAL(valueChanged(double)), this, SLOT(valueChanged()));
		k++;
	}

	connect(_import, SIGNAL(clicked()), this, SLOT(importValues()));
}

EditPoints::~EditPoints()
{
}

//
// public slots:
//

void EditPoints::valueChanged()
{
	for(int i = 0; i < _data->size(); i++)
	{
		_data[0][i] = QPointF(_data[0][i].x(), _values[i]->value());
	}
	_curve->curve(_id)->setSamples(*_data);
	_curve->replot();
}

void EditPoints::importValues()
{
	QtExtra::TextParser parser(63, 1);
	
	if ( parser.exec() == QDialog::Accepted )
    {
		QList<double> list;
		parser.results(list);

		if(list.size() != _values.size())
		{
			// Create ConnectionClosed popup
			QMessageBox::warning(this, tr("Warning"), tr("Incorrect imported values count!"));
			return;
		}

		for(int i = 0; i < _values.size(); i++)
		{
			_values[i]->setValue(list[i]);
		}

	}
}

/*
 * GMAWidget
 */

//
// public
//

GMAWidget::GMAWidget(QVector<QPointF> dataChannelR, QVector<QPointF> dataChannelG, QVector<QPointF> dataChannelB, bool editable, QWidget *parent):
	_dataChannelR(dataChannelR), _dataChannelG(dataChannelG), _dataChannelB(dataChannelB), _editable(editable)
{
    init();
}

GMAWidget::~GMAWidget()
{
	if(_pCurveChannelR) delete _pCurveChannelR;
	if(_pCurveChannelG) delete _pCurveChannelG;
	if(_pCurveChannelB) delete _pCurveChannelB;
	if(_edit) delete _edit;
}

QVector<QPointF> GMAWidget::getData(int channel)
{
	switch(channel)
	{
	case 0: 
		return _pCurveChannelR->data(_idR)[0]; // Red
	case 1: 
		return _pCurveChannelG->data(_idG)[0]; // Green
	case 2: 
		return _pCurveChannelB->data(_idB)[0]; // Blue
	}

	return QVector<QPointF>();
}

//
// private
//

void GMAWidget::init()
{
	Ui::GMAWidget::setupUi(this);

	// Set plots
	_pCurveChannelR = new QwtExtra::EditableCurve();
	_pCurveChannelG = new QwtExtra::EditableCurve();
	_pCurveChannelB = new QwtExtra::EditableCurve();
	colourTabs_tw->addTab(_pCurveChannelR, "Red");
	colourTabs_tw->addTab(_pCurveChannelG, "Green");
	colourTabs_tw->addTab(_pCurveChannelB, "Blue");

	// Set style for setting curve
	_penR.setColor(Qt::red);
	_penR.setWidth(3);
	_penR.setStyle(Qt::DashLine);
	_penG.setColor(Qt::green);
	_penG.setWidth(3);
	_penG.setStyle(Qt::DashLine);
	_penB.setColor(Qt::blue);
	_penB.setWidth(3);
	_penB.setStyle(Qt::DashLine);
	_symbolR = new QwtSymbol();
	_symbolR->setStyle(QwtSymbol::Ellipse);
	_symbolR->setBrush(Qt::red);
	_symbolR->setPen(Qt::red, 1);
	_symbolR->setSize(10, 10);
	_symbolG = new QwtSymbol();
	_symbolG->setStyle(QwtSymbol::Ellipse);
	_symbolG->setBrush(Qt::green);
	_symbolG->setPen(Qt::green, 1);
	_symbolG->setSize(10, 10);
	_symbolB = new QwtSymbol();
	_symbolB->setStyle(QwtSymbol::Ellipse);
	_symbolB->setBrush(Qt::blue);
	_symbolB->setPen(Qt::blue, 1);
	_symbolB->setSize(10, 10);

	// Add all curves to plot
	_idR = _pCurveChannelR->addCurve(_dataChannelR, tr("Red"));
	_idG = _pCurveChannelG->addCurve(_dataChannelG, tr("Green"));
	_idB = _pCurveChannelB->addCurve(_dataChannelB, tr("Blue"));

	// Set editables (only verticle)
	_pCurveChannelR->setEditable(_idR, false, _editable);
	_pCurveChannelG->setEditable(_idG, false, _editable);
	_pCurveChannelB->setEditable(_idB, false, _editable);

	// Set setting curve style
	_pCurveChannelR->curve(_idR)->setPen(_penR);
	_pCurveChannelR->curve(_idR)->setSymbol(_symbolR);
	_pCurveChannelG->curve(_idG)->setPen(_penG);
	_pCurveChannelG->curve(_idG)->setSymbol(_symbolG);
	_pCurveChannelB->curve(_idB)->setPen(_penB);
	_pCurveChannelB->curve(_idB)->setSymbol(_symbolB);

	// Set axis scaling
	_pCurveChannelR->setAxisScale(0, 0.0f, GMA_REG_MAX_VAL);
	_pCurveChannelG->setAxisScale(0, 0.0f, GMA_REG_MAX_VAL);
	_pCurveChannelB->setAxisScale(0, 0.0f, GMA_REG_MAX_VAL);

	connect(_pCurveChannelR, SIGNAL(pointMoved(int, int, QPointF)), this, SLOT(pointMovedChannelR(int, int, QPointF)));
	connect(_pCurveChannelG, SIGNAL(pointMoved(int, int, QPointF)), this, SLOT(pointMovedChannelG(int, int, QPointF)));
	connect(_pCurveChannelB, SIGNAL(pointMoved(int, int, QPointF)), this, SLOT(pointMovedChannelB(int, int, QPointF)));

	// Draw
	_pCurveChannelR->replot();
	_pCurveChannelG->replot();
	_pCurveChannelB->replot();

	connect(new_btn, SIGNAL(clicked()), this, SLOT(exportToNew()));
	connect(editRed_btn, SIGNAL(clicked()), this, SLOT(editRed()));
	connect(editGreen_btn, SIGNAL(clicked()), this, SLOT(editGreen()));
	connect(editBlue_btn, SIGNAL(clicked()), this, SLOT(editBlue()));
	connect(remove_btn, SIGNAL(clicked()), this, SLOT(removeGMA()));
	connect(range_lb, SIGNAL(currentIndexChanged(int)), this, SLOT(changeRange(int)));

	_edit = 0;
}

//
// public slots:
//

void GMAWidget::retranslate()
{
	Ui::GMAWidget::retranslateUi(this);
}

void GMAWidget::pointMovedChannelR(int curveIdx, int point, QPointF prevPosition)
{
	QVector<QPointF> *data = _pCurveChannelR->data(curveIdx);
	QPointF curr = data->at(point);
	double max = (range_lb->currentIndex() == 0)? GMA_REG_MAX_VAL:GMA_PRE_MAX_VAL;
	double min = 0.01f;

	if(curr.x() < min || curr.x() > max ||
		curr.y() < min || curr.y() > max)
	{
		data[0][point] = prevPosition;
		_pCurveChannelR->curve(curveIdx)->setSamples(*data);
		_pCurveChannelR->replot();
	}

	interpolatePlot(curveIdx, point, prevPosition, _pCurveChannelR, _idR);
}

void GMAWidget::pointMovedChannelG(int curveIdx, int point, QPointF prevPosition)
{
	interpolatePlot(curveIdx, point, prevPosition, _pCurveChannelG, _idG);
}

void GMAWidget::pointMovedChannelB(int curveIdx, int point, QPointF prevPosition)
{
	interpolatePlot(curveIdx, point, prevPosition, _pCurveChannelB, _idB);
}

void GMAWidget::interpolatePlot(int intcurveIdx, int point, QPointF prevPosition, QwtExtra::EditableCurve *curve, int id)
{
	QVector<QPointF> *data = curve->data(id);
    double currH = data->at(point).y();
    double oldH, delta;
    currH = data->at(point).y();
    oldH = prevPosition.y();
    delta = currH - oldH;

	int max = (range_lb->currentIndex() == 0)? GMA_REG_MAX_VAL:GMA_PRE_MAX_VAL;

	for(int i = 0; i < data->size(); i++)
    {
        double ph = data->at(i).y();

		if(i == point) 
		{
			continue;
		}

        if(ph > oldH)
		{
			ph = (ph-oldH)/(max-oldH) * (max-currH) + currH;
		}
        else
		{
            ph = currH - (oldH-ph)/(oldH) * (currH);
		}

		data[0][i] = QPointF(data[0][i].x(), ph);
    }
    curve->curve(id)->setSamples(*data);
	curve->replot();
}

void GMAWidget::exportToNew()
{
	QString name = QString();

	bool ok;
	name = QInputDialog::getText(this, tr("Export"), tr("Name:"), QLineEdit::Normal, QString(), &ok);
	if(ok)
	{
		if(name.isEmpty())
		{
			QMessageBox::warning(this, tr("Warning"), tr("Gamma LUT name empty!"));
			return;
		}

		int prevIndex = range_lb->currentIndex();
		if(prevIndex != 0)
		{
			range_lb->setCurrentIndex(0);
		}

		QVector<QPointF> dataRed;
		QVector<QPointF> dataGreen;
		QVector<QPointF> dataBlue;
		for(int i = 0; i < GMA_N_POINTS; i++)
		{
			dataRed.push_back(_pCurveChannelR->data(_idR)[0][i]);
			dataGreen.push_back(_pCurveChannelG->data(_idG)[0][i]);
			dataBlue.push_back(_pCurveChannelB->data(_idB)[0][i]);
		}

		range_lb->setCurrentIndex(prevIndex);

		emit requestExport(dataRed, dataGreen, dataBlue, true, name);
	}
}

void GMAWidget::removeGMA()
{
	emit requestRemove(this);
}

void GMAWidget::editRed()
{
	if(_edit)
	{
		_edit->deleteLater();
		delete _edit;
		_edit = NULL;
	}

	_edit = new EditPoints(_pCurveChannelR, _idR, "RED");
	_edit->show();
}

void GMAWidget::editGreen()
{
	if(_edit)
	{
		_edit->deleteLater();
		delete _edit;
		_edit = NULL;
	}

	_edit = new EditPoints(_pCurveChannelG, _idG, "GREEN");
	_edit->show();
}

void GMAWidget::editBlue()
{
	if(_edit)
	{
		_edit->deleteLater();
		delete _edit;
		_edit = NULL;
	}

	_edit = new EditPoints(_pCurveChannelB, _idB, "BLUE");
	_edit->show();
}

void GMAWidget::changeRange(int index)
{
	QVector<QPointF> *dataRed = _pCurveChannelR->data(_idR);
	QVector<QPointF> *dataGreen = _pCurveChannelG->data(_idG);
	QVector<QPointF> *dataBlue = _pCurveChannelB->data(_idB);

	for(int i = 0; i < dataRed->size(); i++)
	{
		if(index == 0)
		{
			dataRed[0][i] = QPointF(dataRed[0][i].x()*(1<<GMA_LUT_FRAC), dataRed[0][i].y()*(1<<GMA_LUT_FRAC));
			dataGreen[0][i] = QPointF(dataGreen[0][i].x()*(1<<GMA_LUT_FRAC), dataGreen[0][i].y()*(1<<GMA_LUT_FRAC));
			dataBlue[0][i] = QPointF(dataBlue[0][i].x()*(1<<GMA_LUT_FRAC), dataBlue[0][i].y()*(1<<GMA_LUT_FRAC));
		}
		else if(index = 1)
		{
			dataRed[0][i] = QPointF(dataRed[0][i].x()/(1<<GMA_LUT_FRAC), dataRed[0][i].y()/(1<<GMA_LUT_FRAC));
			dataGreen[0][i] = QPointF(dataGreen[0][i].x()/(1<<GMA_LUT_FRAC), dataGreen[0][i].y()/(1<<GMA_LUT_FRAC));
			dataBlue[0][i] = QPointF(dataBlue[0][i].x()/(1<<GMA_LUT_FRAC), dataBlue[0][i].y()/(1<<GMA_LUT_FRAC));
		}
	}

	_pCurveChannelR->curve(_idR)->setSamples(*dataRed);
	_pCurveChannelG->curve(_idG)->setSamples(*dataGreen);
	_pCurveChannelB->curve(_idB)->setSamples(*dataBlue);

	// Set axis scaling
	if(index == 0)
	{
		_pCurveChannelR->setAxisScale(0, 0.0f, GMA_REG_MAX_VAL);
		_pCurveChannelG->setAxisScale(0, 0.0f, GMA_REG_MAX_VAL);
		_pCurveChannelB->setAxisScale(0, 0.0f, GMA_REG_MAX_VAL);
	}
	else if(index = 1)
	{
		_pCurveChannelR->setAxisScale(0, 0.0f, GMA_PRE_MAX_VAL);
		_pCurveChannelG->setAxisScale(0, 0.0f, GMA_PRE_MAX_VAL);
		_pCurveChannelB->setAxisScale(0, 0.0f, GMA_PRE_MAX_VAL);
	}

	_pCurveChannelR->replot();
	_pCurveChannelG->replot();
	_pCurveChannelB->replot();
}