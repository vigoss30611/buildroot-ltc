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
-- Abstract : 
------------------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "cfg.h"

/*------------------------------------------------------------------------------
    Parse mode constants
------------------------------------------------------------------------------*/
typedef enum {
    CFG_MODE_IDLE           = 100,
    CFG_MODE_READ_NAME      = 101,
    CFG_MODE_READ_VALUE     = 102,
    CFG_MODE_NAME_READY     = 103,
    CFG_MODE_EAT_COMMENT    = 104,
} ParseMode;

/*------------------------------------------------------------------------------
    Parse results
------------------------------------------------------------------------------*/
typedef enum {
    CFG_FMT_OK                          = 200,
    CFG_FMT_ERR_EXPECTED_ALPHA          = 201,
    CFG_FMT_ERR_EXPECTED_BLK_START      = 202,
    CFG_FMT_ERR_UNEXPECTED_BLK_START    = 203,
    CFG_FMT_ERR_UNEXPECTED_BLK_END      = 204,
    CFG_FMT_ERR_UNEXPECTED_VALUE_END    = 205,
    CFG_FMT_ERR_UNEXPECTED_LINE_BREAK   = 206,
    CFG_FMT_ERR_COMMENT                 = 207,
    CFG_FMT_EOB                         = 208
} ParseResult;


/*------------------------------------------------------------------------------

   <++>.<++>  Function: ParseBlock

        Functional description:
          Parse a block from config file

        Inputs:

        Outputs:

------------------------------------------------------------------------------*/
ParseResult ParseBlock( FILE * fid, char *block, u32 *line,
    CfgCallback callback, void **cbParam )
{
    ParseMode     mode = CFG_MODE_IDLE;
    ParseMode     prevMode = CFG_MODE_IDLE;
    ParseResult   subBlockRes;
    CfgCallbackResult cbResult;
    char          blk[1024];
    char          val[1024];
    char          escape;
    i32           blkLen = 0;
    i32           valLen = 0;

    ASSERT(fid);
    ASSERT(callback);

    printf("PARSING %s\n",block);
    while(!feof(fid)) {
        char ch = fgetc(fid); /* read char from file */
        /* Check for comments, if we are not parsing for a key value.
           If 1st comment start mark encountered, check that next
           mark is OK also. Otherwise roll back. */
        if( mode != CFG_MODE_READ_VALUE && ch == '/') {
            if(fgetc(fid) == '*') {
                prevMode = mode; /* store previous mode */
                mode = CFG_MODE_EAT_COMMENT; /* comment eating mode */
                continue;
            }
            else
                fseek( fid, -1, SEEK_CUR ); /* rollback */
        }

        switch(mode) {
            /* Wait for block or value */
            case CFG_MODE_IDLE:
                /* Extra { */
                if ( ch == '{' ) {
                    fprintf(stderr, "Unexpected {, line %d.\n", *line);
                    return CFG_FMT_ERR_UNEXPECTED_BLK_START;
                }
                /* Extra ; */
                else if ( ch == ';' ) {
                    fprintf(stderr, "Unexpected ;, line %d.\n", *line);
                    return CFG_FMT_ERR_UNEXPECTED_VALUE_END;
                }
                /* Block end */
                else if ( ch == '}' ) {
                    if(block)
                        return CFG_FMT_OK;
                    else {
                        fprintf(stderr, "Unexpected }, line %d.\n", *line);
                        return CFG_FMT_ERR_UNEXPECTED_BLK_END;
                    }
                }
                /* Whitespace and linefeeds are ignored. */
                else if (isspace(ch) || ch == '\n') {
                    if (ch =='\n') (*line) ++;
                    continue;
                }
                /* Literal name started */
                else {
                    blk[blkLen++] = ch;
                    mode = CFG_MODE_READ_NAME;
                }
                break;
            /* Read string (block/key name) */
            case CFG_MODE_READ_NAME:
                if (isspace(ch) || ch =='\n') {
                    if( ch == '\n' ) (*line) ++;
                    blk[ blkLen++ ] = 0;
                    mode = CFG_MODE_NAME_READY;
                } else if (!isalnum(ch) && ch != '_') {
                    fprintf(stderr, "Expected { or = near '%.*s', line %d.\n", blkLen, blk, *line);
                    return CFG_FMT_ERR_EXPECTED_ALPHA;
                } else {
                    blk[ blkLen++ ] = ch;
                }
                break;
            /* Literal name read, only thing allowed past this is
               whitespace, linefeed, block start or value start */
            case CFG_MODE_NAME_READY:
                /* Ignore white space */
                if (isspace(ch) || ch =='\n') {
                    if( ch == '\n' ) (*line) ++;
                    continue;
                } else if (ch == '{') {
                    /* Notify caller on new block */
                    callback( block, blk, NULL,
                        CFG_CALLBACK_BLK_START, cbParam );
                    /* Parse sub-block */
                    if(CFG_FMT_OK != (subBlockRes = ParseBlock( fid, blk, line,
                                                        callback, cbParam ))) {
                        return subBlockRes; /* Return error */
                    }
                    blkLen = 0; /* Clear block name */
                    mode = CFG_MODE_IDLE;
                    if (!block)
                        return CFG_FMT_EOB;
                } else if (ch == '=') {
                    if(!block) /* no global values */ {
                        fprintf(stderr, "Expected { near '%.*s', line %d.\n", blkLen, blk, *line);
                        return CFG_FMT_ERR_EXPECTED_BLK_START;
                    }
                    /* Parse value */
                    mode = CFG_MODE_READ_VALUE;
                } else {
                    fprintf(stderr, "Expected { or = near '%.*s', line %d.\n", blkLen, blk, *line);
                    return CFG_FMT_ERR_EXPECTED_BLK_START;
                }
                break;
            /* Reading value. Whitespace ignored if before value. */
            case CFG_MODE_READ_VALUE:
                if(valLen == 0 && isspace(ch)) {
                    continue;
                } else if (ch == '\\') { /* escape char */
                    escape = fgetc(fid);
                    switch(escape) {
                        case '\\': /* back-slash */
                            val[valLen++] = '\\';
                            break;
                        case '\n': /* line continuation */
                            (*line) ++;
                            break;
                        default:
                            fseek( fid, -1, SEEK_CUR ); /* rollback */
                            break;
                    }
                } else if (ch == '\n') {
                    fprintf(stderr, "Unexpected line break, line %d\n", *line );
                    return CFG_FMT_ERR_UNEXPECTED_LINE_BREAK;
                } else if (ch == ';') {
                    /* Value read. Invoke callback. */
                    val[valLen++] = 0;
                    if(CFG_OK != (cbResult = callback( block, blk, val,
                        CFG_CALLBACK_VALUE, cbParam ))) {
                        fprintf(stderr, "Error reading value '%s.%s', line %d\n", block, blk, *line );
                        return cbResult;
                    }
                    /* Clear param and value name */
                    blkLen = 0;
                    valLen = 0;
                    mode = CFG_MODE_IDLE;
                } else {
                    val[valLen++] = ch;
                }

                break;
            /* Eat comment text */
            case CFG_MODE_EAT_COMMENT:
                /* Check if comment should be terminated */
                switch(ch) {
                    case '\n':
                        (*line) ++;
                        break;
                    case '*':
                        if(fgetc(fid) == '/')
                            mode = prevMode;
                        else /* Comment doesn't end, rollback */
                            fseek( fid, -1, SEEK_CUR );
                }
                break;
        }
    }
    return CFG_FMT_OK;
}

/*------------------------------------------------------------------------------

   <++>.<++>  Function: ParseConfig

        Functional description:
          Parse config file

        Inputs:

        Outputs:

------------------------------------------------------------------------------*/
u32 ParseConfig( char * filename,
    CfgCallback callback, void **cbParam )
{
    u32 numBlocks = 0;
    u32 lines = 1;
    FILE *fid;
    ParseResult res;

    ASSERT(filename);
    ASSERT(callback);

    fid = fopen( filename, "r" );
    if(!fid) {
        fprintf(stderr, "Error opening file '%s'.\n", filename);
    }
    else {
        while (1)
        {
            res = ParseBlock( fid, NULL, &lines, callback, cbParam );
            if (res != CFG_FMT_EOB)
                break;
            numBlocks++;
        }
        fclose(fid);
    }

    return (res == CFG_FMT_OK ? numBlocks : 0);
}
