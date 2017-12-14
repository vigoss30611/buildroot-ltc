/**
******************************************************************************
@file swapbutton.hpp

@brief SwapWidget declaration for example in QtExtra

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
#include <QtExtra/boxslider.hpp>
#include <QWidget>

class SwapWidget: public QWidget
{
	Q_OBJECT
private:
	QtExtra::BoxSlider *slider;
	QtExtra::DoubleBoxSlider *sliderD;

public:
	SwapWidget(QWidget *parent=0);

public slots:
	void swapOrientation(void);
};
