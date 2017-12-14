/**
*******************************************************************************
@file ParameterFileParser.cpp

@brief ISPC::ParameterFileParser implementation

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
#define LOG_TAG "ISPC_FILEPARSER"

#include "ispc/ParameterFileParser.h"
#include <img_errors.h>
#include <felixcommon/userlog.h>

#include <fstream>
#include <cctype>  // isalpha()
#include <map>
#include <string>
#include <vector>
#include <set>

#define N_COMMENTS 2
static const std::string commentStr[N_COMMENTS] = {
    "//",
    "#"
};

ISPC::ParameterList ISPC::ParameterFileParser::parseFile(
    const std::string &filename)
{
    ParameterList params;
    unsigned int i = 0;

    std::ifstream file;
    file.open(filename.c_str(), std::ios::in);
    if (!file.is_open())
    {
        LOG_ERROR("Failed to open file: %s\n", filename.c_str());
        params.validFlag = false;  // No valid parameter list
        return params;  // return an empty parameter list
    }

    while (!file.eof())
    {
        std::string line;
        getline(file, line);

        std::vector<std::string> tokens = getTokens(line);
        if (tokens.size() > 0)
        {
            // skip if is a comment line
            if (isComment(tokens[0]))
            {
                continue;
            }

            // check that the parameter name is valid
            if (!isValidTag(tokens[0]))
            {
                LOG_DEBUG("Token %s is not valid - skipping\n",
                    tokens[0].c_str());
                continue;
            }

            // where are comments begin
            for (i = 1; i < tokens.size(); i++)
            {
                if (isComment(tokens[i]))
                {
                    break;
                }
            }

            // Add all the parsed values in a new parameter
            if (i > 1)
            {
                Parameter newParameter(tokens[0], tokens[1]);
                for (unsigned int j = 2; j < i; j++)
                {
                    newParameter.addValue(tokens[j]);
                }

                // Add the just created parameter in the parameter list
                params.addParameter(newParameter, true);
            }
        }
    }

    return params;
}

#define EXCLUDE_CHAR " \t\n\r"
std::vector<std::string> ISPC::ParameterFileParser::getTokens(
    const std::string &line)
{
    std::vector<std::string> tokens;
    size_t tokenStart = 0;
    size_t tokenEnd = 0;
    bool flagComment = false;

    while (tokenStart < line.length())
    {
        tokenStart = line.find_first_not_of(EXCLUDE_CHAR, tokenStart);

        // if found nothing
        if (tokenStart == std::string::npos)
        {
            break;
        }

        tokenEnd = line.find_first_of(EXCLUDE_CHAR, tokenStart);

        std::string token = line.substr(tokenStart, tokenEnd - tokenStart);

        // Check that the token doesn't contain comments
        flagComment = isComment(token);

        if (token.length() > 0)
        {
            if (1 == token.length())
            {
                if (token[0] != '\n' && token[0] != '\r')
                {
                    tokens.push_back(token);
                }
            }
            else
            {
                tokens.push_back(token);
            }
        }
        else
        {
            break;  // end of line
        }

        // end of the line or comment found
        if (tokenEnd == std::string::npos || flagComment)
        {
            break;
        }
        tokenStart = tokenEnd;
    }

    return tokens;
}
#undef EXCLUDE_CHAR

bool ISPC::ParameterFileParser::isComment(const std::string &token)
{
    for (int c = 0; c < N_COMMENTS; c++)
    {
        if (token.size() >= commentStr[c].size()
            && 0 == commentStr[c].compare(0, commentStr[c].size(), token))
        {
            return true;
        }
    }
    return false;
}

bool ISPC::ParameterFileParser::isValidTag(const std::string &token)
{
    // first character must be a letter
    if (!isalpha(token[0]))
    {
        return false;
    }

    for (std::string::const_iterator it = token.begin(); it != token.end();
        it++)
    {
        if (!isalpha(*it) && !isdigit(*it) && (*it) != '_' && (*it) != '-')
        {
            return false;
        }
    }

    return true;
}

IMG_RESULT ISPC::ParameterFileParser::save(const ParameterList &parameters,
    std::ostream &file)
{
    std::map<std::string, Parameter>::const_iterator it = parameters.begin();

    while (it != parameters.end())
    {
        if (!it->first.empty() && it->second.size() > 0)
        {
            std::vector<std::string>::const_iterator jt = it->second.begin();
            file << it->first;
            while (jt != it->second.end())
            {
                file << " " << *jt;
                jt++;
            }
            file << std::endl;
        }
        else
        {
            LOG_WARNING("Failed to save parameter '%s' which has %d values\n",
                it->first.c_str(), it->second.size());
        }

        it++;
    }

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ParameterFileParser::save(const ParameterList &parameters,
    const std::string &filename) {
    IMG_RESULT res = IMG_ERROR_FATAL;
    std::ofstream file;
    file.open(filename.c_str(), std::ios::out);
    if (!file.fail()) {
        res = save(parameters, file);
    }
    file.close();
    return res;
}

IMG_RESULT ISPC::ParameterFileParser::saveGrouped(
    const ParameterList &parameters, std::ostream &file,
    bool bSaveUngroupped)
{
    std::map<std::string, ParameterGroup>::const_iterator groupIt =
        parameters.groupsBegin();
    std::map<std::string, Parameter>::const_iterator paramIt;

    while (groupIt != parameters.groupsEnd())
    {
        std::set<std::string>::const_iterator it =
            groupIt->second.parameters.begin();

        file << groupIt->second.header << std::endl;

        while (it != groupIt->second.parameters.end() && !file.fail())
        {
            const Parameter *p = parameters.getParameter(*it);
            if (p)
            {
                file << p->getTag();
                for (unsigned int i = 0; i < p->size(); i++)
                {
                    file << " " << p->getString(i);
                }
                file << std::endl;
            }
            /* else the group created the parameter but it is not in the list
             * (e.g. the list has been cleared since) */

            it++;  // next parameter
        }
        file << std::endl;

        groupIt++;  // next group
    }

    if (bSaveUngroupped)
    {
        bool first = true;
        paramIt = parameters.begin();

        file << std::endl;

        while (paramIt != parameters.end() && !file.fail())
        {
            if (!paramIt->first.empty() && paramIt->second.size() > 0
                && !parameters.isInGroup(paramIt->first))
            {
                std::vector<std::string>::const_iterator jt =
                    paramIt->second.begin();

                if (first)
                {
                    file << "// Ungrouped parameters" << std::endl;
                    first = false;
                }

                file << paramIt->first;
                while (jt != paramIt->second.end())
                {
                    file << " " << *jt;
                    jt++;
                }
                file << std::endl;
            }

            paramIt++;
        }
    }

    return IMG_SUCCESS;
}

IMG_RESULT ISPC::ParameterFileParser::saveGrouped(
    const ParameterList &parameters, const std::string &filename,
    bool bSaveUngroupped)
{
    IMG_RESULT res = IMG_ERROR_FATAL;
    std::ofstream file;
    file.open(filename.c_str(), std::ios::out);
    if (!file.fail()) {
        res = saveGrouped(parameters, file, bSaveUngroupped);
    }
    file.close();
    return res;
}
