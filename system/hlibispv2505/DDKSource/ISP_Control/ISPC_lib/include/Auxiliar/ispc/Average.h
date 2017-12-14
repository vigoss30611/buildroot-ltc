/**
*******************************************************************************
 @file Average.h

 @brief ISPC::Average declaration

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
#ifndef ISPC_AVERAGE_H_
#define ISPC_AVERAGE_H_

#include <vector>

namespace ISPC {

/**
 * @class mavg
 * @brief implements moving average with parametrized window size
 */
template <class T>
class mavg : private std::vector<T> {
public:
    /**
     * @brief constructor
     * @param window window size
     */
    explicit mavg(const size_t window) :
        std::vector<T>(window, 0), windowSize(window) {
        reset();
    }
    ~mavg() {}

    /**
     * @brief add the sample to the window
     *
     * @param sample next sharpness sample
     * @return new average
     */
    double addSample(const T sample) {
        if (fillLvl == windowSize) {  // do not subtract if buffer empty
            mSum -= *windowIndex;
        }
        mSum += sample;
        *windowIndex++ = sample;
        if (windowIndex == this->end()) {
            windowIndex = this->begin();
        }
        if (fillLvl < windowSize) {
            ++fillLvl;
        }
        mAverage = mSum/fillLvl;
        return mAverage;
    }

    double getAverage() { return mAverage; }
    /**
     * @brief resets the internal average to 0
     */
    void reset(void) {
        fillLvl = 0;
        windowIndex = this->begin();
        mAverage = 0;
        mSum = 0;
    }

private:
    mavg() : windowSize(0) {}

    /** @brief size of moving average window */
    const size_t windowSize;
    /** @brief current index in MAVG window */
    typename std::vector<T>::iterator windowIndex;
    /** @brief fill level in the MAVG */
    size_t fillLvl;
    /** @brief sum of currently stored samples */
    double mSum;
    /** @brief average of currently stored samples */
    double mAverage;
};

} /* namespace ISPC */

#endif /* ISPC_AVERAGE_H_ */
