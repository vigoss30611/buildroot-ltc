/**
*******************************************************************************
@file computebase.hpp

@brief Base class for all compute class (e.g. CCMCompute, LSHCompute)

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
#ifndef COMPUTEBASE_HPP
#define COMPUTEBASE_HPP

#include <QtCore/QObject>
#include <QtCore/QThread>
#include <string>

#ifdef WIN32
/* disable C4290: C++ exception specification ignored except to indicate a
 * function is not __declspec(nothrow) */
#pragma warning(disable : 4290)
#endif

class ComputeBase: public QObject
{
    Q_OBJECT

protected:
    /** @brief prefix in LOG */
    std::string prefix;
    /** @brief The thread that this object will run on */
    QThread *pThread;

public:
    /**
     * @brief creates a object on its own thread
     *
     * @note to start computation use a signal/slot mechanism
     */
    explicit ComputeBase(const std::string &prefix, QObject *parent = 0);

    /**
     * @brief destroy the object and stop its thread
     *
     * @note call this once all computations are finished (e.g. with a
     * deleteLater call)
     */
    virtual ~ComputeBase();

signals:
    void logMessage(QString) const;
};

#endif /* COMPUTEBASE_HPP */
