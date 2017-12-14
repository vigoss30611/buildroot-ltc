/**
*******************************************************************************
 @file Connection.cpp

 @brief Implementation of ISPC::Connection

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
#include "ispc/Connection.h"

#include <img_errors.h>
#include <ci/ci_api.h>

#include <felixcommon/userlog.h>
#define LOG_TAG "ISPC_CONNECTION"

//
// private
//

CI_CONNECTION * ISPC::CI_Connection::pCI_Connection = NULL;
int ISPC::CI_Connection::nCI_Connections = 0;

IMG_RESULT ISPC::CI_Connection::connect()
{
    IMG_RESULT ret;
    LOG_DEBUG("Enter conns=%d (%p)\n", nCI_Connections, pCI_Connection);

    if (0 == nCI_Connections && !pCI_Connection)
    {
        ret = CI_DriverInit(&(pCI_Connection));
        if (ret)
        {
            LOG_ERROR("Unable to initialize connection to CI\n");
            return IMG_ERROR_FATAL;
        }
    }

    nCI_Connections++;
    return IMG_SUCCESS;
}

IMG_RESULT ISPC::CI_Connection::disconnect()
{
    IMG_RESULT ret;
    LOG_DEBUG("Enter conns=%d (%p)\n", nCI_Connections, pCI_Connection);

    if (0 >= nCI_Connections)
    {
        LOG_ERROR("Unable to close connection to CI (invalid "\
            "connection counter)\n");
        return IMG_ERROR_FATAL;
    }
    nCI_Connections--;

    if (0 == nCI_Connections)
    {
        ret = CI_DriverFinalise(pCI_Connection);
        if (ret)
        {
            LOG_ERROR("Error closing CI connection\n");
        }
        pCI_Connection = NULL;
    }

    return IMG_SUCCESS;
}

//
// public
//

ISPC::CI_Connection::CI_Connection(): connected(false)
{
    IMG_RESULT ret = connect();
    if (ret)
    {
        LOG_ERROR("Failed to connect (returned %d)\n", ret);
    }
    else
    {
        connected = true;
    }
}

ISPC::CI_Connection::~CI_Connection()
{
    if (connected)
    {
        IMG_RESULT ret = disconnect();
        if (ret)
        {
            LOG_ERROR("Failed to disconnect (returned %d)\n", ret);
        }
    }
}


CI_CONNECTION* ISPC::CI_Connection::getConnection()
{
    if (connected)
    {
        return pCI_Connection;
    }
    return NULL;
}

const CI_CONNECTION* ISPC::CI_Connection::getConnection() const
{
    if (connected)
    {
        return pCI_Connection;
    }
    return NULL;
}

int ISPC::CI_Connection::getNConnections() const
{
    return nCI_Connections;
}

