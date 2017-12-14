/*------------------------------------------------------------------------------
--                                                                            --
--       This software is confidential and proprietary and may be used        --
--        only as expressly authorized by a licensing agreement from          --
--                                                                            --
--                            Hantro Products Oy.                             --
--                                                                            --
--                   (C) COPYRIGHT 2006 HANTRO PRODUCTS OY                    --
--                            ALL RIGHTS RESERVED                             --
--                                                                            --
--                 The entire notice above must be reproduced                 --
--                  on all copies and should not be removed.                  --
--                                                                            --
--------------------------------------------------------------------------------
--
--  Abstract : Integration test register testbench
--
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
    1. Include headers
------------------------------------------------------------------------------*/

/* For encoder EWL interface */
#include "ewl.h"

/* For printing and file IO */
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#ifdef USE_EFENCE
#include "efence.h"
#endif

#define ERR_OUTPUT stdout

/*------------------------------------------------------------------------------
    2. External compiler flags
--------------------------------------------------------------------------------

                 
--------------------------------------------------------------------------------
    3. Module defines
------------------------------------------------------------------------------*/

/* User selectable testbench configuration */

/* Buffer size in 64-bit words */
#define BUFFER_SIZE		(4096)

/* Amount of data to be copied from input buffer 
   to output buffer in 64-bit words */
#define DATA_LENGTH		(2048)

/* Global variables */

/* EWL instance */
const void *ewlInst = NULL;

/* SW/HW shared memories */
EWLLinearMem_t inbuf;
EWLLinearMem_t outbuf;

/*------------------------------------------------------------------------------
    4. Local function prototypes
------------------------------------------------------------------------------*/
static int AllocRes(void);
static void FreeRes(void);
static void TestRegister(void);

void MemSet(u32 * buffer, int size);

/*------------------------------------------------------------------------------

    main

	This testbench runs a number of tests for the ASIC integration register.
	It uses the EWL interface to access the ASIC registers and to allocate
	HW/SW shared memories. 

------------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
    u32 id;
    i32 ret;
    EWLInitParam_t param;

    /* First we call the EWLReadAsicID function to read the ID from the 
     * ASIC register */
    id = EWLReadAsicID();

    /* Print the ID to the screen */
    fprintf(stdout, "\nASIC Integration register test\n");
    fprintf(stdout, "HW ID: 0x%08x\n", id);

    /* We set the EWL parameters */
    param.clientType = EWL_CLIENT_TYPE_H264_ENC;
    /* Then we call the EWLInit function to initialize the EWL
     * The EWL implementation is platform specific but by using
     * the EWL interface we make this testbench platform independent. */
    ewlInst = EWLInit(&param);

    /* Check the return value and exit if unsuccessful */
    if(!ewlInst)
    {
        fprintf(ERR_OUTPUT, "Failed to initialize EWL\n");
        return -1;
    }

    /* Allocate input and output buffers */
    if(AllocRes() != 0)
    {
        fprintf(ERR_OUTPUT, "Failed to allocate the external resources!\n");
        goto end;
    }

    if(EWLReserveHw(ewlInst) != EWL_OK)
    {
        fprintf(ERR_OUTPUT, "Failed to lock HW!\n");
        goto end;
    }

    /* Perform the integration register tests */
    TestRegister();

    EWLReleaseHw(ewlInst);

  end:

    /* Free memories */
    FreeRes();

    /* Release EWL instance */
    ret = EWLRelease(ewlInst);
    if(ret != EWL_OK)
    {
        fprintf(ERR_OUTPUT, "Failed to release the instance. Error code: %8i\n",
                ret);
    }

    return ret;
}

/*------------------------------------------------------------------------------

    AllocRes

    Allocate the physical memories used by both SW and HW.

    To access the memory HW uses the physical linear address (bus address) 
    and SW uses virtual address (user address).

------------------------------------------------------------------------------*/
int AllocRes(void)
{
    i32 ret;

    /* Call EWL to allocate buffer for HW input */
    ret = EWLMallocLinear(ewlInst, BUFFER_SIZE * 8, &inbuf);

    if(ret != EWL_OK)
    {
        return -1;
    }

    /* Call EWL to allocate buffer for HW output */
    ret = EWLMallocLinear(ewlInst, BUFFER_SIZE * 8, &outbuf);

    if(ret != EWL_OK)
    {
        return -1;
    }

    /* Print the buffer information */
    printf("Input buffer size:\t%8d bytes\nOutput buffer size:\t%8d bytes\n",
           inbuf.size, outbuf.size);

    printf("Input buffer bus address:\t0x%08x\n", inbuf.busAddress);
    printf("Input buffer user address:\t0x%08x\n", (u32) inbuf.virtualAddress);
    printf("Output buffer bus address:\t0x%08x\n", outbuf.busAddress);
    printf("Output buffer user address:\t0x%08x\n",
           (u32) outbuf.virtualAddress);

    return 0;
}

/*------------------------------------------------------------------------------

    FreeRes

------------------------------------------------------------------------------*/
void FreeRes(void)
{

    EWLFreeLinear(ewlInst, &inbuf);
    EWLFreeLinear(ewlInst, &outbuf);

}

/*------------------------------------------------------------------------------

    TestRegister

------------------------------------------------------------------------------*/
void TestRegister(void)
{
    i32 i;
    u32 value;
    i32 ret;

    /* Interrupt test: integration test register bit 0 
     * When bit 0 is set to 1 HW should generate interrupt */
    fprintf(stdout,
            "\nASIC Integration register test bit 0\n\tIRQ generation\n");

    EWLWriteReg(ewlInst, 0x04, 0);  /* enable IRQ generation */

    value = 0x1;
    EWLWriteReg(ewlInst, 0xC, value);

    /* Check for interrupt */
    ret = EWLWaitHwRdy(ewlInst, NULL);

    if(ret != EWL_OK)
    {
        fprintf(ERR_OUTPUT, "\tFAILED to receive interrupt. Error code: %8i\n",
                ret);
    }
    else
    {
        fprintf(stdout, "\tPASSED\n");
    }

    EWLWriteReg(ewlInst, 0xC, 0);   /* clear test IRQ generation */
    EWLWriteReg(ewlInst, 0x04, 0);  /* clear IRQ status */

    /* Register cache coherency test: integration test register bit 1 
     * When bit 1 is set to 1 HW should increase the value of bits 31:28 */
    fprintf(stdout,
            "ASIC Integration register test bit 1\n\tRegister aceess\n");
    value = 0xA0000002;
    EWLWriteReg(ewlInst, 0xC, value);

    /* Read the updated value from register */
    value = EWLReadReg(ewlInst, 0xC);

    if(value != 0xB0000002)
    {
        fprintf(ERR_OUTPUT,
                "\tFAILED to increment register. Updated register value: 0x%08x\n",
                value);
    }
    else
    {
        fprintf(stdout, "\tPASSED\n");
    }

    /* Memory cache coherency test: integration test register bit 2 
     * When bit 2 is set to 1 HW should copy data from base address of swreg5
     * to base address of swreg6. Data length is in bits 20:3 in 64-bit words */
    fprintf(stdout, "ASIC Integration register test bit 2\n");
    fprintf(stdout, "\tCopying %i bytes of data\n", DATA_LENGTH * 8);

    /* Prepare data buffers */
    EWLmemset(inbuf.virtualAddress, 0, BUFFER_SIZE * 8);
    EWLmemset(outbuf.virtualAddress, 0, BUFFER_SIZE * 8);

    MemSet(inbuf.virtualAddress, DATA_LENGTH * 8);

    /* Write input buffer bus address to swreg5 */
    EWLWriteReg(ewlInst, 0x14, inbuf.busAddress);

    /* Write output buffer bus address to swreg6 */
    EWLWriteReg(ewlInst, 0x18, outbuf.busAddress);

    /* Write test bit and data length to swreg3 */
    value = 0x00000004 | (DATA_LENGTH << 3);
    EWLWriteReg(ewlInst, 0xC, value);

    /* Check for interrupt */
    ret = EWLWaitHwRdy(ewlInst, NULL);

    if(ret != EWL_OK)
    {
        fprintf(ERR_OUTPUT, "\tFAILED to receive interrupt. Error code: %8i\n",
                ret);
    }

    ret = 0;

    /* Check that the output buffer has correct data, using 32-bit words */
    for(i = 0; i < DATA_LENGTH * 2; i++)
    {
        if(outbuf.virtualAddress[i] != inbuf.virtualAddress[i])
        {
            fprintf(ERR_OUTPUT,
                    "\tFAILED at 0x%08x [0x%08x] != [0x%08x]\n",
                    (u32) (outbuf.virtualAddress + i), outbuf.virtualAddress[i],
                    inbuf.virtualAddress[i]);
            ret = -1;
            break;
        }
    }

    /* Check that the output buffer beyond the copied area is not changed */
    for(i = DATA_LENGTH * 2; i < BUFFER_SIZE * 2; i++)
    {
        if(outbuf.virtualAddress[i] != 0x0)
        {
            fprintf(ERR_OUTPUT, "\tBuffer changed at 0x%08x [0x%08x]\n",
                    (u32) (outbuf.virtualAddress + i),
                    outbuf.virtualAddress[i]);
            ret = -1;
            break;
        }
    }

    EWLWriteReg(ewlInst, 0x0C, 0);  /* clear test IRQ generation */
    EWLWriteReg(ewlInst, 0x04, 0);  /* clear IRQ status */

    if(ret == 0)
    {
        fprintf(stdout, "\tPASSED\n\n");
    }
}

/* initialize memory buffer with random stuff */
void MemSet(u32 * buffer, int size)
{
    int i;
    int seed = (int) time(NULL);

    srand(seed++);
    for(i = 0; i < (size / 4); i++)
    {
        if(i >= RAND_MAX)
            srand(seed++);

        buffer[i] = rand();
    }
}
