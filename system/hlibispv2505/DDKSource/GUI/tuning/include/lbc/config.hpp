/********************************************************************************
@file lbc/config.hpp

@brief Display the LBCWidget needed for LBCTuning

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
#ifndef LBCCONFIG_HPP
#define LBCCONFIG_HPP

#include "ui/ui_lbcwidget.h"
#include <QDialog>
#include <QtExtra/boxslider.hpp>

/**
 * @ingroup VISION_TUNING
 */
class LBCConfig: public QDialog, public Ui::LBCWidget
{
	Q_OBJECT
	
private:
	void init();
	QtExtra::DoubleBoxSlider *sharpnessDoubleBoxSlider;
	QtExtra::DoubleBoxSlider *brightnessDoubleBoxSlider;
	QtExtra::DoubleBoxSlider *contrastDoubleBoxSlider;
	QtExtra::DoubleBoxSlider *saturationDoubleBoxSlider;

public:
	LBCConfig(QWidget *parent = 0);

signals:
	void removeSig(LBCConfig*);

public slots:
	void reqRemoveSlot();

};

#endif //LBCCONFIG_HPP