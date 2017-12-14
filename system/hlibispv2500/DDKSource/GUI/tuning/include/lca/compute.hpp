/**
******************************************************************************
@file lca/compute.hpp

@brief Compute LCA coeffs

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
#ifndef LCACOMPUTE_HPP
#define LCACOMPUTE_HPP

#include "lca/tuning.hpp"
#include "computebase.hpp"

#include <QtCore/QObject>
#include <opencv2/core/core.hpp>
#include <vector>

//struct to store the difference in position between a block in different planes.
//We also overload some operators to easily calculate averages and other manipulations
struct BlockDiff
{
	double error;
	double dX, dY;
	double x, y;

	BlockDiff(double error, double x, double y, double dX, double dY);

	BlockDiff operator+(const BlockDiff &otherBlock);

	BlockDiff operator-(const BlockDiff &otherBlock);

	BlockDiff operator/(const double& divisor);

	BlockDiff operator*(const double& mult);

	bool operator<(const BlockDiff &other) const;

	bool operator>(const BlockDiff &other) const;
};

struct lca_output
{
    enum CoeffData
    {
        RED_GREEN0,
        RED_GREEN1,
        BLUE_GREEN0,
        BLUE_GREEN1 
    };
    cv::Mat coeffsX[4]; // 3 per channel - CoeffData specifies what channel
    cv::Mat coeffsY[4]; // 3 per channel - CoeffData specifies what channel
    double errorsX[4]; // CoeffData specifies what channel
    double errorsY[4]; // CoeffData specifies what channel
    int shiftX;
    int shiftY;

    // for display
    cv::Mat inputImage;
    std::vector<BlockDiff> blockDiffs[4];
};

#define LCA_N_STEPS (1+4) // load + 1 per channel
#define LCA_MIN_FEATURES 20

class LCACompute: public ComputeBase
{
    Q_OBJECT

protected:
    lca_input inputs;
    int step; // current step

    lca_output* computeLCA(const cv::Mat &inputImage, const std::vector<cv::Rect> &roiVector);

public:
    LCACompute(QObject *parent=0);
    
    //virtual ~LCACompute();

    static cv::Mat filterMatrix(const cv::Mat &inputImage);
    static std::vector<cv::Rect> locateFeatures(int minFeatureSize, int maxFeatureSize, const cv::Mat &inputImage);

public slots:
    int compute(const lca_input *input);

signals:
    void finished(LCACompute *toStop, lca_output *pOutput);
    void currentStep(int step);
};

#endif // LCACOMPUTE_HPP
