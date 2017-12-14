/**
*******************************************************************************
@file flxconverter.cpp

@brief Simple test application that takes an FLX as input and save it.

This application can be used to transform multi-segment FLX video to
single-segment FLX video

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
#include <cstdlib>
#include <cstdio>

#include "felixcommon/userlog.h"
#define LOG_TAG "FLX_CONV"
#include "sim_image.h"
#include "img_errors.h"
#include "image.h"  // to access CImage classes

const char *getName(enum sSimImageColourModel eColourModel)
{
    switch (eColourModel)
    {
    case SimImage_RGB24:
        return "RGB24";
    case SimImage_RGB32:
        return "RGB32";
    case SimImage_RGB64:
        return "RGB64";
    case SimImage_RGGB:
        return "RGGB";
    case SimImage_GRBG:
        return "GRBG";
    case SimImage_GBRG:
        return "GBRG";
    case SimImage_BGGR:
        return "BGGR";
    }
    return "?fmt?";
}

int convert(const char *inputFilename, const char *outputFilename)
{
    IMG_RESULT ret;
    sSimImageIn inputFLX;
    sSimImageOut outputFLX;
    unsigned int f;

    ret = SimImageIn_init(&inputFLX);
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("failed to init input FLX object\n");
        return EXIT_FAILURE;
    }

    ret = SimImageOut_init(&outputFLX);
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("failed to init output FLX object\n");
        return EXIT_FAILURE;
    }

    ret = SimImageIn_loadFLX(&inputFLX, inputFilename);
    if (IMG_SUCCESS != ret)
    {
        if (IMG_ERROR_NOT_SUPPORTED == ret)
        {
            LOG_ERROR("'%s' format is not supported\n", inputFilename);
        }
        else
        {
            LOG_ERROR("failed to open '%s' for reading\n", inputFilename);
        }
        return EXIT_FAILURE;
    }
    printf("Input FLX: %s (%dx%d %db %s %d frames)\n",
        inputFilename, inputFLX.info.ui32Width, inputFLX.info.ui32Height,
        (int)inputFLX.info.ui8BitDepth, getName(inputFLX.info.eColourModel),
        inputFLX.nFrames);

    memcpy(&(outputFLX.info), &(inputFLX.info), sizeof(struct SimImageInfo));

    ret = SimImageOut_create(&outputFLX);
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("failed to configure the output FLX object from the "\
            "input FLX object\n");
        SimImageIn_close(&inputFLX);
        return EXIT_FAILURE;
    }

    ret = SimImageOut_open(&outputFLX, outputFilename);
    if (IMG_SUCCESS != ret)
    {
        LOG_ERROR("failed to open '%s' for writing\n", outputFilename);
        SimImageOut_clean(&outputFLX);
        SimImageIn_close(&inputFLX);
        return EXIT_FAILURE;
    }
    printf("Output FLX: %s\n", outputFilename);

    for (f = 0; f < inputFLX.nFrames; f++)
    {
        ret = SimImageIn_convertFrame(&inputFLX, f);
        if (IMG_SUCCESS != ret)
        {
            LOG_ERROR("failed to load frame %d from '%s'\n", f, inputFilename);
            SimImageOut_close(&outputFLX);
            SimImageIn_close(&inputFLX);
            return EXIT_FAILURE;
        }

#if 0
        // we can't do that directly because formats are different
        ret = SimImageOut_addFrame(&outputFLX, inputFLX.pBuffer,
            inputFLX.info.ui32Width*inputFLX.info.ui32Height);
        if (IMG_SUCCESS != ret)
        {
            LOG_ERROR("failed to add frame %d to '%s'\n", f, outputFilename);
            SimImageOut_close(&outputFLX);
            SimImageIn_close(&inputFLX);
            return EXIT_FAILURE;
        }
#else
        {
            CImageFlx *CImageOut = NULL;
            const CImageFlx *CImageIn = NULL;

            CImageOut = static_cast<CImageFlx*>(outputFLX.saveToFLX);
            CImageIn = static_cast<const CImageFlx*>(inputFLX.loadFromFLX);

            for (int c = 0; c < outputFLX.nPlanes; c++)
            {
                memcpy(CImageOut->chnl[c].data, CImageIn->chnl[c].data,
                    CImageIn->chnl[c].chnlWidth*CImageIn->chnl[c].chnlHeight
                    *sizeof(PIXEL));
            }
        }
#endif

        ret = SimImageOut_write(&outputFLX);
        if (IMG_SUCCESS != ret)
        {
            LOG_ERROR("failed to write frame %d to '%s'\n", f, outputFilename);
            SimImageOut_close(&outputFLX);
            SimImageIn_close(&inputFLX);
            return EXIT_FAILURE;
        }
    }
    printf("%d frame(s) converted\n", f);

    SimImageOut_close(&outputFLX);
    SimImageIn_close(&inputFLX);
    return EXIT_SUCCESS;
}

void usage()
{
    printf("Transforms multi-segment input FLX files into single segment "\
        "output FLX files. Does not support all FLX formats.\n");
    printf("usage:\n");
    printf("\tinput_filename [output_filename]\n");
    printf("\n");
    printf("input_filename: input FLX file <required>\n");
    printf("output_filename: output FLX file <optional> if not provided "\
        "give saves to 'output.flx'\n");
}

int main(const int argc, const char *argv[])
{
    int ret;
    if (argc < 2)
    {
        LOG_ERROR("Need at least an input file\n");
        usage();
        return EXIT_FAILURE;
    }
    if (argc < 3)
    {
        ret = convert(argv[1], "output.flx");
    }
    else
    {
        ret = convert(argv[1], argv[2]);
    }

    return ret;
}
