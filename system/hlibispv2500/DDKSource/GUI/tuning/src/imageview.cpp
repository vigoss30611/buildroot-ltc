/**
******************************************************************************
@file imageview.cpp

@brief Implementation of ImageView

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
#include "imageview.hpp"

#include "ui/ui_imageview.h"

#include "buffer.hpp"
#include "felixcommon/pixel_format.h"

#include <QGraphicsSceneEvent>
#include <QContextMenuEvent>
#include <QGraphicsPixmapItem>
#include <QFileDialog>
#include <QMenu>
#include <QAction>

#include <QtExtra/exception.hpp>
#include <QtExtra/scalableview.hpp>

#include <qwt_plot.h>
//#include <qwt_plot_barchart.h>
#include <qwt_plot_histogram.h>
#include <qwt_column_symbol.h>
#include <qwt_plot_picker.h>
#include <qwt_picker_machine.h>

#include <qwt_plot_zoomer.h>

#include <cv.h>
#include <iostream>
#include <img_errors.h>
#include <sim_image.h>
#include <image.h>

class HistogramPosition: public QwtPlotZoomer
{
public:
    QVector<QwtIntervalSample> *histData;
    int nElem; // number of elements in the negative part of the histogram data (the positive starts after that)

    HistogramPosition( QWidget *canvas ):
    QwtPlotZoomer( canvas )
        {
            setTrackerMode( AlwaysOn );
            histData = 0;
        }

    ~HistogramPosition()
        {
            if (histData)
            {
                delete histData;
            }
        }

    virtual QwtText trackerTextF( const QPointF &pos ) const
        {
            QColor bg( Qt::white );
            int posX = (int)floor(pos.x());
            QString coordinate = QString::number(posX);
            bg.setAlpha( 200 );

            coordinate = QString::number(posX) + " no data ";

            if ( histData )
            {
                int pos;
                if ( posX < 0 )
                {
                    pos = abs(posX)-1;
                }
                else
                {
                    pos = posX + nElem;
                }

                if ( pos < 2*nElem )
                {
                    coordinate = QString::number(posX) + " = " + QString::number((int)histData->at(pos).value);
                }
            }

            QwtText text(coordinate);
            text.setBackgroundBrush( QBrush( bg ) );
            return text;
        }
};

//
// ImageView
//

//
// private
//

void ImageView::init()
{
    Ui::ImageView::setupUi(this);

    //pBuffer = 0;
    pScene = 0;
    pPixmap = 0;
    pHistTab = 0;

	genHists = true;

	saveAsJPG_act = new QAction("to change", this);
    saveAsFLX_act = new QAction("to change", this);
    saveAsYUV_act = new QAction("to change", this);

    QGridLayout *pLay = new QGridLayout();

    for ( int c = 0 ; c < 4 ; c++ )
    {
        /*histogramsPlots[c] = new QwtPlot();
          pLay->addWidget(histogramsPlots[c], c, 0);*/
        histogramsPlots[c] = 0;
    }

    showStatistics(true);

	connect(actionSave_FLX_file, SIGNAL(triggered()), this, SLOT(saveBuffer()));
    connect(actionSave_YUV_file, SIGNAL(triggered()), this, SLOT(saveBuffer()));
    connect(actionSave_displayed_image_as_JPG, SIGNAL(triggered()), this, SLOT(saveDisplay()));

	connect(saveAsFLX_act, SIGNAL(triggered()), this, SLOT(saveBuffer()));
    connect(saveAsYUV_act, SIGNAL(triggered()), this, SLOT(saveBuffer()));
    connect(saveAsJPG_act, SIGNAL(triggered()), this, SLOT(saveDisplay()));

    pView->bZoom = false; // does not work very well yet
    pView->minZoom = 0.25;
    pView->maxZoom = 2.0;
    pView->stepZoom = 0.25;
    connect(pView, SIGNAL(mousePressed(QPointF,Qt::MouseButton)), this, SLOT(mousePressedSlot(QPointF,Qt::MouseButton)));
    //connect(pView, SIGNAL(mouseReleased(qreal,qreal,Qt::MouseButton)), this, SIGNAL(mouseReleased(qreal,qreal,Qt::MouseButton)));

    retranslate();
}

void ImageView::computeHistograms()
{
    if ( !sBuffer.data.empty() )
    {
        QWidget *hist_tab = new QWidget();
        QGridLayout *pLay = new QGridLayout();

        for ( int c = 0 ; c < 4 ; c++ )
        {
            /// @ use detach() from Curve so that Plot does not have to be destroyed
            if ( histogramsPlots[c] ) // the plots need to be destroyed otherwise they are not updated correctly... when the reason is found they can be created in init() again
            {
                //histogramsPlots[c]->setVisible(true);

                pLay->removeWidget(histogramsPlots[c]);
                delete histogramsPlots[c];
                histogramsPlots[c] = 0;
            }
            //histogramsPlots[c]->setVisible(false);
        }

        QwtPlotHistogram *pHistogram[4];
        QwtColumnSymbol *symbol = 0;

        QColor colours[4];
        QVector<QwtIntervalSample> *histData[4];

        //int minHist[4] = { 0, 0, 0, 0};
        //int maxHist[4] = { 0, 0, 0, 0};
        double allMin = 0, allMax = 0;

        //std::map<int, unsigned> *histDataMap[4];
        int nChannels = 4;
        int nElem;
        {
            int bitdepth = 8; ///< @ get bitdepth from image!

            if ( sBuffer.mosaic != MOSAIC_NONE )
            {
                nChannels = 4;
                // warning: format is aligned to 10b when loaded
                switch(sBuffer.pxlFormat)
                {
                    //case BAYER_RGGB_8: // it's already 8b bitdepth
                case BAYER_RGGB_10:
                    bitdepth = 10;
                    break;
                case BAYER_RGGB_12:
                    //bitdepth = 12;
                    bitdepth = 10;
                    break;
                }
            }
            else
            {
                nChannels = 3;
            }
            nElem = 1<<bitdepth;
            for ( int c = 0 ; c < nChannels ; c++ )
            {
                histData[c] = new QVector<QwtIntervalSample>();

                histData[c]->insert(0, nElem*2, QwtIntervalSample(0, 0, 0));
            }

            allMax = double(nElem);

            std::vector<cv::Mat> channels;
            cv::Mat histograms;
            cv::split(sBuffer.data, channels);
            float range[] = { -1*nElem, nElem-1};
            const float *histRange = {range};
            int histSize = 2<<bitdepth;
            int nChan[] = {0};

            for ( int c = 0 ; c < nChannels ; c++ )
            {
                int nImg = 1;
                histograms = cv::Mat();
                cv::calcHist(&channels[c], 1, nChan, cv::Mat(), histograms, 1, &histSize, &histRange, true, true); // uniform and accumulate

                histData[c]->clear();
                for ( int i = 0 ; i < histSize ; i++ )
                {
                    // to cope with negative numbers the bottom elements are negative and the top are positive
                    // to cope with subsampling we could duplicate the information
                    histData[c]->push_back(QwtIntervalSample(histograms.at<float>(i), (i-nElem), (i-nElem+1)));
                }
            }



        } // compute hist

        if ( nChannels == 4 )
        {
            int m_r =0, m_g1=1, m_g2=2, m_b=3;

            //CVBuffer::mosaicInfo(sBuffer.mosaic, m_r, m_g1, m_g2, m_b);
            // splitted as RGGB

            colours[m_r] = Qt::red;
            colours[m_g1] = Qt::green;
            colours[m_g2] = Qt::green;
            colours[m_b] = Qt::blue;
        }
        else
        {
            colours[0] = Qt::red;
            colours[1] = Qt::green;
            colours[2] = Qt::blue;
        }

        int h = 0;
        for ( int h = 0 ; h < nChannels ; h++ )
        {
            histogramsPlots[h] = new QwtPlot();

            pHistogram[h] = new QwtPlotHistogram();
            pHistogram[h]->setSamples( *(histData[h]) );

            symbol = new QwtColumnSymbol( QwtColumnSymbol::Box );
            //symbol->setLineWidth( 1 );
            symbol->setPalette( QPalette( colours[h] ) );
            pHistogram[h]->setSymbol(symbol);

            //pHistogram[h]->setMargin(0);
            //pHistogram[h]->setSpacing(0);

            pHistogram[h]->attach(histogramsPlots[h]);
            histogramsPlots[h]->setAxisScale(QwtPlot::xBottom, allMin, allMax, allMax);//floor((allMax-allMin)/2.0));

            histogramsPlots[h]->setAxisAutoScale(QwtPlot::yLeft, true);
            histogramsPlots[h]->enableAxis(QwtPlot::yLeft, false); // hides the axis

            histogramsPlots[h]->replot();

            HistogramPosition *pTracker = new HistogramPosition(histogramsPlots[h]->canvas());
            pTracker->histData = histData[h];
            pTracker->nElem = nElem;
            //pTracker->negHistData = negHistData[h];
            //pTracker->phistDataMap = histDataMap[h];

            pLay->addWidget(histogramsPlots[h], h, 0);
        }

        hist_tab->setLayout(pLay);

        if ( pHistTab )
        {
            info_tabs->removeTab( info_tabs->indexOf(pHistTab) );
            delete pHistTab;
        }

        info_tabs->addTab(hist_tab, tr("Histograms"));
        pHistTab = hist_tab;
    }
}

//
// protected
//

void ImageView::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu;

    bool bYUV = false;
    PIXELTYPE sYUVType;

    if ( PixelTransformYUV(&sYUVType, sBuffer.pxlFormat) == IMG_SUCCESS )
    {
        menu.addAction(saveAsYUV_act);
    }
    else
    {
        menu.addAction(saveAsFLX_act);
    }
    menu.addAction(saveAsJPG_act);

    menu.exec(event->globalPos());
}

void ImageView::closeEvent(QCloseEvent *event)
{
    QWidget::closeEvent(event);
    emit closed();
}

//
// public
//

ImageView::ImageView(QWidget *parent): QMainWindow(parent)
{
    init();
}

ImageView::ImageView(CVBuffer *pBuff, QWidget *parent): QMainWindow(parent)
{
    init();

    if ( pBuff && !pBuff->data.empty() )
    {
        loadBuffer(*pBuff);
    }
}

ImageView::~ImageView()
{
    if ( pScene )
    {
        delete pScene;
    }
}

int ImageView::loadBuffer(CVBuffer sBuff, bool gammaCorrect)
{
    int ret = EXIT_SUCCESS;
    QImage *pImage = 0;

    if ( gammaCorrect )
    {
        pImage = sBuff.gammaCorrect().convertToQImage();
    }
    else
    {
        pImage = sBuff.convertToQImage();
    }

    ret = loadQImage(pImage);

    delete pImage;

    sBuffer = sBuff.clone();
    if(genHists) computeHistograms();


	PIXELTYPE sYUVType;

    if ( PixelTransformYUV(&sYUVType, sBuffer.pxlFormat) == IMG_SUCCESS )
    {
		menuFile->removeAction(actionSave_FLX_file);
    }
    else
    {
		menuFile->removeAction(actionSave_YUV_file);
    }


    retranslate();

    return ret;
}

int ImageView::loadQImage(QImage *pImage)
{
    QGraphicsScene *pNewScene = new QGraphicsScene();
    QGraphicsPixmapItem *pNewPixmap;

    pNewPixmap = pNewScene->addPixmap( QPixmap::fromImage(*pImage).scaled(pImage->width(), pImage->height(), Qt::KeepAspectRatio) );

    pView->setScene(pNewScene);

    if ( pScene )
    {
        delete pScene;
    }
    pScene = pNewScene;
    pPixmap = pNewPixmap;
    //connect(pScene, SIGNAL(mousePressed(qreal, qreal, Qt::MouseButton)), this, SLOT(mousePressedSlot(qreal, qreal, Qt::MouseButton)));
    //connect(pScene, SIGNAL(mouseReleased(qreal, qreal, Qt::MouseButton)), this, SLOT(mouseReleasedSlot(qreal, qreal, Qt::MouseButton)));

    return EXIT_SUCCESS;
}

void ImageView::addGrid(const std::vector<cv::Rect> &coord, const QPen &pen, const QBrush &brush)
{
    if ( pScene )
    {
        for ( std::vector<cv::Rect>::const_iterator it = coord.begin() ; it != coord.end() ; it++ )
        {
            QRect qrectangle((*it).x, (*it).y, (*it).width, (*it).height);
            pScene->addRect(qrectangle, pen, brush);
        }
    }
}

void ImageView::addGrid(cv::Rect *coords, unsigned w, unsigned h, const QPen &pen, const QBrush &brush)
{
    if ( pScene )
    {
        for ( unsigned j = 0 ; j < h ; j++ )
        {
            for ( unsigned i = 0 ; i < w ; i++ )
            {
                QRect qrectangle(coords[j*w+i].x, coords[j*w+i].y, coords[j*w+i].width, coords[j*w+i].height);
                pScene->addRect(qrectangle, pen, brush);
            }
        }
    }
}

void ImageView::markPoints(QList<QPointF> points, const QPen &pen, const QBrush &brush)
{
	if ( pScene && pPixmap )
    {
		for(int i = 0; i < points.size(); i++)
		{
			pScene->addEllipse(QRectF(points[i].x(), points[i].y(), 1, 1), pen, brush);
		}
    }
}

void ImageView::clearImage()
{
	loadQImage(sBuffer.convertToQImage());
}

void ImageView::showStatistics(bool show)
{
    if ( show )
    {
        if ( main_lay->indexOf(pView) >= 0 )
        {
            main_lay->removeWidget(pView);
            imgtab_lay->addWidget(pView, 0, 0);
        } 
    }
    else
    {
        if ( imgtab_lay->indexOf(pView) >= 0 )
        {
            imgtab_lay->removeWidget(pView);
			main_lay->addWidget(pView, 0, 0);
        }
    }
    splittedView->setVisible(show);
}

void ImageView::saveBuffer(const QString &filename)
{
    QString toSave = filename;

    PIXELTYPE sYUVType;
    bool bYUV = false;
    QString proposedName;

    if ( PixelTransformYUV(&sYUVType, sBuffer.pxlFormat) == IMG_SUCCESS )
    {
        bYUV = true;
        proposedName = QString::number(sBuffer.data.cols) + "x" + QString::number(sBuffer.data.rows) + "_444p.yuv";
        //QString::fromLatin1(FormatString(sBuffer.pxlFormat)) + ".yuv";
    }

    if ( filename.isEmpty() )
    {
        QString title = tr("Save FLX file");
        QString filter = tr("FLX images (*.flx)");
        if (bYUV )
        {
            title = tr("Save YUV file");
			filter = tr("YUV images (*.yuv)");
        }
        else if ( sBuffer.mosaic == MOSAIC_NONE )
        {
            proposedName = "display.flx";
        }
        else
        {
            proposedName = "dataExtraction.flx";
        }

        toSave = QFileDialog::getSaveFileName(this, title, proposedName, filter);
    }

    if ( !toSave.isEmpty() )
    {
        if ( bYUV )
        {
            /// @ save YUV correctly (currently YUV is 444 not what was received)
            FILE *f = fopen(toSave.toLatin1(), "wb");
            std::vector<cv::Mat> channels;

            cv::split(this->sBuffer.data, channels);

            if ( f )
            {
                int n = 0;
                int toWrite = channels[0].rows*channels[0].cols; // 444
                QString error;

                n = fwrite(channels[0].data, 1, toWrite, f);
                if ( n != toWrite )
                {
                    error += tr("\nY plane (%n", "", n) + tr("/%n bytes)", "", toWrite);
                }
                n = fwrite(channels[1].data, 1, toWrite, f);
                if ( n != toWrite )
                {
                    error += tr("\nU plane (%n", "", n) + tr("/%n bytes)", "", toWrite);
                }
                n = fwrite(channels[2].data, 1, toWrite, f);
                if ( n != toWrite )
                {
                    error += tr("\nV plane (%n", "", n) + tr("/%n bytes)", "", toWrite);
                }

                if ( !error.isEmpty() )
                {
                    QtExtra::Exception ex(tr("Failed to save to YUV file: ", "followed by filename") + toSave, tr("Error writing plane:", "followed by accumulater error string") + error);
                    ex.displayQMessageBox(this);
                }

                fclose(f);
            }
        }
        else // ! bYUV
        {
            sSimImageOut sSimImage;

            SimImageOut_init(&sSimImage);

            sSimImage.info.ui8BitDepth = 8;
            sSimImage.info.ui32Width = sBuffer.data.cols;//pBuffer->width;
            sSimImage.info.ui32Height = sBuffer.data.rows; //pBuffer->height;
            if ( sBuffer.mosaic == MOSAIC_NONE )
            {
                sSimImage.info.eColourModel = SimImage_RGB24;
                sSimImage.info.stride = sBuffer.data.cols*sizeof(IMG_INT16);
            }
            else
            {
                switch(sBuffer.mosaic)
                {
                case MOSAIC_RGGB:
                    sSimImage.info.eColourModel = SimImage_RGGB;
                    break;
                case MOSAIC_GRBG:
                    sSimImage.info.eColourModel = SimImage_GRBG;
                    break;
                case MOSAIC_GBRG:
                    sSimImage.info.eColourModel = SimImage_GBRG;
                    break;
                case MOSAIC_BGGR:
                    sSimImage.info.eColourModel = SimImage_BGGR;
                    break;
                }
                // forced to 10b because scaled to 10b when imported in CVBuffer
                //if ( sBuffer.pxlFormat == BAYER_RGGB_10 )
                {
                    sSimImage.info.ui8BitDepth = 10;
                }
                /*else if ( sBuffer.pxlFormat == BAYER_RGGB_12 )
                {
                  sSimImage.info.ui8BitDepth = 12;
                }*/
                sSimImage.info.stride = sBuffer.data.cols*sizeof(IMG_INT16);
                sSimImage.info.ui32Width *= 2;
                sSimImage.info.ui32Height *= 2;
            }
            sSimImage.info.isBGR = IMG_FALSE;

            SimImageOut_create(&sSimImage);

            if ( SimImageOut_open(&sSimImage, toSave.toLatin1()) != IMG_SUCCESS )
            {
                QtExtra::Exception ex(tr("Failed to open file ") + toSave);
                ex.displayQMessageBox(this);
                return;
            }

            CImageFlx *CImage = (CImageFlx *)sSimImage.saveToFLX; // cheat!
            int nChannels = CImage->GetNColChannels();
            const char* err = 0;

            if ( sBuffer.mosaic == MOSAIC_NONE )
            {
                if ( SimImageOut_addFrame(&sSimImage, sBuffer.data.data, sBuffer.data.step*sBuffer.data.rows) != IMG_SUCCESS )
                {
                    QtExtra::Exception ex(tr("Failed to add frame to FLX object when saving to ") + toSave);
                    ex.displayQMessageBox(this);
                    SimImageOut_close(&sSimImage);
                    return;
                }
            }
            else
            {

                std::vector<cv::Mat> channels;
                cv::split(sBuffer.data, channels);

                for (unsigned int c = 0 ;c < channels.size() ; c++ )
                {
                    for ( int y = 0 ; y < channels[c].rows ; y++ )
                    {
                        for ( int x = 0 ; x < channels[c].cols ; x++ )
                        {
                            double v = 0;
                            if ( channels[c].depth() == CV_8U )
                            {
                                v = channels[c].at<unsigned char>(y, x);
                            }
                            else if ( channels[c].depth() == CV_32F )
                            {
                                v = std::floor(channels[c].at<float>(y, x));
                            }
                            else
                            {
                                QtExtra::Exception ex(tr("Failed to add frame to FLX object when saving to ") + toSave, tr("Unsuported cv format"));
                                ex.displayQMessageBox(this);
                                SimImageOut_close(&sSimImage);
                                return;
                            }
                            CImage->chnl[c].data[y*CImage->chnl[c].chnlWidth + x] = (IMG_UINT16)v;
                        }
                    }
                }

            }

            if ( (err = CImage->SaveFileData(sSimImage.saveContext)) )
            {
                QtExtra::Exception ex(tr("Failed to write to FLX object when saving to ") + toSave, QString(err));
                ex.displayQMessageBox(this);
            }

            SimImageOut_close(&sSimImage);
        }
    }
}

void ImageView::saveDisplay(const QString &filename)
{
    QString toSave = filename;

    if ( filename.isEmpty() )
    {
        toSave = QFileDialog::getSaveFileName(this, tr("Save displayed file"), ".", tr("JPEG (*.jpg *.jpeg)"));
    }

    if ( !toSave.isEmpty() )
    {
        QImage *pImage = sBuffer.convertToQImage();

        if ( pImage )
        {
            pImage->save(toSave, "JPG");
        }
    }
}

QGraphicsScene* ImageView::getScene()
{
    return pScene;
}

int ImageView::clearButImage()
{
    int ret = 1;
    if ( pScene && !sBuffer.data.empty())
    {
        if ( pPixmap )
        {
            pScene->removeItem(pPixmap);
        }
        pScene->clear();
        if ( pPixmap )
        {
            pScene->addItem(pPixmap);
        }
        else
        {
            QImage *pImage = sBuffer.convertToQImage();

            if ( pImage )
            {
                ret = loadQImage(pImage);

                delete pImage;
            }
        }
    }

    return ret;
}

void ImageView::retranslate()
{
    Ui::ImageView::retranslateUi(this);

    if ( !sBuffer.data.empty() )
    {
        int w, h;
        w = sBuffer.data.cols;
        h = sBuffer.data.rows;
        if ( sBuffer.mosaic != MOSAIC_NONE )
        {
            w *= 2;
            h *= 2;
        }

        size_lbl->setText(tr("Size: %n", "width followed by height", w) + tr("x%n (pixels)", "height", h));
        QString format = QString::fromLatin1(FormatString(sBuffer.pxlFormat));

        switch(sBuffer.mosaic)
        {
        case MOSAIC_RGGB:
            format += " RGGB";
            break;
        case MOSAIC_GRBG:
            format += " GRBG";
            break;
        case MOSAIC_GBRG:
            format += " GBRG";
            break;
        case MOSAIC_BGGR:
            format += " BGGR";
            break;
        }

        if (sBuffer.pxlFormat == BAYER_RGGB_12)
        {
            format += " (subsampled to 10b)";
        }

        fmt_lbl->setText(tr("Format: ", "followed by format string") + format);
    }

	saveAsJPG_act->setText(tr("Save displayed image as JPG"));
    saveAsFLX_act->setText(tr("Save FLX image"));
    saveAsYUV_act->setText(tr("Save YUV image"));
}

void ImageView::mousePressedSlot(QPointF p, Qt::MouseButton button)
{
    QPointF sceneP = pView->mapToScene(p.toPoint());
    int _y = floor(sceneP.ry());
    int _x = floor(sceneP.rx());
    
    if ( _y < 0 || _x < 0 || _x >= sBuffer.data.cols || _y >= sBuffer.data.rows )
    {
        return;
    }

    QString message = tr("%n", "", _x)+ tr(",%n", "", _y);

    if ( !sBuffer.data.empty() )
    {
        QString hex = "-";
        QString percent = "-";
        message += " -";
        unsigned int nchan = 3;
        unsigned int maxVal = 0;
        unsigned int i;
        
        if ( sBuffer.mosaic != MOSAIC_NONE )
        {
            nchan = 4;
            maxVal = 1<<10; // normalised to 10b when loaded
        }
        else
        {
            PIXELTYPE sType;
            if ( PixelTransformYUV(&sType, sBuffer.pxlFormat) != IMG_SUCCESS )
            {
                PixelTransformRGB(&sType, sBuffer.pxlFormat);
            }
            if ( sType.ui8BitDepth > 0 )
            {
                maxVal = 1<<sType.ui8BitDepth;
            }
        }

        std::vector<cv::Mat> splitted;
        cv::split(sBuffer.data, splitted);

        CV_Assert(splitted.size() <= nchan);

        for ( i = 0 ; i < splitted.size() ; i++ )
        {
            //IMG_INT16 v = pBuffer->data[_y*pBuffer->stride + nchan*_x + i];
            //std::cout << "pressed at x=" << _x << " y=" << _y << std::endl;

            int v = -1;
            switch ( splitted[i].depth() )
            {
            case CV_32F:
                v = floor(splitted[i].at<float>(_y, _x));
                break;
            case CV_8U:
                v = splitted[i].at<unsigned char>(_y, _x);
                break;
            }

            message += " " + QString::number(v);
            hex += " 0x" + QString::number(v, 16);
            if ( maxVal != 0 )
            {
                percent += " " + QString::number(floor((double)v/maxVal*100.0)) + "%";
            }
        }
        message = message + hex;
        if ( maxVal != 0 )
        {
            message += percent;
        }
    }

    infoLine->setText(message);
    
    emit mousePressed(sceneP.rx(), sceneP.ry(), button);
}

void ImageView::mouseReleasedSlot(qreal x, qreal y, Qt::MouseButton button)
{
    emit mouseReleased(x, y, button);
}

