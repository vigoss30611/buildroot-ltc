/**
******************************************************************************
@file lcacompute.cpp

@brief LCACompute implementation

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

#include "lca/compute.hpp"

#include "imageUtils.h"

#include <ctime>
#include <cmath>
#include <queue>
#ifdef USE_MATH_NEON
#include <mneon.h>
#endif
/*
 * BlockDiff
 */


BlockDiff::BlockDiff(double error, double x, double y, double dX, double dY): error(error), x(x), y(y), dX(dX), dY(dY)
{
}

BlockDiff BlockDiff::operator+(const BlockDiff &otherBlock)
{
	BlockDiff result(this->error+otherBlock.error, this->x + otherBlock.x, this->y + otherBlock.y, this->dX+otherBlock.dX, this->dY+otherBlock.dY);
	return result;
}

BlockDiff BlockDiff::operator-(const BlockDiff &otherBlock)
{
	BlockDiff result(this->error-otherBlock.error, this->x - otherBlock.x, this->y - otherBlock.y, this->dX-otherBlock.dX, this->dY-otherBlock.dY);
	return result;
}

BlockDiff BlockDiff::operator/(const double& divisor)
{
	BlockDiff result(this->error/divisor, this->x/divisor, this->y/divisor, this->dX/divisor, this->dY/divisor);
	return result;
}

BlockDiff BlockDiff::operator*(const double& mult)
{
	BlockDiff result(this->error*mult, this->x*mult, this->y*mult, this->dX*mult, this->dY*mult);
	return result;
}

bool BlockDiff::operator<(const BlockDiff &other) const
{
	return this->error>other.error;
}

bool BlockDiff::operator>(const BlockDiff &other) const
{
	return this->error<other.error;
}

/*
 * local static
 */

//binarize all planes of an image
static cv::Mat binarizeChannel(cv::Mat &channel, double offset=10)
{
	double min, max;
	cv::minMaxIdx(channel,&min,&max);
	
	channel = (channel-min)/(max-min)*255.0;
	channel.convertTo(channel,CV_8U);

	cv::Mat binaryChannel;

	cv::adaptiveThreshold(channel,binaryChannel,255,CV_ADAPTIVE_THRESH_MEAN_C,CV_THRESH_BINARY_INV,25,20);
	return binaryChannel;
}

//binarize all planes of an image
static cv::Mat binarizeImage(const cv::Mat &image, double offset)
{

	cv::Mat channels[4];
	cv::split(image,channels);

	cv::Mat binaryChannels[4];

	binaryChannels[0] = binarizeChannel(channels[0],offset);
	binaryChannels[1] = binarizeChannel(channels[1],offset);
	binaryChannels[2] = binarizeChannel(channels[2],offset);
	binaryChannels[3] = binarizeChannel(channels[3],offset);

	cv::Mat binaryImage;
	cv::merge(binaryChannels,4,binaryImage);

	return binaryImage;

}

//return a 2d filter kernel
static cv::Mat createFilterKernel()
{

//float fKernel[7][7] = {			{ 0.00, 0.00, 0.02, 0.04, 0.02, 0.00, 0.00},
//								{ 0.00, 0.00, 0.02, 0.04, 0.02, 0.00, 0.00},
//								{ 0.02, 0.02, 0.02, 0.02, 0.02, 0.02, 0.02},
//								{ 0.04, 0.04, 0.04, 0.04, 0.04, 0.04, 0.04},
//								{ 0.02, 0.02, 0.02, 0.04, 0.02, 0.02, 0.02},
//								{ 0.00, 0.00, 0.02, 0.04, 0.02, 0.00, 0.00},
//								{ 0.00, 0.00, 0.02, 0.04, 0.02, 0.00, 0.00},
//					};

float fKernel[7][7] = {			{ 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f},
								{ 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f},
								{ 0.00f, 0.00f, 0.05f, 0.10f, 0.05f, 0.00f, 0.00f},
								{ 0.00f, 0.00f, 0.10f, 0.40f, 0.10f, 0.00f, 0.00f},
								{ 0.00f, 0.00f, 0.05f, 0.10f, 0.05f, 0.00f, 0.00f},
								{ 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f},
								{ 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f, 0.00f}
						};

	cv::Mat kernel = cv::Mat(7,7,CV_32FC1,fKernel);

	return kernel.clone();
}

//normalize all planes of an image between its min and max values
static void normalizeImage(cv::Mat &image)
{

	double min, max;
	cv::Mat channels[4];
	cv::split(image,channels);
	
	cv::minMaxIdx(channels[0],&min,&max);
	channels[0] = (channels[0]-min)/(max-min);
	
	cv::minMaxIdx(channels[1],&min,&max);
	channels[1] = (channels[1]-min)/(max-min);
	
	cv::minMaxIdx(channels[2],&min,&max);
	channels[2] = (channels[2]-min)/(max-min);

	cv::minMaxIdx(channels[3],&min,&max);
	channels[3] = (channels[3]-min)/(max-min);

	cv::merge(channels,4,image);

}

//apply filter to enhance centre of crosses in the calibration image
static cv::Mat filterImage(cv::Mat &image)
{
	cv::Mat crossKernel = createFilterKernel();
	cv::Mat filteredImage;
	cv::filter2D(image,filteredImage,-1,crossKernel);

	return filteredImage;
}

//iterative filling of a given region until we flood it completely
static void fillROI(cv::Mat &img, int row, int col, std::vector<cv::Point2d> &region)
{
	for(int dRow=-1; dRow<=1;dRow++)
	{
		for(int dCol=-1;dCol<=1;dCol++)
		{
			if(row+dRow<0 || col+dCol<0 || row+dRow>= img.rows|| col+dCol>= img.cols)
				continue;

			if(img.at<unsigned char>(row+dRow,col+dCol)==255)
			{
				img.at<unsigned char>(row+dRow,col+dCol) = 0;

				region.push_back(cv::Point(row+dRow,col+dCol));
				fillROI(img, row+dRow, col+dCol, region);
			}
		}
	}
}

//get the min and max x and y pixel values of a ROI
static void getCoords(const std::vector<cv::Point2d> &roi, int &minRow, int &minCol, int &maxRow, int &maxCol)
{
	minRow = (int) roi[0].x;
	minCol = (int) roi[0].y;
	maxRow = (int) roi[0].x;
	maxCol = (int) roi[0].y;
	
	for(std::vector<cv::Point2d>::const_iterator it = roi.begin(); it!=roi.end();it++)
	{
		if(it->x<minRow)
			minRow=(int)it->x;
		
		if(it->y<minCol)
			minCol=(int)it->y;
		
		if(it->x>maxRow)
			maxRow=(int)it->x;

		if(it->y>maxCol)
			maxCol=(int)it->y;
	}
}

//Get region of interest by finding connected areas in the image with a certain pixel value.
//Once a value is find all the connected pixels of the region are stored by flood fill
static std::vector<cv::Rect> getChannelROIs(cv::Mat &channel, int margin, int minSize, int maxSize, double maxRatio)
{
	std::vector<cv::Rect> ROIVector;

	cv::Mat channelCopy = channel.clone();

	for(int row=margin; row<channelCopy.rows-margin; row++)
	{
		for(int col=margin; col<channelCopy.cols-margin; col++)
		{
			if(channelCopy.at<unsigned char>(row, col) == 255)
			{
				std::vector<cv::Point2d> roi;
				fillROI(channelCopy, row,col, roi);

				int minRow, minCol, maxRow, maxCol;
				getCoords(roi, minRow, minCol, maxRow, maxCol);


				//calculate how 'filled' the found feature is
				double fillRatio = double(roi.size())/((maxRow-minRow+1)*(maxCol-minCol+1));
				
				//filter too small, to big or too 'filled' ROIs (too filled ROIs are not crosses
				int width = maxRow-minRow+1;
				int height = maxCol-minCol+1;
				
				if(minSize!= -1 && (width < minSize || height < minSize))
					continue;
		
				if(maxSize!= -1 && (width > maxSize|| height > maxSize))
					continue;

				if(fillRatio>maxRatio)
					continue;
				

				ROIVector.push_back(cv::Rect(minCol,minRow, maxCol-minCol+1, maxRow-minRow+1));
			}
		}
	}

	return ROIVector;
}

//given a list of region find the region displacement with respect of other image plane with an exhaustive search in 
// the given area
static std::vector<BlockDiff> getWBlockDiffs(const std::vector<cv::Rect> &ROIList, cv::Mat imA, cv::Mat imB, int rX, int rY)
{

	std::vector<BlockDiff> diffVectors;
	
	//iterate through all the elements in the ROI list
	for(std::vector<cv::Rect>::const_iterator it = ROIList.begin(); it != ROIList.end(); it++)
	{
		std::priority_queue<BlockDiff,std::vector<BlockDiff> > pQueue;
		double totalError = 0;
		double bDX=0, bDY=0;

		//Try all the combinations of position in the range -rX to rX and -rY to rY
		for(int dX = -rX; dX < rX; dX++)
		{

			for(int dY = -rY; dY < rY; dY++)
			{

				if(it->x<=0 || it->y <=0 || (it->x+it->width)>=imA.cols || (it->y+it->height)>=imB.rows)
					continue;
				cv::Mat r1 = imA(*it);
				cv::Rect searchRect(it->x+dX,it->y+dY, it->width, it->height);
				
				if(searchRect.x<=0 || searchRect.y <=0 || (searchRect.x+searchRect.width)>=imA.cols || (searchRect.y+searchRect.height)>=imB.rows)
					continue;

				cv::Mat r2 = imB(searchRect);
				cv::Mat areaDiff = (r1-r2);
				cv::Mat powError;
				cv::pow(areaDiff,2,powError);
				double error = cv::mean(powError)(0);
				

				//store the difference in a priority queue so we can later easily get the best values
				pQueue.push(BlockDiff(error*error,it->x+(it->width)/2, it->y+(it->height)/2,dX,dY));
			}
		}

		if(pQueue.size()<1)	//no matches for the block have been found
			continue;

		//Store in the return list the best ROI position (or a combination of the n best positions)
		diffVectors.push_back(pQueue.top());	
		//pQueue.pop();
		//diffVectors.push_back(pQueue.top());	
	}


	return diffVectors;
}

//iterative grid search of the optimal coefficients between a given range.
static cv::Mat coefficientSearch(cv::Mat samples, cv::Mat targets, double minCoeff[3], double maxCoeff[3], double steps, int iterations)
{
	
	double minError = 999999999999;
	cv::Mat bestCoeffs;

	double currentMin[3];
	currentMin[0]=minCoeff[0];
	currentMin[1]=minCoeff[1];
	currentMin[2]=minCoeff[2];
	double currentMax[3];
	currentMax[0]=maxCoeff[0];
	currentMax[1]=maxCoeff[1];
	currentMax[2]=maxCoeff[2];

		

	for(int iter =0; iter<iterations;iter++)
	{
		double dC1 = (currentMax[0]-currentMin[0])/steps;
		double dC2 = (currentMax[1]-currentMin[1])/steps;
		double dC3 = (currentMax[2]-currentMin[2])/steps;
		for(double c1 = currentMin[0]; c1 <=currentMax[0]; c1 += dC1)
		{
			for(double c2 = currentMin[1]; c2 <=currentMax[1]; c2 += dC2)
			{
				for(double c3 = currentMin[2]; c3 <=currentMax[2]; c3 += dC3)
				{
					cv::Mat coeffs(3,1,CV_64F);
					coeffs.at<double>(0)=c1;
					coeffs.at<double>(1)=c2;
					coeffs.at<double>(2)=c3;

					cv::Mat prediction = samples*coeffs;
					cv::Mat errorVals = prediction-targets;
					cv::Mat sqErrorVals;
					cv::pow(errorVals,2,sqErrorVals);
					double currentError = cv::mean(sqErrorVals)(0);
					if(currentError<minError)
					{
						minError = currentError;
						bestCoeffs = coeffs.clone();
					}
				}
			}
		}



		currentMin[0] = M_MAX(minCoeff[0], bestCoeffs.at<double>(0)-dC1*steps/4);
		currentMin[1] = M_MAX(minCoeff[1], bestCoeffs.at<double>(1)-dC1*steps/4);
		currentMin[2] = M_MAX(minCoeff[2], bestCoeffs.at<double>(2)-dC1*steps/4);

		currentMax[0] = M_MIN(maxCoeff[0], bestCoeffs.at<double>(0)+dC1*steps/4);
		currentMax[1] = M_MIN(maxCoeff[1], bestCoeffs.at<double>(1)+dC1*steps/4);
		currentMax[2] = M_MIN(maxCoeff[2], bestCoeffs.at<double>(2)+dC1*steps/4);
	}

	return bestCoeffs;
}

//generate the samples from the set of ROIs detected in the image and corresponding displacements.
//Calculate the optimal parameters for the LCA correction polynomial
static void calculateLCACoefficients(const std::vector<BlockDiff> &blockDiffVector, int centerX, int centerY, double scaleX, double scaleY, cv::Mat &coefficientsX, cv::Mat &coefficientsY, double &errorX, double &errorY)
{
	cv::Mat samplesX,samplesY;
	cv::Mat targetsX, targetsY;

	for(std::vector<BlockDiff>::const_iterator it = blockDiffVector.begin(); it !=blockDiffVector.end(); it++)
	{
		double distanceX, distanceY; 

		distanceX = (it->x-centerX)*scaleX;
		distanceY = (it->y-centerY)*scaleY;

		cv::Mat sampleX;
		if(distanceX>=0)
		{
			sampleX.push_back(distanceX);
			sampleX.push_back(distanceX*distanceX);
			sampleX.push_back(distanceX*distanceX*distanceX);
			sampleX = sampleX.t();
			samplesX.push_back(sampleX);
			targetsX.push_back(it->dX);
		}
		else
		{
			distanceX = -distanceX;
			sampleX.push_back(distanceX);
			sampleX.push_back(distanceX*distanceX);
			sampleX.push_back(distanceX*distanceX*distanceX);
			sampleX = sampleX.t();
			samplesX.push_back(sampleX);
			targetsX.push_back(-it->dX);
		}


		cv::Mat sampleY;
		if(distanceY>=0)
		{
			sampleY.push_back(distanceY);
			sampleY.push_back(distanceY*distanceY);
			sampleY.push_back(distanceY*distanceY*distanceY);
			sampleY = sampleY.t();
			samplesY.push_back(sampleY);
			targetsY.push_back(it->dY);
		}
		else
		{	distanceY = -distanceY;
			sampleY.push_back(distanceY);
			sampleY.push_back(distanceY*distanceY);
			sampleY.push_back(distanceY*distanceY*distanceY);
			sampleY = sampleY.t();
			samplesY.push_back(sampleY);
			targetsY.push_back(-it->dY);
		}


	}

	//compute solution using opencv least squares
//	cv::solve(samplesX, targetsX,coefficientsX,cv::DECOMP_NORMAL);
//	cv::solve(samplesY, targetsY,coefficientsY,cv::DECOMP_NORMAL);


	//.. or compute solution with grid search in a constrained range
	double minCoeff[3] = {-15.99,-15.99,-15.99};
	double maxCoeff[3] = { 15.99, 15.99, 15.99};
	coefficientsX = coefficientSearch(samplesX, targetsX, minCoeff, maxCoeff,16,10);
	coefficientsY = coefficientSearch(samplesY, targetsY, minCoeff, maxCoeff,16,10);


	//calculate correction error
	cv::Mat predictionX = samplesX*coefficientsX;
	cv::Mat predictionY = samplesY*coefficientsY;

	cv::Mat errorMatX = predictionX-targetsX;
	cv::Mat errorMatY = predictionY-targetsY;
	cv::Mat sqEX, sqEY;
	cv::pow(errorMatX,2,sqEX);
	cv::pow(errorMatY,2,sqEY);

	errorX = cv::mean(sqEX)(0);
	errorY = cv::mean(sqEY)(0);
}

/*
 * public static
 */


// filter the input image (binarize + smooth)
cv::Mat LCACompute::filterMatrix(const cv::Mat &inputImage)
{
    cv::Mat binaryImage = binarizeImage(inputImage,20);

    cv::Mat binaryChannels[4];
	cv::split(binaryImage,binaryChannels);

    //Apply filter to the binary image to smooth the binarized features
	binaryImage.convertTo(binaryImage, CV_32F);
	cv::Mat filtered = filterImage(binaryImage);
	normalizeImage(filtered);

    return filtered;
}

std::vector<cv::Rect> LCACompute::locateFeatures(int minFeatureSize, int maxFeatureSize, const cv::Mat &inputImage)
{
    cv::Mat filtered = filterMatrix(inputImage);
	cv::Mat planes[4];
	cv::split(filtered,planes);

    //get positions for cross centers thresholding the filtered image
	
	cv::Mat binaryOutput[4];
	for(int i=0;i<4;i++)
	{
		planes[i].convertTo(planes[i],CV_8U,255);
		cv::adaptiveThreshold(planes[i],binaryOutput[i],255,CV_ADAPTIVE_THRESH_MEAN_C,CV_THRESH_BINARY,31,-50);
	}

    //generate the coordinates for search blocks around the located centres
	std::vector<cv::Rect> roiVector = getChannelROIs(binaryOutput[0], 20, minFeatureSize, maxFeatureSize, 0.5);

    return roiVector;
}

/*
 * protected
 */

lca_output* LCACompute::computeLCA(const cv::Mat &inputImage, const std::vector<cv::Rect> &roiVector)
{
    std::ostringstream os;

    cv::Mat filtered = filterMatrix(inputImage);
    cv::Mat lcaImagePlanes[4];
	cv::split(filtered,lcaImagePlanes);
	cv::Mat searchImages[4];
    double min,max;
    lca_output *result = new lca_output();

	for(int i=0;i<4;i++)
	{
		cv::minMaxIdx(lcaImagePlanes[i],&min,&max);
		searchImages[i] = (lcaImagePlanes[i]-min)/(max-min);
	}

	result->blockDiffs[0] = getWBlockDiffs(roiVector, searchImages[1], searchImages[0], 7,7);
	result->blockDiffs[1] = getWBlockDiffs(roiVector, searchImages[2], searchImages[0], 7,7);
	result->blockDiffs[2] = getWBlockDiffs(roiVector, searchImages[1], searchImages[3], 7,7);
	result->blockDiffs[3] = getWBlockDiffs(roiVector, searchImages[2], searchImages[3], 7,7);


	//find image centre if not specified
	if(inputs.centerX==-1)
		inputs.centerX = int(inputImage.cols/2);
	if(inputs.centerY==-1)
		inputs.centerY = int(inputImage.rows/2);
        
    int maxPositionX = std::max(inputs.centerX, inputImage.cols-inputs.centerX);
	int maxPositionY = std::max(inputs.centerY, inputImage.rows-inputs.centerY);

    os.str("");
    os << prefix << "image centre: "<<inputs.centerX <<", "<< inputs.centerY << std::endl;
    os << prefix << "max positions: " << maxPositionX << ", " << maxPositionY << std::endl;
    emit logMessage(QString::fromStdString(os.str()));

    //calculate scaling factor for rows and cols
#ifdef USE_MATH_NEON
	double logRows = (double)logf_neon((float)maxPositionY)/(double)logf_neon(2.0f);
	double logCols = (double)logf_neon((float)maxPositionX)/(double)logf_neon(2.0f);
#else
	double logRows = log((double)maxPositionY)/log(2.0);
	double logCols = log((double)maxPositionX)/log(2.0);
#endif
	double rowShifting = 10-ceil(logRows);
	double colShifting = 10-ceil(logCols);

	rowShifting = std::min(0.0,rowShifting);
	colShifting = std::min(0.0,colShifting);

	//for plotting
#ifdef USE_MATH_NEON
	double rowScale = (double)powf_neon(2.0f, (float)rowShifting)/1024.0*2;
	double colScale = (double)powf_neon(2.0f, (float)colShifting)/1024.0*2;
#else
	double rowScale = pow(2,rowShifting)/1024.0*2;
	double colScale = pow(2,colShifting)/1024.0*2;
#endif
    result->shiftX = abs(rowScale);
    result->shiftY = abs(colScale);
    
    for ( int c = 0 ; c < 4 ; c++ )
    {
        calculateLCACoefficients(result->blockDiffs[c], inputs.centerX, inputs.centerY, colScale, rowScale, result->coeffsX[c], result->coeffsY[c], result->errorsX[c], result->errorsY[c]);

        emit currentStep(++step);
        os.str("");
        os << prefix << "channel " << c << " completed" << std::endl;
        emit logMessage(QString::fromStdString(os.str()));
    }

    return result;
}

/*
 * public
 */

LCACompute::LCACompute(QObject *parent): ComputeBase("LCA: ", parent)
{
}

/*
 * public slots
 */

int LCACompute::compute(const lca_input *input)
{
    std::ostringstream os;
    this->inputs = *input;

    step = 0;
    time_t startTime = time(0);

    cv::Mat inputImage;
	try 
    {
		inputImage = loadBAYERFlxFile(inputs.imageFilename);
	}
	catch (std::exception e)
	{
        os.str("");
        os << prefix << "could not load FLX input image: " << e.what() << std::endl;
        emit logMessage(QString::fromStdString(os.str()));

        emit finished(this, 0);
		return EXIT_FAILURE;
	}

    if(inputs.roiVector.size()<LCA_MIN_FEATURES)
	{
        os.str("");
        os << prefix << "not enough features found to compute LCA (" << inputs.roiVector.size() << " " << LCA_MIN_FEATURES << " needed)" << std::endl;
        emit logMessage(QString::fromStdString(os.str()));

        emit finished(this, 0);
		return EXIT_FAILURE;;
	}
    
    os.str("");
    os << prefix << "using file " << inputs.imageFilename << std::endl;
    emit logMessage(QString::fromStdString(os.str()));

    emit currentStep(++step);

    lca_output *result = computeLCA(inputImage, inputs.roiVector);

    result->inputImage = inputImage;

    startTime = time(0)-startTime;

    os.str("");
    os << prefix << "ran into " << (int)startTime << " sec" << std::endl;
    emit logMessage(QString::fromStdString(os.str()));

    emit finished(this, result);
    return EXIT_SUCCESS;
}
