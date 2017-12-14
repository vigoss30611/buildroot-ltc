/**
******************************************************************************
@file lcapreview.cpp

@brief Implementation of LCAPreview

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

#include "lca/preview.hpp"

#include "imageview.hpp"

#include <QGraphicsLineItem>

#define CIRCLE_DIAMETER 10

/*
 * private
 */

void LCAPreview::init()
{
	Ui::LCAPreview::setupUi(this);
	
    pImageView = new ImageView();
	pImageView->menubar->hide();

	connect(channel, SIGNAL(currentIndexChanged(int)), this, SLOT(channelChanged(int)));
	connect(dispAmplifier, SIGNAL(valueChanged(double)), this, SLOT(displayVectors(double)));

    main_lay->addWidget(pImageView, main_lay->rowCount(), 0, 1, -1);
    pImageView->showStatistics(false);

    retranslate();
}

/*
 * public
 */ 

LCAPreview::LCAPreview(QWidget *parent): QWidget(parent)
{
	init();
}

void LCAPreview::setResults(const lca_output &output, const lca_input &param)
{
    parameters = param;
	results = output;
	
    CVBuffer cvBuff;

    cvBuff.data = output.inputImage;
    cvBuff.mosaic = MOSAIC_RGGB;
    cvBuff.pxlFormat = BAYER_RGGB_10;

    pImageView->loadBuffer(cvBuff);

    channelChanged(channel->currentIndex());
}

/*
 * public slots
 */
 
void LCAPreview::channelChanged(int sel)
{
    int c = channel->itemData(sel).toInt(); // use the QVariant info

    // polynomial info
    if ( results.coeffsX[c].rows > 0 )
    {
	    polyX0->setValue(results.coeffsX[c].at<double>(0));
        polyX1->setValue(results.coeffsX[c].at<double>(1));
        polyX2->setValue(results.coeffsX[c].at<double>(2));

        polyY0->setValue(results.coeffsY[c].at<double>(0));
        polyY1->setValue(results.coeffsY[c].at<double>(1));
        polyY2->setValue(results.coeffsY[c].at<double>(2));
	
        // set error
        errorX->setValue(results.errorsX[c]);
        errorY->setValue(results.errorsY[c]);

	    // load image with amp
    	displayVectors();
    }
}

void LCAPreview::displayVectors(double amplifier)
{
	if ( amplifier <= 0.0 )
    {
        amplifier = dispAmplifier->value();
    }

    pImageView->clearButImage();
    int c = channel->itemData(channel->currentIndex()).toInt(); // use the QVariant info
    QPen pen(Qt::red);
    if ( c == lca_output::BLUE_GREEN0 || c == lca_output::BLUE_GREEN1 )
    {
        pen.setColor(Qt::blue);
    }
    pen.setWidth(2);

    std::vector<BlockDiff>::const_iterator it;
    for ( it = results.blockDiffs[c].begin() ; it != results.blockDiffs[c].end() ; it++ )
    {
        QGraphicsLineItem *line = new QGraphicsLineItem(
            it->x, it->y,
            it->x+it->dX*amplifier, it->y+it->dY*amplifier
        );
        
        line->setPen(pen);
        pImageView->getScene()->addItem(line);
    }

    QGraphicsEllipseItem *circle = new QGraphicsEllipseItem(parameters.centerX - (CIRCLE_DIAMETER/2), parameters.centerY - (CIRCLE_DIAMETER/2), CIRCLE_DIAMETER, CIRCLE_DIAMETER);
    circle->setPen(pen);
    pImageView->getScene()->addItem(circle);
} 

void LCAPreview::retranslate()
{
    Ui::LCAPreview::retranslateUi(this);

    int c = channel->currentIndex();

    channel->clear();
    channel->insertItem(channel->count(), tr("Red / Green 0"), lca_output::RED_GREEN0);
    channel->insertItem(channel->count(), tr("Red / Green 1"), lca_output::RED_GREEN1);
    channel->insertItem(channel->count(), tr("Blue / Green 0"), lca_output::BLUE_GREEN0);
    channel->insertItem(channel->count(), tr("Blue / Green 1"), lca_output::BLUE_GREEN1);

    if ( c < 0 )
    {
        c = 0;
    }
    channel->setCurrentIndex(c);
}
