/**
*******************************************************************************
@file ci_modules.h

@brief Function to manipulate the modules configuration

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
#ifndef CI_MODULES_H_
#define CI_MODULES_H_

#include <img_types.h>

#include "ci/ci_api_structs.h"
#include "ci/ci_modules_structs.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup CI_API_MOD Pipeline Modules (CI_Module)
 * @ingroup CI_API
 * @brief Pipeline module structures and functions
 * @{
 */
/*-----------------------------------------------------------------------------
 * Following elements are in the IMG_CI_API documentation module
 *---------------------------------------------------------------------------*/

IMG_RESULT CI_ModuleIIF_verif(const CI_MODULE_IIF *pImagerInterface,
    IMG_UINT8 context, const CI_HWINFO *pHWInfo);

/**
 * @brief Verify that Lens-Shading and White Balance configuration is valid
 */
IMG_RESULT CI_ModuleLSH_verif(const CI_MODULE_LSH_MAT *pMatrix,
    const CI_MODULE_IIF *pImagerInterface, const CI_HWINFO *pHWInfo);

/**
 * @brief Verifies that a scaler configuration is valid
 *
 * @return IMG_SUCCESS
 * @return IMG_ERROR_FATAL if the values are not fitting into the taps
 */
IMG_RESULT CI_ModuleScaler_verif(const struct CI_MODULE_SCALER *pScaler);

IMG_RESULT CI_ModuleDPF_verif(const struct CI_MODULE_DPF *pDefectivePixel,
    const CI_HWINFO *pHWInfo);

IMG_RESULT CI_ModuleWBC_verif(const struct CI_MODULE_WBC *pWBC);

/**
 * @brief Verifies that the TNM configuration is valid
 *
 * @return IMG_SUCCESS
 */
IMG_RESULT CI_ModuleTNM_verif(const struct CI_MODULE_TNM *pTNM);

IMG_RESULT CI_StatsEXS_verif(const struct CI_MODULE_EXS *pEXS);

IMG_RESULT CI_StatsFLD_verif(const struct CI_MODULE_FLD *pFLD);

IMG_RESULT CI_StatsENS_verif(const struct CI_MODULE_ENS *pENS);

IMG_RESULT CI_StatsWBS_verif(const struct CI_MODULE_WBS *pWBS);

IMG_RESULT CI_StatsHIS_verif(const struct CI_MODULE_HIS *pHIS);

IMG_RESULT CI_StatsAWS_verif(const struct CI_MODULE_AWS *pAWS,
    const CI_MODULE_IIF* pIIF);

IMG_INT32 ToFixed(float floatVal, IMG_UINT32 fractional);

IMG_RESULT ToFixedVerif(float floatVal, IMG_UINT32 *outFixed,
    IMG_UINT32 integer, IMG_UINT32 fractional);

/**
 * @}
 */
/*-----------------------------------------------------------------------------
 * End of the CI_API documentation module
 *---------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* CI_MODULES_H_ */
