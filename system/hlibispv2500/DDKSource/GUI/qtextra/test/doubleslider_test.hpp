/**
******************************************************************************
@file doubleslider_test.hpp

@brief unit tests for DoubleSlider

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
#include <QtHelp/QtHelp>

#include "QtExtra/doubleslider.hpp"

namespace QtExtraTest {

#define N_INTERVALS 3

class DoubleSliderTest: public QObject
{
    Q_OBJECT
private:
	QtExtra::DoubleSlider *slider;
    

    int intervalsI[N_INTERVALS][2]; // minI/maxI
    double intervalsD[N_INTERVALS][2]; // minD/maxD
	
public:
	DoubleSliderTest();

private slots:
	void initTestCase();
	void cleanupTestCase();

    void fromDoubleAndInt_Test_data();
    void fromDoubleAndInt_Test();
};

}
