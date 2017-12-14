#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <map>
#include <algorithm>
#include <string>
#include <deque>

#include "img_types.h"
#include "img_defs.h"
#include "img_errors.h"
#include "felixcommon/lshgrid.h"

/*
 * this is normally part of MC and CI
 */

#define LSH_MIN_DIFF 4

#ifdef INFOTM_SENSOR_OTP_DATA_MODULE
#define LSH_MAX_DIFF 12
#else
#define LSH_MAX_DIFF 10
#endif

/** @brief Number of fractional bits in the LSH grid vertex */
#define LSH_VERTEX_2_6_FRAC 10
/** @brief Number of integer bits in the LSH grid vertex */
#define LSH_VERTEX_2_6_INT 3
/** @brief Is the LSH grid vertex signed */
#define LSH_VERTEX_2_6_SIGNED 0


/** @brief Number of fractional bits in the LSH grid vertex */
#define LSH_VERTEX_2_x_FRAC 10
/** @brief Number of integer bits in the LSH grid vertex */
#define LSH_VERTEX_2_x_INT 2
/** @brief Is the LSH grid vertex signed */
#define LSH_VERTEX_2_x_SIGNED 0

static int LSH_VERTEX_INT = LSH_VERTEX_2_6_INT;
static int LSH_VERTEX_FRAC = LSH_VERTEX_2_6_FRAC;
static int LSH_VERTEX_SIGNED = LSH_VERTEX_2_6_SIGNED;

#define PREC(REG) REG##_INT, REG##_FRAC, REG##_SIGNED, #REG

static void IMG_range(IMG_INT32 nBits, IMG_BOOL isSigned, IMG_INT32 *pMin,
    IMG_INT32 *pMax)
{
    *pMin = (-1 * isSigned)*(1 << (nBits - isSigned));  // 0 if not signed
    *pMax = (1 << (nBits - isSigned)) - 1;
}

/**
* @ingroup INT_FCT
*/
IMG_INT32 IMG_clip(IMG_INT32 val, IMG_INT32 nBits, IMG_BOOL isSigned,
    const char *dbg_regname)
{
    IMG_INT32 maxValue, minValue, ret;
    IMG_range(nBits, isSigned, &minValue, &maxValue);
    ret = IMG_MAX_INT(minValue, IMG_MIN_INT(val, maxValue));
    if (ret != val)
    {
        printf("%s clipped from 0x%x to 0x%x\n", dbg_regname, val, ret);
    }
    return ret;
}

/**
* @ingroup INT_FCT
*/
IMG_INT32 IMG_Fix_Clip(LSH_FLOAT fl, IMG_INT32 intBits, IMG_INT32 fractBits,
    IMG_BOOL isSigned, const char *dbg_regname)
{
    int flSign = 1;
    IMG_INT32 conv;
    IMG_INT32 ret;

    if (isSigned && fl < 0.0)
    {
        flSign = -1;
    }

    if (fractBits < 0)
    {
        conv = (IMG_INT32)fl;
        ret = IMG_clip(conv, intBits + 0, isSigned, dbg_regname);
        ret = ret >> abs(fractBits);
    }
    else
    {
        conv = (IMG_INT32)(fl*(IMG_INT64)(1 << fractBits) + 0.5 * flSign);
        ret = IMG_clip(conv, intBits + fractBits, isSigned, dbg_regname);
    }

    return ret;
}

/*
 * end of MC/CI borrowing
 */

enum lshType_t
{
    LSH_INPUT = 0,
    LSH_OUTPUT = 1,
    LSH_INPUT_IS_OUTPUT = 2
};

typedef IMG_UINT16 GRID_TEMP;

struct GRID
{
    std::string fName;
    LSH_GRID grid;
    lshType_t lshType;

    GRID() : grid(), lshType(LSH_INPUT)
    {
    }

    GRID(const char *filename, lshType_t type)
        : fName(filename), grid(), lshType(type)
    {
    }

    /* not done in destructor to avoid doing a copy constructor */
    void clean()
    {
        LSH_Free(&grid);
    }

    IMG_RESULT load()
    {
        IMG_RESULT ret;
#ifdef INFOTM_SENSOR_OTP_DATA_MODULE
        ret = LSH_Load_bin(&grid, fName.c_str(), -1, NULL);
#else
        ret = LSH_Load_bin(&grid, fName.c_str());
#endif
        if (IMG_SUCCESS != ret)
        {
            printf("failed to load %s\n", fName.c_str());
            LSH_Free(&grid);
        }
        return ret;
    }
};

typedef std::map<GRID_TEMP, GRID> GRID_LIST;

/**
 * @brief get the bits per difference for a matrix
 *
 * @param i matrix information
 * @param verbose to print additional information
 * @param[out] maxDiff absolute maximum difference found
 * @param[out] differences optional pointer to store the found differences.
 * @note the stored differences are NOT absolute values.
 */
int getBitsPerDiff(GRID_LIST::const_iterator i, bool verbose, int &maxDiff,
    std::deque<int> *differences = NULL);

void cleanall(GRID_LIST &pLSH)
{
    GRID_LIST::iterator i = pLSH.begin();
    while (i != pLSH.end())
    {
        i->second.clean();
        i++;
    }
    pLSH.clear();
}

/**
 * @brief save a given GRID_LIST to a single file
 *
 * This includes matrix values and differences.
 */
IMG_RESULT LSH_Save_Grids_txt(const GRID_LIST &pLSH, const char *filename)
{
    FILE *f = NULL;
    int nOfGrids = pLSH.size();

    if (pLSH.size() == 0 || filename == NULL)
    {
        return IMG_ERROR_INVALID_PARAMETERS;
    }

    f = fopen(filename, "w");
    if (f != NULL)
    {
        int c, x, y;
        GRID_LIST::const_iterator i;
        for (i = pLSH.begin(); i != pLSH.end(); i++)
        {
            fprintf(f, "\n\nTemperature %d Matrix %dx%d ts %d\n",
                (int)i->first,
                i->second.grid.ui16Width, i->second.grid.ui16Height,
                i->second.grid.ui16TileSize);
            fprintf(f, "Values:\n");
            for (c = 0; c < LSH_MAT_NO; c++)
            {
                fprintf(f, "Channel %d\n", c);
                for (y = 0; y < i->second.grid.ui16Height; y++)
                {
                    for (x = 0; x < i->second.grid.ui16Width; x++)
                    {
                        fprintf(f, "%f ", i->second.grid.apMatrix[c][y*i->second.grid.ui16Width + x]);
                    }
                    fprintf(f, "\n");
                }
            }
        }

        std::deque<int> differences;
        int maxDiff;
        int off;
        for (i = pLSH.begin(); i != pLSH.end(); i++)
        {
            differences.clear();
            maxDiff = 0;
            getBitsPerDiff(i, false, maxDiff, &differences);

            IMG_ASSERT(differences.size() == LSH_MAT_NO*i->second.grid.ui16Height*(i->second.grid.ui16Width - 1));

            fprintf(f, "\n\nTemperature %d Matrix %dx%d ts %d (absolute max diff %d)\n",
                (int)i->first,
                i->second.grid.ui16Width, i->second.grid.ui16Height,
                i->second.grid.ui16TileSize,
                maxDiff);

            off = 0;
            for (c = 0; c < LSH_MAT_NO; c++)
            {
                fprintf(f, "Channel %d\n", c);
                for (y = 0; y < i->second.grid.ui16Height; y++)
                {
                    for (x = 0; x < i->second.grid.ui16Width -1; x++)
                    {
                        IMG_ASSERT((unsigned)off < differences.size());
                        fprintf(f, "%d ", differences[off]);
                        off++;
                    }
                    fprintf(f, "\n");
                }
            }
        }

        fclose(f);
    }
    else
    {
        printf("Failed to open output file %s\n", filename);
        return IMG_ERROR_INVALID_PARAMETERS;
    }
    printf("Information saved in %s\n", filename);
    return IMG_SUCCESS;
}

int interpolate(const GRID_LIST &sGtInput, GRID_LIST &sGtOutput)
{
    const int nOfChannels = LSH_MAT_NO;
    IMG_RESULT ret;
    IMG_UINT16 width = 0, height = 0, tiles = 0;
    GRID_LIST::const_iterator ci;
    GRID_LIST::iterator i;
    int toInterpolate = 0;

    if (sGtInput.size() == 0)
    {
        printf("no inputs\n");
        return EXIT_FAILURE;
    }
    if (sGtOutput.size() == 0)
    {
        printf("no outputs\n");
        return EXIT_FAILURE;
    }

    printf("inputs (%d):\n", (int)sGtInput.size());
    ci = sGtInput.begin();

    width = ci->second.grid.ui16Width;
    height = ci->second.grid.ui16Height;
    tiles = ci->second.grid.ui16TileSize;

    for (; ci != sGtInput.end(); ++ci)
    {
        printf("    %d %s (%dx%d ts=%d)\n",
            (int)ci->first, ci->second.fName.c_str(),
            (int)ci->second.grid.ui16Width, (int)ci->second.grid.ui16Height,
            (int)ci->second.grid.ui16TileSize);

        if (ci->second.grid.ui16Width != width
            || ci->second.grid.ui16Height != height
            || ci->second.grid.ui16TileSize != tiles)
        {
            printf("INCONSISTENT input found!\n");
            return EXIT_FAILURE;
        }
    }

    printf("outputs (%d):\n", (int)sGtOutput.size());
    for (i = sGtOutput.begin(); i != sGtOutput.end(); ++i)
    {
        ret = LSH_AllocateMatrix(&(i->second.grid), width, height, tiles);
        if (IMG_SUCCESS != ret)
        {
            printf("failed to allocate a matrix!\n");
            return EXIT_FAILURE;
        }
        IMG_ASSERT(width == i->second.grid.ui16Width);
        IMG_ASSERT(height == i->second.grid.ui16Height);
        IMG_ASSERT(tiles == i->second.grid.ui16TileSize);

        ci = sGtInput.find(i->first);
        if (ci != sGtInput.end())
        {
            i->second.lshType = LSH_INPUT_IS_OUTPUT;

            printf("    %d %s (same as input %s)\n",
                (int)i->first, i->second.fName.c_str(),
                ci->second.fName.c_str());

            for (int c = 0; c < LSH_MAT_NO; c++)
            {
                // duplicate the input matrix
                memcpy(i->second.grid.apMatrix[c],
                    ci->second.grid.apMatrix[c],
                    width*height*sizeof(LSH_FLOAT));
            }
        }
        else
        {
            printf("    %d %s\n",
                (int)i->first, i->second.fName.c_str());
            toInterpolate++;
        }
    }

    if (toInterpolate > 0)
    {
        // create interpolation lines
        for (int ch = 0; ch < nOfChannels; ch++)
        {
            printf("Interpolation for channel %d...", ch);
            for (int tile = 0; tile < width*height; tile++)
            {
                // for each element create interpolation line
                float S = 0, Sx = 0, Sy = 0, Sxx = 0, Sxy = 0, Syy = 0, Delta = 0, a = 0, b = 0;
                //            S = \sum_{i=1}^n 1 = n,
                //            S_x = \sum_{i=1}^n x_i,
                //            S_y = \sum_{i=1}^n y_i,
                //            S_{xx} = \sum_{i=1}^n x_i^2,
                //            S_{xy} = \sum_{i=1}^n x_i y_i,
                //            S_{yy} = \sum_{i=1}^n y_i^2,
                //            Delta = S * S_{xx} - (S_x)^2.

                //            a = (S * Sxy - Sx * Sy)/Delta
                //            b = (Sxx * Sy - Sx * Sxy)/Delta

                //            y = ax + b

                for (ci = sGtInput.begin(); ci != sGtInput.end(); ++ci)
                {
                    S += 1;
                    Sx += ci->first;
                    Sy += *(ci->second.grid.apMatrix[ch] + tile);
                    Sxx += (ci->first * ci->first);
                    Sxy += (ci->first * (*(ci->second.grid.apMatrix[ch] + tile)));
                    Syy += (*(ci->second.grid.apMatrix[ch] + tile)) * (*(ci->second.grid.apMatrix[ch] + tile));
                    Delta = S * Sxx - Sx * Sx;
                }

                a = (S * Sxy - Sx * Sy) / Delta;
                b = (Sxx * Sy - Sx * Sxy) / Delta;

                // create output matrices using new interpolation lines
                for (i = sGtOutput.begin(); i != sGtOutput.end(); ++i)
                {
                    if (LSH_OUTPUT == i->second.lshType)
                    {
                        *(i->second.grid.apMatrix[ch] + tile) = a * i->first + b;
                    }
                }
            }
            printf(" done\n");
        }
    }

    return EXIT_SUCCESS;
}

void usage()
{
    printf("LSH matrix interpolation tool.\n");
    printf("usage:\n");
    printf("<program> -i path temp [...] -o path temp [...] [-hwv 2.1|2.6|2.7|3.0] [-v] [-h]\n");
    printf("options:\n");
    printf("-i <path:input file> <int:temp>\n");
    printf("    Input matrices per temperature (Kelvin).\n");
    printf("    All input matrices should have the same size and tile - size.\n");
    printf("    Can have several input matrices.\n");
    printf("-o <path:input file> <int:temp>\n");
    printf("    Output matrices per temperature (Kelvin).\n");
    printf("    The output matrices present in the input list will be copied.\n");
    printf("    The input matrices not present in the output list will NOT be saved.\n");
    printf("    Can have several output matrices.\n");
    printf("-v\n");
    printf("    To output information about matrices in gridInput.txt and gridOutput.txt.\n");
    printf("-h\n");
    printf("    To print this help\n");
    printf("\n");
}

void scale(LSH_GRID &g, const LSH_FLOAT factor)
{
    int c, t;

    for (c = 0; c < 4; c++)
    {
        int nTiles = g.ui16Width*g.ui16Height;
        for (t = 0; t < nTiles; t++)
        {
            g.apMatrix[c][t] = g.apMatrix[c][t] * factor;
        }
    }
}

int getBitsPerDiff(GRID_LIST::const_iterator i, bool verbose, int &maxDiff,
    std::deque<int> *differences)
{
    int c;
    unsigned int t, k;
    unsigned int stride = i->second.grid.ui16Width;
    int bitsPerDiff;
    double factor = 1.0;

    maxDiff = 0;
    for (c = 0; c < 4; c++)
    {
        for (t = 0; t < i->second.grid.ui16Height; t++)
        {
            LSH_FLOAT current = i->second.grid.apMatrix[c][t*stride + 0];
            IMG_INT32 prev = IMG_Fix_Clip(current, PREC(LSH_VERTEX));
            IMG_INT32 curr = 0;
            IMG_INT32 diff = 0;

            for (k = 1; k < stride; k++)
            {
                current = i->second.grid.apMatrix[c][t*stride + k];

                curr = IMG_Fix_Clip(current, PREC(LSH_VERTEX));
                diff = curr - prev;
                if (differences)
                {
                    differences->push_back(diff);
                }

                if (diff < 0)
                {
                    diff = (diff * -1) - 1;
                }

                maxDiff = IMG_MAX_INT(diff, maxDiff);

                prev = curr;
            }  // for colunm
        }  // for line
    }  // for channel

    double nbits = log(maxDiff) / log(2.0);
    bitsPerDiff = static_cast<int>(ceil(nbits)) + 1;

    if (verbose)
    {
#ifndef NDEBUG
        printf("    maximum difference %d\n", maxDiff);
#endif
        printf("    maximum difference found needs %d bits in the "\
            "HW format.\n",
            bitsPerDiff);
    }

    return bitsPerDiff;
}

LSH_FLOAT getDiffFactor(GRID_LIST::const_iterator i, bool verbose)
{
    int maxDiff;
    int bitsPerDiff = getBitsPerDiff(i, verbose, maxDiff);
    LSH_FLOAT factor = 1.0;

    if (bitsPerDiff > LSH_MAX_DIFF)
    {
        // 1 less bit because difference is signed
        // -1 to get maximum value
        factor = static_cast<LSH_FLOAT>((1 << (LSH_MAX_DIFF - 1)) - 1)
            / maxDiff;
    }
    return factor;
}

int verifyOutput(GRID_LIST &sGtList, bool singleScale, bool verbose,
    const LSH_FLOAT HW_maxLSHGain, const LSH_FLOAT HW_maxWBGain,
    const int HW_maxBitPerDiff)
{
    GRID_LIST::iterator i;
    int c, t;
    int maxDiffPerBit = 0;
    LSH_FLOAT factor;
    std::map<GRID_TEMP, LSH_FLOAT> scaleGains;
    std::map<GRID_TEMP, LSH_FLOAT> scaleDiff;
    std::map<GRID_TEMP, LSH_FLOAT>::iterator it;

    printf("Verify output grids to ensure:\n");
    printf("    max gain %f\n", HW_maxLSHGain);
    printf("    max bits per diff %d\n", HW_maxBitPerDiff);

    for (i = sGtList.begin(); i != sGtList.end(); i++)
    {
        LSH_FLOAT currentMax = 0.0f;
        int nTiles = i->second.grid.ui16Width*i->second.grid.ui16Height;
        IMG_INT32 maxDiff = 0;
        LSH_FLOAT totalRescaled = 1.0f;

        if (verbose)
        {
            printf("Gain verification for temperature %d\n", i->first);
        }

        for (c = 0; c < 4; c++)
        {
            for (t = 0; t < nTiles; t++)
            {
                currentMax = std::max(currentMax, i->second.grid.apMatrix[c][t]);
            }
        }

        if (verbose)
        {
            printf("    maximum gain of %f\n", currentMax);
        }

        if (currentMax > HW_maxLSHGain)
        {
            factor = HW_maxLSHGain / currentMax;
        }
        else
        {
            factor = 1.0;
        }
        scaleGains[i->first] = factor;
    }

    if (singleScale)
    {
        // find the smallest multiplier to apply to all matrices
        it = scaleGains.begin();
        factor = it->second;
        it++;
        while (it != scaleGains.end())
        {
            factor = std::min(factor, it->second);
            it++;
        }
    }

    printf("Applying Gains scaling:\n");
    for (i = sGtList.begin(); i != sGtList.end(); i++)
    {
        if (singleScale)
        {
            scaleGains[i->first] = factor;
        }
        scale(i->second.grid, scaleGains[i->first]);

        printf("    multiplying matrix %d by %f\n",
            i->first, scaleGains[i->first]);
    }

    for (i = sGtList.begin(); i != sGtList.end(); i++)
    {
        // this is a 2 pass process as scaling down the matrix for max gain
        // may affect the maximum difference

        if (verbose)
        {
            printf("Difference verification for temperature %d\n", i->first);
        }
        factor = getDiffFactor(i, verbose);
            scaleDiff[i->first] = factor;
    }

    if (singleScale)
    {
        // find the smallest multiplier to apply to all matrices
        it = scaleDiff.begin();
        factor = it->second;
        it++;
        while (it != scaleDiff.end())
        {
            factor = std::min(factor, it->second);
            it++;
        }
    }

    printf("Applying bits per diff scaling:\n");
    maxDiffPerBit = 0;
    int maxDiff;
    for (i = sGtList.begin(); i != sGtList.end(); i++)
    {
        if (singleScale)
        {
            scaleDiff[i->first] = factor;
        }
        scale(i->second.grid, scaleDiff[i->first]);

        printf("    multiplying matrix %d by %f\n",
            i->first, scaleDiff[i->first]);

        maxDiffPerBit = std::max(maxDiffPerBit,
            getBitsPerDiff(i, verbose, maxDiff));
    }

    printf("Verification of LSH matrices recomends the following parameters:\n");
    printf("******\n");
    printf("LSH_CTRL_BITS_DIFF %d\n", maxDiffPerBit);
    printf("LSH_CTRL_CORRECTIONS %d\n", sGtList.size());
    c = 0;
    i = sGtList.begin();
    while (i != sGtList.end())
    {
        factor = 1 / (scaleGains[i->first] * scaleDiff[i->first]);
        printf("\n");
        printf("LSH_CTRL_FILE_%d %s\n", c, i->second.fName.c_str());
        printf("LSH_CTRL_TEMPERATURE_%d %d\n", c, i->first);
        printf("LSH_CTRL_SCALE_WB_%d %f\n",
            c, factor);
        // WB gains
        if (factor > HW_maxWBGain)
        {
            printf("# warning LSH_CTRL_SCALE_WB_%d > than max WB gains %f\n",
                i, HW_maxWBGain);
        }

        c++;
        i++;
    }
    printf("******\n");

    return EXIT_SUCCESS;
}

float maxValue(int i, int f, int s, const char *dbg)
{
    float m = static_cast<float>((1 << (i + f - s)) - 1);
    int d = (1 << f);
    return m / d;
}

int main(const int argc, const char *argv[])
{
    int ret;
    int nOfInputTemps = 0;

    GRID_LIST sGtInput;
    GRID_LIST sGtOutput;
    GRID_LIST::iterator i;
    GRID_LIST::const_iterator ci;
    bool verbose = false;
    bool singleScale = true;
    int p;

#if 0
    printf("ARGUMENTS \n");
    for (int x = 0; x < argc; x++)
    {
        printf("argv nr %d = %s \n", x, argv[x]);
    }
#endif

    for (p = 1; p < argc; p++)
    {
        if (strncmp(argv[p], "-v", 2) == 0)
        {
            verbose = true;
        }
        else if (strncmp(argv[p], "-i", 2) == 0)
        {
            if (p + 2 < argc)
            {
                int temp;

                if (sscanf(argv[p + 2], "%d", &temp) != 1)
                {
                    printf("second argument of -i is not an int\n");
                    usage();
                    cleanall(sGtInput);
                    cleanall(sGtOutput);
                    return EXIT_FAILURE;
                }

                GRID t(argv[p + 1], LSH_INPUT);
                if (IMG_SUCCESS != t.load())
                {
                    cleanall(sGtInput);
                    cleanall(sGtOutput);
                    return EXIT_FAILURE;
                }
                i = sGtInput.find(temp);
                if (sGtInput.end() != i)
                {
                    printf("Warning: overriding previous input file %s with %s for temperature %d\n",
                        i->second.fName.c_str(), argv[p + 1], temp);
                    i->second.clean();
                    i->second = t;
                }
                else
                {
                    sGtInput[temp] = t;
                }
                p += 2;
                continue; // next parameter set
            }
            else
            {
                printf("-i need more parameters\n");
                usage();
                cleanall(sGtInput);
                cleanall(sGtOutput);
                return EXIT_FAILURE;
            }
        }
        else if (strncmp(argv[p], "-o", 2) == 0)
        {
            if (p + 2 < argc)
            {
                int temp;

                if (sscanf(argv[p + 2], "%d", &temp) != 1)
                {
                    printf("second argument of -o is not an int\n");
                    usage();
                    cleanall(sGtInput);
                    cleanall(sGtOutput);
                    return EXIT_FAILURE;
                }

                GRID t(argv[p + 1], LSH_OUTPUT);
                i = sGtOutput.find(temp);
                if (sGtOutput.end() != i)
                {
                    printf("Warning: overriding previous output file %s with %s for temperature %d\n",
                        i->second.fName.c_str(), argv[p + 1], temp);
                    i->second = t;
                }
                else
                {
                    sGtOutput[temp] = t;
                }
                p += 2;
                continue; // next parameter set
            }
            else
            {
                printf("-o need more parameters\n");
                usage();
                cleanall(sGtInput);
                cleanall(sGtOutput);
                return EXIT_FAILURE;
            }
        }
        else if (strncmp(argv[p], "-hwv", 4) == 0)
        {
            if (p + 1 < argc)
            {
                const char *hwv = argv[p + 1];
                int major = 0;
                int minor = 0;
                int s;

                s = sscanf(hwv, "%d.%d", &major, &minor);

                if (s != 2)
                {
                    printf("-hwv need parameter that looks like <int>.<int>\n");
                    usage();
                    cleanall(sGtInput);
                    cleanall(sGtOutput);
                    return EXIT_FAILURE;
                }
                else
                {
                    printf("Change info to support HW %d.%d\n", major, minor);
                    // from hw 3 use MAX_GAIN_2_6 (default)
                    if (2 == major)
                    {
                        if (6 > minor)
                        {
                            LSH_VERTEX_INT = LSH_VERTEX_2_x_INT;
                            LSH_VERTEX_FRAC = LSH_VERTEX_2_x_FRAC;
                            LSH_VERTEX_SIGNED = LSH_VERTEX_2_x_SIGNED;
                        }
                        // 2.x is default
                    }
                    else if (2 > major)
                    {
                        printf("unsuppored HW version %d.%d\n", major, minor);
                        usage();
                        cleanall(sGtInput);
                        cleanall(sGtOutput);
                        return EXIT_FAILURE;
                    }
                }
                p++;
            }
            // after -hwv to avoid taking -hwv as -h
            else if (strncmp(argv[p], "-h", 2) == 0)
            {
                usage();
                cleanall(sGtInput);
                cleanall(sGtOutput);
                return EXIT_SUCCESS;
            }
            else
            {
                printf("-hwv need more parameters\n");
                usage();
                cleanall(sGtInput);
                cleanall(sGtOutput);
                return EXIT_FAILURE;
            }
        }
        else
        {
            printf("unknown parameter %s\n", argv[p]);
        }
    }

    // interpolate intputs into outputs
    ret = interpolate(sGtInput, sGtOutput);

    if (EXIT_SUCCESS == ret)
    {
        if (verbose)
        {
            LSH_Save_Grids_txt(sGtInput, "gridInput.txt");
            LSH_Save_Grids_txt(sGtOutput, "gridOutput_preverif.txt");
        }

        float HW_maxLSHGain = maxValue(PREC(LSH_VERTEX));
        float HW_maxWBGain = maxValue(PREC(LSH_VERTEX));
        // verifyOutput given outputs to fit LSH maximums
        ret = verifyOutput(sGtOutput, singleScale, verbose,
            HW_maxLSHGain, HW_maxWBGain, LSH_MAX_DIFF);
    }

    if (EXIT_SUCCESS == ret)
    {
        if (verbose)
        {
            LSH_Save_Grids_txt(sGtOutput, "gridOutput_postverif.txt");
        }

        printf("saving output...");
        for (ci = sGtOutput.begin(); ci != sGtOutput.end(); ++ci)
        {
            ret = LSH_Save_bin(&ci->second.grid, ci->second.fName.c_str());
            if (IMG_SUCCESS != ret)
            {
                printf("Warning: failed to save matrix for temperature %d in %s\n",
                    (int)ci->first, ci->second.fName.c_str());
            }
        }
        printf(" done\n");
    }

    cleanall(sGtInput);
    cleanall(sGtOutput);
    return ret;
}
