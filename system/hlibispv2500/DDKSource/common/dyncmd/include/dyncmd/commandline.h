/**
******************************************************************************
 @file commandline.h

 @brief Parse a list of parameters generated from command line or simple text file.

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

#ifndef _DYNCMD_
#define _DYNCMD_

#ifdef DOXYGEN_CREATE_MODULES
/**
 * @defgroup IMGLIB_DYNCMD IMGLIB DynCMD: Dynamic Command line and file parser
 * @brief Command line and file parser that creates the list of arguments dynamically.
 *
 * The usage of the library is in 3 steps:
 * @li add a source of parameters with DYNCMD_AddCommandLine() or DYNCMD_AddFile()
 * @li register each parameters dynamically DYNCMD_RegisterParameter()
 * @li clean DYNCMD_ReleaseParameters()
 *
 * Parsing the command line with DYNCMD_AddCommandLine() also allows a text file to be <i>sourced</i> for additional parameters.
 * 
 * The text file format is plain text, the # character is used for comments (the whole line until \n).
 * More than one parameter is allowed per line but there is a maximum number of character per line MAX_LINE (usually 255) that can be changed at compilation time.
 *
 * "dynamic" declaration of parameters means that parameters are registered while running (allowing a tree to be built according to the currently active options).
 * The limitation of such approach is that it is not easy to check that the command is correct (it is actually not checked).
 * Moreover printing the help is only printing what is registered - i.e. can vary according to the chosen options.
 * 
 * The advantage is that only parameters that are used are searched for.
 * The help printing is also relative to your chosen options.
 *
 * Possible improvements could be the usage of a linked list for storing commands and parameters.
 * This would allow the removal of duplicate commands or found parameters.
 * eg: list of command is {"-foo" "1" "-bar" "-foo" "2"}, when registering foo the last elements {"-foo" "2"} can be taken out of the list as well as their duplicates.
 * The list would therefore be {"-bar"}, i.e. the list of unregistered commands.
 * This will also prevent the {"-foo" "-bar" "-foo" "1"} list to be mistaken for correct (the 1st foo should have an int parameter)
 *
 * See the usage example @ref dyncmd_example.c
 * @example dyncmd_example.c
 *
We want an executable that has the following usage:
 * @li -source to load the configuration from a text file
 * @li -format flag to choose format: 1=MIPI 0=BT656
 * @li -MIPI_LF activate MIPI Line Flag - valid only for MIPI format - optional
 * @li -h print help
 *
 * @subsection usage Print usage
 *
 * Printing the usage depends on the parameter your registered.
 *
 * If some parameters are registered only when some others are activated it creates the tree of dependencies.
 *
 * The only limitation of such design is that it is not possible to get the whole usage printed of options are mutually exclusives.
 *
 * Given no parameters the result is:
@verbatim
usage()
could not find the format
-source         : to load the configuration from a text file
-format         : flag to choose format: 1=MIPI 0=BT656
-h      : print help
@endverbatim
 *
 * Given the parameters
@verbatim -format 1 @endverbatim
 the result is:
@verbatim
format to use MIPI
  MIPI_LF=no
@endverbatim
 *
 * Given the parameters
@verbatim -format 1 -h @endverbatim
 the result is:
@verbatim
usage()
-source         : to load the configuration from a text file
-format         : flag to choose format: 1=MIPI 0=BT656
-MIPI_LF        : activate MIPI Line Flag - valid only for MIPI format - optional
-h      : print help
@endverbatim
 *
 * @subsection overwrite Overwrite parameters
 *
 * The last parameter of the list is taken.
 * This applies to using text files to get command line.
 *
 * Given the file MIPI.txt
@verbatim
# configuration values when using MIPI
-format 1
-MIPI_LF 0
@endverbatim
 and the paramters
@verbatim
-format 0 -source MIPI.txt -MIPI_LF 1 -unregistered
@endverbatim
 the result is:
@verbatim
format to use MIPI
  MIPI_LF=yes
@endverbatim
 *
 * Sourcing this file is equivalent of writing the command line:
@verbatim -format 0 -format 1 -MIPI_LF 0 -MIPI_LF 1 -unregistered
 * 
 * @{
 */
/*-------------------------------------------------------------------------
 * Following elements are in the IMGLIB_FILEIO documentation module
 *------------------------------------------------------------------------*/
#endif // DOXYGEN_CREATE_MODULES

#include <img_types.h>
#include <img_defs.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Available parameters types
 */
typedef enum _eDYNCMDLineTypes_
{
	DYNCMDTYPE_STRING = 0,	/**< @brief Command followed by a string */
	DYNCMDTYPE_BOOL8,		/**< @brief Command followed by a boolean value (0,1) @warning IMG_BOOL8 MUST be used*/
	DYNCMDTYPE_FLOAT,		/**< @brief Command followed by a floating point value */
	DYNCMDTYPE_UINT,		/**< @brief Command followed by an unsigned int */
	DYNCMDTYPE_INT,			/**< @brief Command followed by a signed int */
	DYNCMDTYPE_COMMAND,		/**< @brief Simple command with no other parameter */
}DYNCMDLINE_TYPE;


typedef enum _eDYNCMDLineCodes_
{
	RET_FOUND = 0,		/**< @brief parameter found */
	RET_NOT_FOUND=1,	/**< @brief parameter not found */
	RET_INCORRECT,		/**< @brief parameter found but its definition was incorrect (e.g. requiring an int but having none) */
	RET_INCOMPLETE,		/**< @brief an array parameter was incomplete: some parameter were found but not all (e.g. 1st element found but not 2nd) */
	RET_ERROR,			/**< @brief other error */
}DYNCMDLINE_RETURN_CODE;

/**
 *
 * @param pszParam string to extract object from - not const to support the case when eType is STRINGx
 *
 * @warning Floating point parameter registered as an int will take only the first digits (e.g. -1.5 will read -1)
 * @warning Floating points values can be retrieved using ".2" or "-0.2" but not "-.2"
 *
 * @param n is used to move to that offset after pvData is casted to the correct type (e.g. ((IMG_INT*)pvData)[n])
 *
 */
DYNCMDLINE_RETURN_CODE DYNCMD_getObjectFromString(DYNCMDLINE_TYPE eType, const char* pszParam, void *pvData);

DYNCMDLINE_RETURN_CODE DYNCMD_getArrayFromString(DYNCMDLINE_TYPE eType, const char* pszParam, void *pvData, int n);

/**
 * @brief Add a parameter to the list of parameters to check for on the commandline.
 *
 * @note If the parameter is added twice the global structure is updated and the previous element is lost
 *
 * @copydoc DYNCMD_getObjectFromString
 *
 * @param pszPattern Pattern which describes the parameter e.g. "-v"
 * @param eType The type of parameter being handled (string int etc.) - also defines the parameter size in bytes!
 * @warning if the parameter is a string keep in mind that the string is DUPLICATED and that previous elements in pvData are overwritten
 * @param pszDesc Description of the parameter
 * @param pvData pointer to store the data if it exists in the commandline
 * @param argc number of commandline parameters in argv list
 * @param argv list of commandline parameters
 *
 * @return RET_FOUND if the parameter was found on the commandline
 * @return RET_ERROR if the parameter caused an error of some sort (e.g.  type information)
 * @return RET_NOT_FOUND if the parameter is not found on the commandline
 *
 */
DYNCMDLINE_RETURN_CODE DYNCMD_RegisterParameter(const char *pszPattern, DYNCMDLINE_TYPE eType, const char *pszDesc, void *pvData);

/**
 * @brief Similar to DYNCMD_RegisterParameter() but for an array (multiple parameters)
 *
 * @return RET_ERROR if type is commands or arrays of string is not supported and will return RET_ERROR too
 */
DYNCMDLINE_RETURN_CODE DYNCMD_RegisterArray(const char *pszPattern, DYNCMDLINE_TYPE eType, const char *pszDesc, void *pvData, int uiNElem);

/**
 * @brief To know if a command line has unregistered elements - prints 1 line per unknown parameter
 *
 * @warning Does not verify that a parameter expecting a value is followed by the correct value
 *
 * @Input argc number of elements in argv
 * @Input argv command line
 * @Output pui32Result number of unregistered elements (>0 if the command line has unregistered elements)
 *
 * @return IMG_SUCCESS if the command line was parsed succesfully
 * @return IMG_ERROR_INVALID_PARAMETER if the argc is lower than 1 or pbResult is NULL
 * @return IMG_ERROR_UNEXPECTED_STATE if no parameter are yet registered
 */
IMG_RESULT DYNCMD_HasUnregisteredElements(IMG_UINT32 *pui32Result);

/**
 * @brief Prints out the descriptions of all the registered parameters
 */
void DYNCMD_PrintUsage();

/**
 * @brief Frees all the memory allocated by the commandline parser
 */
void DYNCMD_ReleaseParameters();

/**
 * @brief Add a command line as source of the parameters.
 *
 * Register a command line with the possibility to source text files from it.
 * The pszSrc is the argument name that requieres a path. That path is used by calling DYNCMD_AddFile()
 *
 * The last given parameter is the one used when searching for a value
 *
 * @param argc number of arguments
 * @param argv the argument list
 * @param pszSrc argument that is considered as 'source' for a file
 */
IMG_RESULT DYNCMD_AddCommandLine(int argc, char *argv[], const char *pszSrc);

/**
 * @brief Add a file as command line source
 *
 * @note can be called explicitely or via the DYNCMD_AddCommandLine()
 */
IMG_RESULT DYNCMD_AddFile(const char *pszFileName, const char *pszComment);

#ifdef __cplusplus
}
#endif

#ifdef DOXYGEN_CREATE_MODULES
 /*
  * @}
  */
/*-------------------------------------------------------------------------
 * end of the IMGLIB_DYNCMD documentation module
 *------------------------------------------------------------------------*/
#endif

#endif //_DYNCMD_
