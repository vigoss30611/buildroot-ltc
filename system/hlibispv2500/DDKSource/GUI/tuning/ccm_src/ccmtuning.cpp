/********************************************************************************
@file ccmtuning.cpp

@brief CCMTuning class implementation

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
#include "ccm/tuning.hpp"
#include "ccm/compute.hpp"

#include "buffer.hpp"

//#include "flxdisplay.h"
//#include "flxloader.h"

#include "imageview.hpp"

#include <iostream>
#include <QtExtra/ostream.hpp>

#include "genconfig.hpp"

#if 1 // until we have a real gui integration
#include "ccm/config.hpp"
#endif


#include <QGraphicsView>
#include <QGraphicsScene>
#include <QMessageBox>
#include <QFileDialog>
#include <QTabWidget>

#include <QwtExtra/editablecurve.hpp>

#include <cstring>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <ispc/TemperatureCorrection.h>
#ifdef USE_MATH_NEON
#include <mneon.h>
#endif

#ifdef TUNE_CCM_DBG
#include "imageUtils.h"
#endif

#include "ispc/ModuleWBC.h"
#include "ispc/ModuleCCM.h"
#include "ispc/ModuleBLC.h"
#include "ispc/ParameterFileParser.h"

#define USE_IQCHART 0

// standard macbetch expected colours without gamma applied normalized to 0-255
const static double g_expectedRGB[MACBETCH_N][3] = {
    {44.1835,   21.3088,   14.5246},
    {140.5072,  76.8705,   55.9623},
    {28.1609,   50.4522,   86.6413},
    {26.1942,   38.4355,   13.4521},
    {57.4822,   55.8087,   110.9321},
    {31.7798,   132.8182,  103.9713},
    {182.5029,  50.8805,   6.7101},
    {16.4490,   27.1228,   100.1055},
    {138.9965,  22.8381,   31.0325},
    {26.5198,   11.1244,   35.9018},
    {90.5893,   130.2332,  12.3180},
    {199.9627,  90.6523,   5.6398},
    {6.2177,    12.1687,   74.6432},
    {16.2366,   77.0557,   16.3935},
    {110.7031,  7.6120,    10.2251},
    {219.1058,  147.0440,  2.0720},
    {128.3243,  22.5678,   78.0875},
    {0,         63.2336,   97.8949},
    {233.7620,  233.3998,  222.7754},
    {148.1977,  150.3515,  148.9093},
    {90.4827,   91.7229,   91.4789},
    {47.8865,   49.2886,   49.2818},
    {22.1434,   22.8468,   23.0950},
    {8.0572,    8.1105,    8.3148},
};

const static double g_expectedRGB_IQ[MACBETCH_N][3] = {
    {42.0,  27.2,   21.7},
    {76.3,  56.3,   46.2},
    {31.3,  43.4,   54.0},
    {28.9,  39.1,   17.3},
    {44.1,  39.0,   51.8},
    {35.6,  67.7,   59.5},
    {71.2,  35.5,   0.1},
    {10.0,  31.1,   51.9},
    {74.3,  31.9,   34.1},
    {26.5,  16.5,   36.2},
    {48.2,  61.5,   25.8},
    {74.9,  50.1,   0.1},
    {0,     0.0,    56.9},
    {0,     48.6,   0.0},
    {75.0,  0.0,    0.0},
    {89.3,  76.8,   0.2},
    {54.1,  20.6,   35.5},
    {0,     52.5,   65.4},
    {100.0, 100.0,  100.0},
    {65.0,  65.0,   65.0},
    {39.0,   39.0,   39.0},
    {21.0,   21.0,   21.0},
    {10.0,   10.0,   10.0},
    {3.0,    3.0,    3.0},
};

const static double g_patchWeight[MACBETCH_N] = {
    1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1,
    1, 1, 1, 1, 1, 1, 
    1, 1, 1, 1, 1, 1
};

#define DF_BLACK_PATCH (MACBETCH_N-1)
#define DF_WHITE_PATCH (MACBETCH_N-MACBETCH_W)

#define DF_BLC_R 18 // default red sensor black
#define DF_BLC_G1 18 // default green 1 sensor black
#define DF_BLC_G2 18 // default green 2 sensor black
#define DF_BLC_B 18 // default B sensor black

#define DF_SYSBLACK 64 // default system black level

//
// algorithms parameters - run-time modifiable
//
#define DF_ITER 100000 //1000000

#define DF_SEND_INFO 500 //1500 

// nb of iteration where the matrix is generated randomly
#define DF_WARM_ITERATIONS 1000 //50000

//
// algorithm parameters - non-modifiable
//

#define DF_CHANGE_RAND 40 // change the value if the result of rand()%MOD is less than CMP

#define DF_TEMPERATURE_STR 1.0 // strenght of the temperature (multiplier)

#define DF_NORMALISE true
#define DF_VERBOSE false
#define DF_ERRORGRAPH false
#define DF_GAMMADISPLAY true
#define DF_CIE94 true

#define MASSIVE_ERROR 3.0 // black
#define BIG_ERROR 2.0 // red
#define AVERAGE_ERROR 1.0 // orange
#define ACCEPTABLE_ERROR 0.5 // yellow - less than that is white

const static double g_matrixMax[9] = {   1.98, 1.0, 1.0,
                            1.0, 1.98, 1.0,
                            1.0, 1.0, 1.98 };
const static double g_matrixMin[9] = {   -1.98, -1.0, -1.0,
                            -1.0, -1.98, -1.0,
                            -1.0, -1.0, -1.98 };
const static double g_offsetMax[3] = { 98.0, 98.0, 98.0 };
const static double g_offsetMin[3] = { -98.0, -98.0, -98.0 };


#define RED_LUM_DF 0.299
#define GREEN_LUM_DF 0.587
#define BLUE_LUM_DF 0.114

#define DF_CHECKWBCGAIN false
#define DF_WBCGAINMAX 2.0

/*
 * ccm_input
 */

ccm_input::ccm_input()
{
    // input reset
#if USE_IQCHART
	int n=0;
	for(n=0;n<MACBETCH_N;n++)
	{
		expectedRGB[n][0]=g_expectedRGB_IQ[n][0]*2.55;
		expectedRGB[n][1]=g_expectedRGB_IQ[n][1]*2.55;
		expectedRGB[n][2]=g_expectedRGB_IQ[n][2]*2.55;
		
	}
#else    
    memcpy(expectedRGB, g_expectedRGB, sizeof(g_expectedRGB));
#endif
    whitePatchI = DF_WHITE_PATCH;
    blackPatchI = DF_BLACK_PATCH;

    sensorBlack[0] = DF_BLC_R;
    sensorBlack[1] = DF_BLC_G1;
    sensorBlack[2] = DF_BLC_G2;
    sensorBlack[3] = DF_BLC_B;

    systemBlack = DF_SYSBLACK;

    niterations = DF_ITER;
    nwarmiterations = DF_WARM_ITERATIONS;
    sendinfo = DF_SEND_INFO;
    temperatureStrength = DF_TEMPERATURE_STR;
    changeProb = DF_CHANGE_RAND;
    memcpy(patchWeight, g_patchWeight, sizeof(g_patchWeight));

    memcpy(matrixMax, g_matrixMax, sizeof(g_matrixMax));
    memcpy(matrixMin, g_matrixMin, sizeof(g_matrixMin));
    memcpy(offsetMax, g_offsetMax, sizeof(g_offsetMax));
    memcpy(offsetMin, g_offsetMin, sizeof(g_offsetMin));

    // only used if TUNE_CCM_WBC_TARGET_AVG is defined at compilation time
    luminanceCorrection[0] = RED_LUM_DF;
    luminanceCorrection[1] = GREEN_LUM_DF;
    luminanceCorrection[2] = BLUE_LUM_DF;
    // otherwise uses
    bCheckMaxWBCGain = DF_CHECKWBCGAIN;
    maxWBCGain = DF_WBCGAINMAX;

    bnormalise = DF_NORMALISE;
    bverbose = DF_VERBOSE;
    berrorGraph = DF_ERRORGRAPH;
    bgammadisplay = DF_GAMMADISPLAY;
    bCIE94 = DF_CIE94;
}

/*
 * CCMTuning
 */

/*
 * private
 */

void CCMTuning::init()
{
    Ui::CCMTuning::setupUi(this);

    images_grp = new QTabWidget();
    blc_tab = 0;
    wbc_tab = 0;
    best_tab = 0;
    finalError = 0;

    currBest = 0.0;
    
    errorCurve->setVisible(false);
    result_grp->removeTab( result_grp->indexOf(errorCurve) );

    result_tab->setEnabled(true);
    //progress->setEnabled(true);
    progress->setVisible(false);
    save_btn->setEnabled(false);
	integrate_btn->setEnabled(false);

	blc_sensor0->setRange(ISPC::ModuleBLC::BLC_SENSOR_BLACK.min, ISPC::ModuleBLC::BLC_SENSOR_BLACK.max);
	blc_sensor1->setRange(ISPC::ModuleBLC::BLC_SENSOR_BLACK.min, ISPC::ModuleBLC::BLC_SENSOR_BLACK.max);
	blc_sensor2->setRange(ISPC::ModuleBLC::BLC_SENSOR_BLACK.min, ISPC::ModuleBLC::BLC_SENSOR_BLACK.max);
	blc_sensor3->setRange(ISPC::ModuleBLC::BLC_SENSOR_BLACK.min, ISPC::ModuleBLC::BLC_SENSOR_BLACK.max);
	blc_system->setRange(ISPC::ModuleBLC::BLC_SYS_BLACK.min, ISPC::ModuleBLC::BLC_SYS_BLACK.max);
    
    connect(save_btn, SIGNAL(clicked()), this, SLOT(saveResult()));
    connect(reset_btn, SIGNAL(clicked()), this, SLOT(reset()));

	connect(remove_btn, SIGNAL(clicked()), this, SLOT(removeSlot()));
	connect(default_rbtn, SIGNAL(clicked()), this, SLOT(setDefaultSlot()));

    warning_icon->setPixmap(this->style()->standardIcon(WARN_ICON_TW).pixmap(QSize(WARN_ICON_W,WARN_ICON_H)));
    warning_lbl->setVisible(false);
    warning_icon->setVisible(false);

    blc_grp->setEnabled(false);
    wbc_grp->setEnabled(false);
    matrix_grp->setEnabled(false);

	advSettLookUp_btn->setEnabled(false);

	connect(advSettLookUp_btn, SIGNAL(clicked()), this, SLOT(advSettLookUp()));

	if(_isVLWidget)
	{
		remove_btn->hide();
		save_btn->hide();
		default_rbtn->hide();
		gridLayout->removeItem(verticalSpacer);
	}
	else
	{
		integrate_btn->hide();
	}
}

void CCMTuning::addGrid(QGraphicsScene *pScene, QRect *checkerCoords)
{
    for ( int j = 0 ; j < MACBETCH_H ; j++ )
    {
        for ( int i = 0 ; i < MACBETCH_W ; i++ )
        {
            pScene->addRect(checkerCoords[j*MACBETCH_W+i], QPen(Qt::white));
        }
    }
}

/*
 * public
 */

QString CCMTuning::checkDependencies() const
{
    blc_config blc = genConfig->getBLC();
    QString mess;

    if ( !pResult )
    {
        mess += tr("CCM calibration was not run\n");
        return mess;
    }

    for ( int c = 0 ; c < 4 ; c++ )
    {
        if ( inputs.sensorBlack[c] != blc.sensorBlack[c] )
        {
            mess += tr("BLC sensor black level values changed since CCM were computed\n");
            break;
        }
    }
    if ( inputs.systemBlack != blc.systemBlack )
    {
        mess += tr("BLC system black level values changed since CCM were computed\n");
    }

    if ( pResult && ! mess.isEmpty() )
    {
        warning_lbl->setText(mess);
        warning_lbl->setVisible(true);
        warning_icon->setVisible(true);
    }
    else
    {
        warning_lbl->setVisible(false);
        warning_icon->setVisible(false);
    }

    return mess;
}

CCMTuning::CCMTuning(GenConfig *genConfig, bool isVLWidget, QWidget *parent): VisionModule(parent)
{
	_isVLWidget = isVLWidget;

    this->pResult = 0;
    this->genConfig = genConfig;
    init();
}

CCMTuning::~CCMTuning()
{
    // genConfig is not owned by this object!
    if ( pResult )
    {
        delete pResult;
    }
}

void CCMTuning::changeCSS(const QString &css)
{
	_css = css;

	setStyleSheet(css);
}

/*
 * public slot
 */

int CCMTuning::tuning()
{
    if ( this->inputs.inputFilename.isEmpty() )
    {
        return EXIT_FAILURE;
    }
    
    CCMCompute *pCompute = new CCMCompute();

    connect(this, SIGNAL(compute(const ccm_input*)), 
        pCompute, SLOT(compute(const ccm_input*)));

    connect(pCompute, SIGNAL(displayBuffer(int, CVBuffer*)), 
        this, SLOT(displayBuffer(int, CVBuffer*)));
	connect(pCompute, SIGNAL(finished(CCMCompute*, ISPC::ParameterList*, double *)), 
		this, SLOT(computationFinished(CCMCompute*, ISPC::ParameterList *, double *)));
    connect(pCompute, SIGNAL(addError(int, double, int, double)),
        this, SLOT(addError(int, double, int, double)));
    connect(pCompute, SIGNAL(logMessage(QString)), 
        this, SIGNAL(logMessage(QString)));
    
    progress->setMaximum(this->inputs.niterations);
    progress->setValue(0);

    result_tab->setEnabled(false);
    progress->setVisible(true);

    warning_icon->setVisible(false);
    warning_lbl->setVisible(false);
    
    if ( this->inputs.berrorGraph )
    {
        errorCurve->setVisible(true);
        result_grp->addTab(errorCurve, QString());
        Ui::CCMTuning::retranslateUi(this); // to give the title
    }
    else
    {
        int i = result_grp->indexOf(errorCurve);
        errorCurve->setVisible(false);
        if ( i >= 0 )
        {
            result_grp->removeTab(i);
        }
    }

    emit compute(&(this->inputs));
    emit changeCentral(getDisplayWidget());
    emit changeResult(getResultWidget());

	if(_isVLWidget)
	{
		gridLayout->addWidget(images_grp, 0, 1);
	}

    return EXIT_SUCCESS;
}

int CCMTuning::reset()
{
    if ( progress->isVisible() ) // it's computing
    {
        return EXIT_FAILURE;
    }

#if 1
    // this is done here until we have a real GUI integration
    {
        CCMConfig sconf;

		if(_isVLWidget)
		{
			sconf.changeCSS(_css);
		}

        sconf.setConfig(this->inputs);

		// Copy temp from ccmTuning to ccmConfig
		sconf.temperature->setValue(temperature->value());
        
        if ( sconf.exec() == QDialog::Accepted )
        {
            this->inputs = sconf.getConfig();

            blc_config blc = genConfig->getBLC();
            for ( int i = 0 ; i < 4 ; i++ )
            {
                this->inputs.sensorBlack[i] = blc.sensorBlack[i];
            }
            this->inputs.systemBlack = blc.systemBlack;
            inputFilename->setText(inputs.inputFilename);

            // display results
            blc_sensor0->setValue( inputs.sensorBlack[0] );
            blc_sensor1->setValue( inputs.sensorBlack[1] );
            blc_sensor2->setValue( inputs.sensorBlack[2] );
            blc_sensor3->setValue( inputs.sensorBlack[3] );
            blc_system->setValue( inputs.systemBlack );

            blc_grp->setEnabled(true);
            wbc_grp->setEnabled(true);
            matrix_grp->setEnabled(true);

			advSettLookUp_btn->setEnabled(true);

			// Copy temp from ccmConfig to ccmTuning
			temperature->setValue(sconf.temperature->value());
        }
        else
        {
            return EXIT_FAILURE;
        }
    }
#endif

    if ( blc_tab )
    {
        images_grp->removeTab(images_grp->indexOf(blc_tab));
        delete blc_tab;
        blc_tab = 0;
    }
    if ( wbc_tab )
    {
        images_grp->removeTab(images_grp->indexOf(wbc_tab));
        delete wbc_tab;
        wbc_tab = 0;
    }
    if ( best_tab )
    {
        images_grp->removeTab(images_grp->indexOf(best_tab));
        delete best_tab;
        best_tab = 0;
    }
    if ( finalError )
    {
        result_grp->removeTab(result_grp->indexOf(finalError));
        delete finalError;
        finalError = 0;
    }

    //isComputing is false

    this->tuning();

    return EXIT_SUCCESS;
}

void CCMTuning::addError(int ie, double error, int ib, double best)
{
    if ( this->inputs.berrorGraph )
    {
        QVector<QPointF> errData, bestData;
        static int errCurve = 0, bestCurve = 1;
#ifdef USE_MATH_NEON
		double logErr = (double)log10f_neon((float)error), logBest = (double)log10f_neon((float)best);
#else
		double logErr = log10(error), logBest = log10(best);
#endif

        if ( this->inputs.bCIE94 )
        {
            logErr = error;
            logBest = best;
        }

        if ( errorCurve->nCurves() > 0 )
        {
            errData = *errorCurve->data(errCurve);
            bestData = *errorCurve->data(bestCurve);

            errorCurve->removeCurve(errCurve);
            errorCurve->removeCurve(bestCurve);
        }

        errData.push_back(QPointF(double(ie), logErr));
        errCurve = errorCurve->addCurve(errData, tr("Error"));
        errorCurve->curve(errCurve)->setPen(QPen(Qt::black));

        bestData.push_back(QPointF(double(ib), logBest));
        bestCurve = errorCurve->addCurve(bestData, tr("Best"));
        errorCurve->curve(bestCurve)->setPen(QPen(Qt::red));

        //errorCurve->setAxisAutoScale(true);
        //errorCurve->setAxisScale(QwtPlot::yLeft, 0.0, 100000.0, 10000.0);
        errorCurve->setAxisScale(QwtPlot::xBottom, 0.0, double(inputs.niterations), double(inputs.niterations/5));
        errorCurve->replot();
    }
    currBest = best;
    progress->setValue(ie);
}

void CCMTuning::displayErrors(double *patchError)
{
    QGridLayout *pLayout;
    if ( finalError )
    {
        return;
    }
    else
    {
        finalError = new QWidget();
        pLayout = new QGridLayout();
    }
    double average = 0.0, sum = 0.0;
    QString avgstr;

    for ( int p = 0 ; p < MACBETCH_N ; p++ )
    {
        sum += patchError[p];
    }
    average = sum/MACBETCH_N;
    avgstr = tr(" avg=") + QString::number(average);
    finalError->setToolTip(tr("sum=")+QString::number(sum) + avgstr);
    
    for ( int v = 0 ; v < MACBETCH_H ; v++ )
    {
        for ( int h = 0 ; h < MACBETCH_W ; h++ )
        {
            int p = v*MACBETCH_W+h;
            double perr = patchError[p];
            QLabel *pLbl = 0;
            
            pLbl = new QLabel(QString::number(perr));
            
            pLbl->setToolTip(tr("patch=") + QString::number(patchError[p]) + avgstr);
            
            if ( perr > average*MASSIVE_ERROR )
            {
                pLbl->setStyleSheet("QLabel { background-color : black; color : white }");
            }
            else if ( perr > average*BIG_ERROR )
            {
                pLbl->setStyleSheet("QLabel { background-color : red; color : white }");
            }
            else if ( perr > average*AVERAGE_ERROR )
            {
                pLbl->setStyleSheet("QLabel { background-color : orange; color : white }");
            }
            else if ( perr > average*ACCEPTABLE_ERROR )
            {
                pLbl->setStyleSheet("QLabel { background-color : yellow; color : black }");
            }
            else
            {
                pLbl->setStyleSheet("QLabel { background-color : white; color : black }");
            }
            pLbl->setAlignment(Qt::AlignCenter);
            
            pLayout->addWidget( pLbl, v, h );
        }
    }

    finalError->setLayout(pLayout);
    //finalError->setVisible(true);
    result_grp->addTab(finalError, "error" ); // real title set in retranslate

    if ( !this->inputs.bCIE94 )
    {
        currBest = sum; // for the title
    }
    retranslate();
}

void CCMTuning::displayBuffer(int b, CVBuffer *pBuffer)
{
    ImageView *pView = 0;
    bool addTarget = false;
#if defined(TUNE_CCM_DBG)
    std::string title;
#endif
       
    switch(b)
    {
    case ORIG_BUFFER:
        if (! blc_tab )
        {
            blc_tab = new ImageView();
			blc_tab->menubar->hide();
			blc_tab->showDPF_cb->hide();
            images_grp->addTab(blc_tab, "blc"); // real title set in retranslate
            retranslate();
        }
        pView = blc_tab;
#if defined(TUNE_CCM_DBG)
        title="ORIG_BUFFER";
#endif
        break;

    case WBC_BUFFER:
        addTarget = true;
        if (! wbc_tab )
        {
            wbc_tab = new ImageView();
			wbc_tab->menubar->hide();
			wbc_tab->showDPF_cb->hide();
            images_grp->addTab(wbc_tab, "wbc"); // real title set in retranslate
            retranslate();
        }
        pView = wbc_tab;
#if defined(TUNE_CCM_DBG)
        title="WBC_BUFFER";
#endif
        break;

    case BEST_BUFFER:
        addTarget = true;
        if (! best_tab )
        {
            best_tab = new ImageView();
			best_tab->menubar->hide();
			best_tab->showDPF_cb->hide();
            images_grp->addTab(best_tab, "best"); // real title set in retranslate
            retranslate();
        }
        pView = best_tab;
#if defined(TUNE_CCM_DBG)
        title="BEST_BUFFER";
#endif
        break;
    }

    if ( pBuffer && !pBuffer->data.empty() && pView )
    {
        pView->loadBuffer(*pBuffer, inputs.bgammadisplay);
        images_grp->setCurrentIndex(images_grp->indexOf(pView));
        
        //pView->showHistograms();

#if defined(TUNE_CCM_DBG)
        displayImage(pBuffer->data.clone(), title, 0, 0, pBuffer->data.cols, pBuffer->data.rows);
#endif

        if ( addTarget )
        {
            for ( int p = 0 ; p < MACBETCH_N ; p++ )
            {
                QColor patchC;
                if ( b == BEST_BUFFER && inputs.bgammadisplay )
                {
                    double gamma[3];
                    for ( int c = 0 ; c < 3 ; c++ )
                    {
                        gamma[c] = CVBuffer::gammaCorrect(this->inputs.expectedRGB[p][c]);
                    }
                    
                    patchC = QColor::fromRgb( floor(gamma[0]), floor(gamma[1]), floor(gamma[2]) );
                }
                else
                {
                    patchC = QColor::fromRgb( floor(this->inputs.expectedRGB[p][0]), floor(this->inputs.expectedRGB[p][1]), floor(this->inputs.expectedRGB[p][2]) );
                }
                int w = inputs.checkerCoords[p].width;///2;
                int h = inputs.checkerCoords[p].height/2;
                int x1 = inputs.checkerCoords[p].x;//+w/2;
                int y1 = inputs.checkerCoords[p].y+h/2;
                cv::Rect patchP(x1, y1, w, h);
                                                
                pView->addGrid(&patchP, 1, 1, QPen(patchC), QBrush(patchC));
            }
        }

        pView->addGrid(inputs.checkerCoords, MACBETCH_W, MACBETCH_H, QPen(Qt::white));
    }
}

void CCMTuning::computationFinished(CCMCompute *toStop, ISPC::ParameterList *_result, double *patchErrors)
{
    toStop->deleteLater();

    displayErrors(patchErrors);

#if defined(TUNE_CCM_DBG)
        _result->save("ccm_result.txt");
#endif
    
    displayResult(*_result);

    delete patchErrors;
    if ( this->pResult )
    {
        delete this->pResult;
    }
	printf("pResult is ready!\n");
    this->pResult = _result;
    progress->setVisible(false);

#ifdef USE_MATH_NEON
#if defined(TUNE_CCM_DBG)
    QMessageBox::information(0, tr("Computation finished"), tr("Computation finished, best error ") + QString::number((double)log10f_neon((float)currBest)));
#endif
#else
#if defined(TUNE_CCM_DBG)
    QMessageBox::information(0, tr("Computation finished"), tr("Computation finished, best error ") + QString::number(log10(currBest)));
#endif
#endif
}

void CCMTuning::displayResult(const ISPC::ParameterList &_result)
{
	ISPC::ModuleBLC blc;
	ISPC::ModuleWBC wbc;
	ISPC::ModuleCCM ccm;

	blc.load(_result);
	wbc.load(_result);
	ccm.load(_result);

	blc_sensor0->setValue( blc.aSensorBlack[0] );
    blc_sensor1->setValue( blc.aSensorBlack[1] );
    blc_sensor2->setValue( blc.aSensorBlack[2] );
    blc_sensor3->setValue( blc.aSensorBlack[3] );

	blc_system->setValue( blc.ui32SystemBlack );

    // white balance correction
	wbc_gain0->setValue( wbc.aWBGain[0] );
    wbc_gain1->setValue( wbc.aWBGain[1] );
    wbc_gain2->setValue( wbc.aWBGain[2] );
    wbc_gain3->setValue( wbc.aWBGain[3] );

	wbc_clip0->setValue( wbc.aWBClip[0] );
    wbc_clip1->setValue( wbc.aWBClip[1] );
    wbc_clip2->setValue( wbc.aWBClip[2] );
    wbc_clip3->setValue( wbc.aWBClip[3] );

    // colour correction matrix
	ccm_matrix0->setValue( ccm.aMatrix[0] );
    ccm_matrix1->setValue( ccm.aMatrix[1] );
    ccm_matrix2->setValue( ccm.aMatrix[2] );
    ccm_matrix3->setValue( ccm.aMatrix[3] );
    ccm_matrix4->setValue( ccm.aMatrix[4] );
    ccm_matrix5->setValue( ccm.aMatrix[5] );
    ccm_matrix6->setValue( ccm.aMatrix[6] );
    ccm_matrix7->setValue( ccm.aMatrix[7] );
    ccm_matrix8->setValue( ccm.aMatrix[8] );

	ccm_offsets0->setValue( ccm.aOffset[0] );
    ccm_offsets1->setValue( ccm.aOffset[1] );
    ccm_offsets2->setValue( ccm.aOffset[2] );

    result_tab->setEnabled(true);
    save_btn->setEnabled(true);
    progress->setVisible(false);
	if(_isVLWidget)
	{
		integrate_btn->setEnabled(true);
	}
    result_grp->setCurrentIndex( result_grp->indexOf(result_tab) );
}

int CCMTuning::saveResult(const QString &_filename)
{
    QString filename;
        
    if ( _filename.isEmpty() )
    {
        filename = QFileDialog::getSaveFileName(this, tr("Output FelixArgs file"), QString(), tr("*.txt"));

        // on linux the filename may not have .txt - disabled because it does not warn if the file is already present
        /*int len = filename.count();
        if ( !(filename.at(len-4) == '.' && filename.at(len-3) == 't' && filename.at(len-2) == 'x' && filename.at(len-1) == 't')  )
        {
            filename += ".txt";
        }*/
    }
    else
    {
        filename = _filename;
    }

    if ( filename.isEmpty() )
    {
        return EXIT_FAILURE;
    }

	ISPC::ParameterList res;
    try
    {
		saveParameters(res);

        if(ISPC::ParameterFileParser::saveGrouped(res, std::string(filename.toLatin1())) != IMG_SUCCESS)
        {
            throw(QtExtra::Exception(tr("Failed to save FelixParameter file with CCM parameters")));
        }
    }
    catch(QtExtra::Exception e)
    {
        e.displayQMessageBox(this);
        return EXIT_FAILURE;
    }
    
    QString mess = "CCM: saving FelixSetupArgs in " + filename + "\n";
    emit logMessage(mess);
    
    return EXIT_SUCCESS;
}

void CCMTuning::saveParameters(ISPC::ParameterList &list) const throw(QtExtra::Exception)
{
    if (! this->pResult )
    {
        throw(QtExtra::Exception(tr("No results to save for CCM")));
    }

	ISPC::ModuleCCM ccm;
	ccm.load(*pResult);
	ccm.save(list, ISPC::ModuleCCM::SAVE_VAL);

	ISPC::ModuleWBC wbc;
	wbc.load(*pResult);
	wbc.save(list, ISPC::ModuleWBC::SAVE_VAL);

    // calibration info
    std::string calibInfo = "//CCM ";  
	ISPC::Parameter inputImage(calibInfo + "input image", std::string(this->inputs.inputFilename.toLatin1()));
	list.addParameter(inputImage);
    
	std::string param;
    for ( int i = 0 ; i < 4 ; i++ )
    {
		param.append(QString::number(inputs.sensorBlack[i]).toLatin1());
		param.append(" ");
    }
	ISPC::Parameter blcSensorBlack(calibInfo + ISPC::ModuleBLC::BLC_SENSOR_BLACK.name + " used", param);
	list.addParameter(blcSensorBlack);

	ISPC::Parameter blcSysBlack(calibInfo + ISPC::ModuleBLC::BLC_SYS_BLACK.name + " used", std::string(QString::number(inputs.systemBlack).toLatin1()));
	list.addParameter(blcSysBlack);

	ISPC::Parameter temp(calibInfo + "TEMPERATURE", std::string(QString::number(temperature->value()).toLatin1()));
	list.addParameter(temp);
}

void CCMTuning::saveParameters(ISPC::ParameterList &list, int ccmN) const throw(QtExtra::Exception)
{
	if (! this->pResult )
    {
        throw(QtExtra::Exception(tr("No results to save for CCM")));
    }

	ISPC::ModuleCCM ccm;
	ISPC::ModuleBLC blc;
	ISPC::ModuleWBC wbc;

	ccm.load(*pResult);
    blc.load(*pResult);
	wbc.load(*pResult);

	std::string groupName = "WB_";
	std::string groupName2 = "//WB ";
	std::string ccmNS = std::string(QString::number(ccmN).toLatin1());
    std::ostringstream os;

	ISPC::Parameter inputImage(groupName2 + "input image" + "_" + ccmNS, std::string(this->inputs.inputFilename.toLatin1()));
	list.addParameter(inputImage);

	os.str("");
	for ( int i = 0 ; i < 4 ; i++ ) os << inputs.sensorBlack[i] << " ";
	ISPC::Parameter blcSensorBlack(groupName2 + ISPC::ModuleBLC::BLC_SENSOR_BLACK.name + "_" + ccmNS, os.str());
	list.addParameter(blcSensorBlack);

	os.str("");
	os << inputs.systemBlack;
	ISPC::Parameter blcSysBlack(groupName2 + ISPC::ModuleBLC::BLC_SYS_BLACK.name + "_" + ccmNS, os.str());
	list.addParameter(blcSysBlack);

	os.str("");
    for ( int i = 0 ; i < 9 ; i++ ) os << ccm.aMatrix[i] << " ";
	ISPC::Parameter ccmMatrix(ISPC::TemperatureCorrection::WB_CCM_S.name + "_" + ccmNS, os.str());
	list.addParameter(ccmMatrix);

	os.str("");
	for ( int i = 0 ; i < 3 ; i++ ) os << ccm.aOffset[i] << " ";
	ISPC::Parameter ccmOffset(ISPC::TemperatureCorrection::WB_OFFSETS_S.name + "_" + ccmNS, os.str());
	list.addParameter(ccmOffset);

	os.str("");
	for ( int i = 0 ; i < 4 ; i++ ) os << wbc.aWBGain[i] << " ";
	ISPC::Parameter wbcGains(ISPC::TemperatureCorrection::WB_GAINS_S.name + "_" + ccmNS, os.str());
	list.addParameter(wbcGains);

	ISPC::Parameter wbcTemp(ISPC::TemperatureCorrection::WB_TEMPERATURE_S.name + "_" + ccmNS, std::string(QString::number(temperature->value()).toLatin1()));
	list.addParameter(wbcTemp);
}

void CCMTuning::retranslate()
{
    Ui::CCMTuning::retranslateUi(this);

    if ( blc_tab )
    {
        images_grp->setTabText(images_grp->indexOf(blc_tab), tr("Input image (BLC applied)"));
    }
    if ( wbc_tab )
    {
        images_grp->setTabText(images_grp->indexOf(wbc_tab), tr("CCM base (WBC applied)"));
    }
    if ( best_tab )
    {
        images_grp->setTabText(images_grp->indexOf(best_tab), tr("CCM result"));
    }
    if ( finalError )
    {
        QString title = tr("Error CIE94 ") + QString::number(currBest/MACBETCH_N);
        
        result_grp->setTabText(result_grp->indexOf(finalError), title);   
    }
}

void CCMTuning::removeSlot()
{
	emit reqRemove(this);
}

void CCMTuning::setDefaultSlot()
{
	emit reqDefault(this);
}

void CCMTuning::resetDefaultSlot()
{
	default_rbtn->setChecked(false);
}

// LSH advace settings Lookup option
void CCMTuning::advSettLookUp()
{
	CCMConfigAdv advSettLookUp;

	if(_isVLWidget)
    {
		advSettLookUp.setStyleSheet(_css);
	}

	advSettLookUp.setConfig(inputs); // upload settings from inputs
	advSettLookUp.buttonBox->hide(); // don't need Accept/Reject buttons
	advSettLookUp.setReadOnly(true); // settings are not to be edited here
    
	advSettLookUp.exec();
}


