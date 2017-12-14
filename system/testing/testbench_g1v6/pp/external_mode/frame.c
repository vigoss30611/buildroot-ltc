/*------------------------------------------------------------------------------
--                                                                            --
--       This software is confidential and proprietary and may be used        --
--        only as expressly authorized by a licensing agreement from          --
--                                                                            --
--                            Hantro Products Oy.                             --
--                                                                            --
--                   (C) COPYRIGHT 2011 HANTRO PRODUCTS OY                    --
--                            ALL RIGHTS RESERVED                             --
--                                                                            --
--                 The entire notice above must be reproduced                 --
--                  on all copies and should not be removed.                  --
--                                                                            --
--------------------------------------------------------------------------------
--
--  Abstract :
------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "frame.h"

/*------------------------------------------------------------------------------

   <++>.<++>  Function: FrameBytes

        Functional description:
          Calculate number of bytes required by frame.

        Inputs:

        Outputs:

------------------------------------------------------------------------------*/
u32 FrameBytes( PixelData *p, RgbFormat * r )
{
    u32 bytes = 0;

    ASSERT(p);

    switch( PP_INTERNAL_FMT(p->fmt) ) {
        case YUV422:
            bytes = p->width * p->height * 2;
            break;
        case YUV420:
            bytes = p->width * p->height * 1.5;
            break;
        case YUV400:
            bytes = p->width * p->height;
            break;
        case RGBP:
            bytes = r->bytes; /* RgbFormat knows this better */
            break;
        case YUV444:
            bytes = p->width * p->height * 3;
            break;
        default:
            ASSERT(FALSE);
    }
    return bytes;
}


/*------------------------------------------------------------------------------

   <++>.<++>  Function: ReadFrame

        Functional description:
          Read a frame from a YUV or RGB or similar sequence

        Inputs:

        Outputs:

------------------------------------------------------------------------------*/
bool ReadFrame( char * filename, PixelData * p, u32 frame, PixelFormat fmt,
    u32 separateFields)
{
    FILE * fid;
    u32     bytes;
    u32     y, x, w, h;
    u32     read = 0;
    long streamPos;

    ASSERT(filename);
    ASSERT(p);
    ASSERT(p->base);

    if( !(PP_INTERNAL_FMT(fmt) == YUV422 ||
          PP_INTERNAL_FMT(fmt) == YUV420) )
        return FALSE;

    if(! (fid = fopen( filename, "r" )) ) {
        return FALSE;
    }

    p->fmt = PP_INTERNAL_FMT(fmt);
    bytes = FrameBytes(p, NULL);

    streamPos = bytes;
    streamPos *= frame;
    fseeko64( fid, streamPos, SEEK_SET );
    if(feof( fid )) {
        fclose(fid);
        return FALSE;
    }

    if (separateFields)
    {
        w =  PP_INTERNAL_FMT(fmt) == YUV420 ? p->width : 2*p->width;
        h = p->height;
        for (y = 0; y < h/2; y++)
        {
            read = fread(p->base+y*w,w,1,fid);
            read = fread(p->base+h/2*w+y*w,w,1,fid);
        }
        if (PP_INTERNAL_FMT(fmt) == YUV420)
        {
            for (y = 0; y < p->height/4; y++)
            {
                read = fread(p->base+h*w+y*w,w,1,fid);
                read = fread(p->base+h*w+h/4*w+y*w,w,1,fid);
            }
        }
    }
    else
        read = fread(p->base,bytes,1,fid);

    fclose(fid);
    return read > 0 ? TRUE : FALSE;
}

/*------------------------------------------------------------------------------

   <++>.<++>  Function: WriteByteOrdered

        Functional description:

        Inputs:

        Outputs:

------------------------------------------------------------------------------*/
u32 WriteByteOrdered( FILE *fid, u8 *buffer, u32 bytes, ByteFormat * bf, bool flush )
{
    static u8   writeBuf[ PP_MAX_WORD_WIDTH_BYTES ];
    static u32  filled = 0;
    u32         i;
    u32         written = 0;

    /* If flush specified, just purge the remaining stuff, if there is
       anything left. */
    if(flush) {
        if(filled) {
            for( i = filled ; i < bf->wordWidth ; ++i ) {
                writeBuf[ bf->byteOrder[i] ] = 0;
            }
            fwrite( writeBuf, sizeof(u8), filled, fid );
            filled = 0;
        }
        return bf->wordWidth;
    }

    /* Pack bytes */
    for( i = 0 ; i < bytes ; ++i ) {
        writeBuf[ bf->byteOrder[filled++] ] = buffer[ i ];
        /* Write on word boundary */
        if(filled == bf->wordWidth ) {
            written += fwrite( writeBuf, sizeof(u8), filled, fid );
            filled = 0;
        }
    }
    return written;
}


/*------------------------------------------------------------------------------

   <++>.<++>  Function: PackRgbaBytes

        Functional description:
          Pack RGB channels into byte array using specified format.

        Inputs:

        Outputs:

------------------------------------------------------------------------------*/
void PackRgbaBytes( u8 c0, u8 c1, u8 c2, u8 c3, RgbFormat *fmt, u8 *buf )
{
    u32 val = 0;
    u32 fmtBytes = fmt->bytes + (fmt->bytes&1); /* round up to 32/16 bits */
    u32 bitOffs = fmt->bytes * 8;

    /* one-mask is not needed as padding is now all zeroes */
#if 0
    const u32 oneMask =
        (0xFFFFFFFF & ~(fmt->mask[0] | fmt->mask[1] | fmt->mask[2] | fmt->mask[3]))
        << ((4 - fmtBytes)*8) & /* shift mask to correct alignment */
        ~((1<<fmt->pad[4])-1); /* last padding goes as zeroes . . . */
#endif
    /* Shift values to target precision. */
    /* done already beforehand! */
/*    c0 = c0 >> ( 8 - fmt->chan[0] );
    c1 = c1 >> ( 8 - fmt->chan[1] );
    c2 = c2 >> ( 8 - fmt->chan[2] );*/

    /* Code to implement rounding, not used as of now */
    /*
    u32 v0, v1, v2;
    v0 = (fmt->chan0 == 8) ? c0 :
        (((u32)c0) + (1 << ( 7 - fmt->chan0 ))) >> ( 8 - fmt->chan0 );
    v1 = (fmt->chan1 == 8) ? c1 :
        (((u32)c1) + (1 << ( 7 - fmt->chan1 ))) >> ( 8 - fmt->chan1 );
    v2 = (fmt->chan2 == 8) ? c2 :
        (((u32)c2) + (1 << ( 7 - fmt->chan2 ))) >> ( 8 - fmt->chan2 );
    v0 = SATURATE( 0, (1 << fmt->chan0)-1, v0 );
    v1 = SATURATE( 0, (1 << fmt->chan1)-1, v1 );
    v2 = SATURATE( 0, (1 << fmt->chan2)-1, v2 );
    val = ( v0 << ( 32 - (fmt->pad0 + fmt->chan0 ) ) ) |
          ( v1 << ( 32 - (fmt->pad0 + fmt->chan0 + fmt->pad1 + fmt->chan1 ) ) ) |
          ( v2 << ( 32 - (fmt->pad0 + fmt->chan0 + fmt->pad1 + fmt->chan1 + fmt->pad2 + fmt->chan2 ) ) );
    */
    /* Pack values to a 32-bit integer. Any extra padding will be added to
       the end. */
    val = ( c0 << ( bitOffs - (fmt->pad[0] + fmt->chan[0] ) ) ) |
          ( c1 << ( bitOffs - (fmt->pad[0] + fmt->chan[0] + fmt->pad[1] + fmt->chan[1] ) ) ) |
          ( c2 << ( bitOffs - (fmt->pad[0] + fmt->chan[0] + fmt->pad[1] + fmt->chan[1] + fmt->pad[2] + fmt->chan[2] ) ) ) |
          ( c3 << ( bitOffs - (fmt->pad[0] + fmt->chan[0] + fmt->pad[1] + fmt->chan[1] + fmt->pad[2] + fmt->chan[2] + fmt->pad[3] + fmt->chan[3] ) ) );
    /* Add one-padding to unused bits */

/*    val |= oneMask;*/
    switch(fmtBytes) {
        case 2:
            buf[0] = (val >> 8) & 0xFF;
            buf[1] = val & 0xFF;
            break;
        case 4:
            buf[0] = (val >> 24) & 0xFF;
            buf[1] = (val >> 16) & 0xFF;
            buf[2] = (val >> 8) & 0xFF;
            buf[3] = val & 0xFF;
            break;
        default:
            ASSERT(FALSE);
    }
}

/*------------------------------------------------------------------------------

   <++>.<++>  Function: WriteFrame

        Functional description:
          Write a frame into a YUV or RGB or similar sequence. Output file
          will be truncated when first frame is written.

        Inputs:

        Outputs:

------------------------------------------------------------------------------*/
void WriteFrame( char * filename, PixelData * p, int frame, RgbFormat * r, ByteFormat * bf)
{
    FILE * fid;
    u32    bytes, w, h, x, y, c;
    u8     rgb[ PP_MAX_RGB_BYTES ]; /* Byte array used in packing interleaved
                                       RGB channels. */
    long streamPos;

    ASSERT(filename);
    ASSERT(p);
    ASSERT(p->base);

    bytes = FrameBytes(p, r);
    /* Open file. For the first write, open in "w" mode which truncates any
       existing file into 0 bytes. Subsequent writes are performed in "a"
       mode. */
    if(! (fid = fopen( filename, frame == 0 ? "w" : "a")) ) {
        return;
    }
    streamPos = bytes;
    streamPos *= frame;
    fseeko64( fid, streamPos, SEEK_SET );
    w = p->width;
    h = p->height;

    /* Channels interleaved? */
    if(PP_IS_INTERLEAVED(p->fmt)) {
        switch(p->fmt) {
            case RGB:
                /* Write interleaved channels */
                for( y = 0 ; y < h ; ++y ) {
                    for( x = 0 ; x < w ; ++x ) {
                        /* Pack RGB data into bytes */
                        PackRgbaBytes( p->chan[r->order[0]][y][x],
                                       p->chan[r->order[1]][y][x],
                                       p->chan[r->order[2]][y][x],
                                       p->chan[r->order[3]][y][x],
                                       r,
                                       rgb );
                        /* Write only 32 / 16 bytes */
                        WriteByteOrdered( fid, rgb, r->bytes + (r->bytes&1), bf, FALSE );
/*                        fwrite( rgb, sizeof(unsigned char), bytes, fid );*/
                    }
                }
                break;
            case YUYV422:
                /* Write interleaved channels */
                for( y = 0 ; y < h ; ++y ) {
                    for( x = 0 ; x < w>>1 ; x++ ) {
                        WriteByteOrdered( fid, &p->yuv.Y[y][x*2], 1, bf, FALSE );
                        WriteByteOrdered( fid, &p->yuv.Cb[y][x], 1, bf, FALSE );
                        WriteByteOrdered( fid, &p->yuv.Y[y][1+x*2], 1, bf, FALSE );
                        WriteByteOrdered( fid, &p->yuv.Cr[y][x], 1, bf, FALSE );
                        /*
                        fputc( p->yuv.Y[y][x*2], fid );
                        fputc( p->yuv.Cb[y][x], fid );
                        fputc( p->yuv.Y[y][1+x*2], fid );
                        fputc( p->yuv.Cr[y][x], fid );
                        */
                    }
                }
                break;
            default:
                ASSERT(0);
                break;
        }
    }
    /* Chroma channels interleaved? */
    else if (PP_IS_YUV_CH_INTERLEAVED(p->fmt)) {
        /* Should always be an YUV format!! */
        if(PP_IS_YUV(p->fmt)) {
            /* Write luma channel */
            for( y = 0 ; y < h ; ++y ) {
                WriteByteOrdered( fid, p->yuv.Y[y], w, bf, FALSE );
                /*fwrite( p->yuv.Y[y], sizeof(unsigned char), w, fid );*/
            }
            /* Write chroma channels */
            switch(p->fmt) {
                case YUV420C:
                    w >>= 1;
                    h >>= 1;
                    break;
                case YUV422C:
                    w >>= 1;
                    break;
                default:
                    break;
            }
            for( y = 0 ; y < h ; ++y ) {
                for( x = 0 ; x < w ; ++x ) {
                    WriteByteOrdered( fid, &p->yuv.Cb[y][x], 1, bf, FALSE );
                    WriteByteOrdered( fid, &p->yuv.Cr[y][x], 1, bf, FALSE );
                    /*
                    fputc( p->yuv.Cb[y][x], fid );
                    fputc( p->yuv.Cr[y][x], fid );
                    */
                }
            }
        }
        else {
            ASSERT(0);
        }
    }
    /* Planar image */
    else {
        /* YUV */
        if(PP_IS_YUV(p->fmt)) {
            /* Write luma channel */
            for( y = 0 ; y < h ; ++y ) {
                WriteByteOrdered( fid, p->yuv.Y[y], w, bf, FALSE );
                /*fwrite( p->yuv.Y[y], sizeof(unsigned char), w, fid );*/
            }
            /* Write chroma channels */
            switch(p->fmt) {
                case YUV400:
                    w = h = 0; /* no chroma */
                    break;
                case YUV420:
                    w >>= 1;
                    h >>= 1;
                    break;
                case YUV422:
                    w >>= 1;
                    break;
                default:
                    break;
            }
            for( y = 0 ; y < h ; ++y ) {
                WriteByteOrdered( fid, p->yuv.Cb[y], w, bf, FALSE );
                /*fwrite( p->yuv.Cb[y], sizeof(unsigned char), w, fid );*/
            }
            for( y = 0 ; y < h ; ++y ) {
                WriteByteOrdered( fid, p->yuv.Cr[y], w, bf, FALSE );
                /*fwrite( p->yuv.Cr[y], sizeof(unsigned char), w, fid );*/
            }
        }
        /* RGB */
        else if (p->fmt == RGBP) {
            /* Write planar channels */
            for( c = 0 ; c < PP_MAX_CHANNELS - 1 ; ++c ) {
                for( y = 0 ; y < h ; ++y ) {
                    for( x = 0 ; x < w ; ++x ) {
                        WriteByteOrdered( fid, &p->chan[c][y][x], 1, bf, FALSE );
                        /*fputc( p->chan[r->order[c]][y][x], fid );*/
                    }
                }
            }
        }
        else {
            ASSERT(0);
        }
    }
    /* flush to word boundary */
    WriteByteOrdered( fid, NULL, 0, bf, TRUE );
    fclose(fid);
}

/*------------------------------------------------------------------------------

   <++>.<++>  Function: ReleaseFrame

        Functional description:
          Release memory allocated by frame.

        Inputs:

        Outputs:

------------------------------------------------------------------------------*/
void ReleaseFrame( PixelData * p )
{
    u32 j;

    ASSERT(p);

    if(p->base) {
        /*free(p->base);*/
        /*p->base = 0;*/
        for( j = 0 ; j < PP_MAX_CHANNELS; ++j ) {
            free(p->chan[j]);
            p->chan[j] = 0;
        }
    }
}

/*------------------------------------------------------------------------------

   <++>.<++>  Function: SetupFrame

        Functional description:
          Setup pointers for frame

        Inputs:

        Outputs:

------------------------------------------------------------------------------*/
void SetupFrame( PixelData * p )
{
    u32 chanBytes; /* bytes per channel */
    u32 rowBytes;  /* bytes for row indices per channel */
    u32 j, i, offs = 0;
    ASSERT(p);
    chanBytes = p->width * p->height;
    rowBytes = sizeof(u8*) * p->height;
    for( j = 0 ; j < PP_MAX_CHANNELS ; ++j ) {
        offs = 0;
        if(p->chan[ j ]) free(p->chan[ j ]);
        p->chan[ j ] = (u8**)malloc( rowBytes );
        ASSERT(p->chan[j]);
        for( i = 0 ; i < p->height ; ++i, offs += p->width ) {
            p->chan[ j ][i] = p->base + j * chanBytes + offs;
        }
    }
}

/*------------------------------------------------------------------------------

   <++>.<++>  Function: AllocFrame

        Functional description:
          Allocate memory for frame and set pointers.

        Inputs:

        Outputs:

------------------------------------------------------------------------------*/
void AllocFrame( PixelData * p )
{
    u32 chanBytes; /* bytes per channel */
    ASSERT(p);

    chanBytes = p->width * p->height;

    ASSERT(p->base);
    SetupFrame(p);
}


/*------------------------------------------------------------------------------

   <++>.<++>  Function: CopyFrame

        Functional description:
          Copy frame

        Inputs:

        Outputs:

------------------------------------------------------------------------------*/
void CopyFrame( PixelData * dst, PixelData * src )
{
    ReleaseFrame(dst);
    dst->width = src->width;
    dst->height = src->height;
    dst->fmt = src->fmt;
    AllocFrame(dst);
    memcpy( dst->base, src->base, dst->width * dst->height * PP_MAX_CHANNELS );
}
