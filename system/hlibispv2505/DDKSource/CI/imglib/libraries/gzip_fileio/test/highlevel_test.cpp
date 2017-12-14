
#include <cstdlib>
#include <cstdio>
#include <gtest/gtest.h>

#include <img_types.h>
#include <img_defs.h>
#include <img_errors.h>

#include "gzip_fileio.h"

//
// Invalid
//

#define N_MODES 6
// modes listed in 'man fopen'
static const char *modes[N_MODES] = {"r", "r+", "w", "w+", "a", "a+"};
// mode available on windows
static const char *extended_modes[N_MODES] = {"rb", "r+b", "wb", "w+b", "ab", "a+b"};

TEST(Invalid, open)
{
	IMG_HANDLE file = NULL;
	
	ASSERT_EQ(IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE, IMG_FILEIO_OpenFile("doesnotexists.txt", "r", &file, IMG_FALSE));
	ASSERT_TRUE(file == NULL);

	ASSERT_EQ(IMG_ERROR_INVALID_PARAMETERS, IMG_FILEIO_OpenFile(NULL, NULL, NULL, IMG_FALSE));
	ASSERT_EQ(IMG_ERROR_INVALID_PARAMETERS, IMG_FILEIO_OpenFile("doesnotexists.txt", NULL, NULL, IMG_FALSE));
	ASSERT_EQ(IMG_ERROR_INVALID_PARAMETERS, IMG_FILEIO_OpenFile("doesnotexists.txt", "r", NULL, IMG_FALSE));

#if !defined(WIN32) || defined(FILEIO_USEZLIB)// on win32 fopen() assert if open format is wrong
	ASSERT_EQ(IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE, IMG_FILEIO_OpenFile("doesnotexists.txt", "k", &file, IMG_TRUE)) << "unkown open format";
#endif
	ASSERT_TRUE(file == NULL);
}

TEST(Invalid, close)
{
	ASSERT_EQ(IMG_ERROR_INVALID_PARAMETERS, IMG_FILEIO_CloseFile(NULL));
}

TEST(Invalid, reopen)
{
	FILE *f = fopen("reopen_test.txt", "w"); // creates
	ASSERT_TRUE(f != NULL) << "could not create reopen_test.txt";
	fclose(f);

	IMG_HANDLE file = NULL;

	ASSERT_EQ (IMG_SUCCESS, IMG_FILEIO_OpenFile("reopen_test.txt", "r", &file, IMG_FALSE));

	EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, IMG_FILEIO_ReopenFile(NULL, NULL, NULL));
	EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, IMG_FILEIO_ReopenFile(&file, NULL, NULL));
	EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, IMG_FILEIO_ReopenFile(NULL, "reopen_test.txt", NULL));
	EXPECT_EQ(IMG_ERROR_INVALID_PARAMETERS, IMG_FILEIO_ReopenFile(NULL, NULL, "k")) << "unkown open format"; 
	
	IMG_FILEIO_CloseFile(file);
}

//
// HighLevel
//

TEST(HighLevel, open_modes)
{
	char name[16];
	IMG_RESULT expected = IMG_SUCCESS;
	IMG_BOOL bCompress = IMG_FALSE;
	
	for ( int i = 0 ; i < N_MODES ; i++ )
	{
		FILE *f = NULL;
		IMG_HANDLE file = NULL;

#ifdef FILEIO_USEZLIB
		for ( bCompress = 0 ; bCompress <= 1 ; bCompress++ )
		{
#endif
		expected = IMG_SUCCESS;

		sprintf(name, "mode%d.txt", i);
		f = fopen(name, "w"); // creates
		ASSERT_TRUE(f != NULL) << "cannot create " << name;
		fclose(f);

#ifdef FILEIO_USEZLIB
		if ( bCompress == IMG_TRUE )
		{
			expected = i%2 == 1 ? IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE : IMG_SUCCESS;
			// because gzip does not support the read/write (r+, w+, a+)
		}
#endif

		EXPECT_EQ(expected, IMG_FILEIO_OpenFile(name, modes[i], &file, bCompress)) << modes[i] << " " << bCompress;
		if ( file != NULL )
		{
			IMG_FILEIO_CloseFile(file);
			file = NULL;
		}

#ifdef FILEIO_USEZLIB
		if ( bCompress == IMG_TRUE )
		{
			expected = i%2 == 1 ? IMG_ERROR_COULD_NOT_OBTAIN_RESOURCE : IMG_SUCCESS;
			// because gzip does not support the read/write (r+b, w+b, a+b)
		}
#endif

		EXPECT_EQ(expected, IMG_FILEIO_OpenFile(name, extended_modes[i], &file, bCompress)) << extended_modes[i] << " " << bCompress;
		if ( file != NULL )
		{
			IMG_FILEIO_CloseFile(file);
			file = NULL;
		}

#ifdef FILEIO_USEZLIB
		} // for compressed
#endif

	} // for mode
}

TEST(HighLevel, reopen_modes)
{
	char name[16];

	for ( int i = 0 ; i < N_MODES ; i++ )
	{
		FILE *f = NULL;
		IMG_HANDLE file = NULL;

		sprintf(name, "mode%d.txt", i);
		f = fopen(name, "w"); // creates
		ASSERT_TRUE(f != NULL) << "cannot create " << name;
		fclose(f);

		EXPECT_EQ(IMG_SUCCESS, IMG_FILEIO_OpenFile(name, modes[i], &file, IMG_FALSE));
		if ( file != NULL )
		{
			IMG_FILEIO_CloseFile(file);
			break; // no more test
		}

		EXPECT_EQ(IMG_SUCCESS, IMG_FILEIO_ReopenFile(&file, name, extended_modes[i]));

		if ( file != NULL )
		{
			IMG_FILEIO_CloseFile(file);
			file = NULL;
		}
	}
}

//
// Main
//
int main(int argc, char *argv[])
{
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}
