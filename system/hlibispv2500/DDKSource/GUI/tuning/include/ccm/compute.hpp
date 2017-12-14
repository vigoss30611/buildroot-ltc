/**
******************************************************************************
@file ccm/compute.hpp

@brief Compute CCM based on given input in a separate QThread

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
#ifndef CCMCOMPUTE_HPP
#define CCMCOMPUTE_HPP

#include "ccm/tuning.hpp"
#include "computebase.hpp"
#include "buffer.hpp"
#include "ispc/ModuleCCM.h"
#include "ispc/ModuleBLC.h"
#include "ispc/ModuleLSH.h"
#include "ispc/ModuleWBC.h"

// private class instenciated when calling CCMTuning::tuning()
// part of the header to generate the moc
class CCMCompute: public ComputeBase
{
    Q_OBJECT
public:
    // class methods
    static double random(double _min, double _max);
	static cv::Scalar applyCCM(const cv::Scalar &sInput, const ISPC::ModuleCCM &sCCM);
    static cv::Scalar RGBtoXYZ(const cv::Scalar &inputs, double norm=256.0);
    static cv::Scalar XYZtoLAB(const cv::Scalar &inputs);
    static cv::Scalar RGBtoLAB(const cv::Scalar &inputs); // assuming same illumination
    static double CIE94(const cv::Scalar &lab1, const cv::Scalar &lab2);

    static void normaliseMatrix(ISPC::ModuleCCM &sCCM);

    static void printCCM(std::ostream &os, const ISPC::ModuleCCM &sCCM);
    
    //static cv::Scalar gammaCorrect(const cv::Scalar &sInput);
    
protected:
    // not as part of the stack of compute() because they can be used to display on screen
    //BufferDouble *pOrigBuffer;
    //BufferDouble *pWBCBufferRGB;
    CVBuffer sOrigBuffer;
    CVBuffer sWBCBufferRGB;
    bool verbose;

    // object methods
    void randomMatrix(ISPC::ModuleCCM &sCCM);

    CVBuffer applyBLC(const CVBuffer &sBuffer, const ISPC::ModuleBLC &sBLC);
    CVBuffer applyWBC(const CVBuffer &sInput, const ISPC::ModuleWBC &sWBC, const ISPC::ModuleBLC &sBLC);
    
    void computeWBCGains(const CVBuffer &sBuffer, ISPC::ModuleWBC &sWBC);
    CVBuffer rescaleBuffer(const CVBuffer &sInput);
    double* computeCCM(const CVBuffer &sBuffer, ISPC::ModuleCCM &sCCM, const ISPC::ModuleWBC &sWBC);

    static double computeError(const cv::Scalar &patchAverage, const cv::Scalar &expectedRGB, const ISPC::ModuleCCM &sCCM, double patchWeigth);
    double computeError(const cv::Scalar *patchAverages, const ISPC::ModuleCCM &sCCM);

    static double computeCIE94(const cv::Scalar &patchAverage, const cv::Scalar &expectedRGB, const ISPC::ModuleCCM &sCCM, double patchWeight);
    double computeCIE94(const cv::Scalar *patchAverages, const ISPC::ModuleCCM &sCCM);

    CVBuffer applyCCM(const CVBuffer &sBuffer, const ISPC::ModuleCCM &sCCM);
    

public:
    ccm_input inputs;

    CCMCompute(QObject *parent=0);
        
public slots:
    int compute(const ccm_input *inputs);

signals:
    void displayBuffer(int b, CVBuffer *pBuffer);
    void addError(int ie, double error, int ib, double best);
	void finished(CCMCompute *toStop, ISPC::ParameterList *result, double *patchErrors);
};

#endif //CCMCOMPUTE_HPP