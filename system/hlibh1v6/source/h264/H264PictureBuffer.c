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
------------------------------------------------------------------------------*/

#include "enccommon.h"
#include "H264PictureBuffer.h"

/*------------------------------------------------------------------------------
	External compiler flags
------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------
	Module defines
------------------------------------------------------------------------------*/

/* Enable this to print contents of picture buffer */
/*#define TRACE_PIC_BUFFER*/

/*------------------------------------------------------------------------------
	Local function prototypes
------------------------------------------------------------------------------*/
static i32 Alloc(refPic *refPic, i32 width, i32 height);
static void RefPicListInitialization(picBuffer *ref);
static void ResetRefPic(refPic *refPic);
#ifdef TRACE_PIC_BUFFER
static void PrintPicBuffer(picBuffer *picBuffer);
#endif

/*------------------------------------------------------------------------------
	PictureBufferAlloc
------------------------------------------------------------------------------*/
i32 H264PictureBufferAlloc(picBuffer *picBuffer, i32 width, i32 height)
{
	i32 i;

	/* Be sure that everything is initialized if something goes wrong */
	EWLmemset(picBuffer->refPic, 0, sizeof(picBuffer->refPic));
	EWLmemset(picBuffer->refPicList, 0, sizeof(picBuffer->refPicList));

	/* Reference frame base (lum,cb) and macroblock stuff */
	for (i = 0; i < BUFFER_SIZE + 1; i++) {
		if (Alloc(&picBuffer->refPic[i], width, height) != ENCHW_OK) {
			H264PictureBufferFree(picBuffer);
			return ENCHW_NOK;
		}
		/* Back reference pointer (pointer to itself) */
		picBuffer->refPic[i].refPic = &picBuffer->refPic[i];
	}
	picBuffer->cur_pic = &picBuffer->refPic[0];

	return ENCHW_OK;
}

/*------------------------------------------------------------------------------
	PictureBufferFree
------------------------------------------------------------------------------*/
void H264PictureBufferFree(picBuffer *picBuffer)
{
	EWLmemset(picBuffer->refPic, 0, sizeof(picBuffer->refPic));
}

/*------------------------------------------------------------------------------
	Alloc Reserve memory of reference picture.
------------------------------------------------------------------------------*/
i32 Alloc(refPic *refPic, i32 width, i32 height)
{
	refPic->picture.lumWidth  = width;
	refPic->picture.lumHeight = height;
	refPic->picture.chWidth   = width/2;
	refPic->picture.chHeight  = height/2;
        refPic->picture.lum       = 0;
        refPic->picture.cb        = 0;

	return ENCHW_OK;
}

/*------------------------------------------------------------------------------
	InitializePictureBuffer
------------------------------------------------------------------------------*/
#define false 0
#define true 1
void H264InitializePictureBuffer(picBuffer *picBuffer)
{
	i32 i;

	/* I frame (key frame) resets reference pictures */
	if (picBuffer->cur_pic->i_frame) {
		picBuffer->cur_pic->p_frame = false;
		picBuffer->cur_pic->ipf = true;
		picBuffer->cur_pic->grf = false; /* Only mark LTR when told. */
		picBuffer->cur_pic->arf = false;
		for (i = 0; i < BUFFER_SIZE + 1; i++) {
			if (&picBuffer->refPic[i] != picBuffer->cur_pic) {
				ResetRefPic(&picBuffer->refPic[i]);
			}
		}
	}

	/* Initialize reference picture list, note that API (user) can change
	 * reference picture list */
	for (i = 0; i < BUFFER_SIZE; i++) {
		ResetRefPic(&picBuffer->refPicList[i]);
	}
	RefPicListInitialization(picBuffer);
}

/*------------------------------------------------------------------------------
	UpdatePictureBuffer
------------------------------------------------------------------------------*/
void H264UpdatePictureBuffer(picBuffer *picBuffer)
{
	refPic *cur_pic, *tmp, *refPic, *refPicList;
	i32 i, j;

	refPicList = picBuffer->refPicList;	/* Reference frame list */
	refPic     = picBuffer->refPic;		/* Reference frame store */
	cur_pic    = picBuffer->cur_pic;	/* Reconstructed picture */
	picBuffer->last_pic = picBuffer->cur_pic;

	/* Reset old marks from reference frame store if user wants to change
	 * current ips/grf/arf frames. */

	/* Input picture marking */
	for (i = 0; i < picBuffer->size + 1; i++) {
		if (&refPic[i] == cur_pic) continue;
		if (cur_pic->ipf) refPic[i].ipf = false;
		if (cur_pic->grf) {
            refPic[i].grf = false;
            /*refPic[i].search = false;*/
            if (!cur_pic->i_frame)
                refPic[i].ipf = false;
        }
		if (cur_pic->arf) refPic[i].arf = false;
	}

	/* Reference picture marking */
	for (i = 0; i < picBuffer->size; i++) {
		for (j = 0; j < picBuffer->size + 1; j++) {
			if (refPicList[i].grf) refPic[j].grf = false;
			if (refPicList[i].arf) refPic[j].arf = false;
		}
	}

	/* Reference picture status is changed */
	for (i = 0; i < picBuffer->size; i++) {
		if (refPicList[i].grf) refPicList[i].refPic->grf = true;
		if (refPicList[i].arf) refPicList[i].refPic->arf = true;
	}

	/* Find new picture not used as reference and set it to new cur_pic */
	for (i = 0; i < picBuffer->size + 1; i++) {
		tmp = &refPic[i];
		if (!tmp->ipf && !tmp->arf && !tmp->grf) {
			picBuffer->cur_pic = &refPic[i];
			break;
		}
	}
}

/*------------------------------------------------------------------------------
	RefPicListInitialization
------------------------------------------------------------------------------*/
void RefPicListInitialization(picBuffer *picBuffer)
{
	refPic *cur_pic, *refPic, *refPicList;
	i32 i, j = 0;

	refPicList = picBuffer->refPicList;	/* Reference frame list */
	refPic     = picBuffer->refPic;		/* Reference frame store */
	cur_pic    = picBuffer->cur_pic;	/* Reconstructed picture */

	/* The first in the list is immediately previous picture. Note that
	 * cur_pic (the picture under reconstruction) is skipped */
	for (i = 0; i < picBuffer->size + 1; i++) {
		if (refPic[i].ipf && (&refPic[i] != cur_pic)) {
			refPicList[j++] = refPic[i];
			break;
		}
	}

	/* The second in the list is golden frame */
	for (i = 0; i < picBuffer->size + 1; i++) {
		if (refPic[i].grf && (&refPic[i] != cur_pic)) {
			refPicList[j++] = refPic[i];
			break;
		}
	}

	/* The third in the list is alternative reference frame */
	for (i = 0; i < picBuffer->size + 1; i++) {
		if (refPic[i].arf && (&refPic[i] != cur_pic)) {
			refPicList[j] = refPic[i];
			break;
		}
	}

	/* Reset the ipf/grf/arf flags */
	for (i = 0; i < picBuffer->size; i++) {
		/*refPicList[i].ipf = false;*/
		refPicList[i].arf = false;
	}
}

/*------------------------------------------------------------------------------
	ResetRefPic
------------------------------------------------------------------------------*/
void ResetRefPic(refPic *refPic)
{
	refPic->poc = -1;
	refPic->i_frame = false;
	refPic->p_frame = false;
	refPic->show = false;
	refPic->ipf = false;
	refPic->arf = false;
	refPic->grf = false;
	refPic->search = false;
}

/*------------------------------------------------------------------------------
	PictureBufferSetRef

        Set the ASIC reference and reconstructed frame buffers based
        on the user preference and picture buffer.
------------------------------------------------------------------------------*/
void H264PictureBufferSetRef(picBuffer *picBuffer, asicData_s *asic, u32 numViews)
{
    i32 i, j, refIdx = -1, refIdx2 = -1;
    refPic *refPicList = picBuffer->refPicList;
    i32 noGrf = 0, noArf = 0;

    /* Amount of buffered frames limits grf/arf availability. */
    if (picBuffer->size < 2) { picBuffer->cur_pic->grf = false; }
    picBuffer->cur_pic->arf = false;

    /* ASIC can use one or two reference frame, use the first ones marked
     * for reference but don't use the same frame for both. */
    for (i = 0; i < BUFFER_SIZE; i++) {
        if ((i < picBuffer->size) && refPicList[i].search &&
            (refPicList[i].ipf || refPicList[i].grf)) {
            if (refIdx == -1)
                refIdx = i;
            else if ((refIdx2 == -1) &&
                     (refPicList[refIdx].picture.lum !=
                      refPicList[i].picture.lum))
                refIdx2 = i;
            else
                refPicList[i].search = 0;
        } else {
            refPicList[i].search = 0;
        }
    }

    /* If no reference specified, use ipf if available, otherwise ltr */
    if (refIdx == -1)
    {
        refIdx = 0;
        picBuffer->refPicList[refIdx].search = 1;
    }

    asic->regs.mvRefIdx[0] = asic->regs.markCurrentLongTerm = 0;

    /* Set the reference buffer for ASIC, no reference for intra frames */
    if (numViews == 1 && picBuffer->cur_pic->p_frame) {
        /* Mark the ref pic that is used */
        ASSERT(picBuffer->refPicList[refIdx].search == 1);

        /* Check that enough frame buffers is available. */
        ASSERT(refPicList[refIdx].picture.lum);

        asic->regs.internalImageLumBaseR[0] = refPicList[refIdx].picture.lum;
        asic->regs.internalImageChrBaseR[0] = refPicList[refIdx].picture.cb;
        asic->regs.internalImageLumBaseR[1] = refPicList[refIdx].picture.lum;
        asic->regs.internalImageChrBaseR[1] = refPicList[refIdx].picture.cb;
        asic->regs.ref2Enable = 0;
        if (refIdx || (refPicList[refIdx].grf && !refPicList[refIdx].i_frame))
            asic->regs.mvRefIdx[0] = 1; /* long-term as primary ref */

        /* Enable second reference frame usage */
        if (refIdx2 != -1 && !refPicList[refIdx].grf) {
            asic->regs.internalImageLumBaseR[1] = refPicList[refIdx2].picture.lum;
            asic->regs.internalImageChrBaseR[1] = refPicList[refIdx2].picture.cb;
            asic->regs.ref2Enable = 1;
        }
    }

    if (picBuffer->cur_pic->grf)
    {
        asic->regs.markCurrentLongTerm = 1; /* current marked as long term */
    }

    /* Set the reconstructed frame buffer for ASIC. Luma can be written
     * to same buffer but chroma read and write buffers must be different. */
    asic->regs.recWriteDisable = 0;
    if (!picBuffer->cur_pic->picture.lum) {
        refPic *cur_pic = picBuffer->cur_pic;
        refPic *cand;
        i32 recIdx = -1;

        /* No free luma buffer so we must "steal" a luma buffer from
         * some other ref pic that is no longer needed. */
        for (i = 0; i < picBuffer->size + 1; i++) {
            noGrf = noArf = 0;
            cand = &picBuffer->refPic[i];
            if (cand == cur_pic) continue;
            /* Also check the list for frame buffer copy flags. */
            for (j = 0; j < picBuffer->size; j++)
                if (refPicList[j].refPic == cand) {
                    noGrf = refPicList[j].grf;
                    noArf = refPicList[j].arf;
                }
            /* Buffer is available if cur_pic refreshes the reference. */
            if (((cur_pic->ipf | cand->ipf        ) == cur_pic->ipf) &&
                ((cur_pic->grf | cand->grf | noGrf) == cur_pic->grf) &&
                ((cur_pic->arf | cand->arf | noArf) == cur_pic->arf))
                recIdx = i;
        }

        if (recIdx >= 0) {
            /* recIdx is overwritten or unused so steal it */
            cur_pic->picture.lum = picBuffer->refPic[recIdx].picture.lum;
            picBuffer->refPic[recIdx].picture.lum = 0;
        } else {
            /* No available buffer found, must be no refresh */
            ASSERT((cur_pic->ipf | cur_pic->grf | cur_pic->arf) == 0);
            asic->regs.recWriteDisable = 1;
        }
    }

    if (!picBuffer->cur_pic->ipf && !picBuffer->cur_pic->grf)
        asic->regs.recWriteDisable = 1;

    if (numViews == 1)
    {
        asic->regs.internalImageLumBaseW = picBuffer->cur_pic->picture.lum;
        asic->regs.internalImageChrBaseW = picBuffer->cur_pic->picture.cb;
    }

    /* For P-frames the chroma buffers must be different. */
    if (picBuffer->cur_pic->p_frame){
        if(asic->regs.internalImageChrBaseR[0] == asic->regs.internalImageChrBaseW)
        {
            printf("ASSERT ! asic->regs.internalImageChrBaseR[0] (%d) == asic->regs.internalImageChrBaseW(%d)\n",
                asic->regs.internalImageChrBaseR[0], asic->regs.internalImageChrBaseW);
        }
    }

    /* If current picture shall refresh grf/arf remove marks from ref list */
    for (i = 0; i < picBuffer->size; i++) {
        if (picBuffer->cur_pic->grf)
            picBuffer->refPicList[i].grf = false;
    }

#ifdef TRACE_PIC_BUFFER
    PrintPicBuffer(picBuffer);
#endif
}

#ifdef TRACE_PIC_BUFFER
/*------------------------------------------------------------------------------
	PrintPicBuffer
------------------------------------------------------------------------------*/
void PrintPicBuffer(picBuffer *picBuffer)
{
	i32 i;
	refPic *refPic;

	refPic = picBuffer->cur_pic;
	printf("Current picture\n");
	printf("Poc i_frame p_frame show ipf grf arf search:\n");
		printf("%-3i %-7i %-7i %-4i %-3i %-3i %-3i %-6i %p %p %p %p\n",
			refPic->poc,
			refPic->i_frame,
			refPic->p_frame,
			refPic->show,
			refPic->ipf,
			refPic->grf,
			refPic->arf,
			refPic->search,
			refPic,
			refPic->refPic,
                        refPic->picture.lum, refPic->picture.cb);

	refPic = picBuffer->refPic;
	printf("Ref refPic[picBuffer->size + 1] \n");
	printf("Poc i_frame p_frame show ipf grf arf search:\n");
	for (i = 0; i < picBuffer->size + 1; i++) {
		printf("%-3i %-7i %-7i %-4i %-3i %-3i %-3i %-6i %p %p %p %p\n",
			refPic[i].poc,
			refPic[i].i_frame,
			refPic[i].p_frame,
			refPic[i].show,
			refPic[i].ipf,
			refPic[i].grf,
			refPic[i].arf,
			refPic[i].search,
			&refPic[i],
			refPic[i].refPic,
                        refPic[i].picture.lum, refPic[i].picture.cb);
	}

	refPic = picBuffer->refPicList;
	printf("Ref refPicList[picBuffer->size] \n");
	printf("Poc i_frame p_frame show ipf grf arf search:\n");
	for (i = 0; i < picBuffer->size; i++) {
		printf("%-3i %-7i %-7i %-4i %-3i %-3i %-3i %-6i %p %p %p %p\n",
			refPic[i].poc,
			refPic[i].i_frame,
			refPic[i].p_frame,
			refPic[i].show,
			refPic[i].ipf,
			refPic[i].grf,
			refPic[i].arf,
			refPic[i].search,
			&refPic[i],
			refPic[i].refPic,
                        refPic[i].picture.lum, refPic[i].picture.cb);
	}
}
#endif
