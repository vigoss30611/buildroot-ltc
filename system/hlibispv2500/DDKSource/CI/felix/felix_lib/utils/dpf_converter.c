/**
******************************************************************************
@file dpf_converted.c

@brief Converts the DPF write map outputted by Felix HW to DPF read map

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
#include <dyncmd/commandline.h>
#include <felix_hw_info.h> // info about DPF maps

IMG_STATIC_ASSERT(DPF_MAP_OUTPUT_SIZE == 8, dpf_map_output_size_changed);
IMG_STATIC_ASSERT(DPF_MAP_INPUT_SIZE == 4, dpf_map_input_size_changed);

#include <stdio.h>
#include <stdlib.h>

#define DPF_CONV_CHECK_FMT

#define DPF_MAP_ACC(reg, field, val) DPF_MAP_ ## reg ## _OFFSET*8 + DPF_MAP_ ## reg ## _ ## field ## _ ## val
// taken from reg_io2.h
#define FIELD_RD(ui32RegValue, group, reg, field)                            \
    ((ui32RegValue & group##_##reg##_##field##_MASK) >> group##_##reg##_##field##_SHIFT)

// taken from reg_io2.h
#define FIELD_WR(ui32RegValue, group, reg, field, ui32Value)                                            \
{                                                                                                                \
    if ( ((IMG_UINT32)(IMG_INT32)ui32Value) > (group##_##reg##_##field##_LSBMASK) && \
        (((IMG_UINT32)(IMG_INT32)ui32Value) & (IMG_UINT32)~(group##_##reg##_##field##_LSBMASK)) !=  \
        (IMG_UINT32)~(group##_##reg##_##field##_LSBMASK) ) \
        { \
        printf("%s: %d does not fit in %s:%s:%s\n", __FUNCTION__, ui32Value, #group, #reg, #field); \
    } \
    (ui32RegValue) = ((ui32RegValue) & ~(group##_##reg##_##field##_MASK)) |                                                        \
        (((IMG_UINT32) (ui32Value) << (group##_##reg##_##field##_SHIFT)) & (group##_##reg##_##field##_MASK));    \
}

struct parameters
{
    char *pszInput;
    char *pszOutput;
    IMG_BOOL8 bVerbose; // print all dropped or just nb of dropped
    
    IMG_BOOL8 bFalsePositive;
    IMG_BOOL8 bFalseNegative;
    
    IMG_UINT confidence[2]; //min and max confidence to take into account

    IMG_UINT infoParams[2]; // 1st is height, 2nd is parallelism
};

int load_parameters(int argc, char *argv[], struct parameters *pParam)
{
    int missing = 0;
    int iRet = 0;
    IMG_BOOL8 bPrintHelp = IMG_FALSE;
    memset(pParam, 0, sizeof(struct parameters));
    
    //pParam->pszInput = NULL;
    //pParam->nCorrected = 0;
    //pParam->pszOutput = NULL;
    //pParam->bVerbose = IMG_FALSE;
    pParam->bFalsePositive = 1;
    pParam->bFalseNegative = 1;
    //pParam->confidence[0] = 0;
    pParam->confidence[1] = 255;
    pParam->infoParams[0] = 0;
    
    if (DYNCMD_RegisterParameter("-h", DYNCMDTYPE_COMMAND, "Usage information", NULL) == RET_FOUND)
    {
        bPrintHelp = IMG_TRUE;
    }

    iRet = DYNCMD_RegisterParameter("-input", DYNCMDTYPE_STRING, "Input DPF map (write-map format)", &(pParam->pszInput));
    if (iRet == RET_NOT_FOUND && !bPrintHelp)
    {
        fprintf(stderr, "ERROR: -input not specified\n");
        missing++;
    }
    
    iRet = DYNCMD_RegisterParameter("-output", DYNCMDTYPE_STRING, "Output DPF map base-name (read-map format - nb of elements added when writing to disk)", &(pParam->pszOutput));
    if (iRet == RET_NOT_FOUND && !bPrintHelp)
    {
        fprintf(stderr, "INFO: -output not specified - default used\n");
        pParam->pszOutput = "dpf_read\0";
    }

    iRet = DYNCMD_RegisterParameter("-falsePositive", DYNCMDTYPE_BOOL8, "0 no false positive, >0 take false positive into account", &(pParam->bFalsePositive));
    if (iRet !=RET_FOUND && !bPrintHelp)
    {
        if (iRet != RET_NOT_FOUND)
        {
            fprintf(stderr, "ERROR: -falsePositive expects an unsigned int but none was found\n");
            missing++;
        }
    }
    
    iRet = DYNCMD_RegisterParameter("-falseNegative", DYNCMDTYPE_BOOL8, "0 no false negative, >0 take false negative into account", &(pParam->bFalseNegative));
    if (iRet !=RET_FOUND && !bPrintHelp)
    {
        if (iRet != RET_NOT_FOUND)
        {
            fprintf(stderr, "ERROR: -falseNegative expects an unsigned int but none was found\n");
            missing++;
        }
    }
    
    iRet = DYNCMD_RegisterArray("-confidence", DYNCMDTYPE_UINT, "The minimum and maximum confidence values to take into account", pParam->confidence, 2);
    if (iRet != RET_FOUND)
    {
        if ( iRet == RET_INCOMPLETE )
        {
            fprintf(stderr, "ERROR: not enough values found when parsing -confidence\n");
            missing++;
        }
        else if ( iRet != RET_NOT_FOUND )
        {
            fprintf(stderr, "ERROR: failure when parsing -confidence\n");
            missing++;
        }
    }

    iRet = DYNCMD_RegisterArray("-analyse", DYNCMDTYPE_UINT, "Print analyse of the generated Input DPF Map. 1st parameter is image height (default is 0 - no information displayed), 2nd is desired parallelism. If parallelism is not specified using 1.", pParam->infoParams, 2);
    if (iRet != RET_FOUND && !bPrintHelp)
    {
        if ( iRet == RET_INCOMPLETE )
        {
            pParam->infoParams[1] = 1;
            printf("Parallelism not specified for -analyse, using %d\n", pParam->infoParams[1]);
        }
        else if ( iRet != RET_NOT_FOUND )
        {
            fprintf(stderr, "ERROR: failure when parsing -analyse\n");
            missing++;
        }
    }
    
    DYNCMD_RegisterParameter("-info", DYNCMDTYPE_COMMAND, "Information about the map formats", NULL);
    if(iRet == RET_FOUND && !bPrintHelp)
    {
        printf("X = X coordinate, Y = Y coordinate, V = value, C = confidence index, S = false negative (0)/false positive (1)\n");
        printf("Write-map format: %dBytes: <X=%d..%d, Y=%d..%d, V=%d..%d, C=%d..%d, S=%d..%d>\n",
            DPF_MAP_OUTPUT_SIZE,
            DPF_MAP_ACC(COORD, X_COORD, SHIFT), DPF_MAP_ACC(COORD, X_COORD, LENGTH), //DPF_MAP_COORD_OFFSET*8 + DPF_MAP_COORD_X_COORD_SHIFT, DPF_MAP_COORD_OFFSET*8 + DPF_MAP_COORD_X_COORD_SHIFT + DPF_MAP_COORD_X_COORD_LENGTH,
            DPF_MAP_ACC(COORD, Y_COORD, SHIFT), DPF_MAP_ACC(COORD, Y_COORD, LENGTH),
            DPF_MAP_ACC(DATA, PIXEL, SHIFT), DPF_MAP_ACC(DATA, PIXEL, LENGTH),
            DPF_MAP_ACC(DATA, CONFIDENCE, SHIFT), DPF_MAP_ACC(DATA, CONFIDENCE, LENGTH),
            DPF_MAP_ACC(DATA, STATUS, SHIFT), DPF_MAP_ACC(DATA, STATUS, LENGTH)
        );
        printf("Read-map format: %dBytes: <X=%d..%d, Y=%d..%d>\n",
            DPF_MAP_INPUT_SIZE,
            DPF_MAP_ACC(COORD, X_COORD, SHIFT), DPF_MAP_ACC(COORD, X_COORD, LENGTH),
            DPF_MAP_ACC(COORD, Y_COORD, SHIFT), DPF_MAP_ACC(COORD, Y_COORD, LENGTH)
        );
    }
    
    iRet = DYNCMD_RegisterParameter("-v", DYNCMDTYPE_COMMAND, "Print information about dropped values", NULL);
    if(iRet == RET_FOUND)
    {
        pParam->bVerbose = IMG_TRUE;
    }

    /*
     * must be last action
     */
    
    DYNCMD_HasUnregisteredElements(&iRet);

    if ( missing != 0 || bPrintHelp )
    {
        DYNCMD_PrintUsage();
        if (bPrintHelp)
        {
            missing = 1;
        }
    }
    
    return -missing;
}

static int compareU16Coords_maxX = 1;
static int compareU16Coords(const void *p1, const void *p2)
{
    IMG_UINT32 v1 = *(IMG_UINT32*)p1, v2 = *(IMG_UINT32*)p2;
    int v1X, v1Y, v2X, v2Y;
    int p1Pos, p2Pos;

    v1X = FIELD_RD(v1, DPF_MAP, COORD, X_COORD);
    v1Y = FIELD_RD(v1, DPF_MAP, COORD, Y_COORD);

    v2X = FIELD_RD(v2, DPF_MAP, COORD, X_COORD);
    v2Y = FIELD_RD(v2, DPF_MAP, COORD, Y_COORD);

    p1Pos = v1Y*compareU16Coords_maxX + v1X;
    p2Pos = v2Y*compareU16Coords_maxX + v2X;

    if ( p1Pos < p2Pos )
    {
        return -1; // p1 goes before
    }
    else if ( p1Pos == p2Pos )
    {
        return 0; // duplicate!
    }
    return 1;
}

void infoAboutInputMap(const IMG_UINT16* pDPFOutput, int nDefects, IMG_UINT height, IMG_UINT p)
{
    int currWin, nWindow = 0;
    int d;
    int y;
    int maxDefects = 0;
    int *nDefectPerWindow = NULL;
    int linesPerWin = DPF_WINDOW_HEIGHT(p);
    static int nD = 2;

    nWindow = height/linesPerWin +1;

    nDefectPerWindow = (int*)calloc(nWindow, sizeof(int));
    if ( nDefectPerWindow == NULL )
    {
        return;
    }

    for ( d = 0 ; d < nD*nDefects ; d+=nD )
    {
        //y = FIELD_RD(pDPFOutput[d + DPF_MAP_COORD_OFFSET/4], DPF_MAP, COORD, X_COORD);
        y = pDPFOutput[d + 1];
        currWin = y/linesPerWin;

        nDefectPerWindow[currWin]++;
    }

    printf("\nNb of defects per windows (height=%d and parallelism=%d) -> window height=%d -> %d windows)\n", height, p, DPF_WINDOW_HEIGHT(p), nWindow);
    maxDefects = 0;
    currWin = 0; // use it as a sum
    for ( y = 0 ; y < nWindow ; y++ )
    {
        maxDefects = IMG_MAX_INT(maxDefects, nDefectPerWindow[y]);
        currWin += nDefectPerWindow[y];

        printf("%d (line %d to %d): %d\n", y, y*linesPerWin, (y+1)*linesPerWin-1, nDefectPerWindow[y]);
    }
    printf("Max defects is %d - needs %d bytes in DPF internal buffer - found %d defects\n", maxDefects, maxDefects*DPF_MAP_INPUT_SIZE, currWin);

    free(nDefectPerWindow);
}

int isSorted(const IMG_UINT16* pDPFInput, int nDefects, int maxX)
{
    int i;
    int position = 0, prev = 0;
    int ret = 0;

    for ( i = 2 ; i < 2*nDefects ; i+=2 )
    {
        prev = pDPFInput[(i-2) + 1]*maxX + pDPFInput[(i-2)];
        position = pDPFInput[i + 1]*maxX + pDPFInput[i];

        if ( prev > position )
        {
            fprintf(stderr, "ERROR: current %d{%d,%d} < previous %d{%d,%d}\n", i/2, pDPFInput[i], pDPFInput[i + 1], (i-2)/2, pDPFInput[(i-2)], pDPFInput[(i-2) + 1]);
            return -1;
        }
        else if ( prev == position )
        {
            ret=1;
        }
    }
    return ret;
}

#define FILENAME_SIZE 256+15 //(15 extra for end of name)
int convertWriteToReadFormat(const struct parameters *pParam)
{
    FILE *fInput = NULL, *fOutput = NULL;
    int i;
    int ret = EXIT_SUCCESS;
    int nDrop = 0;
    IMG_UINT32 *pOutput = NULL;
    int nCorrected = 0, nalloc = 1024;
    char filename[FILENAME_SIZE];
    IMG_UINT32 vInput[2];
    int maxX = 0;
    int minConf = 255, maxConf = 0;
    double sumConf = 0;
    int nFalsePos = 0; // nFalseNeg is nCorrected-nFalsePos
    
    if ( !pParam->bFalseNegative && !pParam->bFalsePositive )
    {
        fprintf(stderr, "ERROR: dropping for both false positive and false negative enabled: no output will be generated\n");
        return EXIT_FAILURE;
    }
    if ( sizeof(vInput) != DPF_MAP_OUTPUT_SIZE )
    {
        fprintf(stderr, "ERROR: sizeof(vInput)=%u is too small for DPF_MAP_OUTPUT_SIZE=%d\n", (unsigned int)sizeof(vInput), DPF_MAP_OUTPUT_SIZE);
        return EXIT_FAILURE;
    }
    
    printf("Convert DPF Write format to DPF read format (confidence %d-%d, false negative %d, false positive %d)\n",
        pParam->confidence[0], pParam->confidence[1], pParam->bFalseNegative, pParam->bFalsePositive
    );
    printf("Input: %s\n", pParam->pszInput);
    printf("Output: %s\n", pParam->pszOutput);

    pOutput = (IMG_UINT32*)calloc(nalloc, sizeof(IMG_UINT32));
    if ( pOutput == NULL )
    {
        fprintf(stderr, "ERROR: failed to allocate output map (%u Bytes)\n", nalloc*sizeof(IMG_UINT32));
        return EXIT_FAILURE;
    }

    if ( (fInput = fopen(pParam->pszInput, "rb")) == NULL )
    {
        fprintf(stderr, "ERROR: failed to open input file '%s'\n", pParam->pszInput);
        return EXIT_FAILURE;
    }

    for ( i = 0 ; (fread(vInput, DPF_MAP_OUTPUT_SIZE, 1, fInput) == 1) && ret == EXIT_SUCCESS ; i++ )
    {
        IMG_UINT32 vOutput = 0;

        int v = 0;
        IMG_BOOL8 bDrop = IMG_FALSE;
            
#ifdef DPF_CONV_CHECK_FMT // test valid format
        {
            int coordMask = DPF_MAP_COORD_X_COORD_MASK|DPF_MAP_COORD_Y_COORD_MASK;
            int dataMask = DPF_MAP_DATA_STATUS_MASK|DPF_MAP_DATA_CONFIDENCE_MASK|DPF_MAP_DATA_PIXEL_MASK;
            int vC, vD;

            vC = vInput[DPF_MAP_COORD_OFFSET/4];
            vD = vInput[DPF_MAP_DATA_OFFSET/4];

            if ( (vC&coordMask) != vC || (vD&dataMask) != vD )
            {
                fprintf(stderr, "WARNING: binary is corrupted at correction %d (0x%x): coord=0x%08x (coord mask=0x%08x) data=0x%08x (data mask=0x%08x)\n",
                    i, i*DPF_MAP_OUTPUT_SIZE, vC, coordMask, vD, dataMask);
            }
        }
#endif
            
        v = FIELD_RD(vInput[DPF_MAP_COORD_OFFSET/4], DPF_MAP, COORD, X_COORD);
        //FIELD_WR(vOutput, DPF_MAP, COORD, X_COORD, v);
        maxX = IMG_MAX_INT(maxX, v);

        //v = FIELD_RD(vInput[DPF_MAP_COORD_OFFSET/4], DPF_MAP, COORD, Y_COORD);
        //FIELD_WR(vOutput, DPF_MAP, COORD, Y_COORD, v);
        vOutput = vInput[DPF_MAP_COORD_OFFSET/4];

        v = FIELD_RD(vInput[DPF_MAP_DATA_OFFSET/4], DPF_MAP, DATA, CONFIDENCE);

        if ( v < (int)pParam->confidence[0] || v > (int)pParam->confidence[1] )
        {
            if ( pParam->bVerbose) printf("DROP: correction %d (0x%x 0x%x) - confidence = %d\n", i, vInput[0], vInput[1], v);
            nDrop++;
            continue;
        }
        else
        {
            minConf = IMG_MIN_INT(minConf, v);
            maxConf = IMG_MAX_INT(maxConf, v);
            sumConf += v;
        }

        v = FIELD_RD(vInput[DPF_MAP_DATA_OFFSET/4], DPF_MAP, DATA, STATUS);
        if ( v == 0 && (!pParam->bFalseNegative) || v == 1 && (!pParam->bFalsePositive) )
        {
            if ( pParam->bVerbose) printf("DROP: correction %d (0x%x 0x%x) - status = %d\n", i, vInput[0], vInput[1], v);
            nDrop++;
            continue;
        }
        if ( v == 1 )
        {
            nFalsePos++;
        }

        IMG_ASSERT(vInput[DPF_MAP_COORD_OFFSET/4] == vOutput);
        pOutput[nCorrected++] = vOutput;

        if ( nCorrected == nalloc )
        {
            IMG_UINT32 *pTmp = NULL;

            pTmp = (IMG_UINT32*)calloc(2*nalloc, sizeof(IMG_UINT32));
            if ( pTmp == NULL )
            {
                fprintf(stderr, "ERROR: failed to allocate output map (%u Bytes)\n", nalloc*sizeof(IMG_UINT32));
                ret = EXIT_FAILURE;
                break;
            }
            memcpy(pTmp, pOutput, nalloc*sizeof(IMG_UINT32));
            free(pOutput);
            pOutput = pTmp;
            nalloc*=2;
        }
    }

    printf("Reading done: %d corrections dropped - %d kept - %d found\n", nDrop, nCorrected, nDrop+nCorrected);
    printf("Confidence (excluding drops): min %d, max %d, avg %3.2lf\n", minConf, maxConf, sumConf/nCorrected);
    printf("Status (excluding drops): %d false positive - %d false negative\n", nFalsePos, nCorrected-nFalsePos);

    if ( ret == EXIT_SUCCESS && nCorrected > 0 )
    {
        IMG_ASSERT(nCorrected == i-nDrop);

        compareU16Coords_maxX = maxX+1;
        qsort(pOutput, nCorrected, sizeof(IMG_UINT32), &compareU16Coords);
                
        ret=isSorted((IMG_UINT16*)pOutput, nCorrected, maxX+1);
        if ( ret > 0 )
        {
            printf("WARNING: input has duplicates!\n");
        }

        if ( ret >= 0 )
        {
            ret = EXIT_SUCCESS;

            if ( strlen(pParam->pszOutput) < FILENAME_SIZE-15 ) // 15 needed but 1 room for \0
            {
                sprintf(filename, "%s_%05d.dat", pParam->pszOutput, nCorrected);

                if ( (fOutput = fopen(filename, "wb")) == NULL )
                {
                    fprintf(stderr, "ERROR: failed to open output file '%s'\n", filename);
                    ret = EXIT_FAILURE;
                }
                else
                {
                    if( (ret=fwrite(pOutput, DPF_MAP_INPUT_SIZE, nCorrected, fOutput)) < nCorrected )
                    {
                        fprintf(stderr, "ERROR: failed to write correction map! (fwrite returned %d)\n", ret);
                        ret = EXIT_FAILURE;
                    }
                    else
                    {
                        printf("conversion written to %s\n", filename);
                    }
                    fclose(fOutput);
                }

                if ( pParam->infoParams[0] > 0 )
                {
                    infoAboutInputMap((const IMG_UINT16*)pOutput, nCorrected, pParam->infoParams[0], pParam->infoParams[1]);
                }
            }
            else
            {
                fprintf(stderr, "ERROR: output filename '%s' is too long (max %d characters)\n", pParam->pszOutput, FILENAME_SIZE-15);
                ret = EXIT_FAILURE;
            }
            
        }
        else
        {
            fprintf(stderr, "ERROR: internal error, could not sort the input map!\n");
            ret = EXIT_FAILURE;
        }
    }

    free(pOutput);
    fclose(fInput);    
    return ret;
}

int main(int argc, char *argv[])
{
    int ret;
    struct parameters sParam;
    
    if ( DYNCMD_AddCommandLine(argc, argv, "") != 0 )
    {
        fprintf(stderr, "ERROR: failed to initialise DYNCMD\n");
        return EXIT_FAILURE;
    }

    if ( (ret=load_parameters(argc, argv, &sParam)) != 0 )
    {
        if ( ret > 0 )    return EXIT_SUCCESS;
        return EXIT_FAILURE;
    }
    
    ret = convertWriteToReadFormat(&sParam);

    DYNCMD_ReleaseParameters(); // released last because we use some strings
    return ret;
}
