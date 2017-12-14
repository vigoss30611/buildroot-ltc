/**
******************************************************************************
 @file doubleslider.cpp

 @brief DoubleSlider class implementation

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
#include "QtExtra/doubleslider.hpp"

#include <QWidget>
#include <QSlider>

#include <cmath>
#ifdef USE_MATH_NEON
#include <mneon.h>
#endif

static int cint(double x)
{
	int res;
	double intpart; // to avoid segfault in WIN32
#ifdef USE_MATH_NEON
	if (modf_neon((float)x,(float)&intpart)>=0.5f)
#else
	if (modf(x,&intpart)>=0.5f)
#endif
		res= (int) (x>=0?ceil(x):floor(x));
	else
		res= (int) (x<0?floor(x):ceil(x));
	return res;
}

using namespace QtExtra;

// private

void DoubleSlider::init()
{
    QSlider::setMinimum(0);
    QSlider::setMaximum(1000);
    
    connect(this, SIGNAL(valueChanged(int)), this, SLOT(reemitValueChanged(int)) );
}

// protected

int DoubleSlider::fromDouble(double v) const
{
    double result = (maximum()-minimum())/(_maxd-_mind); // range
    result = result*(v-_mind) + minimum();
    return (int)cint(result);
}

double DoubleSlider::fromInt(int v) const
{
    double result = (_maxd-_mind)/(maximum()-minimum()); // range
    result = (v - minimum())*result + _mind;
    return result;
}

// public

DoubleSlider::DoubleSlider(QWidget *parent): QSlider(parent)
{
    _mind = 0.0;
    _maxd = 1.0;
    init();
}

DoubleSlider::DoubleSlider(double minimum, double maximum, QWidget *parent): QSlider(parent)
{
    _mind = minimum;
    _maxd = maximum;
    init();
}

double DoubleSlider::doubleValue(void) const
{
    return fromInt(value());
}

// slots

void DoubleSlider::setDoubleValue(double newValue)
{
	blockSignals(true);
	{
		QSlider::setValue(fromDouble(newValue));
	}
	blockSignals(false);
}

void DoubleSlider::reemitValueChanged(int newValue)
{
    emit doubleValueChanged(fromInt(newValue));
}

void DoubleSlider::setMaximumD(double newMax)
{
	double currentValue = doubleValue();
	_maxd = newMax;
	setDoubleValue(currentValue);
}

void DoubleSlider::setMinimumD(double newMin)
{
	double currentValue = doubleValue();
	_mind = newMin;
	setDoubleValue(currentValue);
}

void DoubleSlider::setMaximum(int newMax)
{
	double currentValue = doubleValue();
	QSlider::setMaximum(newMax);
	setDoubleValue(currentValue);
}

void DoubleSlider::setMinimum(int newMin)
{
	double currentValue = doubleValue();
	QSlider::setMinimum(newMin);
	setDoubleValue(currentValue);
}
