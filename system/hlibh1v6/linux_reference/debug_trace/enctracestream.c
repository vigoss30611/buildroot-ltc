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
--  Description :  Encoder system model, stream tracing
--
------------------------------------------------------------------------------*/

#include <string.h>
#include "enctracestream.h"

/*------------------------------------------------------------------------------
  External compiler flags
--------------------------------------------------------------------------------

NO_2GB_LIMIT: Don't check 2GB limit when writing stream.trc

--------------------------------------------------------------------------------
  Module defines
------------------------------------------------------------------------------*/

/* Structure for storing stream trace information */
traceStream_s traceStream;

/*------------------------------------------------------------------------------
  Local function prototypes
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
  EncOpenStreamTrace()
------------------------------------------------------------------------------*/
i32 EncOpenStreamTrace(const char *filename)
{
	FILE *file;
        u32 reopen = 0;

        if (traceStream.file) {
                reopen = 1;
                EncCloseStreamTrace();
        }

	traceStream.file = fopen(filename, "a");

	if (traceStream.file == NULL) {
		fprintf(stderr, "Unable to open <%s>.", filename);
		return -1;
	}

	file = traceStream.file;
	fprintf(file, " Vop\t   Bit\tID   Data  Len\tBitPattern\t\t\tDescription\n");
	fprintf(file, "-----------------------------------------------");
	fprintf(file, "-----------------------------------------\n");

        /* If reopening trace file don't reset the tracing */
        if (reopen)
                return 0;

	traceStream.frameNum = 0;
	traceStream.bitCnt = 0;
	traceStream.disableStreamTrace = 0;
	traceStream.id = 0;

	return 0;
}

/*------------------------------------------------------------------------------
  EncCloseStreamTrace()
------------------------------------------------------------------------------*/
void EncCloseStreamTrace(void)
{
	if (traceStream.file != NULL)
		fclose(traceStream.file);
	traceStream.file = NULL;
}



/*------------------------------------------------------------------------------
  EncTraceStream()  Writes per symbol trace in ASCII format. Trace can be
  disable with traceStream.disableStreamTrace e.g. for unwanted headers.
  Id can be used to differentiate the source of the stream eg. SW=0 HW=1
------------------------------------------------------------------------------*/
void EncTraceStream(i32 value, i32 numberOfBits)
{
	static u32 lines = 0;
	FILE *file = traceStream.file;
	i32 i, j = 0, k = 0;

#ifndef NO_2GB_LIMIT
	/* Check for file size limit 2GB, approx 50 bytes/line */
	lines++;
	if (file && (lines*50 >= (1U<<31))) {
		fclose(file); file = traceStream.file = NULL;
	}
#endif

	if (file != NULL && !traceStream.disableStreamTrace) {
		fprintf(file,"%4i\t%6i\t%2i %6i  %2i\t",
				traceStream.frameNum, traceStream.bitCnt,
				traceStream.id, value, numberOfBits);

                if (numberOfBits < 0)
                    return;

		for (i = numberOfBits; i; i--) {
			if (value & (1 << (i - 1))) {
				fprintf(file, "1");
			} else {
				fprintf(file, "0");
			}
			j++;
			if (j == 4) {
				fprintf(file, " ");
				j = 0;
				k++;
			}
		}
		for (i = numberOfBits + k; i < 32; i += 8) {
			fprintf(file, "\t");
		}
	}

	traceStream.bitCnt += numberOfBits;
}

/*------------------------------------------------------------------------------
  EncCommentMbType()
------------------------------------------------------------------------------*/
void EncCommentMbType(const char *comment, i32 mbNum)
{
	FILE *file = traceStream.file;

	if (file == NULL || traceStream.disableStreamTrace) {
		return;
	}
	fprintf(file, "mb_type %-22s [%.3i]\n", comment, mbNum);
}

/*------------------------------------------------------------------------------
  EncComment()
------------------------------------------------------------------------------*/
void EncComment(const char *comment)
{
	FILE *file = traceStream.file;

	if (file == NULL || traceStream.disableStreamTrace) {
		return;
	}
	fprintf(file, "%s\n", comment);
}


