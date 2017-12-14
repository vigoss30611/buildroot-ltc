/**
*******************************************************************************
 @file sensor_phy.h

 @brief Header describing the interface of imager's PHY and V2500 gasket

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
#ifndef _GASKET_PHY_
#define _GASKET_PHY_

#ifdef __cplusplus
extern "C" {
#endif

#include "img_types.h"
#include "pciuser.h"

struct CI_GASKET; /* defined in ci_api_struct.h */
struct CI_CONNECTION; /* defined in ci_api_struct.h */

/**
 * @defgroup SENSOR_API_PHY Sensor PHY control
 * @ingroup SENSOR_API
 * @brief Control of the imager's PHY
 */
/*-----------------------------------------------------------------------------
 * Following elements are in the PHY documentation module
 *---------------------------------------------------------------------------*/
 
/**
 * @brief Control of the PHY
 *
 * @note May need to be re-implemented to support the actual customer's PHY
 */ 
typedef struct _sensor_phy_
{
    /**
     * @brief Maps IMG FPGA PHY to user-space
     * @note May need to be changed by customers
     */
	user_device_mapping sPhyRegs;
    /**
     * @brief IMG FPGA PHY registers in user-space
     * @note May need to be changed by customers
     */
	volatile IMG_UINT32 *puiPhyRegs;
    
    /**
     * @brief Connection to the low level driver to access gasket reservation
     * functionalities
     */
    struct CI_CONNECTION *psConnection;
    /**
     * @brief Gasket object used for reservation of the V2500 Gasket
     */
	struct CI_GASKET *psGasket;
} SENSOR_PHY;


/**
 * @brief Initialise the Gasket hardware
 */
SENSOR_PHY* SensorPhyInit();

/**
 * @brief Close the access to the I2Cdevice
 *
 * @param psSensorPhy the pointer to the sensor device to close
 */
void SensorPhyDeinit(SENSOR_PHY *psSensorPhy);

/**
 * @brief Turn on/off the imager gasket
 *
 * @param psSensorPhy The Internal Sensor structure being operated on.
 * @param bEnable flag for enable/disable
 * @param uiMipiLanes number of MIPI lanes to enable. Set to 0 for parallel 
 * imagers
 * @param ui8SensorNum value to write to the SENSOR_SELECT register of the PHY
 */
IMG_RESULT SensorPhyCtrl(SENSOR_PHY *psSensorPhy, IMG_BOOL bEnable,
		IMG_UINT8 uiMipiLanes, IMG_UINT8 ui8SensorNum);
		
		
#ifdef INFOTM_ISP
/**
 * @brief Change bayer format
 *
 * @param psSensorPhy the pointer to the sensor device to close
 * @param bayerFmt the bayer format to switch to
 *
 * Add by feng.qu@infotm.com
 */
IMG_RESULT SensorPhyBayerFormat(SENSOR_PHY *psSensorPhy, enum MOSAICType bayerFmt);
#endif //INFOTM_ISP		

/**
 * @brief discover the i2c device
 */
#ifdef INFOTM_ISP 
IMG_RESULT find_i2c_dev(char *i2c_dev_name, unsigned int len, unsigned int addr, const char *i2c_adaptor);
#else
IMG_RESULT find_i2c_dev(char *i2c_dev_name, unsigned int len);
#endif //INFOTM_ISP

/**
 * @}
 */
/*-----------------------------------------------------------------------------
 * End of PHY documentation module
 *---------------------------------------------------------------------------*/

#ifdef __cplusplus
}
#endif 
#endif // _GASKET_PHY_
