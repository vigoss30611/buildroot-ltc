/*!
******************************************************************************
 @file   : tal_test.c

 @brief

 @Author Tom Hollis

 @date   23/03/2011

         <b>Copyright 2011 by Imagination Technologies Limited.</b>\n
         All rights reserved.  No part of this software, either
         material or conceptual may be copied or distributed,
         transmitted, transcribed, stored in a retrieval system
         or translated into any human or computer language in any
         form by any means, electronic, mechanical, manual or
         other-wise, or disclosed to third parties without the
         express written permission of Imagination Technologies
         Limited, Unit 8, HomePark Industrial Estate,
         King's Langley, Hertfordshire, WD4 8LZ, U.K.

 <b>Description:</b>\n
         Target Abrstraction Layer (TAL) C Source File for interrupts

 <b>Platform:</b>\n
         Platform Independent

 @Version
         1.0

******************************************************************************/

#include <img_errors.h>

#include <tal.h>
#include <tal_intdefs.h>

/*!
******************************************************************************
##############################################################################
 Category:              TAL Test Functions
##############################################################################
******************************************************************************/

/*!
******************************************************************************
*/
static
IMG_BOOL
IsEqual (
    IMG_UINT64                      ui64ReadValue,
    IMG_UINT64                      ui64RequValue,
    IMG_UINT64                      ui64Enable,
	IMG_UINT64						ui64Gash
)
{
    /* Unreferenced parameter */
    (void)ui64Gash;

    /* If the required value has been detected...*/
    return ((ui64ReadValue & ui64Enable) == (ui64Enable & ui64RequValue));
}
/*!
******************************************************************************
*/
static
IMG_BOOL
IsNotEqual (
    IMG_UINT64                      ui64ReadValue,
    IMG_UINT64                      ui64RequValue,
    IMG_UINT64                      ui64Enable,
	IMG_UINT64						ui64Gash
)
{
    /* Unreferenced parameter */
    (void)ui64Gash;

    /* If the required value has been detected...*/
    return ((ui64ReadValue & ui64Enable) != (ui64Enable & ui64RequValue));
}
/*!
******************************************************************************
*/
static
IMG_BOOL
IsLess (
    IMG_UINT64                      ui64ReadValue,
    IMG_UINT64                      ui64RequValue,
    IMG_UINT64                      ui64Enable,
	IMG_UINT64						ui64Gash
)
{
    /* Unreferenced parameter */
    (void)ui64Gash;

    return ((ui64ReadValue & ui64Enable) < (ui64Enable & ui64RequValue));
}

/*!
******************************************************************************
*/
static
IMG_BOOL
IsLessEq (
    IMG_UINT64                      ui64ReadValue,
    IMG_UINT64                      ui64RequValue,
    IMG_UINT64                      ui64Enable,
	IMG_UINT64						ui64Gash
)
{
    /* Unreferenced parameter */
    (void)ui64Gash;

    return ((ui64ReadValue & ui64Enable) <= (ui64Enable & ui64RequValue));
}
/*!
******************************************************************************
*/
static
IMG_BOOL
IsGreater (
    IMG_UINT64                      ui64ReadValue,
    IMG_UINT64                      ui64RequValue,
    IMG_UINT64                      ui64Enable,
	IMG_UINT64						ui64Gash
)
{
    /* Unreferenced parameter */
    (void)ui64Gash;

    return ((ui64ReadValue & ui64Enable) > (ui64Enable & ui64RequValue));
}
/*!
******************************************************************************
*/
static
IMG_BOOL
IsGreaterEq (
    IMG_UINT64                      ui64ReadValue,
    IMG_UINT64                      ui64RequValue,
    IMG_UINT64                      ui64Enable,
	IMG_UINT64						ui64Gash
)
{
    /* Unreferenced parameter */
    (void)ui64Gash;

    return ((ui64ReadValue & ui64Enable) >= (ui64Enable & ui64RequValue));
}

static
IMG_BOOL
CircBufTest (
    IMG_UINT64                      ui64ReadValue,
    IMG_UINT64                      ui64WriteOffset,
    IMG_UINT64                      ui64PacketSize,
	IMG_UINT64						ui64BufferSize
)
{
	IMG_UINT64 ui64FreeSpace;
    /* Calculate "FreeSpace" */
	if (ui64ReadValue > ui64WriteOffset)
	{
		ui64FreeSpace = ui64ReadValue - ui64WriteOffset - 1;
	}
	else
	{
		ui64FreeSpace = (ui64ReadValue - ui64WriteOffset) + (ui64BufferSize - 1);
	}

	// Check whether the packet will fit in the free space
	return (ui64PacketSize <= ui64FreeSpace);
}

/*!
******************************************************************************

 @Function              tal_GetChkFunc

******************************************************************************/
static TAL_pfnCheckFunc tal_GetChkFunc(
    IMG_UINT32                      ui32CheckFuncIdExt
)
{
    IMG_UINT32      ui32CheckFuncId = (ui32CheckFuncIdExt & TAL_CHECKFUNC_MASK);

	switch (ui32CheckFuncId)
	{
	case TAL_CHECKFUNC_ISEQUAL  :
		return IsEqual;
	case TAL_CHECKFUNC_LESS     :
		return IsLess;
	case TAL_CHECKFUNC_LESSEQ   :
		return IsLessEq;
	case TAL_CHECKFUNC_GREATER  :
		return IsGreater;
	case TAL_CHECKFUNC_GREATEREQ    :
		return IsGreaterEq;
	case TAL_CHECKFUNC_NOTEQUAL    :
		return IsNotEqual;
	case TAL_CHECKFUNC_CBP:
		return CircBufTest;
	default:
		break;
	}

#if !defined (IMG_KERNEL_MODULE)
#ifndef TAL_LIGHT
    printf("ERROR: TAL - Check func Id 0x%X is invalid\n", ui32CheckFuncId);
#endif
#endif
    IMG_ASSERT(IMG_FALSE);
    return IMG_NULL;
}

/*!
******************************************************************************

 @Function              TAL_TestWord

******************************************************************************/
IMG_BOOL TAL_TestWord(
    IMG_UINT64                      ui64ReadVal,
	IMG_UINT64						ui64TestVal_WrOff,
	IMG_UINT64						ui64Enable_PackSize,
	IMG_UINT64						ui64BufferSize,
	IMG_UINT32						ui32CheckFuncIdExt
)
{
	TAL_pfnCheckFunc	pfnCheckFunc;

	/* Get check function */
    pfnCheckFunc = tal_GetChkFunc(ui32CheckFuncIdExt);
	if (pfnCheckFunc == IMG_NULL) return IMG_FALSE;

    // Check whether we meet the exit criteria
    return pfnCheckFunc(ui64ReadVal, ui64TestVal_WrOff, ui64Enable_PackSize, ui64BufferSize);
}



/*!
******************************************************************************

 @Function				TALINTVAR_GetValue

******************************************************************************/
IMG_UINT64 TALINTVAR_GetValue (
	IMG_HANDLE                      hMemSpace,
	IMG_UINT32						ui32InternalVarId
)
{
	TAL_sMemSpaceCB *		psMemSpaceCB = (TAL_sMemSpaceCB *)hMemSpace;
	IMG_ASSERT(psMemSpaceCB != IMG_NULL);
	IMG_ASSERT(ui32InternalVarId < TAL_NUMBER_OF_INTERNAL_VARS);

	return psMemSpaceCB->aui64InternalVariables[ui32InternalVarId];
}

/*!
******************************************************************************

 @Function				TALINTVAR_ExecCommand

******************************************************************************/
IMG_RESULT TALINTVAR_ExecCommand(
		IMG_UINT32						ui32CommandId,
		IMG_HANDLE						hDestRegMemSpace,
		IMG_UINT32						ui32DestRegId,
		IMG_HANDLE						hOpRegMemSpace,
		IMG_UINT32                      ui32OpRegId,
		IMG_HANDLE						hLastOpMemSpace,
		IMG_UINT64						ui64LastOperand,
		IMG_BOOL						bIsRegisterLastOperand
)
{
	IMG_UINT64				ui64LastOperandVal = 0;
	TAL_sMemSpaceCB *		psDestMemSpaceCB = (TAL_sMemSpaceCB *)hDestRegMemSpace;
	TAL_sMemSpaceCB *		psOpMemSpaceCB = (TAL_sMemSpaceCB *)hOpRegMemSpace;
	TAL_sMemSpaceCB *		psLastMemSpaceCB = (TAL_sMemSpaceCB *)hLastOpMemSpace;
	IMG_RESULT				rResult = IMG_SUCCESS;

	if (ui32CommandId != TAL_PDUMPL_INTREG_MOV)
	{
		/* Check internal register memory space */
		IMG_ASSERT(psOpMemSpaceCB);
	}

	if (ui32CommandId != TAL_PDUMPL_INTREG_NOT)
	{
			if (bIsRegisterLastOperand)
		{
			IMG_ASSERT(ui64LastOperand < TAL_NUMBER_OF_INTERNAL_VARS);

			/* Check internal register memory space Id */
			IMG_ASSERT(psLastMemSpaceCB);
			ui64LastOperandVal = psLastMemSpaceCB->aui64InternalVariables[ui64LastOperand];
		}
		else
		{
			ui64LastOperandVal = ui64LastOperand;
		}
	}

	/* Run Command */
	switch (ui32CommandId)
	{
	case TAL_PDUMPL_INTREG_MOV:
		/* MOV */
		psDestMemSpaceCB->aui64InternalVariables[ui32DestRegId] = ui64LastOperandVal;
		break;
	case TAL_PDUMPL_INTREG_AND:
		/* AND */
		psDestMemSpaceCB->aui64InternalVariables[ui32DestRegId] =
			psOpMemSpaceCB->aui64InternalVariables[ui32OpRegId] & ui64LastOperandVal;
		break;
	case TAL_PDUMPL_INTREG_OR:
		/* OR */
		psDestMemSpaceCB->aui64InternalVariables[ui32DestRegId] =
			psOpMemSpaceCB->aui64InternalVariables[ui32OpRegId] | ui64LastOperandVal;
		break;
	case TAL_PDUMPL_INTREG_XOR:
		/* XOR */
		psDestMemSpaceCB->aui64InternalVariables[ui32DestRegId] =
			psOpMemSpaceCB->aui64InternalVariables[ui32OpRegId] ^ ui64LastOperandVal;
		break;
	case TAL_PDUMPL_INTREG_NOT:
		/* NOT */
		psDestMemSpaceCB->aui64InternalVariables[ui32DestRegId] =
			~ psOpMemSpaceCB->aui64InternalVariables[ui32OpRegId];
		break;
	case TAL_PDUMPL_INTREG_SHR:
		/* SHR */
		psDestMemSpaceCB->aui64InternalVariables[ui32DestRegId] =
			psOpMemSpaceCB->aui64InternalVariables[ui32OpRegId] >> ui64LastOperandVal;
		break;
	case TAL_PDUMPL_INTREG_SHL:
		/* SHL */
		psDestMemSpaceCB->aui64InternalVariables[ui32DestRegId] =
			psOpMemSpaceCB->aui64InternalVariables[ui32OpRegId] << ui64LastOperandVal;
		break;
	case TAL_PDUMPL_INTREG_ADD:
		/* ADD */
		psDestMemSpaceCB->aui64InternalVariables[ui32DestRegId] =
			psOpMemSpaceCB->aui64InternalVariables[ui32OpRegId] + ui64LastOperandVal;
		break;
	case TAL_PDUMPL_INTREG_SUB:
		/* SUB */
		psDestMemSpaceCB->aui64InternalVariables[ui32DestRegId] =
			psOpMemSpaceCB->aui64InternalVariables[ui32OpRegId] - ui64LastOperandVal;
		break;
	case TAL_PDUMPL_INTREG_MUL:
		/* MUL */
		psDestMemSpaceCB->aui64InternalVariables[ui32DestRegId] =
			psOpMemSpaceCB->aui64InternalVariables[ui32OpRegId] * ui64LastOperandVal;
		break;
	case TAL_PDUMPL_INTREG_IMUL:
		/* IMUL */
		psDestMemSpaceCB->aui64InternalVariables[ui32DestRegId] =
			psOpMemSpaceCB->aui64InternalVariables[ui32OpRegId] * ui64LastOperandVal;
		break;
#if  !defined(IMG_KERNEL_MODULE)	/* Floats are not allowed in the kernel */
	case TAL_PDUMPL_INTREG_DIV:
		/* DIV */
		IMG_ASSERT(ui64LastOperandVal != 0);
		psDestMemSpaceCB->aui64InternalVariables[ui32DestRegId] =
			psOpMemSpaceCB->aui64InternalVariables[ui32OpRegId] / ui64LastOperandVal;
		break;
	case TAL_PDUMPL_INTREG_IDIV:
		/* IDIV */
		IMG_ASSERT(ui64LastOperandVal != 0);
		psDestMemSpaceCB->aui64InternalVariables[ui32DestRegId] =
			psOpMemSpaceCB->aui64InternalVariables[ui32OpRegId] / ui64LastOperandVal;
		break;
	case TAL_PDUMPL_INTREG_MOD:
		/* MOD */
		psDestMemSpaceCB->aui64InternalVariables[ui32DestRegId] =
			psOpMemSpaceCB->aui64InternalVariables[ui32OpRegId] % ui64LastOperandVal;
		break;
#endif	//#if  !defined(IMG_KERNEL_MODULE)	/* Floats are not allowed in the kernel */

	case TAL_PDUMPL_INTREG_EQU:
		/* EQU */
		if (psOpMemSpaceCB->aui64InternalVariables[ui32OpRegId] == ui64LastOperandVal)
		{
			psDestMemSpaceCB->aui64InternalVariables[ui32DestRegId] = 1;
		}
		else
		{
			psDestMemSpaceCB->aui64InternalVariables[ui32DestRegId] = 0;
		}
		break;
	case TAL_PDUMPL_INTREG_LT:
		/* LT */
		if (psOpMemSpaceCB->aui64InternalVariables[ui32OpRegId] < ui64LastOperandVal)
		{
			psDestMemSpaceCB->aui64InternalVariables[ui32DestRegId] = 1;
		}
		else
		{
			psDestMemSpaceCB->aui64InternalVariables[ui32DestRegId] = 0;
		}
		break;
	case TAL_PDUMPL_INTREG_LTE:
		/* LTE */
		if (psOpMemSpaceCB->aui64InternalVariables[ui32OpRegId] <= ui64LastOperandVal)
		{
			psDestMemSpaceCB->aui64InternalVariables[ui32DestRegId] = 1;
		}
		else
		{
			psDestMemSpaceCB->aui64InternalVariables[ui32DestRegId] = 0;
		}
		break;
	case TAL_PDUMPL_INTREG_GT:
		/* GT */
		if (psOpMemSpaceCB->aui64InternalVariables[ui32OpRegId] > ui64LastOperandVal)
		{
			psDestMemSpaceCB->aui64InternalVariables[ui32DestRegId] = 1;
		}
		else
		{
			psDestMemSpaceCB->aui64InternalVariables[ui32DestRegId] = 0;
		}
		break;
	case TAL_PDUMPL_INTREG_GTE:
		/* GTE */
		if (psOpMemSpaceCB->aui64InternalVariables[ui32OpRegId] >= ui64LastOperandVal)
		{
			psDestMemSpaceCB->aui64InternalVariables[ui32DestRegId] = 1;
		}
		else
		{
			psDestMemSpaceCB->aui64InternalVariables[ui32DestRegId] = 0;
		}
		break;
	case TAL_PDUMPL_INTREG_NEQ:
		/* NEQ */
		if (psOpMemSpaceCB->aui64InternalVariables[ui32OpRegId] != ui64LastOperandVal)
		{
			psDestMemSpaceCB->aui64InternalVariables[ui32DestRegId] = 1;
		}
		else
		{
			psDestMemSpaceCB->aui64InternalVariables[ui32DestRegId] = 0;
		}
		break;
	}
	return rResult;
}

