#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "q3ispost.h"
#include "q3denoise.h"
#include "math.h"

using namespace std;


unsigned char T_value[3] = { 20, 20, 20 }; //{Ty,Tu,Tv};
double M_value = 0.5;
DN_HerM_ACDoubleInfo HM_Double[3][DENOISE_HERMITE_SEGMENTS];
int nHMTsize[3] = {DENOISE_HERMITE_SEGMENTS,DENOISE_HERMITE_SEGMENTS,DENOISE_HERMITE_SEGMENTS};
double maskMin = ((double)1 / 2);
const double maskMax = ((double)1);

static int DN_MultipleCount(unsigned long n)
{
    int count = 0;
    if (!n)
        return count;
    while (n)
    {
        count++;
        n >>= 1;
    }
    return count - 1;
}

/*
DN Mask Curve 1 Register Read / Write at Offset

sn[31:20] S Slope of mask value for mask curve. (format 0.0.12)
rn[18:16] R Absolute difference value range exponent : RANGE = 2 ^ R.
yn[15:8] Y Mask value for mask curve. (format 0.0.8)
xn[7:0] X Absolute difference value for mask curve. (format 0.8.0)
*/
#define AC12 4096
#define AC08 256
static int DN_infotmic_writeHermiteRegFile(Q3_REG_VALUE *in_hermitetable)
{
    unsigned long x = 0, regVal = 0x0;
    int curve = 0, reg = 0;
    Q3_REG_VALUE *hermitetable = in_hermitetable;

    #ifdef Q3HW_PORTING
    unsigned int ISPregName [16] ={
        0xF8, //DNCTRL0
        0xB8, //DNMC0
        0xBC, //DNMC1
        0xC0, //DNMC2
        0xC4, //DNMC3
        0xC8, //DNMC4
        0xCC, //DNMC5
        0xD0, //DNMC6
        0xD4, //DNMC7
        0xD8, //DNMC8
        0xDC, //DNMC9
        0xE0, //DNMC10
        0xE4, //DNMC11
        0xE8, //DNMC12
        0xEC, //DNMC13
        0xF0 //DNMC14
    };
    #endif

    #ifdef OUTPUT_FILE
    FILE *output;
    char regData[3][16][50];
    memset(regData, 0, 50);
    char FileName[] = "3D_REG.txt";
    unsigned char ISPregName[16][5] = {
        { "F8, " }, //DNCTRL0
        { "B8, " }, //DNMC0
        { "BC, " }, //DNMC1
        { "C0, " }, //DNMC2
        { "C4, " }, //DNMC3
        { "C8, " }, //DNMC4
        { "CC, " }, //DNMC5
        { "D0, " }, //DNMC6
        { "D4, " }, //DNMC7
        { "D8, " }, //DNMC8
        { "DC, " }, //DNMC9
        { "E0, " }, //DNMC10
        { "E4, " }, //DNMC11
        { "E8, " }, //DNMC12
        { "EC, " }, //DNMC13
        { "F0, " }  //DNMC14
    };
    #endif

    for (curve = YUV_y; curve <= YUV_v; curve++)
    {
        /*    DNCTRL0 Register
        [25:24] IDX ¡V DN mask curve index IDX Indexed registers DNMC14 - 0
        0 Y mask curve
        1 U mask curve
        2 V mask curve
        */
        regVal = 0;
        regVal = (curve & 0x3) << 24;
        reg = 0;
        /* Wirte DNCTRL0 Register */
        #ifdef Q3HW_PORTING
        hermitetable->reg = (ISPOST_BASE_ADDR | ISPregName[reg]);
        hermitetable->value = regVal;
        hermitetable ++;
        #endif//Q3HW_PORTING

        #ifdef OUTPUT_FILE
        #ifdef _WIN32_
        sprintf_s(regData[curve][0], "%08x", regVal);
        #else
        sprintf(regData[curve][0], "%08x", regVal);
        #endif//_WIN32_
        #endif//OUTPUT_FILE

        for ( ; reg<15; reg++)
        {
            regVal = 0;
            /* sn[31:20] S */
            x = HM_Double[curve][reg].sn* AC12;
            x = (x > 0xFFF) ? 0xFFF : x;
            regVal |= x << 20;

            /* rn[18:16] R */
            //if( reg && (!HM_Double[curve][reg].rn))
            //    HM_Double[curve][reg].rn = HM_Double[curve][reg-1].rn;
            regVal |= ((DN_MultipleCount(HM_Double[curve][reg].rn) & 0x7) << 16);

            /* yn[15:8] curve Mask */
            //if( reg && (!HM_Double[curve][reg].yn))
            //    HM_Double[curve][reg].yn =HM_Double[curve][reg-1].yn;

            x = (HM_Double[curve][reg].yn * AC08);
            x = (x > 0xFF) ? 0xFF : x;
            regVal |= (x << 8) & 0xFF00;

            /* xn[7:0] X */
            //if( reg && (HM_Double[curve][reg].xn & 0xFF) < (HM_Double[curve][reg-1].xn & 0xFF))
            //    HM_Double[curve][reg].xn = 0xFF;
            regVal |= (HM_Double[curve][reg].xn & 0xFF);

            #ifdef Q3HW_PORTING
            hermitetable->reg = (ISPOST_BASE_ADDR | ISPregName[reg + 1]);
            hermitetable->value = regVal;
            hermitetable ++;
            #endif//Q3HW_PORTING

            #ifdef OUTPUT_FILE
            #ifdef _WIN32_
            sprintf_s(regData[curve][reg + 1], "%08x", regVal);    /*int cvt string */
            #else
            sprintf(regData[curve][reg + 1], "%08x", regVal);    /*int cvt string */
            #endif //_WIN32_
            #endif//OUTPUT_FILE

        }
    }

    #ifdef OUTPUT_FILE
    //if (b_JPG1Load && b_JPG2Load)
    {
        //output = fopen(FileName, "wt");
        #ifdef _WIN32_
        fopen_s(&output, FileName, "wt");
        #else
        output = fopen(FileName, "wt");
        #endif //_WIN32_
        if (!output)
        {
            printf("Could not open file\n");
            #ifdef _WIN32_
            system("pause");
            #endif //_WIN32_
            exit(0);
        }

        for (curve = YUV_y; curve <= YUV_v; curve++)
        {
            reg = 0;
            fwrite(ISPregName[reg], sizeof(unsigned char), sizeof(ISPregName[reg]) - 1, output);
            fwrite(regData[curve][reg], sizeof(unsigned char), strlen(regData[curve][reg]), output);
            fwrite("\n", sizeof(unsigned char), 1, output);
            reg++;
            for (; reg <= 15; reg++)
            {
                fwrite(ISPregName[reg], sizeof(unsigned char), sizeof(ISPregName[reg]) - 1, output);
                fwrite(regData[curve][reg], sizeof(unsigned char), strlen(regData[curve][reg]), output);
                fwrite("\n", sizeof(unsigned char), 1, output);
            }
        }
        fclose(output);
    }
    printf("Write Hermite %s down.\n",FileName);
    #endif//#ifdef OUTPUT_FILE

    return EXIT_SUCCESS;
}
#if 0
static int DN_findHermiteIndex(int YUVmode,int x)
{
    int idx = 0;

    for (idx=0; idx < nHMTsize[YUVmode]; idx++)
        if (HM_Double[YUVmode][idx].xn > x)
            break;

    return idx-1;
}

static double DN_findHermiteValue_ACDD(int YUVmode, int x, int i)
{
    double p0 = HM_Double[YUVmode][i].xn;
    double h = HM_Double[YUVmode][i].rn;
//    double p1 = HM_Double[YUVmode][i + 1].xn;
    double m0 = HM_Double[YUVmode][i].sn;
    double m1 = HM_Double[YUVmode][i + 1].sn;
    double v0 = HM_Double[YUVmode][i].yn;
    double v1 = HM_Double[YUVmode][i + 1].yn;
    double dv = (v1 - v0);
    double ti = (x - p0) / h;
    double H10 = (ti * ti * ti - 2 * ti * ti + ti) * h;
    double H01 = -2 * ti * ti * ti + 3 * ti * ti;
    double H11 = (ti * ti * ti - ti * ti) * h;

    double Pi = H10 * m0 + H01 * dv + H11 * m1 + v0;
    if (Pi < maskMin)
        Pi = maskMin;
    if (Pi > maskMax)
        Pi = maskMax;

    return Pi;
}
#endif


//
// 3D denoise Algorithm Implementation
//
#define K 1
static double DN_findDenoiseValue(int YUVmode, int absdiff)
{
    double mask;
    double pie = 3.141592657;
    int u8t = 0;
    long double p1 =0, p2=0;
    if (YUVmode == YUV_y)
        u8t = T_value[0];
    else if(YUVmode == YUV_u)
        u8t = T_value[1];
    else if(YUVmode == YUV_v)
        u8t = T_value[2];

    p1 = K*(absdiff - u8t);
    p2 = K * u8t;

    mask = (1-M_value) / pie*atan(p1)+  (1-M_value) / pie*atan(p2) + M_value;

    return mask;
}


static int DN_generateHermiteNormal(int YUVmode, int absdiff)
{
    int idx = 0;
    int start = absdiff - 14;
//    int end = absdiff + 14;
    int yy = 0,jj=0;

    //else if(u8BitAcFormat == BIT_AC_DOUBLE)
    {
        idx = 0;
        HM_Double[YUVmode][idx].xn = 0;
        idx++;

        for ( yy = start >> 1, jj = 2; yy > 0; yy = yy >> 1, jj = jj << 1)
        {
            if ((yy&1) == 1)
            {
                HM_Double[YUVmode][idx].xn = HM_Double[YUVmode][idx-1].xn+jj;
                HM_Double[YUVmode][idx-1].rn = jj;
                idx++;
            }
        }
        //HM[idx-1].xn = absdiff-14;
        HM_Double[YUVmode][idx-1].rn = 8;
        HM_Double[YUVmode][idx].xn = absdiff-6;
        HM_Double[YUVmode][idx].rn = 4;
        idx++;
        HM_Double[YUVmode][idx].xn = absdiff-2;
        HM_Double[YUVmode][idx].rn = 4;
        idx++;
        HM_Double[YUVmode][idx].xn = absdiff+2;
        HM_Double[YUVmode][idx].rn = 4;
        idx++;
        HM_Double[YUVmode][idx].xn = absdiff+6;
        HM_Double[YUVmode][idx].rn = 8;
        idx++;
        HM_Double[YUVmode][idx].xn = absdiff+14;
        HM_Double[YUVmode][idx].rn = 128;
        idx++;
        HM_Double[YUVmode][idx].xn = absdiff + 14 + 128;
        HM_Double[YUVmode][idx].rn = 128;
        idx++;
        HM_Double[YUVmode][idx].xn = absdiff+14+256;
    }
    idx++;
    return idx;
}



static int DN_generateHermiteNone(int YUVmode)
{
    //else if(u8BitAcFormat == BIT_AC_DOUBLE)
    {
        HM_Double[YUVmode][0].xn = 0;
        HM_Double[YUVmode][0].yn = maskMax;
        HM_Double[YUVmode][0].rn = DENOISE_ABSDIFF_MAX;
        HM_Double[YUVmode][0].sn = 0;
        HM_Double[YUVmode][1].xn = DENOISE_ABSDIFF_MAX;
        HM_Double[YUVmode][1].yn = maskMax;
        HM_Double[YUVmode][1].rn = DENOISE_ABSDIFF_MAX;
        HM_Double[YUVmode][1].sn = 0;

    }
    return 1;
}

static int DN_generateHermiteSmall(int YUVmode, int absdiff)
{
    int idx = 0,yy=0,start=0,jj=0;

    //else if(u8BitAcFormat == BIT_AC_DOUBLE)
    {

        HM_Double[YUVmode][idx].xn = 0;
        idx++;
        if (absdiff >= 6)
        {
            start = absdiff - 6;
            for (yy = start >> 1, jj = 2; yy > 0; yy = yy >> 1, jj = jj << 1)
            {
                if ((yy & 1) == 1)
                {
                    HM_Double[YUVmode][idx].xn = HM_Double[YUVmode][idx - 1].xn + jj;
                    HM_Double[YUVmode][idx - 1].rn = jj;
                    idx++;
                }
            }
            HM_Double[YUVmode][idx - 1].rn = 4;
            HM_Double[YUVmode][idx].xn = absdiff - 2;
            HM_Double[YUVmode][idx].rn = 4;
            idx++;
        }
        else if (absdiff >= 2)
        {
            start = 2;
            HM_Double[YUVmode][idx - 1].rn = start;
            HM_Double[YUVmode][idx].xn = start;
            HM_Double[YUVmode][idx].rn = absdiff + 2 - start;
            idx++;
        }
        HM_Double[YUVmode][idx].xn = absdiff + 2;
        HM_Double[YUVmode][idx].rn = 4;
        idx++;
        HM_Double[YUVmode][idx].xn = absdiff + 6;
        HM_Double[YUVmode][idx].rn = 8;
        idx++;
        HM_Double[YUVmode][idx].xn = absdiff + 14;
        HM_Double[YUVmode][idx].rn = 128;
        idx++;
        HM_Double[YUVmode][idx].xn = absdiff + 14 + 128;
        HM_Double[YUVmode][idx].rn = 128;
        idx++;
        HM_Double[YUVmode][idx].xn = absdiff + 14 + 256;
    }
    idx++;
    return idx;
}

static void DN_generateHermiteIndex(unsigned char *T)
{
    for(int i = YUV_y ; i <= YUV_v; i++ )
    {
        T[i] &= 0xFE; // set absdiff even values between 0 and 254.
        if (T[i] == 0)
        nHMTsize[i] = DN_generateHermiteNone(i);
        else if (T[i] < 14)
        nHMTsize[i] = DN_generateHermiteSmall(i,T[i]);
        else
        nHMTsize[i] = DN_generateHermiteNormal(i,T[i]);
    }
    //printf("T=%d, Hermite Entries=%d\n", inT, nHMTsize);
}

int DN_generateHermiteTable(unsigned char *T, Q3_REG_VALUE *hermitetable)
{
    int i = 0, idx = 0;

    maskMin = M_value;

    memset(HM_Double, 0, sizeof(HM_Double));

    DN_generateHermiteIndex(T);

    for(i =YUV_y; i<= YUV_v; i++)
    {
        if(T[i] == 0 || T[i] >= DENOISE_ABSDIFF_MAX)
            return EXIT_FAILURE;
    }

    //else if(u8BitAcFormat == BIT_AC_DOUBLE)
    {
        for(i = YUV_y; i<= YUV_v; i++)
        {
            for (idx=0; idx < DENOISE_HERMITE_SEGMENTS-1; idx++)
            {
                if (HM_Double[i][idx].xn > DENOISE_ABSDIFF_MAX)
                {
                    HM_Double[i][idx].xn = 0xFF;
                    if (idx+1 < DENOISE_HERMITE_SEGMENTS - 1)
                        HM_Double[i][idx+1].xn = 0x100;
                    HM_Double[i][idx].yn = HM_Double[i][idx - 1].yn;
                    HM_Double[i][idx].rn = HM_Double[i][idx - 1].rn;
                    nHMTsize[i] = idx;
                }
                else
                {
                    HM_Double[i][idx].yn = DN_findDenoiseValue(i, HM_Double[i][idx].xn);
                    HM_Double[i][idx].sn = DN_findDenoiseValue(i, HM_Double[i][idx].xn + 1) - DN_findDenoiseValue(i, HM_Double[i][idx].xn);
                    HM_Double[i][idx].rn = HM_Double[i][idx + 1].xn - HM_Double[i][idx].xn;
                }
            }
        }
    }

    if((DN_infotmic_writeHermiteRegFile(hermitetable)) < 0)
        return EXIT_FAILURE ;
    return EXIT_SUCCESS;
}


//==========================================================================================================
//                        Q3 ISP POSE DN API
//==========================================================================================================
int DN_HermiteGenerator(unsigned char *threshold, unsigned char weighting, Q3_REG_VALUE *hermitetable)
{
    unsigned char u8t[3],idx, u8m;

    for(idx =0; idx<3; idx++)
    {
        u8t[idx] = *threshold;
        threshold++;
        u8t[idx]  = (u8t[idx] % 2)? u8t[idx] -1 :u8t[idx];
        u8t[idx]  = (u8t[idx] < T_MIN)? T_MIN :u8t[idx];
        u8t[idx]  = (u8t[idx] > T_MAX)? T_MAX :u8t[idx];
        T_value[idx]  = u8t[idx] ;
        //printf("threshold %d\n",u8t[idx]);
    }

    u8m = weighting;
    u8m = (u8m < M_MIN)? M_MIN :u8m ;
    u8m = (u8m > M_MAX)? M_MAX :u8m ;
    M_value = u8m * 0.1;

    if((DN_generateHermiteTable(T_value, hermitetable)) < 0)
        return EXIT_FAILURE ;

    #if DEBUG_MSG
    printf("M_value %f\n",M_value);
    printf("T_value %d\n",T_value[0]);
    printf("T_value %d\n",T_value[1]);
    printf("T_value %d\n",T_value[2]);
    #endif//#if DEBUG_MSG

    return EXIT_SUCCESS;
}

#if 0
int main(void)
{
    unsigned char weighting = 5;
    unsigned char threshold[3]={20,20,20};
    Q3_REG_VALUE hermiteTable[Q3_DENOISE_REG_NUM];
    memset(hermiteTable, 0, sizeof(hermiteTable));

    if((DN_HermiteGenerator(threshold, weighting, hermiteTable)) < 0 )
        perror("Hermite Generator error.\n");

    #if 1//DEBUG_MSG
    for(int idx = 0; idx< Q3_DENOISE_REG_NUM; idx++)
        printf("idx %8d\t\t0x%8X\t\t0x%X\n",idx, hermiteTable[idx].reg, hermiteTable[idx].value);
    #endif//#if DEBUG_MSG

    return EXIT_SUCCESS;
}
#endif

