/**
******************************************************************************
 @file boxslider.cpp

 @brief BoxSlider and DoubleBoxSlider class implementations

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
#include "QtExtra/boxslider.hpp"

#include <QWidget>
#include <QSlider>
#include <QSpinBox>
#include <QLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>

#include <cmath>
#include <float.h> // DBL_MAX_10_EXP
#ifdef USE_MATH_NEON
#include <mneon.h>
#endif

using namespace QtExtra;

/*---------------
 *
 * BoxSlider
 *
 *---------------*/

// private

void BoxSlider::init(Qt::Orientation orientation)
{
	layout = 0;
	setOrientation(orientation);

    //setFocusProxy(spinBox);
    //setFocusPolicy(Qt::StrongFocus);

	slider->setMinimum(spinBox->minimum());
	slider->setMaximum(spinBox->maximum());

    connect(spinBox, SIGNAL(valueChanged(int)), slider, SLOT(setValue(int)) );
    connect(slider, SIGNAL(valueChanged(int)), spinBox, SLOT(setValue(int)) );
    connect(spinBox, SIGNAL(valueChanged(int)), this, SIGNAL(valueChanged(int)) );
}

// public

BoxSlider::BoxSlider(QWidget* parent): QWidget(parent)
{
    imported = false;
    spinBox = new QSpinBox();
    slider = new QSlider();

	init();
}

BoxSlider::BoxSlider(Qt::Orientation orientation, QWidget *parent): QWidget(parent)
{
    imported = false;
    spinBox = new QSpinBox();
    slider = new QSlider();

    init(orientation);
}

BoxSlider::BoxSlider(QSpinBox *pSpin, QSlider *pSlide, QWidget *parent): QWidget(parent)
{
    imported = true;
    spinBox = pSpin;
    slider = pSlide;

    init();
}

int BoxSlider::value() const
{
    return spinBox->value();
}

int BoxSlider::minimum(void) const
{
    return spinBox->minimum();
}

int BoxSlider::maximum(void) const
{
    return spinBox->maximum();
}

void BoxSlider::setMinimum(int newMin)
{
    spinBox->setMinimum(newMin);
    slider->setMinimum(newMin);
}

void BoxSlider::setMaximum(int newMax)
{
    spinBox->setMaximum(newMax);
    slider->setMaximum(newMax);
}

Qt::Orientation BoxSlider::orientation(void) const
{
	return slider->orientation();
}

// slots

void BoxSlider::setValue(int v)
{
    spinBox->setValue(v);
}

void BoxSlider::setOrientation(Qt::Orientation orientation)
{
    if ( imported ) return;
	if ( layout != NULL )
	{
		layout->removeWidget(spinBox);
		layout->removeWidget(slider);
		delete layout;
	}
	if ( orientation == Qt::Horizontal )
    {
        layout  = new QHBoxLayout(this);
		
        layout->addWidget(spinBox);
        layout->addWidget(slider);
    }
    else
    {
        layout = new QVBoxLayout(this);
        ((QVBoxLayout*)layout)->addWidget(slider, 0, Qt::AlignHCenter);
        layout->addWidget(spinBox);
    }
	layout->setMargin(0);
	slider->setOrientation(orientation);
}

/*---------------------
 *
 * DoubleBoxSlider
 *
 *---------------------*/

void DoubleBoxSlider::init(Qt::Orientation orientation)
{
	layout = 0;

	slider->setMinimumD(spinBox->minimum());
	slider->setMaximumD(spinBox->maximum());

	setOrientation(orientation);
	setDecimals(spinBox->decimals()); // integer min and max for the slider
        
    //setFocusProxy(spinBox);
    //setFocusPolicy(Qt::StrongFocus);
    
    connect(spinBox, SIGNAL(valueChanged(double)), slider, SLOT(setDoubleValue(double)) );
    connect(slider, SIGNAL(doubleValueChanged(double)), spinBox, SLOT(setValue(double)) );
    connect(spinBox, SIGNAL(valueChanged(double)), this, SIGNAL(valueChanged(double)) );
}

// public

DoubleBoxSlider::DoubleBoxSlider(QWidget* parent): QWidget(parent)
{
    imported = false;
    spinBox = new QDoubleSpinBox();
    slider = new DoubleSlider();
    
	init();
}

DoubleBoxSlider::DoubleBoxSlider(Qt::Orientation orientation, QWidget *parent): QWidget(parent)
{
    imported = false;
    spinBox = new QDoubleSpinBox();
    slider = new DoubleSlider();

    init(orientation);
}

DoubleBoxSlider::DoubleBoxSlider(QDoubleSpinBox *pSpin, DoubleSlider *pSlide, QWidget *parent): QWidget(parent)
{
    imported = true;
    spinBox = pSpin;
    slider = pSlide;
    this->setVisible(false);
    
    init();
}

double DoubleBoxSlider::value() const
{
    return spinBox->value();
}

double DoubleBoxSlider::minimum(void) const
{
    return spinBox->minimum();
}

double DoubleBoxSlider::maximum(void) const
{
    return spinBox->maximum();
}

void DoubleBoxSlider::setMinimum(double newMin)
{
    spinBox->setMinimum(newMin);
    slider->setMinimumD(newMin);
}

void DoubleBoxSlider::setMaximum(double newMax)
{
    spinBox->setMaximum(newMax);
    slider->setMaximumD(newMax);
}

Qt::Orientation DoubleBoxSlider::orientation(void) const
{
	return slider->orientation();
}

int DoubleBoxSlider::decimals() const
{
	return spinBox->decimals();
}

double DoubleBoxSlider::singleStep() const
{
	return spinBox->singleStep();
}

// slots

void DoubleBoxSlider::setValue(double v)
{
    spinBox->setValue(v);
}

void DoubleBoxSlider::setOrientation(Qt::Orientation orientation)
{
    if ( imported ) return;
	if ( layout != NULL )
	{
		layout->removeWidget(spinBox);
		layout->removeWidget(slider);
		delete layout;
	}
	if ( orientation == Qt::Horizontal )
    {
        layout  = new QHBoxLayout(this);
		
        layout->addWidget(spinBox);
        layout->addWidget(slider);
    }
    else
    {
        layout = new QVBoxLayout(this);
        ((QVBoxLayout*)layout)->addWidget(slider, 0, Qt::AlignHCenter);
        layout->addWidget(spinBox);
    }
	layout->setMargin(0);
	slider->setOrientation(orientation);
}

void DoubleBoxSlider::setDecimals(int precs)
{
	spinBox->setDecimals(precs);
	// computes the maximum value of the slider so that it can represent the precision+ the max nb of digits held by the min or max
	double maxnb = fabs(minimum());
	int mult = 0;

	if ( fabs(maximum()) > maxnb ) maxnb = fabs(maximum());

	mult=10;
#ifdef USE_MATH_NEON
	for ( int i = 1 ; i < (int)(floor((float)log10f_neon((float)maxnb)))+1 +precs ; i++)
		mult*=10;
#else
	for ( int i = 1 ; i < (int)(floor(log10(maxnb)))+1 +precs ; i++)
		mult*=10;
#endif

	slider->setMaximum(mult);
}

void DoubleBoxSlider::setSingleStep(double step)
{
	spinBox->setSingleStep(step);
}
