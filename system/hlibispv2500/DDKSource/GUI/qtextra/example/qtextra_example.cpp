/**
******************************************************************************
@file qtextra_example.cpp

@brief Example of simple widget

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
#include <QApplication>

#include "swapbutton.hpp"

#include <QtExtra/boxslider.hpp>
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>

SwapWidget::SwapWidget(QWidget *parent): QWidget(parent)
{
	QVBoxLayout *vLayout = new QVBoxLayout(this);
	QPushButton *button = new QPushButton(tr("Swap"));
	slider = new QtExtra::BoxSlider();
	sliderD = new QtExtra::DoubleBoxSlider();

	vLayout->addWidget(button);
	vLayout->addWidget(slider);
	vLayout->addWidget(sliderD);

	connect(button, SIGNAL(clicked()), this, SLOT(swapOrientation()));
}

void SwapWidget::swapOrientation(void)
{
	if ( slider->orientation() == Qt::Horizontal )
	{
		slider->setOrientation(Qt::Vertical);
		sliderD->setOrientation(Qt::Vertical);
	}
	else
	{
		slider->setOrientation(Qt::Horizontal);
		sliderD->setOrientation(Qt::Horizontal);
	}

}

int main(int argc, char** argv)
{
	QApplication app(argc, argv);
	
	SwapWidget wid;

	wid.show();
	
	return app.exec();
}
