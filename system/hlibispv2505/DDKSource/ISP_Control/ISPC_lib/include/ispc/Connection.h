/**
*******************************************************************************
 @file Connection.h

 @brief Declaration of ISPC::Connection which represents a connection to the
 CI kernel driver

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
#ifndef ISPC_CONNECTION_H_
#define ISPC_CONNECTION_H_

#include <ci/ci_api_structs.h>

namespace ISPC {

class CI_Connection
{
private:
    /** @brief Connection to the CI */
    static CI_CONNECTION *pCI_Connection;
    /** @brief Number of CI connections */
    static int nCI_Connections;

    /**
     * @brief Connect to the CI driver
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_UNEXPECTED_STATE if the Camera is in the wrong state
     * @return IMG_ERROR_FATAL if CI failed to initialise
     */
    static IMG_RESULT connect();

    /**
     * @brief Disconnect from the CI driver
     *
     * @return IMG_SUCCESS
     * @return IMG_ERROR_UNEXPECTED_STATE if the Camera is in the wrong state
     * @return IMG_ERROR_FATAL for internal failure
     */
    static IMG_RESULT disconnect();

    bool connected;

public:
    CI_Connection();

    CI_CONNECTION* getConnection();
    const CI_CONNECTION* getConnection() const;
    int getNConnections() const;

    ~CI_Connection();
};

} /* namespace ISPC */

#endif /* ISPC_CONNECTION_H_ */
