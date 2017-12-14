/**
******************************************************************************
@file demoguicommands.hpp

@brief 

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
#ifndef DEMO_GUI_COMMANDS
#define DEMO_GUI_COMMANDS

#include <paramsocket/paramsocket.hpp>

enum GUIParamCMD
{
    GUICMD_QUIT,

	// HWINFO
	GUICMD_GET_HWINFO,

	// PARAMETERlIST
	GUICMD_SET_PARAMETER_LIST,
	GUICMD_SET_PARAMETER_NAME,
	GUICMD_SET_PARAMETER_VALUE,
	GUICMD_SET_PARAMETER_END,
	GUICMD_SET_PARAMETER_LIST_END,
	GUICMD_GET_PARAMETER_LIST,

	// IMAGE
	GUICMD_GET_IMAGE,
	GUICMD_SET_IMAGE,
	GUICMD_SET_IMAGE_RECORD,

	// SENSOR
	GUICMD_GET_SENSOR,

	// AE
	GUICMD_GET_AE,
	GUICMD_SET_AE_ENABLE,

	// AWB
	GUICMD_GET_AWB,
	GUICMD_SET_AWB_MODE,

	// AF
	GUICMD_GET_AF,
	GUICMD_SET_AF_ENABLE,
	GUICMD_SET_AF_TRIG,

	// TNM
	GUICMD_GET_TNM,
	GUICMD_SET_TNM_ENABLE,

	// LIVEFEED
	GUICMD_GET_LF,
	GUICMD_SET_LF_ENABLE,
	GUICMD_SET_LF_FORMAT,
	GUICMD_SET_LF_REFRESH,

	// DPF
	GUICMD_GET_DPF,
	GUICMD_GET_DPF_MAP,
	GUICMD_SET_DPF_MAP,

	// LSH
	GUICMD_ADD_LSH_GRID,
    GUICMD_REMOVE_LSH_GRID,
    GUICMD_SET_LSH_GRID,
    GUICMD_SET_LSH_ENABLE,
    GUICMD_GET_LSH,

	// LBC
	GUICMD_GET_LBC,
	GUICMD_SET_LBC_ENABLE,

	// DNS
	GUICMD_SET_DNS_ENABLE,

	// ENS
	GUICMD_GET_ENS,

	// GMA
	GUICMD_GET_GMA,
	GUICMD_SET_GMA,

    // PDP
    GUICMD_SET_PDP_ENABLE,

	// TEST
	GUICMD_GET_FB,

    GUICMD_CUSTOM ///< @brief Not used - can be used by other set of command not to clash with those ones
};

#endif // DEMO_GUI_COMMANDS
