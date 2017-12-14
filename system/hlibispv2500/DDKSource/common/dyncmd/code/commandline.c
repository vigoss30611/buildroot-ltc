/**
******************************************************************************
 @file commandline.c

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

#include <img_types.h>
#include <img_defs.h>
#include <img_errors.h>

#include <stdio.h>
#include <errno.h>

#include "dyncmd/commandline.h"

#ifdef DOXYGEN_CREATE_MODULES
/**
 * @defgroup IMGLIB_DYNCMD IMGLIB DynCMD: Dynamic Command line and file parser
 *
 * @{
 */
/*-------------------------------------------------------------------------
 * Following elements are in the IMGLIB_FILEIO documentation module
 *------------------------------------------------------------------------*/
#endif // DOXYGEN_CREATE_MODULES

#ifndef MAX_PARAMS
#define MAX_PARAMS 255
#endif
#ifndef MAX_LINE
#define MAX_LINE 255
#endif

/**
 * @brief Describes and contains a parameter (and its possible following value) from the command line
 */
typedef struct _sDYNCMDLine_
{
	IMG_CHAR *pszFlag;	/**< @brief Command name */
	DYNCMDLINE_TYPE eParamType;	/**< @brief Command type */
	const IMG_CHAR *pszDesc;	/**< @brief Description string */
	void * pvData;				/**< @brief Stored data (casted according to eParamType). */
	int nElem; /** @brief number of elements the parameter need - >1 for arrays */
	int nFound; /** @brief number of elements found - needed when the array is partially found and printing unused parameters */
}DYNCMDLINE_STRUCT;

static int g_nParams=0;
static DYNCMDLINE_STRUCT *asCommandlineParams[MAX_PARAMS]={NULL};

static int g_nCmd = 0;
static char *g_CmdList[MAX_PARAMS] = {NULL};
static IMG_BOOL8 g_bCmdAdded = IMG_FALSE;

static char* getTypeName(DYNCMDLINE_TYPE eType)
{
	switch(eType)
	{
	case DYNCMDTYPE_STRING:
		return "<string>";
	case DYNCMDTYPE_BOOL8:
		return "<bool=0/1>";
	case DYNCMDTYPE_FLOAT:
		return "<float>";
	case DYNCMDTYPE_UINT:
		return "<uint>";
	case DYNCMDTYPE_INT:
		return "<int>";
	case DYNCMDTYPE_COMMAND:
		return ""; // for command return nothing because it is not expecting any following parameter
	default:
		return "<Unknown>";
	}
}

DYNCMDLINE_RETURN_CODE DYNCMD_getArrayFromString(DYNCMDLINE_TYPE eType, const char* pszParam, void *pvData, int n)
{
	switch(eType)
	{
	case DYNCMDTYPE_BOOL8:
		return DYNCMD_getObjectFromString(eType, pszParam, &( ((IMG_BOOL8*)pvData)[n] ));

	case DYNCMDTYPE_FLOAT:
		return DYNCMD_getObjectFromString(eType, pszParam, &( ((IMG_FLOAT*)pvData)[n] ));

	case DYNCMDTYPE_UINT:
		return DYNCMD_getObjectFromString(eType, pszParam, &( ((IMG_UINT*)pvData)[n] ));

	case DYNCMDTYPE_INT:
		return DYNCMD_getObjectFromString(eType, pszParam, &( ((IMG_INT*)pvData)[n] ));

	// STRING and COMMAND not supported
	default:
		return RET_INCORRECT;
	}
}

DYNCMDLINE_RETURN_CODE DYNCMD_getObjectFromString(DYNCMDLINE_TYPE eType, const char* pszParam, void *pvData)
{
	DYNCMDLINE_RETURN_CODE eRet;

	if ( eType == DYNCMDTYPE_COMMAND )
	{
		return RET_FOUND;
	}
	else if ( pvData == NULL )
	{
		return RET_ERROR;
	}

	switch (eType)
	{
		case DYNCMDTYPE_BOOL8:
			{
				IMG_INT tmpData = 0;
				if ( sscanf(pszParam, "%d", &tmpData) == 1 )
				{
					eRet = RET_FOUND;
	#if IMG_TRUE != 1 || IMG_FALSE != 0
					if ( tmpData == 1 ) *pvData = IMG_TRUE;
					else if (tmpData == 0 )	*pvData = IMG_FALSE;
	#else
					*(IMG_BOOL8*)pvData = tmpData;
	#endif
					if ( *(IMG_BOOL8*)pvData != IMG_TRUE && *(IMG_BOOL8*)pvData != IMG_FALSE )
					{
						eRet = RET_INCORRECT;
						fprintf(stderr, "value %d is not IMG_TRUE (%d) nor IMG_FALSE (%d)\n", *(IMG_INT*)pvData, IMG_TRUE, IMG_FALSE);
					}
				}
				else
				{
					eRet=RET_INCORRECT;
				}
			}
		break;

		case DYNCMDTYPE_FLOAT:
			if ( sscanf(pszParam, "%f", (IMG_FLOAT*)pvData) == 1 )
			{
				eRet = RET_FOUND;
			}
			else
			{
				eRet = RET_INCORRECT;
			}
			break;
		case DYNCMDTYPE_UINT:
			if ( sscanf(pszParam, "%u", (IMG_UINT*)pvData) == 1 )
			{
				eRet = RET_FOUND;
			}
			else
			{
				eRet = RET_INCORRECT;
			}
			break;
		case DYNCMDTYPE_INT:
			if ( sscanf(pszParam, "%d", (IMG_INT*)pvData) == 1 )
			{
				eRet = RET_FOUND;
			}
			else
			{
				eRet = RET_INCORRECT;
			}
			break;
		case DYNCMDTYPE_STRING: // pvData is a pointer to IMG_CHAR*
			{
				IMG_CHAR ** chararr = (IMG_CHAR**)pvData;
				IMG_SIZE length = strlen(pszParam);
				*chararr = (IMG_CHAR*)IMG_MALLOC(length+1);
				if ( chararr != NULL )
				{
					strncpy(*chararr, pszParam, length);
					(*chararr)[length] = '\0';
				}
				else
				{
					fprintf(stderr, "failed to allocate copy of the string!\n");
					eRet = RET_ERROR;
				}
			}
			eRet=RET_FOUND;
			break;
	}

	return eRet;
}

DYNCMDLINE_RETURN_CODE DYNCMD_RegisterArray(const char *pszPattern, DYNCMDLINE_TYPE eType, const char *pszDesc, void *pvData, int uiNElem)
{
	int n=0;
	IMG_INT32 i32ParamIndex;
	DYNCMDLINE_RETURN_CODE eRet = RET_NOT_FOUND;

	if ( uiNElem > 1 && (eType == DYNCMDTYPE_COMMAND || eType == DYNCMDTYPE_STRING) )
	{
		return RET_ERROR;
	}
	if ( eType != DYNCMDTYPE_COMMAND && pvData == NULL )
	{
		fprintf(stderr, "You must give a data pointer when registering a data type\n");
		return RET_ERROR;
	}
	if ( g_bCmdAdded == IMG_FALSE )
	{
		fprintf(stderr, "You must add a command line or a file before registering a parameter\n");
		return RET_ERROR;
	}

	// we have space in our table, so add the parameter information, and look for that parameter in the commandline, setting it's value as required
	for(n=0;n<g_nParams;n++)
	{
		if(strcmp(asCommandlineParams[n]->pszFlag,pszPattern)==0)
		{
			// we have already added this to the table so we do not need to allocate another entry.
			fprintf(stderr, "the parameter '%s' is already registered\n", pszPattern);
			return RET_ERROR;
		}
	}

	// it is always a new parameter
	if(g_nParams+1>=MAX_PARAMS)
	{
		fprintf(stderr,"Too many commandline params, try increasing the MAX_PARAMS (%d) variable and recompile\n", MAX_PARAMS);
		return RET_ERROR;
	}
	asCommandlineParams[n]=(DYNCMDLINE_STRUCT*)IMG_MALLOC(sizeof(DYNCMDLINE_STRUCT));
	g_nParams++;

	asCommandlineParams[n]->eParamType = eType;
	asCommandlineParams[n]->pszFlag=IMG_STRDUP(pszPattern);
	asCommandlineParams[n]->pszDesc=pszDesc;
	asCommandlineParams[n]->pvData=pvData;
	asCommandlineParams[n]->nElem = eType == DYNCMDTYPE_COMMAND ? 0 : uiNElem;
	asCommandlineParams[n]->nFound = 0;

	//for(ui8ParamIndex=0;ui8ParamIndex<g_nCmd;ui8ParamIndex++)
	// from end to start to get last element
	for(i32ParamIndex=g_nCmd-1; i32ParamIndex>=0 ; i32ParamIndex--)
	{
		if(strcmp(g_CmdList[i32ParamIndex],pszPattern)==0)
		{
			eRet = RET_FOUND;
			if ( eType != DYNCMDTYPE_COMMAND )
			{
				int found;
				for ( found = 0 ; found < uiNElem && eRet == RET_FOUND ; found++ )
				{
					if ( i32ParamIndex+1+found >= g_nCmd )
					{
						eRet = RET_INCORRECT; // not enough parameters
					}
					else
					{
						if ( uiNElem > 1 )
						{
							eRet = DYNCMD_getArrayFromString(eType, g_CmdList[i32ParamIndex+1+found], pvData, found);
						}
						else
						{
							eRet = DYNCMD_getObjectFromString(eType, g_CmdList[i32ParamIndex+1], pvData);
						}
					}
				}
				asCommandlineParams[n]->nFound = found;
				if ( eRet == RET_INCORRECT)
				{
					asCommandlineParams[n]->nFound--;
					if ( asCommandlineParams[n]->nFound>0 )
					{
						eRet = RET_INCOMPLETE; // some found, not all
					}
				}
			}
			break; // stop searching to use only the last one
		}

	}
	return eRet;
}

DYNCMDLINE_RETURN_CODE DYNCMD_RegisterParameter(const char *pszPattern,DYNCMDLINE_TYPE eType,const char *pszDesc, void *pvData)
{
	return DYNCMD_RegisterArray(pszPattern, eType, pszDesc, pvData, 1);
}

IMG_RESULT DYNCMD_HasUnregisteredElements(IMG_UINT32 *pui32Result)
{
	int i,j;
	IMG_UINT32 unregistered = 0;

	if ( pui32Result == NULL )
	{
		return IMG_ERROR_INVALID_PARAMETERS;
	}

	if ( g_nParams == 0 || g_bCmdAdded == IMG_FALSE )
	{
		fprintf(stderr, "no registered parameters or command line/ file not added\n");
		return IMG_ERROR_UNEXPECTED_STATE;
	}

	for ( i = 0 ; i < g_nCmd ; i++ )
	{
		IMG_BOOL8 bFound = IMG_FALSE;

		for ( j = 0 ; j < g_nParams && bFound != IMG_TRUE ; j++ )
		{
			if ( strcmp(g_CmdList[i], asCommandlineParams[j]->pszFlag) == 0 )
			{
				bFound = IMG_TRUE;
				i+=asCommandlineParams[j]->nFound; // skip next elements, they are correct - it's 0 for commands
			}// if
			
		}//for j


		if ( bFound == IMG_FALSE ) // not found
		{
			unregistered++;
			fprintf(stderr, "  '%s' unknown parameter (%d)\n", g_CmdList[i], i);
		}
	}//for i

	*pui32Result = unregistered;
	return IMG_SUCCESS;
}

void DYNCMD_PrintUsage()
{
	int n=0;
	printf("usage()\n");
	for(n=0;n<g_nParams;n++)
	{
		printf("%s", asCommandlineParams[n]->pszFlag);

		printf(" %s", getTypeName(asCommandlineParams[n]->eParamType));
		if ( asCommandlineParams[n]->nElem > 1 )
		{
			printf("x%d", asCommandlineParams[n]->nElem);
		}
			
		printf("\t: %s\n", asCommandlineParams[n]->pszDesc);
	}
	printf("\n");
}

IMG_RESULT DYNCMD_AddCommandLine(int argc, char *argv[], const char *pszSrc)
{
	IMG_UINT32 lenPszSrc;
	IMG_INT i;

	g_bCmdAdded = IMG_TRUE;
	lenPszSrc = strlen(pszSrc);

	if ( lenPszSrc != 0 )
	{
		char *tmp;
		/** @warning give a tmp value for register param not to trigger the error when it's NULL.
		 * This value is not going to be found: no parameters are registered yet.
		 * No danger of segmentation fault later because parameters cannot be searched for later.
		 */
		// added to appear in the usage()
		DYNCMD_RegisterParameter(pszSrc, DYNCMDTYPE_STRING, "to load the configuration from a text file", &tmp);
	}

	// do not use the program name as a parameter
	for ( i = 1 ; i < argc && g_nCmd < MAX_PARAMS ; i++ )
	{
		if ( lenPszSrc != 0 && strncmp(pszSrc, argv[i], lenPszSrc) == 0 )
		{
			if ( i+1 < argc ) // there is a file
			{
				i++; // next

				if ( DYNCMD_AddFile(argv[i], "#") != IMG_SUCCESS )
				{
					fprintf(stderr, "failed to source file '%s'\n", argv[i]);
					return IMG_ERROR_INVALID_PARAMETERS;
				}
			}
			else
			{
				fprintf(stderr, "parameter '%s' needs a file path as value\n", pszSrc);
				return IMG_ERROR_INVALID_PARAMETERS;
			}
		}
		else // normal param
		{
			int argvLen = strlen(argv[i]);
			g_CmdList[g_nCmd] = (IMG_CHAR*)IMG_MALLOC(argvLen+1);
			strncpy(g_CmdList[g_nCmd], argv[i], argvLen);
			g_CmdList[g_nCmd][argvLen] = '\0';

			g_nCmd++;
		}
	}

	if ( g_nCmd >= MAX_PARAMS )
	{
		fprintf(stderr, "MAX_PARAMS(%d) is not big enought - recompile with larger one\n", MAX_PARAMS);
		return IMG_ERROR_FATAL;
	}
	return IMG_SUCCESS;
}

/// @brief delimitor used for strtok
#define DELIM " \t\r\n"
IMG_RESULT DYNCMD_AddFile(const char *pszFileName, const char *pszComment)
{
	FILE *pFile;
	IMG_CHAR lineBuffer[MAX_LINE];

	if (pszFileName == NULL)
	{
		return IMG_ERROR_INVALID_PARAMETERS;
	}

	if ( pszComment == NULL )
	{
		return IMG_ERROR_INVALID_PARAMETERS;
	}

	if ( (pFile = fopen(pszFileName, "r")) == NULL )
	{
		fprintf(stderr, "failed to open file '%s' - errno %d\n", pszFileName, errno);
		return IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE;
	}

	g_bCmdAdded = IMG_TRUE;

	while ( !feof(pFile) )
	{
		char *token;
		if ( fgets(lineBuffer, MAX_LINE-1, pFile) == NULL ) // could not read
			break; // it's the end

		// get a token from a string
		// never returns an empty token - nor a token with a delim character
		// to continue parsing the same string next call uses NULL
		token = strtok(lineBuffer, DELIM);
		while(token != NULL && strncmp(token, pszComment, strlen(pszComment)) != 0)
		{
			int argvLen = strlen(token);
			
			// add to the list of arguments
			g_CmdList[g_nCmd] = (IMG_CHAR*)IMG_MALLOC(argvLen+1);
			strncpy(g_CmdList[g_nCmd], token, argvLen);
			g_CmdList[g_nCmd][argvLen] = '\0';

			g_nCmd++;

			// get next token
			token = strtok(NULL, DELIM);
		}
	}

	fclose(pFile);
	return IMG_SUCCESS;
}
#undef DELIM

void DYNCMD_ReleaseParameters()
{
	int n=0;
	for(n=0;n<g_nParams;n++)
	{
		IMG_FREE(asCommandlineParams[n]->pszFlag);
		IMG_FREE(asCommandlineParams[n]);
		asCommandlineParams[n]=0;
	}
	g_nParams=0;
	for (n=0;n<g_nCmd;n++)
	{
		IMG_FREE(g_CmdList[n]);
		g_CmdList[n]=0;
	}
	g_nCmd=0;
	g_bCmdAdded = IMG_FALSE;
}

#ifdef DOXYGEN_CREATE_MODULES
 /*
  * @}
  */
/*-------------------------------------------------------------------------
 * end of the IMGLIB_DYNCMD documentation module
 *------------------------------------------------------------------------*/
#endif
