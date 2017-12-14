/* Copyright 2012 Google Inc. All Rights Reserved. */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "tb_cfg.h"

#ifdef _ASSERT_USED
#include <assert.h>
#endif
#ifdef _ASSERT_USED
#define ASSERT(expr) if (!(expr)) printf("+++ G2 ASSERT(%s:%d) 444:"#expr"...\n", __FILE__, __LINE__)//assert(expr)
#else
#define ASSERT(expr)                                                        \
  if (!(expr)) {                                                            \
    printf("+++ G2 ASSERT(%s:%d) 444:"#expr"...\n", __FILE__, __LINE__);    \
  }
#endif

/*------------------------------------------------------------------------------
    Parse mode constants
------------------------------------------------------------------------------*/
enum ParseMode {
  CFG_MODE_IDLE = 100,
  CFG_MODE_READ_NAME = 101,
  CFG_MODE_READ_VALUE = 102,
  CFG_MODE_NAME_READY = 103,
  CFG_MODE_EAT_COMMENT = 104,
};

/*------------------------------------------------------------------------------
    Parse results
------------------------------------------------------------------------------*/
enum ParseResult {
  CFG_FMT_OK = 200,
  CFG_FMT_ERR_EXPECTED_ALPHA = 201,
  CFG_FMT_ERR_EXPECTED_BLK_START = 202,
  CFG_FMT_ERR_UNEXPECTED_BLK_START = 203,
  CFG_FMT_ERR_UNEXPECTED_BLK_END = 204,
  CFG_FMT_ERR_UNEXPECTED_VALUE_END = 205,
  CFG_FMT_ERR_UNEXPECTED_LINE_BREAK = 206,
  CFG_FMT_ERR_COMMENT = 207,
};

/* Parse a block from config file. */
static enum ParseResult ParseBlock(FILE *fid, char *block, u32 *line,
                                   TBCfgCallback callback, void *cb_param) {
  enum ParseMode mode = CFG_MODE_IDLE;
  enum ParseMode prev_mode = CFG_MODE_IDLE;
  enum ParseResult sub_block_res;
  enum TBCfgCallbackResult cb_result;
  char blk[1024];
  char val[1024];
  char escape;
  i32 blk_len = 0;
  i32 val_len = 0;

  ASSERT(fid);
  ASSERT(callback);

  while (!feof(fid)) {
    char ch = fgetc(fid); /* read char from file */
    /* Check for comments, if we are not parsing for a key value.
       If 1st comment start mark encountered, check that next
       mark is OK also. Otherwise roll back. */
    if (mode != CFG_MODE_READ_VALUE && ch == '/') {
      if (fgetc(fid) == '*') {
        prev_mode = mode;            /* store previous mode */
        mode = CFG_MODE_EAT_COMMENT; /* comment eating mode */
        continue;
      } else
        fseek(fid, -1, SEEK_CUR); /* rollback */
    }

    switch (mode) {
      /* Wait for block or value */
      case CFG_MODE_IDLE:
        /* Extra { */
        if (ch == '{') {
          fprintf(stderr, "Unexpected {, line %d.\n", *line);
          return CFG_FMT_ERR_UNEXPECTED_BLK_START;
        }
        /* Extra ; */
        else if (ch == ';') {
          fprintf(stderr, "Unexpected ;, line %d.\n", *line);
          return CFG_FMT_ERR_UNEXPECTED_VALUE_END;
        }
        /* Block end */
        else if (ch == '}') {
          if (block)
            return CFG_FMT_OK;
          else {
            fprintf(stderr, "Unexpected }, line %d.\n", *line);
            return CFG_FMT_ERR_UNEXPECTED_BLK_END;
          }
        }
        /* Whitespace and linefeeds are ignored. */
        else if (isspace(ch) || ch == '\n') {
          if (ch == '\n') (*line)++;
          continue;
        }
        /* Literal name started */
        else {
          blk[blk_len++] = ch;
          mode = CFG_MODE_READ_NAME;
        }
        break;
      /* Read string (block/key name) */
      case CFG_MODE_READ_NAME:
        if (isspace(ch) || ch == '\n') {
          if (ch == '\n') (*line)++;
          blk[blk_len++] = 0;
          mode = CFG_MODE_NAME_READY;
        } else if (!isalnum(ch) && ch != '_') {
          fprintf(stderr, "Expected { or = near '%.*s', line %d.\n", blk_len,
                  blk, *line);
          return CFG_FMT_ERR_EXPECTED_ALPHA;
        } else {
          blk[blk_len++] = ch;
        }
        break;
      /* Literal name read, only thing allowed past this is
         whitespace, linefeed, block start or value start */
      case CFG_MODE_NAME_READY:
        /* Ignore white space */
        if (isspace(ch) || ch == '\n') {
          if (ch == '\n') (*line)++;
          continue;
        } else if (ch == '{') {
          /* Notify caller on new block */
          callback(block, blk, NULL, TB_CFG_CALLBACK_BLK_START, cb_param);
          /* Parse sub-block */
          if (CFG_FMT_OK != (sub_block_res = ParseBlock(fid, blk, line,
                                                        callback, cb_param))) {
            return sub_block_res; /* Return error */
          }
          blk_len = 0; /* Clear block name */
          mode = CFG_MODE_IDLE;
        } else if (ch == '=') {
          if (!block) /* no global values */ {
            fprintf(stderr, "Expected { near '%.*s', line %d.\n", blk_len, blk,
                    *line);
            return CFG_FMT_ERR_EXPECTED_BLK_START;
          }
          /* Parse value */
          mode = CFG_MODE_READ_VALUE;
        } else {
          fprintf(stderr, "Expected { or = near '%.*s', line %d.\n", blk_len,
                  blk, *line);
          return CFG_FMT_ERR_EXPECTED_BLK_START;
        }
        break;
      /* Reading value. Whitespace ignored if before value. */
      case CFG_MODE_READ_VALUE:
        if (val_len == 0 && isspace(ch)) {
          continue;
        } else if (ch == '\\') {/* escape char */
          escape = fgetc(fid);
          switch (escape) {
            case '\\': /* back-slash */
              val[val_len++] = '\\';
              break;
            case '\n': /* line continuation */
              (*line)++;
              break;
            default:
              fseek(fid, -1, SEEK_CUR); /* rollback */
              break;
          }
        } else if (ch == '\n') {
          fprintf(stderr, "Unexpected line break, line %d\n", *line);
          return CFG_FMT_ERR_UNEXPECTED_LINE_BREAK;
        } else if (ch == ';') {
          /* Value read. Invoke callback. */
          val[val_len++] = 0;
          if (TB_CFG_OK !=
              (cb_result = callback(block, blk, val, TB_CFG_CALLBACK_VALUE,
                                    cb_param))) {
            fprintf(stderr, "Error reading value '%s.%s', line %d\n", block,
                    blk, *line);
            return cb_result;
          }
          /* Clear param and value name */
          blk_len = 0;
          val_len = 0;
          mode = CFG_MODE_IDLE;
        } else {
          val[val_len++] = ch;
        }

        break;
      /* Eat comment text */
      case CFG_MODE_EAT_COMMENT:
        /* Check if comment should be terminated */
        switch (ch) {
          case '\n':
            (*line)++;
            break;
          case '*':
            if (fgetc(fid) == '/')
              mode = prev_mode;
            else /* Comment doesn't end, rollback */
              fseek(fid, -1, SEEK_CUR);
        }
        break;
    }
  }
  return CFG_FMT_OK;
}

TBBool TBParseConfig(char *filename, TBCfgCallback callback, void *cb_param) {
  TBBool result = TB_FALSE;
  u32 lines = 1;
  FILE *fid;

  ASSERT(filename);
  ASSERT(callback);

  fid = fopen(filename, "r");
  if (!fid) {
    fprintf(stderr, "Error opening file '%s'.\n", filename);
  } else {
    if (CFG_FMT_OK == ParseBlock(fid, NULL, &lines, callback, cb_param))
      result = TB_TRUE;
    fclose(fid);
  }

  return result;
}
