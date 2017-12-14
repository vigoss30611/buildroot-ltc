/**
******************************************************************************
@file ci_converter_test.cpp

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
#include <cstdio>

#include <sim_image.h>

#include "unit_tests.h"
#include "felix.h"

#include "ci/ci_api.h"
#include "ci/ci_converter.h"

class CI_Converter : public ::testing::Test
{
public:
    CI_DG_FRAME sFrame;
    void *cmpData;

    void SetUp()
    {
        memset(&sFrame, 0, sizeof(CI_DG_FRAME));
        cmpData = 0;
    }

    void TearDown()
    {
        if (sFrame.data)
        {
            free(sFrame.data);
            sFrame.data = 0;
        }
        if (cmpData)
        {
            free(cmpData);
            cmpData = 0;
        }
    }

    void runCheck()
    {

    }

    void runCheckParallel(int bitdepth)
    {
        CI_CONVERTER sConverter;

        sSimImageIn flx;
        char filename[64];
        char filenameCheck[64];
        unsigned expectedsize = 0;

        snprintf(filename, 44, "testdata/44x40_%dbit.flx", bitdepth);
        snprintf(filenameCheck, 64, "testdata/44x40_para%d.dat", bitdepth);

        ASSERT_EQ(IMG_SUCCESS, CI_ConverterConfigure(&sConverter,
            CI_DGFMT_PARALLEL, bitdepth))
            << "could not configure the converter for parallel " << bitdepth;
        switch (bitdepth)
        {
        case 10:
            expectedsize = 2560;
            break;
        case 12:
            expectedsize = 5120;
            break;
        default:
            ASSERT_TRUE(0) << "wrong format bitdepth given!";
        }

        ASSERT_EQ(IMG_SUCCESS, SimImageIn_init(&flx));
        ASSERT_EQ(IMG_SUCCESS, SimImageIn_loadFLX(&flx, filename))
            << "failed to open " << filename;
        ASSERT_EQ(IMG_SUCCESS, SimImageIn_convertFrame(&flx, 0))
            << "failed to convert " << filename;

        EXPECT_EQ(44, flx.info.ui32Width);
        EXPECT_EQ(40, flx.info.ui32Height);
        EXPECT_EQ(bitdepth, flx.info.ui8BitDepth);

        FILE *f = fopen(filenameCheck, "rb");
        ASSERT_TRUE(f != NULL) << "failed to open checkfile " << filenameCheck;

        /* note: for a real convertion the frame should be acquired from
         * a Datagen object but we can also do it this way just to convert
         * a frame */

        this->sFrame.ui32AllocSize = CI_ConverterFrameSize(&sConverter,
            flx.info.ui32Width, flx.info.ui32Height);
        ASSERT_EQ(expectedsize, sFrame.ui32AllocSize);

        // use malloc because CI_ConvertFrame should memset to 0!
        sFrame.data = malloc(sFrame.ui32AllocSize);

        ASSERT_EQ(IMG_SUCCESS, CI_ConverterConvertFrame(&sConverter,
            &flx, &sFrame)) << "failed to convert the frame!";

        // use malloc because we should load everything from disk!
        this->cmpData = malloc(sFrame.ui32AllocSize);

        ASSERT_EQ(sFrame.ui32AllocSize, fread(cmpData, 1,
            sFrame.ui32AllocSize, f))
            << "compare " << filenameCheck
            << " data could not load correctly!";

        EXPECT_EQ(0, memcmp(cmpData, sFrame.data, sFrame.ui32AllocSize))
            << "convertion of parallel " << bitdepth
            << "b is not what is expected!";

        free(cmpData);
        cmpData = NULL;

        free(sFrame.data);
        sFrame.data = NULL;

        SimImageIn_close(&flx);
        fclose(f);
    }

    void runCheckMIPI(int bitdepth, bool longFormat = false)
    {
        CI_CONVERTER sConverter;

        sSimImageIn flx;
        char filename[64];
        char filenameCheck[64];
        unsigned int expectedsize = 0;  // allocation size
        unsigned int convertedSize = 0;  // packet size
        CI_CONV_FMT eFmt = CI_DGFMT_MIPI;

        snprintf(filename, 44, "testdata/44x40_%dbit.flx", bitdepth);
        snprintf(filenameCheck, 64, "testdata/44x40_mipi%d.dat", bitdepth);
        if (longFormat)
        {
            eFmt = CI_DGFMT_MIPI_LF;
            snprintf(filenameCheck, 64, "testdata/44x40_mipiLF%d.dat",
                bitdepth);
        }

        ASSERT_EQ(IMG_SUCCESS, CI_ConverterConfigure(&sConverter,
            eFmt, bitdepth))
            << "could not configure the converter for MIPI " << bitdepth;
        // expected allocation size - rounded up to SYSMEM_ALIGNMENT
        expectedsize = 5120;
        switch (bitdepth)
        {
        case 10:
            convertedSize = 61;
            break;
        case 12:
            convertedSize = 72;
            break;
        case 14:
            convertedSize = 83;
            break;
        default:
            ASSERT_TRUE(0) << "wrong format bitdepth given!";
        }

        ASSERT_EQ(IMG_SUCCESS, SimImageIn_init(&flx));
        ASSERT_EQ(IMG_SUCCESS, SimImageIn_loadFLX(&flx, filename))
            << "failed to open " << filename;
        ASSERT_EQ(IMG_SUCCESS, SimImageIn_convertFrame(&flx, 0))
            << "failed to convert " << filename;

        EXPECT_EQ(44, flx.info.ui32Width);
        EXPECT_EQ(40, flx.info.ui32Height);
        EXPECT_EQ(bitdepth, flx.info.ui8BitDepth);

        FILE *f = fopen(filenameCheck, "rb");
        ASSERT_TRUE(f != NULL) << "failed to open checkfile " << filenameCheck;

        /* note: for a real convertion the frame should be acquired from
        * a Datagen object but we can also do it this way just to convert
        * a frame */

        this->sFrame.ui32AllocSize = CI_ConverterFrameSize(&sConverter,
            flx.info.ui32Width, flx.info.ui32Height);
        ASSERT_EQ(expectedsize, sFrame.ui32AllocSize);

        ASSERT_GE(sConverter.eFormat, CI_DGFMT_MIPI);
        ASSERT_TRUE(sConverter.privateData != NULL);

        CI_CONV_MIPI_PRIV *pMIPIPriv =
            (CI_CONV_MIPI_PRIV*)sConverter.privateData;

        // use malloc because CI_ConvertFrame should memset to 0!
        sFrame.data = malloc(sFrame.ui32AllocSize);

        ASSERT_EQ(IMG_SUCCESS, CI_ConverterConvertFrame(&sConverter,
            &flx, &sFrame)) << "failed to convert the frame!";

        EXPECT_EQ(convertedSize,
            sFrame.ui32PacketWidth)
            << "unexpected MIPI packets size for ExtDG!";

        /* the converted size should not take into account that
         * the long format is added to the packet but the function should
         * return that size because it is used as a stride!
         *
         * The stride is using the full with while packet is half 
         * When using MIPI the stride also contains the frame start or
         * frame end packet - FS and FE are both 4B */
        unsigned int convertedStride = convertedSize + 4;
        if (longFormat)
        {
            // adding LS and LE which are 4B each
            convertedStride += 8;
        }
        // round up to system memory limits
        convertedStride = ((convertedStride + SYSMEM_ALIGNMENT - 1)
            / SYSMEM_ALIGNMENT) * SYSMEM_ALIGNMENT;

        EXPECT_EQ(convertedStride, CI_ConverterFrameSize(&sConverter,
            flx.info.ui32Width, 1))
            << "unexpected MIPI stride!";

        // use malloc because we should load everything from disk!
        this->cmpData = malloc(sFrame.ui32AllocSize);

        ASSERT_EQ(sFrame.ui32AllocSize, fread(cmpData, 1,
            sFrame.ui32AllocSize, f))
            << "compare " << filenameCheck
            << " data could not load correctly!";

        EXPECT_EQ(0, memcmp(cmpData, sFrame.data, sFrame.ui32AllocSize))
            << "convertion of MIPI " << bitdepth
            << "b " << (longFormat?"LF":"") << " is not what is expected!";
#if 1
        // to help debug if format convertion is different
        snprintf(filenameCheck, 64, "out_44x40_mipi%d.dat", bitdepth);
        if (longFormat)
        {
            eFmt = CI_DGFMT_MIPI_LF;
            snprintf(filenameCheck, 64, "out_44x40_mipiLF%d.dat",
                bitdepth);
        }

        FILE *fo = fopen(filenameCheck, "wb");
        if (fo)
        {
            fwrite(sFrame.data, 1, sFrame.ui32AllocSize, fo);
            fclose(fo);
        }
#endif

        free(cmpData);
        cmpData = NULL;

        free(sFrame.data);
        sFrame.data = NULL;

        SimImageIn_close(&flx);
        fclose(f);

        CI_ConverterClear(&sConverter);
    }
};

TEST_F(CI_Converter, parallel10)
{
    runCheckParallel(10);
}

TEST_F(CI_Converter, parallel12)
{
    runCheckParallel(12);
}

TEST_F(CI_Converter, MIPI10)
{
    runCheckMIPI(10, false);
}

TEST_F(CI_Converter, MIPI10LF)
{
    runCheckMIPI(10, true);
}

TEST_F(CI_Converter, MIPI12)
{
    runCheckMIPI(12, false);
}

TEST_F(CI_Converter, MIPI12LF)
{
    runCheckMIPI(12, true);
}

TEST_F(CI_Converter, MIPI14)
{
    runCheckMIPI(14, false);
}

TEST_F(CI_Converter, MIPI14LF)
{
    runCheckMIPI(14, true);
}

extern "C" {
    // implemented in ci_converter.c
    IMG_SIZE CI_Convert_Parallel10(void *privateData,
        const IMG_UINT16 *inPixels, IMG_UINT32 inStride,
        IMG_UINT32 NpixelsPerCh, IMG_UINT8 *outPixels);
    IMG_SIZE CI_Convert_Parallel12(void *privateData,
        const IMG_UINT16 *inPixels, IMG_UINT32 inStride,
        IMG_UINT32 NpixelsPerCh, IMG_UINT8 *outPixels);
    IMG_UINT16 MIPI_ComputeCRC(const IMG_UINT8 *dataBytes,
        IMG_UINT32 Nbytes);
    IMG_SIZE MIPI_WriteControl(IMG_UINT8 *pBuf, IMG_INT8 virtualChannel,
        IMG_INT8 dataId, IMG_UINT16 ui16Data);
    IMG_SIZE MIPI_Write_10(const IMG_UINT16 *inPixels,
        IMG_UINT32 inStride, IMG_UINT32 NpixelsPerCh, IMG_UINT8 *outPixels);
    IMG_SIZE MIPI_Write_12(const IMG_UINT16 *inPixels,
        IMG_UINT32 inStride, IMG_UINT32 NpixelsPerCh, IMG_UINT8 *outPixels);
    IMG_SIZE MIPI_Write_14(const IMG_UINT16 *inPixels,
        IMG_UINT32 inStride, IMG_UINT32 NpixelsPerCh, IMG_UINT8 *outPixels);
}
/**
 * From CRC examples MIPI/CSI-2.pdf
 */
TEST(CI_Converter_MIPI, crc)
{
    // data and crcs from p58
    IMG_UINT8 data1[24] = {
        0xFF, 0x00, 0x00, 0x02, 0xB9, 0xDC, 0xF3, 0x72, 0xBB, 0xD4, 0xB8,
        0x5A, 0xC8, 0x75, 0xC2, 0x7C, 0x81, 0xF8, 0x05, 0xDF, 0xFF, 0x00,
        0x00, 0x01 };
    IMG_UINT8 data2[24] = {
        0xFF, 0x00, 0x00, 0x00, 0x1E, 0xF0, 0x1E, 0xC7, 0x4F, 0x82, 0x78,
        0xC5, 0x82, 0xE0, 0x8C, 0x70, 0xD2, 0x3C, 0x78, 0xE9, 0xFF, 0x00,
        0x00, 0x01 };
    IMG_UINT16 crc1 = 0x00F0, crc2 = 0xE569;

    EXPECT_EQ(0xFFFF, MIPI_ComputeCRC(NULL, 0))
        << "MIPI CRC of 0 bytes must be 0xFFFF CSI-2 p56";

    EXPECT_EQ(crc1, MIPI_ComputeCRC(&data1[0], 24));
    EXPECT_EQ(crc2, MIPI_ComputeCRC(&data2[0], 24));
}

TEST(CI_Converter_MIPI, control)
{
    IMG_UINT32 header;

    ASSERT_EQ(4, MIPI_WriteControl(NULL, 0, 0, 0))
        << "MIPI data packet size is not correct";
    /*  add check for DG_MIPI_ID_RAW10 DG_MIPI_ID_RAW12 and
     * DG_MIPI_ID_RAW14 from ci_converter.c */
    static const int raw10 = 0x2b;
    static const int raw12 = 0x2c;
    static const int raw14 = 0x2d;
    /*ASSERT_EQ(0x2b, DG_MIPI_ID_RAW10);
    ASSERT_EQ(0x2c, DG_MIPI_ID_RAW12);
    ASSERT_EQ(0x2d, DG_MIPI_ID_RAW14);*/

    MIPI_WriteControl((IMG_UINT8*)&header, 1, raw10, 42);
    // dataid = (2-bit virtual channel)<<6 | (data type (0x10..0x37)) 8b
    // 1<<6 + DG_MIPI_ID_RAW10
    EXPECT_EQ(1 << 6 | raw10, ((IMG_UINT8*)&header)[0]);
    // nBytes =  (16b) in the middle
    // header >>8 & 0x00FF
    EXPECT_EQ(42, header >> 8 & 0x00FF)
        << "Nbytes is incorrect in the header";
    // ECC table 5 p52-53
    EXPECT_EQ(0x0B, header & 0x000f)
        << "ECC is not correct";
}

TEST(CI_Converter_MIPI, RAW10)
{
    IMG_UINT16 input[4] = { 0x123, 0x167, 0x1AB, 0x1EF };
    IMG_UINT8 verif[5] = { 0x48, 0x59, 0x6A, 0x7B, 0xFF }, output[5];

    IMG_MEMSET(output, 0, 5);
    ASSERT_EQ(5, MIPI_Write_10(&input[0], 2, 2, output));

    EXPECT_EQ(0, IMG_MEMCMP(output, verif, 4)) << "main part is different";
    EXPECT_EQ(verif[4], output[4]) << "mixed byte is different";
}

TEST(CI_Converter_MIPI, RAW12)
{
    IMG_UINT16 input[4] = { 0x123, 0x567, 0x9AB, 0xDEF };
    IMG_UINT8 verif[6] = { 0x12, 0x56, 0x73, 0x9A, 0xDE, 0xFB }, output[6];

    IMG_MEMSET(output, 0, 6);
    ASSERT_EQ(6, MIPI_Write_12(&input[0], 2, 2, output));

    EXPECT_EQ(0, IMG_MEMCMP(&output[0], &verif[0], 2))
        << "1st main part is different";
    EXPECT_EQ(verif[2], output[2]) << "1st mixed byte is different";
    EXPECT_EQ(0, IMG_MEMCMP(&output[3], &verif[3], 2))
        << "2nd main part is different";
    EXPECT_EQ(verif[5], output[5]) << "2nd mixed byte is different";
}

TEST(CI_Converter_MIPI, RAW14)
{
    IMG_UINT16 input[4] = { 0x123, 0x567, 0x9AB, 0xDEF };
    IMG_UINT8 verif[7] = { 0x04, 0x15, 0x26, 0x37, 0xE3, 0xB9, 0xBE },
        output[7];

    IMG_MEMSET(output, 0, 7);
    ASSERT_EQ(7, MIPI_Write_14(&input[0], 2, 2, output));

    EXPECT_EQ(0, IMG_MEMCMP(&output[0], &verif[0], 4))
        << "main part is different";
    EXPECT_EQ(0, IMG_MEMCMP(&output[4], &verif[4], 3))
        << "mixed bytes are different";
}

TEST(CI_Converter_PARALLEL, 10)
{
    IMG_UINT16 input[6] = { 0x123, 0x167, 0x1AB, 0x1EF, 0x123, 0x167 };
    IMG_UINT8 verif[8] = { 0x23, 0x9D, 0xB5, 0x1A, 0xEF, 0x8D, 0x74, 0x16 },
        output[8];

    IMG_MEMSET(output, 0, 8);
    ASSERT_EQ(8, CI_Convert_Parallel10(NULL, &input[0], 2, 3, output));

    EXPECT_EQ(0, IMG_MEMCMP(output, verif, 8)) << "packing failed";
}

TEST(CI_Converter_PARALLEL, 12)
{
    IMG_UINT16 input[4] = { 0x123, 0x567, 0x9AB, 0xDEF };
    IMG_UINT8 verif[6] = { 0x23, 0x71, 0x56, 0xAB, 0xF9, 0xDE }, output[6];

    IMG_MEMSET(output, 0, 6);
    ASSERT_EQ(6, CI_Convert_Parallel12(NULL, &input[0], 2, 2, output));

    EXPECT_EQ(0, IMG_MEMCMP(&output[0], &verif[0], 3))
        << "1st triplet is wrong";
    EXPECT_EQ(0, IMG_MEMCMP(&output[3], &verif[3], 3))
        << "2nd triplet is wrong";
}
