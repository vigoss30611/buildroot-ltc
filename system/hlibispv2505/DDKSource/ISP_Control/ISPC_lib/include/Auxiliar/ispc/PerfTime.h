/**
*******************************************************************************
 @file PerfTime.h

 @brief Performance of functions
 
 LOG_PERF_IN log current time for call graph. Absolute time is stored.
 Save it to be subtracted when LOG_PERF_OUT is called.
 To allow nested logs create a LIFO.
 Function name and time is stored.

 When LOG_PERF_OUT is called the list is searched for matching function name.
 Every entry which does not match current function name is removed.

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
#ifndef ISPC_PERFTIME_H_
#define ISPC_PERFTIME_H_

#include <string>

#include "ispc/Module.h"

namespace ISPC {

/**
 * @brief To check if log perf functionality was compiled in.
 *
 * If returns false then all LOG_Perf functions will be empty and
 * registration will not be possible either.
 */
bool LOG_Perf_Available(void);

/**
 * @brief Register a logging name
 *
 * Logging names that aren't registered will not push elements on the
 * performance stack.
 */
void LOG_Perf_Register(const char *loggingName, bool verbose);

/**
 * @brief Register a module as part of the performance measurement using its
 * logging name
 *
 * Logging names that aren't registered will not push elements on the
 * performance stack.
 */
void LOG_Perf_Register(const ModuleBase *module, bool verbose);

/**
 * @brief Add an input entry to the performance log stack
 *
 * @note loggingName should be the name of the module when given to
 * @ref LOG_Perf_Register() or the entry will be ignored
 *
 * If the loggingName was registered as verbose the VA_ARGS will be passed
 * to @ref LOG_Perf
 */
void LOG_Perf_In(const char *loggingName, const char *file,
    const char *function, unsigned int line);
/**
 * @brief Add an output entry to the performance log stack
 *
 * @note loggingName should be the name of the module when given to
 * @ref LOG_Perf_Register() or the entry will be ignored
 *
 * If the loggingName was registered as verbose the VA_ARGS will be passed
 * to @ref LOG_Perf
 */
void LOG_Perf_Out(const char *loggingName, const char *file,
    const char *function, unsigned int line);

/**
 * @brief If the performance stack has any entries prints the report
 */
void LOG_Perf_Summary();

#define LOG_PERF_IN() \
    LOG_Perf_In(this->getLoggingName(), __FILE__, __FUNCTION__, __LINE__)
#define LOG_PERF_OUT() \
    LOG_Perf_Out(this->getLoggingName(), __FILE__, __FUNCTION__, __LINE__)

}  // namespace ISPC

#endif /* ISPC_PERFTIME_H_ */
