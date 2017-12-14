/**
 @file commandline_test.cpp

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
#include <gtest/gtest.h>
#include <cstdlib>

#include <img_types.h>
#include <img_defs.h>
#include <img_errors.h>

#include <dyncmd/commandline.h>

#ifdef IMG_MALLOC_CHECK
IMG_UINT32 gui32Alloc = 0;
IMG_UINT32 gui32Free = 0;
#endif

extern "C" {
#include "commandline.c"
}

int main(int argc, char *argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

// add test below

TEST(HasUnregisteredElements, verif)
{
	char **argv;
	int argc = 5;
	char program[] = "prgm";
	char command[] = "-command";
	char cmdarg[] = "4";
	char help[] = "-help";
	char other[] = "-other";
	IMG_UINT32 ui32Res;

	argv = (char**)IMG_MALLOC(argc*sizeof(char*));
	argv[0] = program;
	argv[1] = command;
	argv[2] = cmdarg;
	argv[3] = help;
	argv[4] = other;
	
	ASSERT_EQ (IMG_SUCCESS, DYNCMD_AddCommandLine(argc, argv, "")) << "failed to load cmd";

	EXPECT_EQ (IMG_ERROR_UNEXPECTED_STATE, DYNCMD_HasUnregisteredElements(&ui32Res));

	EXPECT_EQ (RET_FOUND, DYNCMD_RegisterParameter("-command", DYNCMDTYPE_UINT, "test command", &ui32Res));

	EXPECT_EQ (IMG_SUCCESS, DYNCMD_HasUnregisteredElements(&ui32Res));
	EXPECT_EQ (2, ui32Res);

	EXPECT_EQ (RET_FOUND, DYNCMD_RegisterParameter("-help", DYNCMDTYPE_COMMAND, "test command", NULL));

	EXPECT_EQ (IMG_SUCCESS, DYNCMD_HasUnregisteredElements(&ui32Res));
	EXPECT_EQ (1, ui32Res);

	DYNCMD_ReleaseParameters();
	IMG_FREE(argv);
}

TEST(DYNCMD_getObjectFromString, param_int)
{
	IMG_INT i;

	EXPECT_EQ(RET_FOUND, DYNCMD_getObjectFromString(DYNCMDTYPE_INT, "1", &i));
	EXPECT_EQ (1, i);

	EXPECT_EQ(RET_FOUND, DYNCMD_getObjectFromString(DYNCMDTYPE_INT, "-1", &i));
	EXPECT_EQ (-1, i);

	EXPECT_EQ(RET_FOUND, DYNCMD_getObjectFromString(DYNCMDTYPE_INT, "0", &i));
	EXPECT_EQ (0, i);

	EXPECT_EQ(RET_FOUND, DYNCMD_getObjectFromString(DYNCMDTYPE_INT, "1.5", &i));
	EXPECT_EQ (1, i);

	EXPECT_EQ(RET_FOUND, DYNCMD_getObjectFromString(DYNCMDTYPE_INT, "-1.5", &i));
	EXPECT_EQ (-1, i);

	EXPECT_NE(RET_FOUND, DYNCMD_getObjectFromString(DYNCMDTYPE_INT, ".5", &i));
	EXPECT_NE(RET_FOUND, DYNCMD_getObjectFromString(DYNCMDTYPE_INT, "-.5", &i));
	EXPECT_NE(RET_FOUND, DYNCMD_getObjectFromString(DYNCMDTYPE_INT, "foo", &i));
}

TEST(DYNCMD_getObjectFromString, DISABLED_param_uint)
{

}

TEST(DYNCMD_getObjectFromString, param_float)
{
	IMG_FLOAT f;

	EXPECT_EQ(RET_FOUND, DYNCMD_getObjectFromString(DYNCMDTYPE_FLOAT, "1", &f));
	EXPECT_EQ (1, f);

	EXPECT_EQ(RET_FOUND, DYNCMD_getObjectFromString(DYNCMDTYPE_FLOAT, "-1", &f));
	EXPECT_EQ (-1, f);

	EXPECT_EQ(RET_FOUND, DYNCMD_getObjectFromString(DYNCMDTYPE_FLOAT, "0", &f));
	EXPECT_EQ (0, f);

	EXPECT_EQ(RET_FOUND, DYNCMD_getObjectFromString(DYNCMDTYPE_FLOAT, "1.5", &f));
	EXPECT_EQ (1.5f, f);

	EXPECT_EQ(RET_FOUND, DYNCMD_getObjectFromString(DYNCMDTYPE_FLOAT, "-1.5", &f));
	EXPECT_EQ (-1.5f, f);

	EXPECT_EQ(RET_FOUND, DYNCMD_getObjectFromString(DYNCMDTYPE_FLOAT, ".5", &f)) << ".5";
	EXPECT_EQ (0.5f, f);

	EXPECT_EQ(RET_FOUND, DYNCMD_getObjectFromString(DYNCMDTYPE_FLOAT, "-.5", &f)) << "-.5";
	EXPECT_EQ (-0.5f, f);

	EXPECT_NE(RET_FOUND, DYNCMD_getObjectFromString(DYNCMDTYPE_FLOAT, "foo", &f));
}

TEST(DYNCMD_getObjectFromString, param_string)
{
	IMG_CHAR *s = NULL;

	ASSERT_EQ(RET_FOUND, DYNCMD_getObjectFromString(DYNCMDTYPE_STRING, "1", &s));
	EXPECT_EQ (0, strcmp(s, "1"));
	IMG_FREE(s); s = NULL;
	
	ASSERT_EQ(RET_FOUND, DYNCMD_getObjectFromString(DYNCMDTYPE_STRING, "-1", &s));
	EXPECT_EQ (0, strcmp(s, "-1"));
	IMG_FREE(s); s = NULL;

	ASSERT_EQ(RET_FOUND, DYNCMD_getObjectFromString(DYNCMDTYPE_STRING, "0", &s));
	EXPECT_EQ (0, strcmp(s, "0"));
	IMG_FREE(s); s = NULL;

	ASSERT_EQ(RET_FOUND, DYNCMD_getObjectFromString(DYNCMDTYPE_STRING, "1.5", &s));
	EXPECT_EQ (0, strcmp(s, "1.5"));
	IMG_FREE(s); s = NULL;

	ASSERT_EQ(RET_FOUND, DYNCMD_getObjectFromString(DYNCMDTYPE_STRING, "-1.5", &s));
	EXPECT_EQ (0, strcmp(s, "-1.5"));
	IMG_FREE(s); s = NULL;

	ASSERT_EQ(RET_FOUND, DYNCMD_getObjectFromString(DYNCMDTYPE_STRING, ".5", &s)) << ".5";
	EXPECT_EQ (0, strcmp(s, ".5"));
	IMG_FREE(s); s = NULL;

	ASSERT_EQ(RET_FOUND, DYNCMD_getObjectFromString(DYNCMDTYPE_STRING, "-.5", &s)) << "-.5";
	EXPECT_EQ (0, strcmp(s, "-.5"));
	IMG_FREE(s); s = NULL;

	ASSERT_EQ(RET_FOUND, DYNCMD_getObjectFromString(DYNCMDTYPE_STRING, "foo", &s));
	EXPECT_EQ (0, strcmp(s, "foo"));
	IMG_FREE(s); s = NULL;
}

TEST(DYNCMD_getObjectFromString, DISABLED_param_bool)
{

}

TEST(RegisterParameter, startFrom1)
{
	char **argv;
	int argc = 2;
	char program[] = "prgm";
	char command[] = "-command";
	argv = (char**)malloc(argc*sizeof(char*));
	argv[0] = program;
	argv[1] = command;
	
	ASSERT_EQ (IMG_SUCCESS, DYNCMD_AddCommandLine(argc, argv, ""))<<"failed to load cmd";

	EXPECT_EQ (RET_NOT_FOUND, DYNCMD_RegisterParameter("prgm", DYNCMDTYPE_COMMAND, "test command", NULL));

	free(argv);
	DYNCMD_ReleaseParameters();
}

TEST(RegisterParameter, param_command)
{
	char **argv;
	int argc = 2;
	char program[] = "prgm";
	char command[] = "-command";
	argv = (char**)malloc(argc*sizeof(char*));
	argv[0] = program;
	argv[1] = command;
	
	ASSERT_EQ (IMG_SUCCESS, DYNCMD_AddCommandLine(argc, argv, "")) << "failed to load cmd";

	EXPECT_EQ (RET_FOUND, DYNCMD_RegisterParameter("-command", DYNCMDTYPE_COMMAND, "test command", NULL));
	EXPECT_EQ (RET_NOT_FOUND, DYNCMD_RegisterParameter("-supercommand", DYNCMDTYPE_COMMAND, "test supercommand", NULL));

	DYNCMD_ReleaseParameters();
	free(argv);
}

TEST(RegisterParameter, param_array)
{
	char **argv;
	int argc = 9;
	char program[] = "prgm";
	char command[] = "-array";
	char seccommand[] = "-array2";
	int intArray[3];
	argv = (char**)malloc(argc*sizeof(char*));
	argv[0] = program;
	argv[1] = command;
	argv[2] = "1";
	argv[3] = "2";
	argv[4] = "3";
	argv[5] = seccommand;
	argv[6] = "1";
	argv[7] = "2";
	argv[8] = "3";

	ASSERT_EQ (IMG_SUCCESS, DYNCMD_AddCommandLine(argc, argv, "")) << "failed to load cmd";

	EXPECT_EQ (RET_INCOMPLETE, DYNCMD_RegisterArray(command, DYNCMDTYPE_INT, "test command", intArray, 4));
	EXPECT_EQ (RET_INCOMPLETE, DYNCMD_RegisterArray(seccommand, DYNCMDTYPE_INT, "test command", intArray, 4));

	DYNCMD_ReleaseParameters();

	ASSERT_EQ (IMG_SUCCESS, DYNCMD_AddCommandLine(argc, argv, "")) << "failed to load cmd";

	EXPECT_EQ (RET_FOUND, DYNCMD_RegisterArray(command, DYNCMDTYPE_INT, "test command", intArray, 3));
	EXPECT_EQ (RET_FOUND, DYNCMD_RegisterArray(seccommand, DYNCMDTYPE_INT, "test command", intArray, 3));

	DYNCMD_ReleaseParameters();
	free(argv);
}

TEST(AddSource, DISABLED_cmdonly)
{

}

static void testCommands()
{
	char *buff = NULL;

	// find first non comment
	EXPECT_EQ (RET_FOUND, DYNCMD_RegisterParameter("command", DYNCMDTYPE_COMMAND, "first non comment", NULL));
	EXPECT_EQ (RET_FOUND, DYNCMD_RegisterParameter("command2", DYNCMDTYPE_COMMAND, "with a comment", NULL));
	EXPECT_EQ (RET_FOUND, DYNCMD_RegisterParameter("command3", DYNCMDTYPE_COMMAND, "with a leading space", NULL));
	EXPECT_EQ (RET_FOUND, DYNCMD_RegisterParameter("command4", DYNCMDTYPE_COMMAND, "with a leading tabular", NULL));

	// string
	EXPECT_EQ (RET_FOUND, DYNCMD_RegisterParameter("name", DYNCMDTYPE_STRING, "string", &buff));
	EXPECT_EQ (0, strcmp("super", buff));
	IMG_FREE(buff); buff = NULL;

	EXPECT_EQ (RET_FOUND, DYNCMD_RegisterParameter("super2", DYNCMDTYPE_STRING, "string", &buff));
	EXPECT_EQ (0, strcmp("name2", buff));
	IMG_FREE(buff); buff = NULL;

	EXPECT_EQ (RET_FOUND, DYNCMD_RegisterParameter("test", DYNCMDTYPE_STRING, "values", &buff));
	EXPECT_EQ (0, strcmp("values", buff));
	IMG_FREE(buff); buff = NULL;

	// several commands in one line
	EXPECT_EQ (RET_FOUND, DYNCMD_RegisterParameter("first", DYNCMDTYPE_COMMAND, "first non comment", NULL));
	EXPECT_EQ (RET_FOUND, DYNCMD_RegisterParameter("second", DYNCMDTYPE_COMMAND, "first non comment", NULL));
	EXPECT_EQ (RET_FOUND, DYNCMD_RegisterParameter("third", DYNCMDTYPE_COMMAND, "first non comment", NULL));
	EXPECT_EQ (RET_FOUND, DYNCMD_RegisterParameter("fourth", DYNCMDTYPE_COMMAND, "first non comment", NULL));
	EXPECT_EQ (RET_FOUND, DYNCMD_RegisterParameter("fith", DYNCMDTYPE_COMMAND, "first non comment", NULL));
	EXPECT_EQ (RET_FOUND, DYNCMD_RegisterParameter("sixth", DYNCMDTYPE_COMMAND, "first non comment", NULL));
	EXPECT_EQ (RET_FOUND, DYNCMD_RegisterParameter("seventh", DYNCMDTYPE_COMMAND, "first non comment", NULL));
	EXPECT_EQ (RET_FOUND, DYNCMD_RegisterParameter("eigth", DYNCMDTYPE_COMMAND, "first non comment", NULL));

	// other values
	{
		IMG_UINT ui;
		IMG_INT i;
		IMG_FLOAT f;
		IMG_BOOL8 b;

		EXPECT_EQ (RET_FOUND, DYNCMD_RegisterParameter("negint", DYNCMDTYPE_INT, "int", &i));
		EXPECT_EQ (-1, i);

		EXPECT_EQ (RET_FOUND, DYNCMD_RegisterParameter("posint", DYNCMDTYPE_INT, "int", &i));
		EXPECT_EQ (2, i);

		EXPECT_EQ (RET_FOUND, DYNCMD_RegisterParameter("nullint", DYNCMDTYPE_INT, "int", &i));
		EXPECT_EQ (0, i);

		EXPECT_EQ (RET_FOUND, DYNCMD_RegisterParameter("uint", DYNCMDTYPE_UINT, "uint", &ui));
		EXPECT_EQ (3, ui);

		EXPECT_EQ (RET_FOUND, DYNCMD_RegisterParameter("nulluint", DYNCMDTYPE_UINT, "uint", &ui));
		EXPECT_EQ (0, ui);

		EXPECT_EQ (RET_FOUND, DYNCMD_RegisterParameter("posfloat", DYNCMDTYPE_FLOAT, "float", &f));
		EXPECT_EQ (1.5f, f);

		EXPECT_EQ (RET_FOUND, DYNCMD_RegisterParameter("negfloat", DYNCMDTYPE_FLOAT, "float", &f));
		EXPECT_EQ (-1.5f, f);

		EXPECT_EQ (RET_FOUND, DYNCMD_RegisterParameter("superfloat", DYNCMDTYPE_FLOAT, "float", &f));
		EXPECT_EQ (0.5f, f);

		EXPECT_EQ (RET_FOUND, DYNCMD_RegisterParameter("supernegfloat", DYNCMDTYPE_FLOAT, "float", &f));
		EXPECT_EQ (-0.5f, f);

		EXPECT_EQ (RET_FOUND, DYNCMD_RegisterParameter("true", DYNCMDTYPE_BOOL8, "boolean", &b));
		EXPECT_EQ (IMG_TRUE, b);

		EXPECT_EQ (RET_FOUND, DYNCMD_RegisterParameter("false", DYNCMDTYPE_BOOL8, "boolean", &b));
		EXPECT_EQ (IMG_FALSE, b);
	}
	// test arrays
	{
		IMG_INT intArraySize;
		IMG_INT *intArray = 0; // use dynamic array
		
		EXPECT_EQ (RET_FOUND, DYNCMD_RegisterParameter("int_array_size", DYNCMDTYPE_UINT, "int_array_size", &intArraySize));
		
		intArray = (IMG_INT*)IMG_MALLOC(intArraySize*sizeof(IMG_INT));
		ASSERT_TRUE(intArray != NULL);
		EXPECT_EQ (RET_FOUND, DYNCMD_RegisterArray("int_array", DYNCMDTYPE_UINT, "int_array", intArray, intArraySize));

		for ( int i = 1 ; i <= intArraySize ; i++ )
		{
			if ( i%2 == 0 )
			{
				EXPECT_EQ(-1*i, intArray[i-1]);
			}
			else
			{
				EXPECT_EQ(i, intArray[i-1]);
			}
		}
		IMG_FREE(intArray);

		IMG_FLOAT floatArray[4]; // use static array
		EXPECT_EQ (RET_FOUND, DYNCMD_RegisterArray("float_array", DYNCMDTYPE_FLOAT, "float_array", &(floatArray[0]), 4));
		for ( int i = 1 ; i <= 4 ; i++ )
		{
			if ( i%2 == 0 )
			{
				EXPECT_FLOAT_EQ(-0.1f*i, floatArray[i-1]);
			}
			else
			{
				EXPECT_FLOAT_EQ(0.1f*i, floatArray[i-1]);
			}
		}
	}
}

#define TESTFILE "test_correct.txt"

TEST(AddFile, fileonly)
{
	ASSERT_EQ(IMG_SUCCESS, DYNCMD_AddFile(TESTFILE, "#"));

	testCommands();

	DYNCMD_ReleaseParameters();
}

TEST(AddFile, cmdPlusFile)
{
	// the last given value is taken
	// uint is 3 in the file - overwrite the cmd 0
	// posfloat is 1.5 in the file - overwritten by cmd 3.5
	char **argv;
	int argc = 7;
	char program[] = "prgm";
	char command[] = "uint";
	char value[] = "0";
	char source[] = "source";
	char file[] = TESTFILE;
	char posfloat[] = "posfloat";
	char floatval[] = "3.5";

	IMG_UINT ui;
	IMG_FLOAT f;

	argv = (char**)malloc(argc*sizeof(char*));
	argv[0] = program;
	argv[1] = command;
	argv[2] = value;
	argv[3] = source;
	argv[4] = file;
	argv[5] = posfloat;
	argv[6] = floatval;

	ASSERT_EQ (IMG_SUCCESS, DYNCMD_AddCommandLine(argc, argv, source)) << "failed to load cmd";

	// this is only in the file
	EXPECT_EQ (RET_FOUND, DYNCMD_RegisterParameter("command", DYNCMDTYPE_COMMAND, "first non comment", NULL));

	EXPECT_EQ (RET_FOUND, DYNCMD_RegisterParameter("uint", DYNCMDTYPE_UINT, "uint", &ui));
	EXPECT_EQ (3, ui);

	EXPECT_EQ (RET_FOUND, DYNCMD_RegisterParameter("posfloat", DYNCMDTYPE_FLOAT, "float", &f));
	EXPECT_EQ (3.5, f);

	DYNCMD_ReleaseParameters();
	free(argv);
}

TEST(AddFile, longComment)
{
	// this file has "rem" as the comment base
	ASSERT_EQ(IMG_SUCCESS, DYNCMD_AddFile("test_longcomments.txt", "rem"));

	testCommands();

	DYNCMD_ReleaseParameters();
}

TEST(AddFile, invalid)
{
	EXPECT_EQ (IMG_ERROR_INVALID_PARAMETERS, DYNCMD_AddFile(NULL, NULL)) << "NULL input";
	EXPECT_EQ (IMG_ERROR_INVALID_PARAMETERS, DYNCMD_AddFile(TESTFILE, NULL)) << "NULL comment string";
	EXPECT_EQ (IMG_ERROR_INVALID_PARAMETERS, DYNCMD_AddFile(NULL, "//")) << "NULL comment string";
}
