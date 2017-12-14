/**
*******************************************************************************
 @file ParameterFileParser_test.cpp

 @brief Test ParameterFileParser class

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
#include <ispc/ParameterFileParser.h>
#include <gtest/gtest.h>
#include <fstream>

#undef VERBOSE

#ifndef VERBOSE
#undef printf
#define printf(...)
#endif /* VERBOSE */

namespace {
const std::string whitespaces(" \n\t\r");
const std::vector<std::string> comments({"//", "#"});
}

/**
 * @brief test ISPC::ParameterFileParser::getTokens()
 */
TEST(ParameterFileParser, getTokens)
{
    std::vector<std::string> tokens;

    EXPECT_EQ(ISPC::ParameterFileParser::getTokens("").size(), 0);
    for(auto whitechar : whitespaces)
    {
        {
            std::stringstream line;
            // check single word
            line << "word1" << std::endl;
            tokens = ISPC::ParameterFileParser::getTokens(line.str());
            EXPECT_EQ(1, tokens.size());
            EXPECT_EQ("word1", tokens[0]);

            line << whitechar;
            tokens = ISPC::ParameterFileParser::getTokens(line.str());
            EXPECT_EQ(1, tokens.size());
            EXPECT_EQ("word1", tokens[0]);
        }
        // validate parsing not commented lines
        {
            std::stringstream line;
            // check whitespace at the beginning of line
            // notice that even using '\n' as the whitespace the tokens are
            // still counted
            line << whitechar;
            tokens = ISPC::ParameterFileParser::getTokens(line.str());
            EXPECT_EQ(0, tokens.size());

            line << "word1";
            tokens = ISPC::ParameterFileParser::getTokens(line.str());
            EXPECT_EQ(1, tokens.size());
            EXPECT_EQ("word1", tokens[0]);

            line << whitechar;
            tokens = ISPC::ParameterFileParser::getTokens(line.str());
            EXPECT_EQ(1, tokens.size());
            EXPECT_EQ("word1", tokens[0]);

            line << "word2";
            tokens = ISPC::ParameterFileParser::getTokens(line.str());
            EXPECT_EQ(2, tokens.size());
            EXPECT_EQ("word1", tokens[0]);
            EXPECT_EQ("word2", tokens[1]);

            line << "/";
            tokens = ISPC::ParameterFileParser::getTokens(line.str());
            EXPECT_EQ(2, tokens.size());
            EXPECT_EQ("word1", tokens[0]);
            EXPECT_EQ("word2/", tokens[1]);

            line << whitechar;
            tokens = ISPC::ParameterFileParser::getTokens(line.str());
            EXPECT_EQ(2, tokens.size());
            EXPECT_EQ("word1", tokens[0]);
            EXPECT_EQ("word2/", tokens[1]);

            line << "word3";
            tokens = ISPC::ParameterFileParser::getTokens(line.str());
            EXPECT_EQ(3, tokens.size());
            EXPECT_EQ("word1", tokens[0]);
            EXPECT_EQ("word2/", tokens[1]);
            EXPECT_EQ("word3", tokens[2]);

            printf("checked \"%s\"\n", line.str().c_str());
        }

        // validate parsing of commented lines
        for(auto comment : comments)
        {
            // two cases : whitechar and no whitechar right before comment
            for(auto spaceBeforeComment:
                    std::vector<std::string>({"", std::string(1, whitechar)}))
            {
                // no tag, only comment here
                {
                    std::stringstream line;
                    std::stringstream commentstr;
                    // check whitespace at the beginning of line
                    // notice that even using '\n' as the whitespace the tokens are
                    // still counted
                    line << spaceBeforeComment;
                    tokens = ISPC::ParameterFileParser::getTokens(line.str());
                    EXPECT_EQ(0, tokens.size());

                    commentstr << comment;
                    line << comment;
                    tokens = ISPC::ParameterFileParser::getTokens(line.str());
                    EXPECT_EQ(1, tokens.size());
                    EXPECT_EQ(commentstr.str(), tokens[0]);

                    commentstr << whitechar;
                    line << whitechar;
                    tokens = ISPC::ParameterFileParser::getTokens(line.str());
                    EXPECT_EQ(1, tokens.size());
                    EXPECT_EQ(commentstr.str(), tokens[0]);

                    commentstr << "comment1";
                    line << "comment1";
                    tokens = ISPC::ParameterFileParser::getTokens(line.str());
                    EXPECT_EQ(1, tokens.size());
                    EXPECT_EQ(commentstr.str(), tokens[0]);

                    commentstr << whitechar;
                    line << whitechar;
                    tokens = ISPC::ParameterFileParser::getTokens(line.str());
                    EXPECT_EQ(1, tokens.size());
                    EXPECT_EQ(commentstr.str(), tokens[0]);

                    commentstr << "comment2";
                    line << "comment2";
                    tokens = ISPC::ParameterFileParser::getTokens(line.str());
                    EXPECT_EQ(1, tokens.size());
                    EXPECT_EQ(commentstr.str(), tokens[0]);

                    printf("checked \"%s\"\n", line.str().c_str());
                }
                {
                    std::stringstream line;
                    std::stringstream commentstr;
                    // check whitespace at the beginning of line
                    // notice that even using '\n' as the whitespace the tokens are
                    // still counted
                    line << "word1";
                    tokens = ISPC::ParameterFileParser::getTokens(line.str());
                    EXPECT_EQ(1, tokens.size());
                    EXPECT_EQ("word1", tokens[0]);

                    line << spaceBeforeComment;
                    tokens = ISPC::ParameterFileParser::getTokens(line.str());
                    EXPECT_EQ(1, tokens.size());
                    EXPECT_EQ("word1", tokens[0]);

                    commentstr << comment;
                    line << comment;
                    tokens = ISPC::ParameterFileParser::getTokens(line.str());
                    EXPECT_EQ(2, tokens.size());
                    EXPECT_EQ("word1", tokens[0]);
                    EXPECT_EQ(commentstr.str(), tokens[1]);

                    commentstr << whitechar;
                    line << whitechar;
                    tokens = ISPC::ParameterFileParser::getTokens(line.str());
                    EXPECT_EQ(2, tokens.size());
                    EXPECT_EQ("word1", tokens[0]);
                    EXPECT_EQ(commentstr.str(), tokens[1]);

                    commentstr << "comment1";
                    line << "comment1";
                    tokens = ISPC::ParameterFileParser::getTokens(line.str());
                    EXPECT_EQ(2, tokens.size());
                    EXPECT_EQ("word1", tokens[0]);
                    EXPECT_EQ(commentstr.str(), tokens[1]);

                    commentstr << whitechar;
                    line << whitechar;
                    tokens = ISPC::ParameterFileParser::getTokens(line.str());
                    EXPECT_EQ(2, tokens.size());
                    EXPECT_EQ("word1", tokens[0]);
                    EXPECT_EQ(commentstr.str(), tokens[1]);

                    commentstr << "comment2";
                    line << "comment2";
                    tokens = ISPC::ParameterFileParser::getTokens(line.str());
                    EXPECT_EQ(2, tokens.size());
                    EXPECT_EQ("word1", tokens[0]);
                    EXPECT_EQ(commentstr.str(), tokens[1]);

                    printf("checked \"%s\"\n", line.str().c_str());
                }
            }
        }
    }
}

/**
 * @brief test ISPC::ParameterFileParser::isComment()
 */
TEST(ParameterFileParser, isComment)
{
    for(auto comment : comments)
    {
        for(auto whitechar : whitespaces)
        {
            // comment has to start from 0
            EXPECT_TRUE(ISPC::ParameterFileParser::isComment(comment));
            EXPECT_TRUE(ISPC::ParameterFileParser::isComment(comment+"abc"));
            EXPECT_TRUE(ISPC::ParameterFileParser::isComment(comment+"abc"+whitechar+"abc"));
            EXPECT_TRUE(ISPC::ParameterFileParser::isComment(comment+whitechar+"abc"));
            EXPECT_TRUE(ISPC::ParameterFileParser::isComment(comment+whitechar+"abc"+whitechar+"abc"));

            EXPECT_FALSE(ISPC::ParameterFileParser::isComment("a"+comment));
            EXPECT_FALSE(ISPC::ParameterFileParser::isComment("a"+comment+"abc"));
            EXPECT_FALSE(ISPC::ParameterFileParser::isComment("a"+comment+"abc"+whitechar+"abc"));
            EXPECT_FALSE(ISPC::ParameterFileParser::isComment("a"+comment+whitechar+"abc"));
            EXPECT_FALSE(ISPC::ParameterFileParser::isComment("a"+comment+whitechar+"abc"+whitechar+"abc"));

            EXPECT_FALSE(ISPC::ParameterFileParser::isComment(whitechar+comment));
            EXPECT_FALSE(ISPC::ParameterFileParser::isComment(whitechar+comment+"abc"));
            EXPECT_FALSE(ISPC::ParameterFileParser::isComment(whitechar+comment+"abc"+whitechar+"abc"));
            EXPECT_FALSE(ISPC::ParameterFileParser::isComment(whitechar+comment+whitechar+"abc"));
            EXPECT_FALSE(ISPC::ParameterFileParser::isComment(whitechar+comment+whitechar+"abc"+whitechar+"abc"));
        }
    }
}

/**
 * @brief ISPC::ParameterFileParser::isValidTag()
 */
TEST(ParameterFileParser, isValidTag)
{
    EXPECT_TRUE(ISPC::ParameterFileParser::isValidTag("abc1"));
    EXPECT_TRUE(ISPC::ParameterFileParser::isValidTag("abc1_def"));
    EXPECT_TRUE(ISPC::ParameterFileParser::isValidTag("abc1-def"));
    EXPECT_TRUE(ISPC::ParameterFileParser::isValidTag("ABC1"));
    EXPECT_TRUE(ISPC::ParameterFileParser::isValidTag("ABC1_DEF"));
    EXPECT_TRUE(ISPC::ParameterFileParser::isValidTag("ABC_DEF_0"));
    EXPECT_TRUE(ISPC::ParameterFileParser::isValidTag("ABC1_def"));
    EXPECT_TRUE(ISPC::ParameterFileParser::isValidTag("ABC1-def"));

    EXPECT_FALSE(ISPC::ParameterFileParser::isValidTag("1abc"));
    EXPECT_FALSE(ISPC::ParameterFileParser::isValidTag("_abc"));
    EXPECT_FALSE(ISPC::ParameterFileParser::isValidTag(" abc"));
    EXPECT_FALSE(ISPC::ParameterFileParser::isValidTag("-abc"));
    EXPECT_FALSE(ISPC::ParameterFileParser::isValidTag("_abc_def"));
    EXPECT_FALSE(ISPC::ParameterFileParser::isValidTag("-abc-def"));
    EXPECT_FALSE(ISPC::ParameterFileParser::isValidTag("_ABC"));
    EXPECT_FALSE(ISPC::ParameterFileParser::isValidTag("-ABC_def"));
    EXPECT_FALSE(ISPC::ParameterFileParser::isValidTag("_-ABC-def"));
    EXPECT_FALSE(ISPC::ParameterFileParser::isValidTag("!abc"));
}

namespace {

typedef struct {
    std::string tag;
    std::vector<std::string> values;
    bool valid;
    std::string comment;
    std::string group;
} testTags_t;

/**
 * @brief puts parameter line into ostream
 *
 * @note does not put newlines
 */
std::ostream& operator<< (std::ostream& tagfile, const testTags_t& param)
{
    tagfile << param.tag << " ";
    for(auto value : param.values)
    {
        tagfile << value << " ";
    }
    if(!param.comment.empty())
    {
        tagfile << " // " << param.comment;
    }

    return tagfile;
}

void compare(const testTags_t& vect, const ISPC::Parameter& param)
{
    EXPECT_EQ(vect.tag, param.getTag());
    EXPECT_EQ(vect.values.size(), param.getValues().size());
    EXPECT_EQ(vect.values, param.getValues());
}

const std::vector<testTags_t> testVectors(
{
    { "TAG_EMPTY_NOCOMMENT_VALID_NAME", {  }, false, "" },
    { "TAG_SINGLE_NOCOMMENT_VALID_NAME", { "1" }, true, "" },
    { "TAG_MULTIPLE_NOCOMMENT_VALID_NAME", { "1", "a" }, true, "" },

    { "_TAG_EMPTY_NOCOMMENT_INVALID_NAME", {  }, false, "" },
    { "1TAG_SINGLE_NOCOMMENT_INVALID_NAME", { "1" }, false, "" },
    { "TAG+_MULTIPLE_NOCOMMENT_INVALID_NAME", { "1", "a" }, false, "" },

    { "TAG_EMPTY_COMMENT_VALID_NAME", {  }, false, "abc" },
    { "TAG_SINGLE_COMMENT_VALID_NAME", { "1" }, true, "abc" },
    { "TAG_MULTIPLE_COMMENT_VALID_NAME", { "1", "a" }, true, "abc" },

    { "_TAG_EMPTY_COMMENT_INVALID_NAME", {  }, false, "abc" },
    { "1TAG_SINGLE_COMMENT_INVALID_NAME", { "1" }, false, "abc" },
    { "TAG+_MULTIPLE_COMMENT_INVALID_NAME", { "1", "a" }, false, "abc" },

    // just empty line
    { "", {  }, false, "" },
    // single value
    { "", { "1" }, false, "" },
    // two values
    { "", { "1", "2" }, false, "" },

    // only comment
    { "", {  }, false, "abc" },
    // value and comment
    { "", { "1" }, false, "abc" },
    { "", { "1", "2" }, false, "abc" },

    { "TAG_EMPTY_NOCOMMENT_VALID_NAME_GROUP1", {  }, false, "", "Group1" },
    { "TAG_SINGLE_NOCOMMENT_VALID_NAME_GROUP1", { "1" }, true, "", "Group1" },
    { "TAG_MULTIPLE_NOCOMMENT_VALID_NAME_GROUP1", { "1", "a" }, true, "", "Group1" },

    { "_TAG_EMPTY_NOCOMMENT_INVALID_NAME_GROUP1", {  }, false, "", "Group1" },
    { "1TAG_SINGLE_NOCOMMENT_INVALID_NAME_GROUP1", { "1" }, false, "", "Group1" },
    { "TAG+_MULTIPLE_NOCOMMENT_INVALID_NAME_GROUP1", { "1", "a" }, false, "", "Group1" },

    { "TAG_EMPTY_COMMENT_VALID_NAME_GROUP1", {  }, false, "abc", "Group1" },
    { "TAG_SINGLE_COMMENT_VALID_NAME_GROUP1", { "1" }, true, "abc", "Group1" },
    { "TAG_MULTIPLE_COMMENT_VALID_NAME_GROUP1", { "1", "a" }, true, "abc", "Group1" },

    { "_TAG_EMPTY_COMMENT_INVALID_NAME_GROUP1", {  }, false, "abc", "Group1" },
    { "1TAG_SINGLE_COMMENT_INVALID_NAME_GROUP1", { "1" }, false, "abc", "Group1" },
    { "TAG+_MULTIPLE_COMMENT_INVALID_NAME_GROUP1", { "1", "a" }, false, "abc", "Group1" },

    { "TAG_EMPTY_NOCOMMENT_VALID_NAME_GROUP2", {  }, false, "", "Group2" },
    { "TAG_SINGLE_NOCOMMENT_VALID_NAME_GROUP2", { "1" }, true, "", "Group2" },
    { "TAG_MULTIPLE_NOCOMMENT_VALID_NAME_GROUP2", { "1", "a" }, true, "", "Group2" },

    { "_TAG_EMPTY_NOCOMMENT_INVALID_NAME_GROUP2", {  }, false, "", "Group2" },
    { "1TAG_SINGLE_NOCOMMENT_INVALID_NAME_GROUP2", { "1" }, false, "", "Group2" },
    { "TAG+_MULTIPLE_NOCOMMENT_INVALID_NAME_GROUP2", { "1", "a" }, false, "", "Group2" },

    { "TAG_EMPTY_COMMENT_VALID_NAME_GROUP2", {  }, false, "abc", "Group2" },
    { "TAG_SINGLE_COMMENT_VALID_NAME_GROUP2", { "1" }, true, "abc", "Group2" },
    { "TAG_MULTIPLE_COMMENT_VALID_NAME_GROUP2", { "1", "a" }, true, "abc", "Group2" },

    { "_TAG_EMPTY_COMMENT_INVALID_NAME_GROUP2", {  }, false, "abc", "Group2" },
    { "1TAG_SINGLE_COMMENT_INVALID_NAME_GROUP2", { "1" }, false, "abc", "Group2" },
    { "TAG+_MULTIPLE_COMMENT_INVALID_NAME_GROUP2", { "1", "a" }, false, "abc", "Group2" },

});

void fillParameters(
        const std::vector<testTags_t>& testVectors,
        ISPC::ParameterList& params)
{
    std::map<std::string, ISPC::ParameterGroup> groups;
    for(auto param : testVectors)
    {
        ISPC::Parameter tag(param.tag, param.values);
        tag.setInfo(param.comment);
        params.addParameter(tag);
        // add groups
        if(!param.group.empty())
        {
            // if a group does not exist, then new object is created and
            // reference returned
            groups[param.group].header = "//" + param.group;
            groups[param.group].parameters.insert(param.tag);
        }
    }
    for(auto group: groups)
    {
        params.addGroup(group.first, group.second);
    }
}

void parse(std::stringstream& tagfile)
{
    ISPC::ParameterList params;

    EXPECT_EQ(IMG_SUCCESS, ISPC::ParameterFileParser::parse(tagfile, params));

    // verify loaded parameters
    for(auto param : testVectors)
    {
        if(param.valid)
        {
            EXPECT_TRUE(params.exists(param.tag));
            ISPC::Parameter *found = params.getParameter(param.tag);
            ASSERT_TRUE(NULL!=found);
            compare(param, *found);
            // comment is currently not parsed
            //EXPECT_EQ(param.comment, found->getInfo());
        } else
        {
            EXPECT_FALSE(params.exists(param.tag));
            ASSERT_TRUE(NULL == params.getParameter(param.tag));
        }
    }
}

}

/**
 * @brief ISPC::ParameterFileParser::parse() unix line ending file
 */
TEST(ParameterFileParser, parse_unix)
{
    // firstly generate human readable tags
    std::stringstream tagfile;
    for(auto param : testVectors)
    {
        tagfile << param << '\n';
    }
    // then parse and compare with input data
    parse(tagfile);
}

/**
 * @brief ISPC::ParameterFileParser::parse() windows line ending file
 */
TEST(ParameterFileParser, parse_windows)
{
    std::stringstream tagfile;
    for(auto param : testVectors)
    {
        tagfile << param << "\r\n";
    }
    parse(tagfile);
}

/**
 * @brief ISPC::ParameterFileParser::save() test
 */
TEST(ParameterFileParser, save)
{
    // firstly fill in ParameterList object
    ISPC::ParameterList params;
    fillParameters(testVectors, params);

    // then save human readable tags to a stream
    std::stringstream tagfile;
    EXPECT_EQ(IMG_SUCCESS, ISPC::ParameterFileParser::save(params, tagfile));

    // finally read the tags and compare with input data
    // we could parse the saved tagfile but it may be enough to just parse()
    // and validate the parameters
    parse(tagfile);
}

/**
 * @brief ISPC::ParameterFileParser::save() test
 */
TEST(ParameterFileParser, save_grouped)
{
    // generate ParameterList with groups
    ISPC::ParameterList params;
    fillParameters(testVectors, params);

    // save human readable and grouped tags
    std::stringstream tagfile;
    EXPECT_EQ(IMG_SUCCESS, ISPC::ParameterFileParser::saveGrouped(params, tagfile));

    // we could parse the saved tagfile but it may be enough to just parse()
    // and validate the parameters
    parse(tagfile);
}

