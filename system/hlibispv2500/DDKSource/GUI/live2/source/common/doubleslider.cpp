#include "doubleslider.hpp"

//
// Public Functions
//

VisionLive::DoubleSlider::DoubleSlider(QWidget *parent): QSlider(parent)
{
	_precision = 1000.0f;

	init();
}

VisionLive::DoubleSlider::~DoubleSlider()
{
}

void VisionLive::DoubleSlider::setMinimumD(double value)
{
	setMinimum(int(value*_precision)); 
}

void VisionLive::DoubleSlider::setMaximumD(double value)
{
	setMaximum(int(value*_precision)); 
}

double VisionLive::DoubleSlider::getValueD() 
{ 
	return (((double)value())/_precision); 
}

//
// Private Functions
//

void VisionLive::DoubleSlider::init()
{
	setMinimum(minimum()*_precision);
	setMaximum(maximum()*_precision);

	setOrientation(Qt::Horizontal);

	QObject::connect(this, SIGNAL(valueChanged(int)), this, SLOT(reemitValueChanged()));
}

//
// Public Slots
//

void VisionLive::DoubleSlider::setValueD(double value)
{ 
	setValue(int(value*_precision)); 

	emit valueChanged(getValueD());
}

//
// Private Slots
//

void VisionLive::DoubleSlider::reemitValueChanged()
{
	emit valueChanged(getValueD());
}
