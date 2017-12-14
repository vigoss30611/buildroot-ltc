/**
*******************************************************************************
 @file Matrix.h

 @brief ISPC::Matrix declaration

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
#ifndef ISPC_AUX_MATRIX_H_
#define ISPC_AUX_MATRIX_H_

#include <vector>
#include <iostream>

namespace ISPC {

/**
 * @brief Class implementing a matrix and support for some mathematical
 * operations, initialization, interpolation, etc.
 */
class Matrix
{
public:
    enum state {INVALID, NOT_INITIALIZED, VALID};

private:
    /** @brief Matrix elements, a vector of vectors of double values */
    std::vector<std::vector<double> > coefficients;
    /** @brief Matrix dimensions */
    int rows;
    /** @brief Matrix dimensions */
    int cols;
    /** @brief Matrix state(if it's valid or not) */
    state matrixState;

public:
    /**
     * @brief Construct a matrix with no initialization values and the
     * specified number of rows and cols
     *
     * @param rows Number of rows to be allocated
     * @param cols Number of columns to be allocated
     */
    Matrix(int rows, int cols);

    /**
     * @brief Construct a matrix with values initialized from the
     * provided array and the specified number of rows and cols
     *
     * @param rows Number of rows to be allocated
     * @param cols Number of columns to be allocated
     * @param pValues pointer to an array of rows*cols values to be 
     * used to initialize the matrix
     */
    Matrix(int rows, int cols, const double *pValues);

    /**
     * @brief return the number of rows in the matrix
     * @return number of rows
     */
    int numRows() const;

    /**
     * @brief return the number of columns in the matrix
     * @return number of columns in the image
     */
    int numCols() const;

    /** @brief operator for indexing the matrix */
    std::vector<double> & operator[](int index);

    /** @brief operator for indexing the matrix */
    const std::vector<double> & operator[](int index) const;

    /** @brief operator for adding two matrices */
    Matrix operator+(const Matrix &otherMatrix) const;

    /** @brief operator for adding a matrix and a scalar value */
    Matrix operator+(double value) const;

    /** @brief operator for substracting to matrices */
    Matrix operator-(const Matrix &otherMatrix) const;

    /** @brief operator to multiply a matrix by a scalar */
    Matrix operator*(double value) const;

    /** @brief operator to divide a matrix by a scalar */
    Matrix operator/(double value) const;

    /** @brief apply power */
    Matrix power(double value) const;

    /**
     * @brief Calculate the inverse of the matrix. Will work only with
     * 3x3 matrices
     */
    Matrix inv() const;

    /**
     * @brief Blend two matrices with the blending ratio specified
     * @param otherMatrix Another matrix to blend current one with
     * @param blending Value between 0.0 and 1.0. Amount of blending with
     * the other matrix
     *
     * @return blended matrix
     */
    Matrix blend(const Matrix &otherMatrix, double blending) const;

    /** @brief Initialize the matrix with zeros */
    void zeros();

    /** @brief Initialize the matrix with ones */
    void ones();

    /**
     * @brief Initialize the matrix as an identity (if its an square matrix).
     */
    void identity();

    /** 
     * @brief Normalise all values using accumulated value
     *
     * Delegates to @ref sum() and @ref operator/()
     */
    Matrix& normaliseSum();

    /**
     * @brief Normalise all values using maximum value
     *
     * Delegates to @ref max() and @ref operator/()
     */
    Matrix& normaliseMax();

    /**
     * @brief Apply the given value as minimum on all elements of the matrix
     */
    Matrix& applyMin(double value);

    /**
     * @brief Apply the given value as maximum on all elements of the matrix
     */
    Matrix& applyMax(double value);

    /** @brief Compute the sum of all elements of the matrix */
    double sum() const;

    /** @brief Computes the maximum value of the matrix */
    double max() const;

    /** @brief Computes the minimum value of the matrix */
    double min() const;

    /**
     * @brief Prints the matrix as histograms on give os
     * @param histogram
     * @param os output stream
     * @param symbol of the top value
     */
    static void plotAsHistogram(const ISPC::Matrix &histogram,
        std::ostream &os, char symbol = '^');
};

std::ostream& operator<<(std::ostream &os, const ISPC::Matrix &m);

} /* namespace ISPC */

#endif /* ISPC_AUX_MATRIX_H_ */
