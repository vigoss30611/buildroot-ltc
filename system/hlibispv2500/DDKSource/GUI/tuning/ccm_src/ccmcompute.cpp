/**
******************************************************************************
@file ccmcompute.cpp

@brief CCMCompute class implementation

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
#include "ccm/compute.hpp"
#include "ccm/tuning.hpp"

#include "buffer.hpp"

#include <QtCore/QThread>

#include "imageUtils.h"

#include <cstring>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <algorithm>
#include <ispc/ModuleIIF.h>
#ifdef USE_MATH_NEON
#include <mneon.h>
#endif

#define CHANGE_RAND_MOD 100 // used for the percentage of chance to change the matrix (ramd()%MOD)
#define CHANGE_OSCILLATION 300.0 // oscillation

//#define NO_OFF

double CCMCompute::random(double _min, double _max)
{
    int i = qrand();
    double v = _min + (((double)i) / RAND_MAX)*(_max-_min);

    //printf("random %d -> %lf (%lf..%lf)\n", i, v, _min, _max);
    return v;
}

CVBuffer CCMCompute::applyBLC(const CVBuffer &sInput, const ISPC::ModuleBLC &sBLC)
{
    std::ostringstream os;

    if ( sInput.mosaic == MOSAIC_NONE )
    {
        printf("%s: THE INPUT BUFFER SHOULD BE RGGB\n", __FUNCTION__); // usage error
        throw(1);
    }

    CVBuffer sOutput = sInput.clone();

    if (verbose)
    {
        cv::Scalar avg;

        avg = sInput.computeAverages(this->inputs.checkerCoords[this->inputs.whitePatchI]);
        avg = avg / 4.0; // to scale down from 10b to 8b

        os.str("");
        os << prefix << "before BLC - white patch average " << avg[0] << " " << avg[1] << " " << avg[2] << " " << avg[3] << " (scaled 1/4)" << std::endl;
        
        avg = sInput.computeAverages(this->inputs.checkerCoords[this->inputs.blackPatchI]);
        avg = avg / 4.0;
        
        os << prefix << "before BLC - black patch average " << avg[0] << " " << avg[1] << " " << avg[2] << " " << avg[3] << " (scaled 1/4)" << std::endl;

        emit logMessage(QString::fromStdString(os.str()));
    }

    //int belowBlack = 0;

    std::vector<cv::Mat> channels;

    cv::split(sOutput.data, channels);
    
    for ( int c = 0 ; c < 4 ; c++ )
    {
		channels[c] = channels[c]-((sBLC.aSensorBlack[c])/2.0)*4.0;
        // the value has to be divided by 2 (register precision)
        // and multiplied by 4 to be 10b
    }

    cv::merge(channels, sOutput.data);
    
    if ( verbose )
    {
        cv::Scalar avg;

        avg = sOutput.computeAverages(this->inputs.checkerCoords[this->inputs.whitePatchI]);
        avg = avg/4.0;
        
        os.str("");
        os << prefix << "after BLC - white patch average " << avg[0] << " " << avg[1] << " " << avg[2] << " " << avg[3] << " (scaled 1/4)" << std::endl;
        
        avg = sOutput.computeAverages(this->inputs.checkerCoords[this->inputs.blackPatchI]);
        avg = avg/4.0;
                
        os << prefix << "after BLC - black patch average " << avg[0] << " " << avg[1] << " " << avg[2] << " " << avg[3] << " (scaled 1/4)" << std::endl;

        emit logMessage(QString::fromStdString(os.str()));
    }

    return sOutput;
}

CVBuffer CCMCompute::applyWBC(const CVBuffer &sInput, const ISPC::ModuleWBC &sWBC, const ISPC::ModuleBLC &sBLC)
{
    std::ostringstream os;

    if ( sInput.mosaic == MOSAIC_NONE )
    {
        printf("%s: THE INPUT BUFFER SHOULD BE RGGB\n", __FUNCTION__); // usage error
        throw(1);
    }

    CVBuffer sOutput = sInput.clone();

    if ( verbose )
    {
        cv::Scalar avg;

        avg = sInput.computeAverages(this->inputs.checkerCoords[this->inputs.whitePatchI]);
        avg = avg/4.0; // to scaled down to 8b values
        
        os.str("");
        os << prefix << "before WBC - white patch average " << avg[0] << " " << avg[1] << " " << avg[2] << " " << avg[3] << " (scaled 1/4)" << std::endl;
        
        avg = sInput.computeAverages(this->inputs.checkerCoords[this->inputs.blackPatchI]);
        avg = avg/4.0;
        
        os << prefix << "before WBC - black patch average " << avg[0] << " " << avg[1] << " " << avg[2] << " " << avg[3] << " (scaled 1/4)" << std::endl;

        emit logMessage(QString::fromStdString(os.str()));
    }

    std::vector<cv::Mat> channels;
    cv::split(sOutput.data, channels);

    for ( int c = 0 ; c < 4 ; c++ )
    {
        channels[c] = channels[c]*sWBC.aWBGain[c];
        //channels[c] = channels[c] + sBLC.systemBlack;
    }

    cv::merge(channels, sOutput.data);

    if ( verbose )
    {
        cv::Scalar avg;

        avg = sOutput.computeAverages(this->inputs.checkerCoords[this->inputs.whitePatchI]);
        avg = avg / 4.0;  // to scaled down to 8b values
        
        os.str("");
        os << prefix << "after WBC - white patch average " << avg[0] << " " << avg[1] << " " << avg[2] << " " << avg[3] << " (scaled 1/4)" << std::endl;
        
        avg = sOutput.computeAverages(this->inputs.checkerCoords[this->inputs.blackPatchI]);
        avg = avg / 4.0;
        
        os << prefix << "after WBC - black patch average " << avg[0] << " " << avg[1] << " " << avg[2] << " " << avg[3] << " (scaled 1/4)" << std::endl;

        emit logMessage(QString::fromStdString(os.str()));
    }

    return sOutput;
}

void CCMCompute::computeWBCGains(const CVBuffer &sBuffer, ISPC::ModuleWBC &sWBC)
{
    cv::Scalar whitePatchAvg;
    double whitePatch; // the whole patch average
    std::ostringstream os;
        
    whitePatchAvg = sBuffer.computeAverages(this->inputs.checkerCoords[this->inputs.whitePatchI]); // get the white averages

    whitePatch = 0.0;

    // use the maximum
    double maxG = 0;
    
    for ( int i = 0 ; i < 4 ; i++ )
    {
        whitePatch = std::max(whitePatch, whitePatchAvg[i]);
    }

    os.str("");
    os << prefix << "found white patch average (RGGB) " 
       << whitePatchAvg[0] << " " << whitePatchAvg[1] << " " << whitePatchAvg[2] << " " << whitePatchAvg[3] 
       << " (max " << whitePatch << ")" << std::endl;
    emit logMessage(QString::fromStdString(os.str()));

    for ( int i = 0 ; i < 4 ; i++ )
    {
		sWBC.aWBGain[i] = whitePatch/whitePatchAvg[i];
        maxG = std::max(maxG,sWBC.aWBGain[i]);
    }

    if ( this->inputs.bCheckMaxWBCGain && maxG > this->inputs.maxWBCGain )
    {
        whitePatch = this->inputs.maxWBCGain/maxG;
        
        os.str("");
        os << prefix << " maximum gain " << maxG << " found! Scaling by " << whitePatch << std::endl;
        emit logMessage(QString::fromStdString(os.str()));

        for ( int i = 0 ; i < 4 ; i++ )
        {
            sWBC.aWBGain[i] *= whitePatch;
        }
    }

    // the clip is configured as the minimum of all gains
    whitePatch = sWBC.aWBGain[0]; // whitePatch used as minimum
    for ( int i = 1 ; i < 4 ; i++ )
    {
        whitePatch = std::min((double)whitePatch, sWBC.aWBGain[i]);
    }
    for ( int i = 0 ; i < 4 ; i++ )
    {
		sWBC.aWBClip[i] = whitePatch;
    }

    os.str("");
    os << prefix << " proposed WBC gains (mosaic applied) "
       << sWBC.aWBGain[0] << " " << sWBC.aWBGain[1] << " " << sWBC.aWBGain[2] << " " << sWBC.aWBGain[3]
       << " - clip " << sWBC.aWBClip[0] << " " << sWBC.aWBClip[1] << " " << sWBC.aWBClip[2] << " " << sWBC.aWBClip[3]
       << std::endl;
    emit logMessage(QString::fromStdString(os.str()));
}

CVBuffer CCMCompute::rescaleBuffer(const CVBuffer &sInput)
{
    cv::Scalar blackPatchAvg, whitePatchAvg;
    double inputBlackAvg = 0.0, inputWhiteAvg = 0.0;
    double outputBlackAvg = 0.0, outputWhiteAvg = 0.0;
    double inputScaling = 1.0;
    CVBuffer sOutput;
    CVBuffer sDemosaic;
    std::ostringstream os;
        
    int *tmpBuff = 0;

    if ( sInput.mosaic == MOSAIC_NONE )
    {
        printf("%s: THE INPUT BUFFER SHOULD BE RGGB\n", __FUNCTION__); // usage error
        throw(1);
    }

    if ( verbose )
    {
        cv::Scalar avg;
        
        avg = sInput.computeAverages(this->inputs.checkerCoords[this->inputs.whitePatchI]);
        avg = avg / 4.0; // to display as 8b
        
        os.str("");
        os << prefix << "before demosaic - white patch average " << avg[0] << " " << avg[1] << " " << avg[2] << " " << avg[3] << " (scaled 1/4)" << std::endl;
        
        avg = sInput.computeAverages(this->inputs.checkerCoords[this->inputs.blackPatchI]);
        avg = avg / 4.0;
        
        os << prefix << "before demosaic - black patch average " << avg[0] << " " << avg[1] << " " << avg[2] << " " << avg[3] << " (scaled 1/4)" << std::endl;

        emit logMessage(QString::fromStdString(os.str()));
    }

    sDemosaic = sInput.demosaic(); // BGR output
    cv::cvtColor(sDemosaic.data, sDemosaic.data, CV_RGB2BGR); // convert to RGB to ease computations

    whitePatchAvg = sDemosaic.computeAverages(this->inputs.checkerCoords[this->inputs.whitePatchI]);
    blackPatchAvg = sDemosaic.computeAverages(this->inputs.checkerCoords[this->inputs.blackPatchI]);
    
    for ( int c = 0 ; c < 3 ; c++ )
    {
        inputBlackAvg += blackPatchAvg[c];
        inputWhiteAvg += whitePatchAvg[c];
        outputBlackAvg += this->inputs.expectedRGB[this->inputs.blackPatchI][c];
        outputWhiteAvg += this->inputs.expectedRGB[this->inputs.whitePatchI][c];
    }
    inputBlackAvg /= 3.0;
    inputWhiteAvg /= 3.0;
    outputBlackAvg /= 3.0;
    outputWhiteAvg /= 3.0;

    inputScaling = (outputWhiteAvg-outputBlackAvg)/(inputWhiteAvg-inputBlackAvg);

    if ( verbose )
    {
        os.str("");
        os << prefix << "before rescale (demosaiced) - white patch average  " 
           << whitePatchAvg[0] << " " << whitePatchAvg[1] << " " << whitePatchAvg[2] 
           << " = " << inputWhiteAvg << " - target " << outputWhiteAvg 
           << std::endl;
    
        os << prefix << "before rescale (demosaiced) - black patch average  " 
           << blackPatchAvg[0] << " " << blackPatchAvg[1] << " " << blackPatchAvg[2] 
           << " = " << inputBlackAvg << " - target " << outputBlackAvg 
           << std::endl;
    
        os << prefix << "WBC correction = ( (v-" << inputBlackAvg << ")*" << inputScaling << " )+" << outputBlackAvg << " + " << this->inputs.systemBlack
           << std::endl;

        emit logMessage(QString::fromStdString(os.str()));
    }

    sOutput = sDemosaic.clone();

    std::vector<cv::Mat> channels;
    cv::split(sOutput.data, channels);
    
    for ( int c = 0 ; c < 3 ; c++ )
    {
        channels[c] = (channels[c] - inputBlackAvg) * inputScaling + outputBlackAvg + this->inputs.systemBlack;
    }

    cv::merge(channels, sOutput.data);

    if ( verbose )
    {
        whitePatchAvg = sOutput.computeAverages(this->inputs.checkerCoords[this->inputs.whitePatchI]);
        blackPatchAvg = sOutput.computeAverages(this->inputs.checkerCoords[this->inputs.blackPatchI]);

        os.str("");
        os << prefix << "after rescale (demosaiced) - white patch average  " 
           << whitePatchAvg[0] << " " << whitePatchAvg[1] << " " << whitePatchAvg[2] 
           << " = " << inputWhiteAvg << " - target " << outputWhiteAvg 
           << std::endl;
    
        os << prefix << "after rescale (demosaiced) - black patch average  " 
           << blackPatchAvg[0] << " " << blackPatchAvg[1] << " " << blackPatchAvg[2] 
           << " = " << inputBlackAvg << " - target " << outputBlackAvg 
           << std::endl;

        emit logMessage(QString::fromStdString(os.str()));
    }

    return sOutput;
}

double* CCMCompute::computeCCM(const CVBuffer &sBuffer, ISPC::ModuleCCM &sCCM, const ISPC::ModuleWBC &sWBC)
{
    double bestError, error;
    ISPC::ModuleCCM sBestCCM;

    CVBuffer *pComputeBuffer = 0;
    ISPC::ModuleCCM sComputeCCM;
    int it = 0;
    int lastChanged = 0; // nb of it since last change
    int maxLastChanged = 0;
    double minWBCClip = 1.0;
    std::ostringstream os;

    cv::Scalar patchAverages[MACBETCH_N];

    for ( int i = 0 ; i < 4 ; i++ )
    {
		minWBCClip = std::min((double)minWBCClip, sWBC.aWBClip[i]);
    }

    // fake best matrix as identity
	sBestCCM.aMatrix[0] = +1.0;
    sBestCCM.aMatrix[1] = +0.0;
    sBestCCM.aMatrix[2] = -0.0;

    sBestCCM.aMatrix[3] = -0.0;
    sBestCCM.aMatrix[4] = +1.0;
    sBestCCM.aMatrix[5] = +0.0;

    sBestCCM.aMatrix[6] = -0.0;
    sBestCCM.aMatrix[7] = -0.0;
    sBestCCM.aMatrix[8] = +1.0;

	sBestCCM.aOffset[0] = 0.0;
    sBestCCM.aOffset[1] = 0.0;
    sBestCCM.aOffset[2] = 0.0;

    for ( int p = 0 ; p < MACBETCH_N ; p++ )
    {
        patchAverages[p] = sWBCBufferRGB.computeAverages(this->inputs.checkerCoords[p]);
    }
    
    pComputeBuffer = new CVBuffer();

    *pComputeBuffer = applyCCM(sWBCBufferRGB, sBestCCM);

    if ( verbose )
    {
        cv::Scalar avg;
        avg = pComputeBuffer->computeAverages(this->inputs.checkerCoords[this->inputs.whitePatchI]);

        os.str("");
        os << prefix << "before CCM - white patch average " << avg[0] << " " << avg[1] << " " << avg[2]
           << std::endl;
        
        avg = pComputeBuffer->computeAverages(this->inputs.checkerCoords[this->inputs.blackPatchI]);

        os << prefix << "before CCM - black patch average " << avg[0] << " " << avg[1] << " " << avg[2] 
           << std::endl;

        emit logMessage(QString::fromStdString(os.str()));
    }

#if defined(TUNE_CCM_DBG)
    {
        cv::Mat rescaled = pComputeBuffer->data.clone();
        displayImage(rescaled, "WBC_BUFFER", 0, 0, rescaled.cols, rescaled.rows);
    }
#endif

    {
        cv::Mat rescaled;
        pComputeBuffer->data.convertTo(rescaled, CV_8U);//, 1/4.0); // rescale elements to 0..255 (already demosaiced so receiver will not rescale it)
        pComputeBuffer->data = rescaled;
        cv::cvtColor(pComputeBuffer->data, pComputeBuffer->data, CV_RGB2BGR);

        emit displayBuffer(CCMTuning::WBC_BUFFER, pComputeBuffer);
    }
    
    pComputeBuffer = 0; // pComputeBuffer is deleted by the receiver

    if ( !this->inputs.bCIE94) 
    {
        bestError = computeError(patchAverages, sBestCCM);
    }
    else
    {
        bestError = computeCIE94(patchAverages, sBestCCM);
    }

    emit addError(0, bestError, 0, bestError); 

    double temperature = 0;
    it = 0;
    while ( it < inputs.niterations )
    { 

        if ( it < inputs.nwarmiterations )
        {
            randomMatrix(sComputeCCM);
        }
        else
        {
            ISPC::ModuleCCM sRandomCCM;

            temperature = ((double)inputs.nwarmiterations)/(1+it)*((double)(inputs.niterations-it))/(inputs.niterations);
#ifdef USE_MATH_NEON
			temperature += temperature*(double)sinf_neon((float)(double(it)/CHANGE_OSCILLATION)*2.0f*M_PI);
#else
			temperature += temperature*sin((double(it)/CHANGE_OSCILLATION)*2*M_PI);
#endif
            temperature *= this->inputs.temperatureStrength;

            // use the temperature to smooth the randomness
            randomMatrix(sRandomCCM);

            sComputeCCM = sBestCCM;

            // apply the changes only with a percentage of chance
            for ( int i = 0 ; i < 9 ; i++ )
            {
                int r = qrand()%CHANGE_RAND_MOD;
                if( r < this->inputs.changeProb )
                {
					sComputeCCM.aMatrix[i] = sComputeCCM.aMatrix[i]+(sRandomCCM.aMatrix[i]*temperature);
                }
            }
            for ( int i = 0 ; i < 3 ; i++ )
            {
                int r = qrand()%CHANGE_RAND_MOD;
                if ( r < this->inputs.changeProb )
                {
					sComputeCCM.aOffset[i] = sComputeCCM.aOffset[i]+(sRandomCCM.aOffset[i]*temperature);
                }
            }

            if ( this->inputs.bnormalise )
            {
                normaliseMatrix(sComputeCCM);
            }

            for ( int i = 0 ; i < 9 ; i++ )
            {
				sComputeCCM.aMatrix[i] = std::max(std::min((double)sComputeCCM.aMatrix[i], this->inputs.matrixMax[i]), this->inputs.matrixMin[i]);
            }
            for ( int i = 0 ; i < 3 ; i++ )
            {
				sComputeCCM.aOffset[i] = std::max(std::min((double)sComputeCCM.aOffset[i], this->inputs.offsetMax[i]), this->inputs.offsetMin[i]);
            }

        }

        if ( !this->inputs.bCIE94 )
        {
            error = computeError(patchAverages, sComputeCCM);
        }
        else
        {
            error = computeCIE94(patchAverages, sComputeCCM);
        }

        if ( error < bestError )
        {
            if ( verbose )
            {
                os.str("");
                os << prefix << "New best error at it " << it << ": " << error << " (old " << bestError << ")"
                   << std::endl;
                emit logMessage(QString::fromStdString(os.str()));
            }
                
            bestError = error;
            sBestCCM = sComputeCCM;
            maxLastChanged = std::max(lastChanged, maxLastChanged);
            lastChanged = 0;
        }

        
#if defined(TUNE_CCM_DBG) 
        //printf("temperature %lf\n", temperature);
        os.str("");
        printCCM(os, sComputeCCM);
        emit logMessage(QString::fromStdString(os.str()));
#endif
        if ( it%(this->inputs.sendinfo) == (this->inputs.sendinfo-1) )
        {
            if ( verbose )
            {
                os.str("");
                os << prefix << "it " << it << " error = " << error << " (best " << bestError << " - last changed " << lastChanged << " it ago - temperature " << temperature << ")"
                   << std::endl;
                emit logMessage(QString::fromStdString(os.str()));
            }
            emit addError(it, error, it-lastChanged, bestError);
        }

        it++;
        lastChanged++;
    } // while

    pComputeBuffer = new CVBuffer();

    *pComputeBuffer = applyCCM(sWBCBufferRGB, sComputeCCM);
    
    if ( verbose )
    {
        cv::Scalar avg;
        avg = pComputeBuffer->computeAverages(this->inputs.checkerCoords[this->inputs.whitePatchI]);
        
        os.str("");
        os << prefix << "after CCM - white patch average " << avg[0] << " " << avg[1] << " " << avg[2]
           << std::endl;
        
        avg = pComputeBuffer->computeAverages(this->inputs.checkerCoords[this->inputs.blackPatchI]);
        
        os << prefix << "after CCM - black patch average " << avg[0] << " " << avg[1] << " " << avg[2]
           << std::endl;

        emit logMessage(QString::fromStdString(os.str()));
    }

#if defined(TUNE_CCM_DBG)
    displayImage(pComputeBuffer->data.clone(), "BEST_BUFFER", pComputeBuffer->data.cols, pComputeBuffer->data.rows, pComputeBuffer->data.cols, pComputeBuffer->data.rows);
#endif

    {
        cv::Mat rescaled;
        pComputeBuffer->data.convertTo(rescaled, CV_8U);//, 1/4.0);
        pComputeBuffer->data = rescaled;
        cv::cvtColor(pComputeBuffer->data, pComputeBuffer->data, CV_RGB2BGR);
    }
    emit displayBuffer(CCMTuning::BEST_BUFFER, pComputeBuffer);

    pComputeBuffer = 0; // deleted by the receiver

    emit addError(it, error, it-lastChanged, bestError);
    sCCM = sBestCCM;

    double *patchErrors;

    patchErrors = (double*)malloc(MACBETCH_N*sizeof(double));

    for ( int p = 0 ; p < MACBETCH_N ; p++ )
    {
        //double averages[3];
        cv::Scalar averages;
        cv::Scalar expected;

        if ( verbose )
        {
            //BufferDouble::computeAverages(pWBCBufferRGB, this->inputs.checkerCoords[p], averages);
            averages = sWBCBufferRGB.computeAverages(this->inputs.checkerCoords[p]);

        
            for ( int c = 0 ; c < 3 ; c++ )
            {
                if ( averages[c] != patchAverages[p][c] )
                {
                    printf("error - patch averages %d[%d] was corrupted %lf vs %lf\n",
                        p, c, averages[c],patchAverages[p][c]
                    );
                    os.str("");
                    os << prefix << "error - patch averages " << p << "[" << c << "] was corrupted " << averages[c] << " vs " << patchAverages[p][c]
                       << std::endl;
                    emit logMessage(QString::fromStdString(os.str()));
                }
            }
        
            //applyCCM(&(patchAverages[3*p]), sBestCCM, averages);
            averages = applyCCM(patchAverages[p], sBestCCM);
        
            os.str("");
            os << prefix << "patch " << p << " avg " << averages[0] << " " << averages[1] << " " << averages[2]
               << std::endl;
            emit logMessage(QString::fromStdString(os.str()));
        }
        
        expected[0] = this->inputs.expectedRGB[p][0];
        expected[1] = this->inputs.expectedRGB[p][1];
        expected[2] = this->inputs.expectedRGB[p][2];
        patchErrors[p] = computeCIE94(patchAverages[p], expected, sCCM, this->inputs.patchWeight[p]);
    }

    os.str("");
    os << prefix << "maximum number of iterations without changes: " << maxLastChanged
       << std::endl;
    emit logMessage(QString::fromStdString(os.str()));

    return patchErrors;
}

double CCMCompute::computeError(const cv::Scalar *patchAverages, const ISPC::ModuleCCM &sCCM)
{
    double err = 0.0f;
    for ( int p = 0 ; p < MACBETCH_N ; p++ )
    {
        cv::Scalar expected;
        expected[0] = this->inputs.expectedRGB[p][0];
        expected[1] = this->inputs.expectedRGB[p][1];
        expected[2] = this->inputs.expectedRGB[p][2];
        err += computeError(patchAverages[p], expected, sCCM, this->inputs.patchWeight[p]);
    }
    return err;
}

double CCMCompute::computeError(const cv::Scalar &patchAverage, const cv::Scalar &expectedRGB, const ISPC::ModuleCCM &sCCM, double patchWeigth)
{
    cv::Scalar ccmApplied;
    double thisPatchErr = 0;

    ccmApplied = applyCCM(patchAverage, sCCM);

    for ( int c = 0 ; c < 3 ; c++ )
    {
#if 0 // old error
        thisPatchErr += (patchAvg[c]-this->inputs.expectedRGB[p][c])*(patchAvg[c]-this->inputs.expectedRGB[p][c]);
#else
        thisPatchErr += (expectedRGB[c]-ccmApplied[c])*(expectedRGB[c]-ccmApplied[c]);
#endif
    }
    thisPatchErr = (thisPatchErr*thisPatchErr)*(patchWeigth*patchWeigth*patchWeigth);

    return thisPatchErr;
}

double CCMCompute::computeCIE94(const cv::Scalar &patchAverages, const cv::Scalar &expected, const ISPC::ModuleCCM &sCCM, double patchWeight)
{
    cv::Scalar patchAvg; // need a copy of the average because we apply CCM on it
    cv::Scalar patchAvgLAB; // need a copy of the average because we apply CCM on it
    cv::Scalar expectedLAB;
    double res;
    
    patchAvg = applyCCM(patchAverages, sCCM);
    patchAvgLAB = RGBtoLAB(patchAvg);

    expectedLAB = RGBtoLAB(expected);

    res = CCMCompute::CIE94(expectedLAB, patchAvgLAB);

    return res;
}

double CCMCompute::computeCIE94(const cv::Scalar *patchAverages, const ISPC::ModuleCCM &sCCM)
{
    double err = 0.0f;
    double c;
    for ( int p = 0 ; p < MACBETCH_N ; p++ )
    {
        cv::Scalar expected;
        expected[0] = this->inputs.expectedRGB[p][0];
        expected[1] = this->inputs.expectedRGB[p][1];
        expected[2] = this->inputs.expectedRGB[p][2];
        c = computeCIE94(patchAverages[p], expected, sCCM, this->inputs.patchWeight[p]);
        //err += c*c;
        err += c;
    }
    return err;
}

CVBuffer CCMCompute::applyCCM(const CVBuffer &sInput, const ISPC::ModuleCCM &sCCM)
{
    CVBuffer sResult;
    
    if ( sInput.mosaic != MOSAIC_NONE || sInput.data.empty() )
    {
        return sResult; // CCM is after demosaicer so it's input should be RGB
    }

    sResult = sInput.clone();
    
    std::vector<cv::Mat> channelsO;
    std::vector<cv::Mat> channelsI;

    cv::split(sResult.data, channelsO);
    cv::split(sInput.data, channelsI);

#if defined(TUNE_CCM_DBG)
    {
        cv::Mat rescaled;
        sInput.data.convertTo(rescaled, CV_8U, 1/4.0);
        displayImage(rescaled, "CCM input", 0, 0, rescaled.cols, rescaled.rows);
    }
#endif

    for ( int y = 0 ; y < sInput.data.rows ; y++ )
    {
        for ( int x = 0 ; x < sInput.data.cols ; x++ )
        {
            cv::Scalar inputVal, outputVal;
            
            // RGB stored as BGR
            inputVal[0] = channelsI[0].at<float>(y, x);
            inputVal[1] = channelsI[1].at<float>(y, x);
            inputVal[2] = channelsI[2].at<float>(y, x);

            outputVal = applyCCM(inputVal, sCCM);
            
            channelsO[0].at<float>(y, x) = outputVal[0];
            channelsO[1].at<float>(y, x) = outputVal[1];
            channelsO[2].at<float>(y, x) = outputVal[2];
        }
    }

    cv::merge(channelsO, sResult.data);

#if defined(TUNE_CCM_DBG)
    {
        cv::Mat rescaled;
        sResult.data.convertTo(rescaled, CV_8U, 1/4.0);
        displayImage(rescaled, "CCM output", 0, 0, rescaled.cols, rescaled.rows);
    }
#endif
    
    return sResult;
}

cv::Scalar CCMCompute::applyCCM(const cv::Scalar &sInput, const ISPC::ModuleCCM &sCCM)
{
    cv::Scalar sOutput;
    for ( int c = 0 ; c < 3 ; c++ )
    {
		double v = sInput[0] * sCCM.aMatrix[3*0 + c]
            + sInput[1] * sCCM.aMatrix[3*1 + c] 
            + sInput[2] * sCCM.aMatrix[3*2 + c]
			+ sCCM.aOffset[c];
        sOutput[c] = v; //(IMG_UINT8)std::min(std::max(0, (int)floor(v)), 255);
    }

    return sOutput;
}

cv::Scalar CCMCompute::RGBtoXYZ(const cv::Scalar &inputs, double norm)
{
    ISPC::ModuleCCM sXYZTrans;
    cv::Scalar output;
    // A should be transposed!
	sXYZTrans.aMatrix[0] = 0.4124564;
    sXYZTrans.aMatrix[3] = 0.3575761;
    sXYZTrans.aMatrix[6] = 0.1804375;
    
    sXYZTrans.aMatrix[1] = 0.2126729;
    sXYZTrans.aMatrix[4] = 0.7151522;
    sXYZTrans.aMatrix[7] = 0.0721750;
    
    sXYZTrans.aMatrix[2] = 0.0193339;
    sXYZTrans.aMatrix[5] = 0.1191920;
    sXYZTrans.aMatrix[8] = 0.9503041;

    for ( int c = 0 ; c < 3 ; c++ )
    {
		sXYZTrans.aOffset[c] = 0;
    }

    output = applyCCM(inputs, sXYZTrans);

    for ( int c = 0 ; c < 3 ; c++ )
    {
        output[c] = output[c]/norm; // maximum value normalisation
    }

    return output;
}

cv::Scalar CCMCompute::XYZtoLAB(const cv::Scalar &inputs)
{
    //double reshape[3];
    cv::Scalar reshape, output;
    double k = 903.3;
    double e = 0.008856;
    int c;
    double norm[3] = { 1.0521, 1.00, 0.9184};

    for ( c = 0 ; c < 3 ; c++ )
    {
#if 1
        reshape[c] = inputs[c]*norm[c];
        if ( reshape[c] > e )
        {
#ifdef USE_MATH_NEON
			reshape[c] = (double)powf_neon((float)reshape[c], 1/3.0f);
#else
			reshape[c] = pow(reshape[c], 1/3.0);
#endif
        }
        else
        {
            reshape[c] = (k*reshape[c] +16)/116;
        }
#else
        // old way
        reshape[c] = pow(inputs[c], 1/3.0);
        if ( reshape[c] <= e )
        {
            reshape[c] = (k*reshape[c]+16)/116.0;
        }
#endif
    }

    output[0] = 116*reshape[1]-16; // L
    output[1] = 500*(reshape[0]-reshape[1]);
    output[2] = 200*(reshape[1]-reshape[2]);

    return output;
}

cv::Scalar CCMCompute::RGBtoLAB(const cv::Scalar &inputs)
{
    cv::Scalar output;

    cv::Scalar xyz;

    xyz = RGBtoXYZ(inputs);
    output = XYZtoLAB(xyz);

    return output;
}

double CCMCompute::CIE94(const cv::Scalar &lab1, const cv::Scalar &lab2)
{
    double dEab = 0, dL2 = 0, C1, C2, dCab, dHab, C, Sc, Sh;
    int c;

    for ( c = 0 ; c < 3 ; c++ )
    {
        dEab += (lab2[c]-lab1[c])*(lab2[c]-lab1[c]);
    }
#ifdef USE_MATH_NEON
	dEab = (double)sqrtf_neon((float)dEab);

	dL2 = lab1[0]-lab2[0];
	dL2 = dL2*dL2;
	C1 = (double)sqrtf_neon((float)(lab1[1]*lab1[1] + lab1[2]*lab1[2]));
	C2 = (double)sqrtf_neon((float)(lab2[1]*lab2[1] + lab2[2]*lab2[2]));
	dCab = C1-C2;
	dHab = (double)sqrtf_neon((float)((dEab*dEab) - dL2 - (dCab*dCab)));
	C = (double)sqrtf_neon((float)(C1*C2));
#else
	dEab = sqrt( dEab );

	dL2 = lab1[0]-lab2[0];
	dL2 = dL2*dL2;
	C1 = sqrt(lab1[1]*lab1[1] + lab1[2]*lab1[2]);
	C2 = sqrt(lab2[1]*lab2[1] + lab2[2]*lab2[2]);
	dCab = C1-C2;
	dHab = sqrt( (dEab*dEab) - dL2 - (dCab*dCab) );
	C = sqrt(C1*C2);
#endif
    Sc = 1.0 + 0.045*C;
    Sh = 1.0 + 0.015*C;

#ifdef USE_MATH_NEON
	return (double)sqrtf_neon( (float)(dL2 + (dCab/Sc)*(dCab/Sc) + (dHab/Sh)*(dHab/Sh)) );
#else
	return sqrt( dL2 + (dCab/Sc)*(dCab/Sc) + (dHab/Sh)*(dHab/Sh) );
#endif
}

CCMCompute::CCMCompute(QObject *parent): ComputeBase("CCM: ", parent)
{
}

int CCMCompute::compute(const ccm_input *_inputs)
{
	ISPC::ParameterList configuration;
    std::ostringstream os;
        
    this->inputs = *_inputs;
    this->verbose = this->inputs.bverbose;

    ISPC::ModuleBLC sBLC;
    ISPC::ModuleWBC sWBC; // WBC is part of LSH
    ISPC::ModuleCCM sBestCCM;

    double error = 0;
    time_t startTime = time(NULL);
        
    for ( int i = 0 ; i < 4 ; i++ )
    {
		sWBC.aWBGain[i] = 1.0f; // no gain
		sWBC.aWBClip[i] = 1.0f; // no clipping

		sBLC.aSensorBlack[i] = inputs.sensorBlack[i];
    }
	sBLC.ui32SystemBlack = inputs.systemBlack;
    
	sBLC.save(configuration, ISPC::ModuleBLC::SAVE_VAL);
    sWBC.save(configuration, ISPC::ModuleWBC::SAVE_VAL);
    
    qsrand(time(NULL));

    // run to apply BLC information

    os.str("");
    os << prefix << "using file " << std::string(this->inputs.inputFilename.toLatin1())
       << std::endl;
    

    if ( !this->inputs.bCIE94 )
    {
        os << prefix << "computing without using CIE94 as metic!" << std::endl;
    }

    emit logMessage(QString::fromStdString(os.str()));
    
    // use pWBCBufferRGB as a tmp buffer
    sWBCBufferRGB = CVBuffer::loadFile(this->inputs.inputFilename.toLatin1());
#if defined(TUNE_CCM_DBG)
    {
        CVBuffer dmo = sWBCBufferRGB.demosaic(1/4.0, CV_8U);
        displayImage(dmo.data.clone(), "LOAD_BUFFER", 0, 0, dmo.data.cols, dmo.data.rows);
    }
#endif
    
    sOrigBuffer = applyBLC(sWBCBufferRGB, sBLC);

    CVBuffer *pComputeBuffer = new CVBuffer();
    *pComputeBuffer = sOrigBuffer;

    // no need to rescale to 255 as it will be done in receiver when applying the demosaic
    emit displayBuffer(CCMTuning::ORIG_BUFFER, pComputeBuffer);

    pComputeBuffer = 0; // deleted by the receiver

#if defined(TUNE_CCM_DBG)
    {
        CVBuffer dmo = sOrigBuffer.demosaic(1/4.0, CV_8U);
        displayImage(dmo.data.clone(), "ORIG_BUFFER", 0, 0, dmo.data.cols, dmo.data.rows);
    }
#endif
    
    // 1. compute the WBC cooefs
    computeWBCGains(sOrigBuffer, sWBC);

    sWBC.save(configuration, ISPC::ModuleWBC::SAVE_VAL);

    CVBuffer sWBCBuffer;
    sWBCBuffer = applyWBC(sOrigBuffer, sWBC, sBLC);
        
    // 2. rescale buffer (demosaic and scale colours to the same range than the expected values using black and white patch averages)
    sWBCBufferRGB = rescaleBuffer(sWBCBuffer);
#if defined(TUNE_CCM_DBG)
    {
        CVBuffer dmo = sWBCBuffer.demosaic(1/4.0, CV_8U);
        displayImage(dmo.data, "WBC_RESCALED", 0, 0, dmo.data.cols, dmo.data.rows);
    }
#endif
    
    // 3. compute CCM
    double *patchErrors = computeCCM(sWBCBufferRGB, sBestCCM, sWBC);

    // matrix need to be transposed before saved to file
    ISPC::ModuleCCM tmp = sBestCCM;
    
	sBestCCM.aMatrix[0] = tmp.aMatrix[0];
    sBestCCM.aMatrix[1] = tmp.aMatrix[3];
    sBestCCM.aMatrix[2] = tmp.aMatrix[6];

    sBestCCM.aMatrix[3] = tmp.aMatrix[1];
    sBestCCM.aMatrix[4] = tmp.aMatrix[4];
    sBestCCM.aMatrix[5] = tmp.aMatrix[7];

    sBestCCM.aMatrix[6] = tmp.aMatrix[2];
    sBestCCM.aMatrix[7] = tmp.aMatrix[5];
    sBestCCM.aMatrix[8] = tmp.aMatrix[8];

    for ( int c = 0 ; c < 3 ; c++ )
    {
		sBestCCM.aOffset[c] = tmp.aOffset[c]*2.0; // precision in the file
    }

    sBestCCM.save(configuration, ISPC::ModuleCCM::SAVE_VAL);

    std::vector<std::string> lst;
    lst.push_back(ISPC::ModuleIIF::getMosaicString(sOrigBuffer.mosaic));
    
    //configuration.addParameter(IIF_BAYERFMT, lst);
    //configuration.addGroup(IIF_BAYERFMT, "// format used for calibration");
    
    std::vector<std::string> p;
    p.push_back(std::string(this->inputs.inputFilename.toLatin1()));

	ISPC::Parameter inputImage("//CCM input image", p);
	configuration.addParameter(inputImage);
    
	ISPC::Parameter bayerFormat("//CCM bayer format", lst);
	configuration.addParameter(bayerFormat);
                
	emit finished(this, new ISPC::ParameterList(configuration), patchErrors);

    startTime = time(0)-startTime;

    os.str("");
    os << prefix << this->inputs.niterations << " iterations ran into " << (int)startTime << " sec"
       << std::endl;
    emit logMessage(QString::fromStdString(os.str()));

    return 0;
}

void CCMCompute::randomMatrix(ISPC::ModuleCCM &sCCM)
{
    for ( int i = 0 ; i < 9 ; i++ )
    {
		sCCM.aMatrix[i] = random(this->inputs.matrixMin[i], this->inputs.matrixMax[i]);
    }
    for ( int v = 0 ; v < 3 ; v++ )
    {
#ifdef NO_OFF
        sCCM.aOffset[v] = 0.0f;
#else
		sCCM.aOffset[v] = random(this->inputs.offsetMin[v], this->inputs.offsetMax[v]);
#endif
    }
}

void CCMCompute::normaliseMatrix(ISPC::ModuleCCM &sCCM)
{
    // make sure all the columns add up to 1
    for ( int h = 0 ; h < 3 ; h++ )
    {
        double sum = 0;
        for ( int v = 0 ; v < 3 ; v++ )
        {
			sum += sCCM.aMatrix[v*3 + h];
        }
        sum = (1.0-sum)/3.0; // the modification so that the sum is 1
        for ( int v = 0 ; v < 3 ; v++ )
        {
            sCCM.aMatrix[v*3 + h] += sum;
        }
    }
}

void CCMCompute::printCCM(std::ostream &os, const ISPC::ModuleCCM &sCCM)
{
    os << "Current matrix\n";
    
    for ( int v = 0 ; v < 3 ; v++ )
    {
        for ( int h = 0 ; h < 3 ; h++ )
        {
			os << " " << sCCM.aMatrix[3*v + h];
        }
		os << " + " << sCCM.aOffset[v] << std::endl;
    }
}


