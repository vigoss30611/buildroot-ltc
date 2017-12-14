/*------------------------------------------------------------------------------
--                                                                                                                               --
--       This software is confidential and proprietary and may be used                                   --
--        only as expressly authorized by a licensing agreement from                                     --
--                                                                                                                               --
--                            Verisilicon.                                                                                    --
--                                                                                                                               --
--                   (C) COPYRIGHT 2014 VERISILICON                                                            --
--                            ALL RIGHTS RESERVED                                                                    --
--                                                                                                                               --
--                 The entire notice above must be reproduced                                                 --
--                  on all copies and should not be removed.                                                    --
--                                                                                                                               --
--------------------------------------------------------------------------------*/

#include "tools.h"
#include "stdlib.h"
#include <string.h>

struct memory
{
  struct node *next;
  void *p;
};


/*------------------------------------------------------------------------------
  log2i
------------------------------------------------------------------------------*/
i32 log2i(i32 x, i32 *result)
{
  i32 tmp = 0;

  if (x < 0) return NOK;

  while (x >> ++tmp);

  *result = tmp - 1;

  return (x == 1 << (tmp - 1)) ? OK : NOK;
}

/*------------------------------------------------------------------------------
  check_range
------------------------------------------------------------------------------*/
i32 check_range(i32 x, i32 min, i32 max)
{
  if ((x >= min) && (x <= max)) return OK;

  return NOK;
}

/*------------------------------------------------------------------------------
  malloc_array
  For example array size [5][5]:
  char **c = (char **)malloc_array(5, 5, sizeof(char));
  struct foo **f = (struct foo **)malloc_array(5, 5, sizeof(struct foo));
------------------------------------------------------------------------------*/
void **malloc_array(struct queue *q, i32 r, i32 c, i32 size)
{
  char **p;
  i32 i;

  ASSERT(sizeof(void *) == sizeof(char *));

  if (!(p = qalloc(q, r,  sizeof(char *)))) return NULL;;
  for (i = 0; i < r; i++)
  {
    if (!(p[i] = qalloc(q, c, size))) return NULL;
  }
  return (void **)p;
}

/*------------------------------------------------------------------------------
  qalloc
------------------------------------------------------------------------------*/
void *qalloc(struct queue *q, i32 nmemb, i32 size)
{
  struct memory *m;
  char *p;

  m = malloc(sizeof(struct memory));
  p = calloc(nmemb, size);
  if (!m || !p)
  {
    free(m);
    free(p);
    return NULL;
  }

  m->p = p;
  queue_put(q, (struct node *)m);

  return p;
}

/*------------------------------------------------------------------------------
  qfree
------------------------------------------------------------------------------*/
void qfree(struct queue *q)
{
  struct node *n;
  struct memory *m;

  while ((n = queue_get(q)))
  {
    m = (struct memory *)n;
    free(m->p);
    free(m);
  }
}
