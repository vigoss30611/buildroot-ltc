/**
******************************************************************************
@file doubleslider_test.cpp

@brief Test DoubleSlider class

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
#include "doubleslider_test.hpp"

#include "QtExtra/doubleslider.hpp"

#include <QtTest/QTest>
#include <QtTest/QSignalSpy>

using namespace QtExtraTest;

#define QCOMPARE_DBL(actual, expected, delta) QVERIFY(actual-delta <= expected && actual+delta >=expected)

// private

void QtExtraTest::DoubleSliderTest::initTestCase()
{
	if ( !slider )
		slider = new QtExtra::DoubleSlider();
    
    intervalsI[0][0] = 0;       intervalsD[0][0] = -1.0f;
    intervalsI[0][1] = 360;    intervalsD[0][1] = 1.0f;

    intervalsI[1][0] = 500;    intervalsD[1][0] = 0.0f;
    intervalsI[1][1] = 600;    intervalsD[1][1] = 1.0f;

    intervalsI[2][0] = -200;    intervalsD[2][0] = 1.0f;
    intervalsI[2][1] = 200;    intervalsD[2][1] = 1.5f;
}

void QtExtraTest::DoubleSliderTest::cleanupTestCase()
{
	if ( slider )
	{
		delete slider;
		slider = 0;
	}
}

// public

QtExtraTest::DoubleSliderTest::DoubleSliderTest(): QObject()
{
    //init();
	slider = 0;
}

// private slots

void QtExtraTest::DoubleSliderTest::fromDoubleAndInt_Test_data()
{
    QTest::addColumn<int>("interval");
    QTest::addColumn<int>("integer");
    QTest::addColumn<double>("doublev");

    // interval, integer, double
    for ( int i = 0 ; i < N_INTERVALS ; i++)
    {
        QTest::newRow("min") << i << intervalsI[i][0] << intervalsD[i][0];
        QTest::newRow("max") << i << intervalsI[i][1] << intervalsD[i][1];
        QTest::newRow("middle") << i << (intervalsI[i][0] +(intervalsI[i][1]-intervalsI[i][0])/2) << (intervalsD[i][0] +(intervalsD[i][1]-intervalsD[i][0])/2);
        QTest::newRow("3/4") << i << (intervalsI[i][0] +3*(intervalsI[i][1]-intervalsI[i][0])/4) << (intervalsD[i][0] +3.0*(intervalsD[i][1]-intervalsD[i][0])/4);
        QTest::newRow("1/5") << i << (intervalsI[i][0] +(intervalsI[i][1]-intervalsI[i][0])/5) << (intervalsD[i][0] +(intervalsD[i][1]-intervalsD[i][0])/5);
    }
}

void QtExtraTest::DoubleSliderTest::fromDoubleAndInt_Test()
{
	QSignalSpy integerSpy(slider, SIGNAL(valueChanged(int)));
	QSignalSpy doubleSpy(slider, SIGNAL(doubleValueChanged(double)));
		
	QVERIFY(integerSpy.isValid());
	QVERIFY(doubleSpy.isValid());
	
    // interval, integer, double
    QFETCH(int, interval);
    QFETCH(int, integer);
    QFETCH(double, doublev);

    slider->setMinimum(intervalsI[interval][0]); slider->setMaximum(intervalsI[interval][1]);
    slider->setMinimumD(intervalsD[interval][0]); slider->setMaximumD(intervalsD[interval][1]);

	if ( slider->value() == integer )
		slider->setValue(integer+1); // to generate the value changed!

	slider->setValue(integer);
	
    QCOMPARE_DBL(slider->doubleValue(), doublev, 0.001);
	QCOMPARE(slider->value(), integer);
		
	QVERIFY(!integerSpy.isEmpty());
	QVERIFY(!doubleSpy.isEmpty());
	//QCOMPARE(doubleSpy.count(), 1);

	QCOMPARE(integerSpy.takeLast().at(0).toInt(), integer);
    double v = doubleSpy.takeLast().at(0).toDouble();
	QCOMPARE_DBL(v, doublev, 0.001);

	slider->setDoubleValue(doublev);
	QCOMPARE_DBL(slider->doubleValue(), doublev, 0.001);
	QCOMPARE(slider->value(), integer);

	//QCOMPARE(integerSpy.count(), 2);
	//QCOMPARE(doubleSpy.count(), 2);
    
	//QCOMPARE(integerSpy.takeLast().at(0).toInt(), integer);
	//QCOMPARE(doubleSpy.takeLast().at(0).toDouble(), doublev);
}

QTEST_MAIN(DoubleSliderTest)
