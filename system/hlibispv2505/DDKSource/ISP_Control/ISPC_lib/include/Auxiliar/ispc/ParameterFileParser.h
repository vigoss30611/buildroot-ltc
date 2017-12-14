/**
*******************************************************************************
 @file ParameterFileParser.h

 @brief ISPC::ParameterFileParser declaration

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
#ifndef ISPC_AUX_PARAMETER_FILE_PARSER_H_
#define ISPC_AUX_PARAMETER_FILE_PARSER_H_

#include <string>
#include <ostream>
#include <vector>

#include "ispc/ParameterList.h"

namespace ISPC {

class ParameterList;
/**
 * @brief Can be used for reading a parameter file.
 *
 * It will generate a ParameterList instance populated with the parameters
 * read from a given filename.
 * See @ref parseFile and @ref ParameterList
 */
class ParameterFileParser
{
public:
    /**
     * @brief Parses a given istream and fills a given ParameterList object
     * with the parameters and values parsed from the fileStream
     *
     * @param fileStream istream with parameters strings
     * @param params reference to ParameterList to be populated with the
     * parameters parsed from the file
     *
     * @return error status
     */
    static IMG_RESULT parse(std::istream &fileStream,
            ParameterList &params);
    /**
     * @brief Parses a given file and creates a ParameterList object with
     * the parameters and values parsed from the file
     *
     * @param filename Name of the file to be parsed
     *
     * @return ParameterList instance populated with the parameters parsed
     * from the filename
     */
    static ParameterList parseFile(const std::string &filename);

    /**
     * @brief Splits a file line in 'tokens', basically tags and values
     * separated by spaces, tabs, etc.
     *
     * @param line A string with a whole line from a file
     *
     * @return A string vector with a string for each token parsed
     */
    static std::vector<std::string> getTokens(const std::string &line);

    /**
     * @brief Checks if a given token is a comment or not. It is considered
     * a comment if it starts with # or //
     *
     * @return true if the token is a comment, false otherwise.
     */
    static bool isComment(const std::string &token);

    /**
     * @brief Checks if the given token is a valid tag. A tag should start
     * with a letter followed by any combination of letters, digits
     * and '_' or '-'
     *
     * @return true if the token is a valid tag, false otherwise.
     */
    static bool isValidTag(const std::string &token);

    /** @brief Save a ParameterList to a ostream (does not group parameters) */
    static IMG_RESULT save(const ParameterList &parameters,
        std::ostream &fileStream);

    /** @brief Save a ParameterList to a file (does not group parameters) */
    static IMG_RESULT save(const ParameterList &parameters,
        const std::string &filename);

    /**
     * @brief A less efficient saver but saves the parameters in groups first
     *
     * @param parameters
     * @param fileStream output stream
     * @param bSaveUngroupped Ungroupped parameters are also saved (longer)
     */
    static IMG_RESULT saveGrouped(const ParameterList &parameters,
        std::ostream &fileStream, bool bSaveUngroupped = true);

    /**
     * @brief A less efficient saver but saves the parameters in groups first
     *
     * @param parameters
     * @param filename
     * @param bSaveUngroupped Ungroupped parameters are also saved (longer)
     */
    static IMG_RESULT saveGrouped(const ParameterList &parameters,
        const std::string &filename, bool bSaveUngroupped = true);
};

} /* namespace ISPC */

#endif /* ISPC_AUX_PARAMETER_FILE_PARSER_H_ */
