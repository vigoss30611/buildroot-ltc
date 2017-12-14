/**
*******************************************************************************
@file lshgridview.cpp

@brief LSHGridView implementation

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
#include "lsh/gridview.hpp"

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_symbol.h>

#include <algorithm>

#include "lsh/compute.hpp"

/*
 * LSHGridView
 */

/*
 * private
 */

void LSHGridView::init()
{
    Ui::LSHGridView::setupUi(this);
    data = 0;
    inputs = 0;

    sliceLine = 0;

    inputSlice = new QwtPlot();
    inputSlice->setTitle(tr("Input image slice"));
    infoGrp_lay->addWidget(inputSlice, infoGrp_lay->rowCount(), 0, 1, -1);

    gridSlice = new QwtPlot();
    gridSlice->setTitle(tr("Normalised grid slice"));
    infoGrp_lay->addWidget(gridSlice, infoGrp_lay->rowCount(), 0, 1, -1);

    pGridView = new ImageView();
    pGridView->menuBar()->hide();
    view_lay->addWidget(pGridView);

    inputSliceCurve = 0;
    gridSliceCurve = 0;
    gridTargetCurve = 0;
    gridTargetMSECurve = 0;

    pGridView->showStatistics(false);

    connect(channel, SIGNAL(currentIndexChanged(int)), this,
        SLOT(channelChanged(int)));
    connect(curve, SIGNAL(currentIndexChanged(int)), this,
        SLOT(channelChanged(int)));
    connect(pGridView, SIGNAL(mousePressed(qreal, qreal, Qt::MouseButton)),
        this, SLOT(mousePressed(qreal, qreal, Qt::MouseButton)));
    connect(lineSlider, SIGNAL(valueChanged(int)), this,
        SLOT(lineChanged(int)));
    connect(outputColour, SIGNAL(currentIndexChanged(int)), this,
        SLOT(channelChanged(int)));
    connect(targetColour, SIGNAL(currentIndexChanged(int)), this,
        SLOT(channelChanged(int)));
    connect(targetMSEColour, SIGNAL(currentIndexChanged(int)), this,
        SLOT(channelChanged(int)));

    targetColour->setVisible(false);
    targetColour_lbl->setVisible(false);

    targetMSEColour->setVisible(false);
    targetMSEColour_lbl->setVisible(false);

    retranslate();
}

/*
 * public
 */

LSHGridView::LSHGridView(QWidget *parent) : QWidget(parent)
{
    init();
}

LSHGridView::~LSHGridView()
{
    if (this->data)
    {
        delete this->data;
    }
    if (this->inputs)
    {
        delete this->inputs;
    }
}

/*
 * public slots
 */

void LSHGridView::channelChanged(int notused)
{
    int c = channel->currentIndex();
    if (this->data)
    {
        CVBuffer sBuff;

        sBuff.mosaic = MOSAIC_NONE;
        sBuff.pxlFormat = RGB_888_24;
        sBuff.data = this->data->normalised[c];

        pGridView->loadBuffer(sBuff, false);

        // load slice
        lineSlider->setMinimum(0);
        lineSlider->setMaximum(this->data->channels[0].rows);
        lineChanged(sliceLine);
    }
}

void LSHGridView::lineChanged(int l)
{
    int c = channel->currentIndex();
    double fmin, fmax;
    int x;

    sliceLine = l;

    if (!this->data || !this->inputs
        || l < 0 || l >= this->data->normalised[c].rows)
    {
        return;
    }

    lineSlider->setValue(l);

    QString nfo = tr("Line %n", "", l) + tr("/%n", "",
        this->data->normalised[c].rows - 1);
    if (this->data->targetLSH[c].cols > 0)
    {
        nfo += tr(" - channel fitting mse ")
            + QString::number(this->data->fittingMSE[c]);
    }
    if (this->data->outputMSE[c].cols > 0)
    {
        // compute slice min/max
        // do not use minMaxLoc because we want the min/max for that line only
        fmin = data->outputMSE[c].at<float>(l, 0);
        fmax = fmin;
        for (x = 1; x < data->outputMSE[c].cols; x++)
        {
            fmin = std::min(fmin,
                static_cast<double>(data->outputMSE[c].at<float>(l, x)));
            fmax = std::max(fmax,
                static_cast<double>(data->outputMSE[c].at<float>(l, x)));
        }

        nfo += tr(" - slice output error [")
            + QString::number(fmin)
            + tr("; ") + QString::number(fmax) + tr("]");
    }
    lineInfo_lbl->setText(nfo);

    pGridView->clearButImage();
    cv::Rect lineCoord(0, l, this->data->channels[c].cols, 2);
    pGridView->addGrid(&lineCoord, 1, 1, QPen(Qt::green));

    if (inputSliceCurve)
    {
        inputSliceCurve->detach();
        delete inputSliceCurve;
    }
    inputSliceCurve = new QwtPlotCurve();

    QVector<QPointF>curveSliceData;
    QPen curveStyle;

    if (curve->currentIndex() == 0)
    {
        for (x = inputs->margins[0];
            x < this->data->inputImage[c].cols - inputs->margins[2];
            x++)
        {
            curveSliceData.push_back(QPointF(x,
                this->data->inputImage[c].at<float>(sliceLine, x)));
        }
    }
    else
    {
        for (x = inputs->margins[0];
            x < this->data->smoothedImage[c].cols - inputs->margins[2];
            x++)
        {
            curveSliceData.push_back(QPointF(x,
                this->data->smoothedImage[c].at<float>(sliceLine, x)));
        }
    }

    curveStyle.setColor(Qt::black);
    inputSliceCurve->setSamples(curveSliceData);
    inputSliceCurve->setPen(curveStyle);

    inputSliceCurve->attach(inputSlice);
    inputSlice->setAxisAutoScale(QwtPlot::yLeft, true);
    inputSlice->setAxisScale(QwtPlot::xBottom, 0.0,
        this->data->inputImage[c].cols);

    inputSlice->replot();

    curveSliceData.clear();

    // clear curves
    if (gridSliceCurve)
    {
        gridSliceCurve->detach();
        delete gridSliceCurve;
        gridSliceCurve = 0;
    }
    if (gridTargetCurve)
    {
        gridTargetCurve->detach();
        delete gridTargetCurve;
        gridTargetCurve = 0;
    }
    if (gridTargetMSECurve)
    {
        gridTargetMSECurve->detach();
        delete gridTargetMSECurve;
        gridTargetMSECurve = 0;
    }

    // get display colours
    Qt::GlobalColor outputColor = (Qt::GlobalColor)outputColour->\
        itemData(outputColour->currentIndex()).toInt();
    Qt::GlobalColor targetColor = (Qt::GlobalColor)targetColour->\
        itemData(targetColour->currentIndex()).toInt();
    Qt::GlobalColor targetMSEColor = (Qt::GlobalColor)targetMSEColour->\
        itemData(targetMSEColour->currentIndex()).toInt();

    // display curves which have data and are not transparent
    if (outputColor != Qt::transparent)
    {
        gridSliceCurve = new QwtPlotCurve();

        for (x = 0; x < this->data->channels[c].cols; x++)
        {
            curveSliceData.push_back(QPointF(x * inputs->tileSize,
                this->data->channels[c].at<float>(sliceLine, x)));
        }

        curveStyle.setColor(outputColor);
        gridSliceCurve->setSamples(curveSliceData);
        gridSliceCurve->setPen(curveStyle);

        gridSliceCurve->attach(gridSlice);
        curveSliceData.clear();
    }

    /* because the target is 1 less line than actual curve (we need an
     * extra line in the curve for interpolation in HW) */
    if (this->data->targetLSH[c].cols > 0
        && sliceLine < this->data->targetLSH[c].rows
        && targetColor != Qt::transparent)
    {
        gridTargetCurve = new QwtPlotCurve();

        for (x = 0; x < this->data->targetLSH[c].cols; x++)
        {
            curveSliceData.push_back(QPointF(x * inputs->tileSize,
                this->data->targetLSH[c].at<float>(sliceLine, x)));
        }

        curveStyle.setColor(targetColor);
        gridTargetCurve->setSamples(curveSliceData);
        gridTargetCurve->setPen(curveStyle);

        gridTargetCurve->attach(gridSlice);
        curveSliceData.clear();
    }

    if (this->data->outputMSE[c].cols > 0
        && sliceLine < this->data->outputMSE[c].rows
        && targetMSEColor != Qt::transparent)
    {
        gridTargetMSECurve = new QwtPlotCurve();

        cv::minMaxLoc(data->outputMSE[c], &fmin, &fmax);

        for (x = inputs->margins[0];
            x < this->data->outputMSE[c].cols - inputs->margins[2];
            x++)
        {
            curveSliceData.push_back(QPointF(x,
                this->data->outputMSE[c].at<float>(sliceLine, x) / fmax));
        }

        curveStyle.setColor(targetMSEColor);
        gridTargetMSECurve->setSamples(curveSliceData);
        gridTargetMSECurve->setPen(curveStyle);

        gridTargetMSECurve->attach(gridSlice);
        curveSliceData.clear();
    }

    gridSlice->setAxisAutoScale(QwtPlot::yLeft, true);
    gridSlice->setAxisScale(QwtPlot::xBottom, 0.0,
        this->data->channels[c].cols* inputs->tileSize);
    gridSlice->replot();
}

void LSHGridView::loadData(const lsh_output *pOutput, const lsh_input *pInput)
{
    if (this->data)
    {
        delete this->data;
    }
    this->data = new lsh_output(*pOutput);
    if (this->inputs)
    {
        delete this->inputs;
    }
    this->inputs = new lsh_input(*pInput);

    targetColour->setVisible(this->data->targetLSH[0].cols > 0);
    targetColour_lbl->setVisible(this->data->targetLSH[0].cols > 0);

    targetMSEColour->setVisible(this->data->outputMSE[0].cols > 0);
    targetMSEColour_lbl->setVisible(this->data->outputMSE[0].cols > 0);

    channelChanged(channel->currentIndex());
    pGridView->setMaximumWidth(this->data->normalised[0].cols
        + this->data->normalised[0].cols / 4);
}

void LSHGridView::retranslate()
{
    Ui::LSHGridView::retranslateUi(this);

    int c0 = targetColour->currentIndex();
    int c1 = targetMSEColour->currentIndex();
    int c2 = outputColour->currentIndex();
    QString n;
    int t;

    targetColour->clear();
    targetMSEColour->clear();

    n = tr("No curve");
    t = static_cast<int>(Qt::transparent);
    targetColour->insertItem(targetColour->count(), n, t);
    targetMSEColour->insertItem(targetMSEColour->count(), n, t);
    outputColour->insertItem(targetMSEColour->count(), n, t);

    n = tr("Green");
    t = static_cast<int>(Qt::green);
    targetColour->insertItem(targetColour->count(), n, t);
    targetMSEColour->insertItem(targetMSEColour->count(), n, t);
    outputColour->insertItem(targetMSEColour->count(), n, t);

    n = tr("Red");
    t = static_cast<int>(Qt::red);
    targetColour->insertItem(targetColour->count(), n, t);
    targetMSEColour->insertItem(targetMSEColour->count(), n, t);
    outputColour->insertItem(targetMSEColour->count(), n, t);

    n = tr("Blue");
    t = static_cast<int>(Qt::blue);
    targetColour->insertItem(targetColour->count(), n, t);
    targetMSEColour->insertItem(targetMSEColour->count(), n, t);
    outputColour->insertItem(targetMSEColour->count(), n, t);

    n = tr("Black");
    t = static_cast<int>(Qt::black);
    targetColour->insertItem(targetColour->count(), n, t);
    targetMSEColour->insertItem(targetMSEColour->count(), n, t);
    outputColour->insertItem(targetMSEColour->count(), n, t);

    if (c0 < 0)
    {
        c0 = 1;  // default is green
    }
    targetColour->setCurrentIndex(c0);
    if (c1 < 0)
    {
        c1 = 2;  // default is red
    }
    targetMSEColour->setCurrentIndex(c1);
    if (c2 < 0)
    {
        c2 = 3;  // default is blue
    }
    outputColour->setCurrentIndex(c2);

    c0 = curve->currentIndex();
    curve->clear();
    curve->insertItem(0, tr("Loaded image slice"));
    curve->insertItem(1, tr("Smoothed image slice"));
    if (c0 < 0)
    {
        c0 = 0;  // default is image slice
    }
    curve->setCurrentIndex(c0);
}

void LSHGridView::mousePressed(qreal x, qreal y, Qt::MouseButton button)
{
    lineChanged(floor(y));
}

