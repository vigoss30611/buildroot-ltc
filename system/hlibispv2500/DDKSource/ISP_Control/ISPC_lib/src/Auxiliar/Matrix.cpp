/**
*******************************************************************************
 @file Matrix.cpp

 @brief ISPC::Matrix implementation

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
#include "ispc/Matrix.h"

#include <felixcommon/userlog.h>
#define LOG_TAG "ISPC_MATRIX"

#include <cmath>
#include <vector>
#include <string>
#include <algorithm>
#ifdef USE_MATH_NEON
#include <mneon.h>
#endif

ISPC::Matrix::Matrix(int rows, int cols): rows(rows), cols(cols)
{
    matrixState = NOT_INITIALIZED;

    if (rows < 1 || cols < 1)
    {
        LOG_ERROR("Invalid dimensions for matrix\n");
        matrixState = INVALID;
        return;
    }

    // Resize the matrix to the requested values
    coefficients.resize(rows);
    for (int i = 0 ; i < rows ; i++)
    {
        coefficients[i].resize(cols);
    }

    // initialize the matrix
    zeros();

    matrixState = VALID;
}

ISPC::Matrix::Matrix(int rows, int cols, const double *pValues)
    : rows(rows), cols(cols)
{
    matrixState = NOT_INITIALIZED;

    if (rows < 1 || cols < 1)
    {
        LOG_ERROR("Invalid dimensions for matrix\n");
        matrixState = INVALID;
        return;
    }

    // Resize the matrix to the requested values
    coefficients.resize(rows);
    for (int i = 0; i < rows; i++)
    {
        coefficients[i].resize(cols);
    }

    // initialize the matrix
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            coefficients[i][j] = pValues[i * cols + j];
        }
    }

    matrixState = VALID;
}

int ISPC::Matrix::numRows() const
{
    return rows;
}

int ISPC::Matrix::numCols() const
{
    return cols;
}

std::vector<double> & ISPC::Matrix::operator[](int index)
{
    if (index < rows)
    {
        return coefficients[index];
    }
    LOG_ERROR("invalid row!!! return last row\n");
    return coefficients[rows-1];
}

const std::vector<double> & ISPC::Matrix::operator[](int index) const
{
    if (index < rows)
    {
        return coefficients[index];
    }
    LOG_ERROR("invalid row!!! return last row\n");
    return coefficients[rows-1];
}

ISPC::Matrix ISPC::Matrix::operator+(const Matrix &otherMatrix) const
{
    Matrix resultMatrix(rows, cols);

    if (VALID != matrixState || VALID != otherMatrix.matrixState)
    {
        LOG_ERROR("Invalid matrix state\n");
        return resultMatrix;
    }

    if (otherMatrix.numRows() != rows || otherMatrix.numCols() != cols)
    {
        LOG_ERROR("unable to add matrices with different dimensions "\
            "(%dx%d) and (%dx%d)\n",
            rows, cols, otherMatrix.numRows(), otherMatrix.numCols());
        return resultMatrix;
    }

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            resultMatrix[i][j] = coefficients[i][j] + otherMatrix[i][j];
        }
    }
    return resultMatrix;
}

ISPC::Matrix ISPC::Matrix::operator+(double value) const
{
    Matrix resultMatrix(rows, cols);

    if (VALID != matrixState)
    {
        LOG_ERROR("Invalid matrix state\n");
        return resultMatrix;
    }

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            resultMatrix[i][j] = coefficients[i][j] + value;
        }
    }
    return resultMatrix;
}

ISPC::Matrix ISPC::Matrix::operator-(const Matrix &otherMatrix) const
{
    Matrix resultMatrix(rows, cols);

    if (VALID != matrixState || VALID != otherMatrix.matrixState)
    {
        LOG_ERROR("Invalid matrix state\n");
        return resultMatrix;
    }

    if (otherMatrix.numRows() != rows || otherMatrix.numCols() != cols)
    {
        LOG_ERROR("unable to add matrices with different dimensions "\
            "(%dx%d) and (%dx%d)\n",
            rows, cols, otherMatrix.numRows(), otherMatrix.numCols());
        return resultMatrix;
    }

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            resultMatrix[i][j] = coefficients[i][j] - otherMatrix[i][j];
        }
    }
    return resultMatrix;
}

ISPC::Matrix ISPC::Matrix::operator*(double value) const
{
    Matrix resultMatrix(rows, cols);

    if (VALID != matrixState)
    {
        LOG_ERROR("Invalid matrix state\n");
        return resultMatrix;
    }

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            resultMatrix[i][j] = coefficients[i][j] * value;
        }
    }
    return resultMatrix;
}

ISPC::Matrix ISPC::Matrix::operator/(double value) const
{
    Matrix resultMatrix(rows, cols);

    if (VALID != matrixState)
    {
        LOG_ERROR("Invalid matrix state\n");
        return resultMatrix;
    }

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            resultMatrix[i][j] = coefficients[i][j] / value;
        }
    }
    return resultMatrix;
}

ISPC::Matrix ISPC::Matrix::power(double value) const
{
    Matrix result(rows, cols);

    if (VALID != matrixState)
    {
        LOG_ERROR("Invalid matrix state\n");
        return result;
    }

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
#ifdef USE_MATH_NEON
			result[i][j] = (double)powf_neon((float)(*this)[i][j], (float)value);
#else
			result[i][j] = pow((*this)[i][j], value);
#endif
        }
    }
    return result;
}

ISPC::Matrix ISPC::Matrix::inv() const
{
    Matrix resultMatrix(rows, cols);

    if (VALID != matrixState)
    {
        LOG_ERROR("Invalid matrix state\n");
        return resultMatrix;
    }

    if (3 != cols || 3 != rows)
    {
        LOG_ERROR("Inverse matrix only implemented for 3x3 matrices "\
            "(current matrix %dx%d)\n", rows, cols);
        return resultMatrix;
    }

    double determinant = 0.0
        + coefficients[0][0] * (coefficients[1][1] * coefficients[2][2]
        - coefficients[2][1] * coefficients[1][2])
        - coefficients[0][1] * (coefficients[1][0] * coefficients[2][2]
        - coefficients[1][2] * coefficients[2][0])
        + coefficients[0][2] * (coefficients[1][0] * coefficients[2][1]
        - coefficients[1][1] * coefficients[2][0]);

    if (0.0 == determinant)
    {
        LOG_ERROR("Determinant == 0, no inverse can be computed.\n");
        return resultMatrix;
    }

    double invdet = 1.0/determinant;
    resultMatrix[0][0] = +(coefficients[1][1] * coefficients[2][2]
        - coefficients[2][1] * coefficients[1][2]) * invdet;
    resultMatrix[1][0] = -(coefficients[0][1] * coefficients[2][2]
        - coefficients[0][2] * coefficients[2][1]) * invdet;
    resultMatrix[2][0] = +(coefficients[0][1] * coefficients[1][2]
        - coefficients[0][2] * coefficients[1][1]) * invdet;
    resultMatrix[0][1] = -(coefficients[1][0] * coefficients[2][2]
        - coefficients[1][2] * coefficients[2][0]) * invdet;
    resultMatrix[1][1] = +(coefficients[0][0] * coefficients[2][2]
        - coefficients[0][2] * coefficients[2][0]) * invdet;
    resultMatrix[2][1] = -(coefficients[0][0] * coefficients[1][2]
        - coefficients[1][0] * coefficients[0][2]) * invdet;
    resultMatrix[0][2] = +(coefficients[1][0] * coefficients[2][1]
        - coefficients[2][0] * coefficients[1][1]) * invdet;
    resultMatrix[1][2] = -(coefficients[0][0] * coefficients[2][1]
        - coefficients[2][0] * coefficients[0][1]) * invdet;
    resultMatrix[2][2] = +(coefficients[0][0] * coefficients[1][1]
        - coefficients[1][0] * coefficients[0][1]) * invdet;

    return resultMatrix;
}

ISPC::Matrix ISPC::Matrix::blend(const Matrix &otherMatrix,
    double blending) const
{
    Matrix resultMatrix(rows, cols);

    if (0.0 > blending || 1.0 < blending)
    {
        LOG_ERROR("Blending value must be between 0.0 and 1.0 "\
            "(received:%f)\n", blending);
        return resultMatrix;
    }

    if (VALID != matrixState || VALID != otherMatrix.matrixState)
    {
        LOG_ERROR("Invalid matrix state\n");
        return resultMatrix;
    }

    if (otherMatrix.numRows() != rows || otherMatrix.numCols() != cols)
    {
        LOG_ERROR("Unable to add matrices with different dimensions "\
            "(%dx%d) and (%dx%d)\n",
            rows, cols, otherMatrix.numRows(), otherMatrix.numCols());
        return resultMatrix;
    }

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            resultMatrix[i][j] = coefficients[i][j] * (1.0 - blending)
                + otherMatrix[i][j] * blending;
        }
    }
    return resultMatrix;
}

void ISPC::Matrix::zeros()
{
    // not checking for valid because it can be called while uninitialised
    if (INVALID == matrixState)
    {
        LOG_ERROR("Invalid matrix state\n");
        return;
    }

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            coefficients[i][j] = 0.0;
        }
    }
}

void ISPC::Matrix::ones()
{
    // not checking for valid because it can be called while uninitialised
    if (INVALID == matrixState)
    {
        LOG_ERROR("Invalid matrix state\n");
        return;
    }

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            coefficients[i][j] = 1.0;
        }
    }
}

void ISPC::Matrix::identity()
{
    // not checking for valid because it can be called while uninitialised
    if (INVALID == matrixState)
    {
        LOG_ERROR("Invalid matrix state\n");
        return;
    }

    if (cols != rows)
    {
        LOG_WARNING("Matrix is not square.\n");
    }

    int dim = std::min(cols, rows);

    for (int i = 0; i < dim; i++)
    {
        coefficients[i][i] = 1.0;
    }
}

ISPC::Matrix& ISPC::Matrix::normaliseSum()
{
    double totalValue = this->sum();

    if (0.0 != totalValue)
    {
        for (int i = 0; i < rows; i++)
        {
            for (int j = 0; j < cols; j++)
            {
                coefficients[i][j] = coefficients[i][j] / totalValue;
            }
        }
    }
    return *this;
}

ISPC::Matrix& ISPC::Matrix::normaliseMax()
{
    double maxValue = this->max();

    if (0.0 != maxValue)
    {
        for (int i = 0; i < rows; i++)
        {
            for (int j = 0; j < cols; j++)
            {
                coefficients[i][j] = coefficients[i][j] / maxValue;
            }
        }
    }
    return *this;
}

ISPC::Matrix& ISPC::Matrix::applyMin(double value)
{
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            coefficients[i][j] = std::min(coefficients[i][j], value);
        }
    }
    return *this;
}

ISPC::Matrix& ISPC::Matrix::applyMax(double value)
{
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            coefficients[i][j] = std::max(coefficients[i][j], value);
        }
    }
    return *this;
}

double ISPC::Matrix::sum() const
{
    double totalValue = 0.0;

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            totalValue += coefficients[i][j];
        }
    }
    return totalValue;
}

double ISPC::Matrix::max() const
{
    double maxValue = coefficients[0][0];

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            maxValue = std::max(maxValue, coefficients[i][j]);
        }
    }
    return maxValue;
}

double ISPC::Matrix::min() const
{
    double minValue = coefficients[0][0];

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            minValue = std::min(minValue, coefficients[i][j]);
        }
    }
    return minValue;
}

void ISPC::Matrix::plotAsHistogram(const ISPC::Matrix &histogram,
    std::ostream &os, char symbol)
{
    // Android log adds \n after every log message, so we trick it
    std::string line;
    int nLevels = 14;
    int i;

    os << "Histogram:" << std::endl;

    for (i = 0; i <= nLevels; i++)
    {
        line = " ";
        for (int j = 0; j < histogram.numCols(); j++)
        {
            // if (histogram[0][j] > ((double)1.0 / double(pow(1 + i, 1.5))))
            if (histogram[0][j] >
                (1.0 - static_cast<double>(i * (1.0 / nLevels))))
            {
                if (0 == i)
                {
                    line += "#";
                }
                else
                {
                    line += std::string(1, symbol);
                }
            }
            else
            {
                line += " ";
            }
        }
        os << line << std::endl;
    }
    line.erase();
    line += "<";
    for (i = 0; i <= histogram.numCols(); i++)
    {
        if (0 == i%8)
        {
            line += "|";
        }
        else
        {
            line += "_";
        }
    }
    line += ">";
    os << line << std::endl;
}

std::ostream& operator<<(std::ostream &os, const ISPC::Matrix &m)
{
    for (int i = 0; i < m.numRows(); i++ )
    {
        for (int j = 0; j < m.numCols(); j++)
        {
            os << " " << m[i][j];
        }
        os << std::endl;
    }
    return os;
}
