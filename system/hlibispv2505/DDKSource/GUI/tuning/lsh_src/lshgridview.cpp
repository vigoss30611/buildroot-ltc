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

#include "lsh/tuning.hpp"
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

    sliceLine = 0;
    matLine = 0;

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

    targetColour->setVisible(false);
    targetColour_lbl->setVisible(false);

    targetMSEColour->setVisible(false);
    targetMSEColour_lbl->setVisible(false);

    retranslate();
}

void LSHGridView::updateDisplay()
{
    const int c = channel->currentIndex();
    double mseMin = 0.0, mseMax = 0.0;
    cv::Point matMaxLoc, targMaxLoc;
    double matrixMax = 0.0, targetMax = 0.0;
    cv::Point inMaxLoc;
    double inputMax = 0.0;  // used for input or smoothed image
    int x;
    // is it in the input map? - needed to access margins
    lsh_input::const_iterator it;
    int tempIdx = temperature->currentIndex();

    if (tempIdx < 0 || tempIdx >= (int)data.size())
    {
        return;
    }
    const lsh_output &sdata = data[tempIdx];

    // for the moment skip if asked temperature is not an input image
    it = inputs.inputs.find(sdata.temperature);

    QString nfo = tr("Matrix line %n", "followed by max nb line on matrix",
        matLine)
        + tr("/%n", "", sdata.normalised[c].rows - 1);
    if (!sdata.interpolated)
    {
        nfo += tr(" - Input image line %n", "", sliceLine)
            + tr("/%n", "", sdata.inputImage[0].rows - 1);
    }
    else
    {
        nfo += tr(" - Interpolated output");
    }
    if (sdata.targetLSH[c].cols > 0)
    {
        nfo += tr(" - channel fitting mse ", "followed by value")
            + QString::number(sdata.fittingMSE[c]);
    }
    if (sdata.outputMSE[c].cols > 0)
    {
        // compute slice min/max
        // do not use minMaxLoc because we want the min/max for that line only
        mseMin = sdata.outputMSE[c].at<float>(matLine, 0);
        mseMax = mseMin;
        for (x = 1; x < sdata.outputMSE[c].cols; x++)
        {
            mseMin = std::min(mseMin,
                static_cast<double>(sdata.outputMSE[c].at<float>(matLine, x)));
            mseMax = std::max(mseMax,
                static_cast<double>(sdata.outputMSE[c].at<float>(matLine, x)));
        }

        nfo += tr(" - slice output error [")
            + QString::number(mseMin)
            + tr("; ") + QString::number(mseMax) + tr("]");
    }

    // may have to change the image here! we don't know if the temp changed...
    pGridView->clearButImage();
    cv::Rect lineCoord(0, matLine, sdata.channels[c].cols, 2);
    pGridView->addGrid(&lineCoord, 1, 1, QPen(Qt::green));

    if (inputSliceCurve)
    {
        inputSliceCurve->detach();
        delete inputSliceCurve;
    }
    inputSliceCurve = NULL;

    QVector<QPointF>curveSliceData;
    QPen curveStyle;
    inputSliceCurve = new QwtPlotCurve();

    if (!sdata.interpolated)
    {
        if (curve->currentIndex() == 0)
        {
            for (x = it->second.margins[0];
                x < sdata.inputImage[c].cols - it->second.margins[2];
                x++)
            {
                curveSliceData.push_back(QPointF(x,
                    sdata.inputImage[c].at<float>(sliceLine, x)));
            }

            cv::minMaxLoc(sdata.inputImage[c], NULL, &inputMax, NULL, &inMaxLoc);

            nfo += " - max input at line ";
            nfo += QString::number(inMaxLoc.y);
        }
        else
        {
            cv::Point idx;
            for (x = it->second.margins[0];
                x < sdata.smoothedImage[c].cols - it->second.margins[2];
                x++)
            {
                curveSliceData.push_back(QPointF(x,
                    sdata.smoothedImage[c].at<float>(sliceLine, x)));
            }

            cv::minMaxLoc(sdata.smoothedImage[c], NULL, &inputMax, NULL, &inMaxLoc);

            nfo += " - max smoothed at line ";
            nfo += QString::number(inMaxLoc.y);
        }

        curveStyle.setColor(Qt::black);
        inputSliceCurve->setSamples(curveSliceData);
        inputSliceCurve->setPen(curveStyle);

        inputSliceCurve->attach(inputSlice);
        inputSlice->setAxisAutoScale(QwtPlot::yLeft, false);
        inputSlice->setAxisScale(QwtPlot::yLeft, 0.0, inputMax);
        inputSlice->setAxisScale(QwtPlot::xBottom, 0.0,
            sdata.inputImage[c].cols + 1);

        // inputSlice->replot();

        curveSliceData.clear();
        // inputSlice->setVisible(true);
    }
    pGridView->setMaximumWidth(sdata.normalised[c].cols
        + sdata.normalised[c].cols / 2);
    inputSlice->replot();

    // clear curves
    if (gridSliceCurve)
    {
        gridSliceCurve->detach();
        delete gridSliceCurve;
        gridSliceCurve = NULL;
    }
    if (gridTargetCurve)
    {
        gridTargetCurve->detach();
        delete gridTargetCurve;
        gridTargetCurve = NULL;
    }
    if (gridTargetMSECurve)
    {
        gridTargetMSECurve->detach();
        delete gridTargetMSECurve;
        gridTargetMSECurve = NULL;
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

        for (x = 0; x < sdata.channels[c].cols; x++)
        {
            curveSliceData.push_back(QPointF(x * inputs.tileSize,
                sdata.channels[c].at<float>(matLine, x)));
        }

        curveStyle.setColor(outputColor);
        gridSliceCurve->setSamples(curveSliceData);
        gridSliceCurve->setPen(curveStyle);

        gridSliceCurve->attach(gridSlice);
        curveSliceData.clear();

        cv::minMaxIdx(sdata.channels[c], NULL, &matrixMax);
    }

    /* because the target is 1 less line than actual curve (we need an
    * extra line in the curve for interpolation in HW) */
    if (sdata.targetLSH[c].cols > 0
        && matLine < sdata.targetLSH[c].rows
        && targetColor != Qt::transparent)
    {
        gridTargetCurve = new QwtPlotCurve();

        for (x = 0; x < sdata.targetLSH[c].cols; x++)
        {
            curveSliceData.push_back(QPointF(x * inputs.tileSize,
                sdata.targetLSH[c].at<float>(matLine, x)));
        }

        curveStyle.setColor(targetColor);
        gridTargetCurve->setSamples(curveSliceData);
        gridTargetCurve->setPen(curveStyle);

        gridTargetCurve->attach(gridSlice);
        curveSliceData.clear();

        cv::minMaxIdx(sdata.targetLSH[c], NULL, &targetMax);

        // make the colour chooser for the curve visible
        targetColour->setVisible(true);
        targetColour_lbl->setVisible(true);
    }
    else
    {
        targetColour->setVisible(false);
        targetColour_lbl->setVisible(false);
    }

    if (sdata.outputMSE[c].cols > 0
        && matLine < sdata.outputMSE[c].rows
        && targetMSEColor != Qt::transparent)
    {
        gridTargetMSECurve = new QwtPlotCurve();

        cv::minMaxLoc(sdata.outputMSE[c], &mseMin, &mseMax);

        for (x = it->second.margins[0];
            x < sdata.outputMSE[c].cols - it->second.margins[2];
            x++)
        {
            curveSliceData.push_back(QPointF(x,
                sdata.outputMSE[c].at<float>(matLine, x) / mseMax));
        }

        curveStyle.setColor(targetMSEColor);
        gridTargetMSECurve->setSamples(curveSliceData);
        gridTargetMSECurve->setPen(curveStyle);

        gridTargetMSECurve->attach(gridSlice);
        curveSliceData.clear();

        // make the colour chooser for the curve visible
        targetMSEColour->setVisible(true);
        targetMSEColour_lbl->setVisible(true);
    }
    else
    {
        targetMSEColour->setVisible(false);
        targetMSEColour_lbl->setVisible(false);
    }

    lineInfo_lbl->setText(nfo);

    // gridSlice->setAxisAutoScale(QwtPlot::yLeft, true);
    // max is 10% higher round up
    matrixMax = ceil(std::max(targetMax, matrixMax) * 1.10);
    gridSlice->setAxisScale(QwtPlot::yLeft, 0.0, matrixMax);
    gridSlice->setAxisScale(QwtPlot::xBottom, 0.0,
        sdata.channels[c].cols * inputs.tileSize);
    gridSlice->replot();
}

/*
 * public
 */

LSHGridView::LSHGridView(QWidget *parent) : QWidget(parent), inputs(HW_UNKNOWN)
{
    init();
}

LSHGridView::~LSHGridView()
{
    data.clear();

    if (inputSliceCurve)
    {
        inputSliceCurve->detach();
       delete inputSliceCurve;
    }
    if (gridSliceCurve)
    {
        gridSliceCurve->detach();
        delete gridSliceCurve;
    }
    if (gridTargetCurve)
    {
        gridTargetCurve->detach();
        delete gridTargetCurve;
    }
    if (gridTargetMSECurve)
    {
        gridTargetMSECurve->detach();
        delete gridTargetMSECurve;
    }
}

void LSHGridView::loadData(const lsh_output *pOutput)
{
    // call the copy constructor!
    data.push_back(*pOutput);

    retranslate();
}

void LSHGridView::changeInput(const lsh_input &_input)
{
    inputs = _input;
}

void LSHGridView::clearData()
{
    data.clear();

    retranslate();
}

void LSHGridView::connectButtons(bool conn)
{
    if (conn)
    {
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
        connect(temperature, SIGNAL(currentIndexChanged(int)), this,
            SLOT(channelChanged(int)));
    }
    else
    {
        disconnect(channel, SIGNAL(currentIndexChanged(int)), this,
            SLOT(channelChanged(int)));
        disconnect(curve, SIGNAL(currentIndexChanged(int)), this,
            SLOT(channelChanged(int)));
        disconnect(pGridView, SIGNAL(mousePressed(qreal, qreal, Qt::MouseButton)),
            this, SLOT(mousePressed(qreal, qreal, Qt::MouseButton)));
        disconnect(lineSlider, SIGNAL(valueChanged(int)), this,
            SLOT(lineChanged(int)));
        disconnect(outputColour, SIGNAL(currentIndexChanged(int)), this,
            SLOT(channelChanged(int)));
        disconnect(targetColour, SIGNAL(currentIndexChanged(int)), this,
            SLOT(channelChanged(int)));
        disconnect(targetMSEColour, SIGNAL(currentIndexChanged(int)), this,
            SLOT(channelChanged(int)));
        disconnect(temperature, SIGNAL(currentIndexChanged(int)), this,
            SLOT(channelChanged(int)));
    }
}

/*
 * public slots
 */

void LSHGridView::channelChanged(int notused)
{
    int c = channel->currentIndex();
    int tempIdx = temperature->currentIndex();

    if (c < 0 || tempIdx < 0)
    {
        return;  // no channel selected!
    }

    if ((int)data.size() > tempIdx)
    {
        CVBuffer sBuff;
        const lsh_output &sdata = data[tempIdx];

        sBuff.mosaic = MOSAIC_NONE;
        sBuff.pxlFormat = RGB_888_24;
        sBuff.data = sdata.normalised[c];

        pGridView->loadBuffer(sBuff, false);

        // load slice
        lineSlider->setMinimum(0);
        int v = lineSlider->value();
        if (sdata.interpolated)
        {
            /* assumes that all output have the same size so find the
             * first output that is not interpolated */
            output_list::const_iterator it;
            for (it = data.begin(); it != data.end(); it++)
            {
                if (!it->interpolated)
                {
                    lineSlider->setMaximum(it->inputImage[0].rows - 1);
                    v = std::min(v, it->inputImage[0].rows - 1);
                    break;
                }
            }

        }
        else
        {
            lineSlider->setMaximum(sdata.inputImage[0].rows-1);
            v = std::min(v, sdata.inputImage[0].rows-1);
        }
        lineChanged(v);
    }
}

void LSHGridView::lineChanged(int l)
{
    const int c = channel->currentIndex();
    int tempIdx = temperature->currentIndex();

    if (c < 0 || tempIdx < 0 || data.size() == 0 || l < 0)
    {
        return;
    }

    if ((int)data.size() <= tempIdx)
    {
        return;
    }
    // assumes that all output have the same size
    matLine = l / inputs.tileSize;
    sliceLine = l;

    lineSlider->setValue(l);

    updateDisplay();
}

void LSHGridView::retranslate()
{
    Ui::LSHGridView::retranslateUi(this);

    int c0 = targetColour->currentIndex();
    int c1 = targetMSEColour->currentIndex();
    int c2 = outputColour->currentIndex();
    QString n;
    int t;

    outputColour->clear();
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

    c0 = temperature->currentIndex();
    temperature->clear();
    output_list::const_iterator it;
    c1 = 0;
    for (it = data.begin(); it != data.end(); it++)
    {
        temperature->insertItem(c1, 
            tr("Output for %nK", "", it->temperature));
        c1++;
    }
    if (c0 < 0 || c0 >= c1)
    {
        c0 = 0;
    }
    temperature->setCurrentIndex(c0);

}

void LSHGridView::mousePressed(qreal x, qreal y, Qt::MouseButton button)
{
    lineChanged(floor(y * inputs.tileSize));
}

