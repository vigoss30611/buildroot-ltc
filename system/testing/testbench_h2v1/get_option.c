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

#include <stdio.h>
#include <string.h>
#include "base_type.h"
#include "get_option.h"

static i32 long_option(i32 argc, char **argv, struct option *option,
                       struct parameter *parameter, char **p);
static i32 short_option(i32 argc, char **argv, struct option *option,
                        struct parameter *parameter, char **p);
static i32 parse(i32 argc, char **argv, struct option *option,
                 struct parameter *parameter, char **p, u32 lenght);
static i32 get_next(i32 argc, char **argv, struct parameter *parameter, char **p);

/*------------------------------------------------------------------------------
  get_option parses command line options. This function should be called
  with argc and argv values that are parameters of main(). The function
  will parse the next parameter from the command line. The function
  returns the next option character and stores the current place to
  structure argument. Structure option contain valid options
  and structure parameter contains parsed option.

  option options[] = {
    {"help",           'H', 0}, // No argument
    {"input",          'i', 1}, // Argument is compulsory
    {"output",         'o', 2}, // Argument is optional
    {NULL,              0,  0}  // Format of last line
  }

  Command line format can be
  --input filename
  --input=filename
  --inputfilename
  -i filename
  -i=filename
  -ifilename

  Input argc  Argument count as passed to main().
    argv  Argument values as passed to main().
    option  Valid options and argument requirements structure.
    parameter Option and argument return structure.

  Return  1 Unknown option.
    0 Option and argument are OK.
    -1  No more options.
    -2  Option match but argument is missing.
------------------------------------------------------------------------------*/
i32 get_option(i32 argc, char **argv, struct option *option,
               struct parameter *parameter)
{
  char *p = NULL;
  i32 ret;

  parameter->argument = "?";
  parameter->short_opt = '?';
  parameter->enable = 0;

  if (get_next(argc, argv, parameter, &p))
  {
    return -1;  /* End of options */
  }

  /* Long option */
  ret = long_option(argc, argv, option, parameter, &p);
  if (ret != 1) return ret;

  /* Short option */
  ret = short_option(argc, argv, option, parameter, &p);
  if (ret != 1)  return ret;

  /* This is unknow option but option anyway so argument must return */
  parameter->argument = p;

  return 1;
}

/*------------------------------------------------------------------------------
  long_option
------------------------------------------------------------------------------*/
i32 long_option(i32 argc, char **argv, struct option *option,
                struct parameter *parameter, char **p)
{
  i32 i = 0;
  u32 lenght;

  if (strncmp("--", *p, 2) != 0)
  {
    return 1;
  }

  while (option[i].long_opt != NULL)
  {
    lenght = strlen(option[i].long_opt);
    if (strncmp(option[i].long_opt, *p + 2, lenght) == 0)
    {
      goto match;
    }
    i++;
  }
  return 1;

match:
  lenght += 2;    /* Because option start -- */
  if (parse(argc, argv, &option[i], parameter, p, lenght) != 0)
  {
    return -2;
  }

  return 0;
}

/*------------------------------------------------------------------------------
  short_option
------------------------------------------------------------------------------*/
i32 short_option(i32 argc, char **argv, struct option *option,
                 struct parameter *parameter, char **p)
{
  i32 i = 0;
  char short_opt;

  if (strncmp("-", *p, 1) != 0)
  {
    return 1;
  }

  strncpy(&short_opt, *p + 1, 1);
  while (option[i].long_opt != NULL)
  {
    if (option[i].short_opt  == short_opt)
    {
      goto match;
    }
    i++;
  }
  return 1;

match:
  if (parse(argc, argv, &option[i], parameter, p, 2) != 0)
  {
    return -2;
  }

  return 0;
}

/*------------------------------------------------------------------------------
  parse
------------------------------------------------------------------------------*/
i32 parse(i32 argc, char **argv, struct option *option,
          struct parameter *parameter, char **p, u32 lenght)
{
  char *arg;

  parameter->short_opt = option->short_opt;
  parameter->longOpt = option->long_opt;
  arg = *p + lenght;

  /* Argument and option are together */
  if (strlen(arg) != 0)
  {
    /* There should be no argument */
    if (option->enable == 0)
    {
      return -1;
    }

    /* Remove = */
    if (strncmp("=", arg, 1) == 0)
    {
      arg++;
    }
    parameter->enable = 1;
    parameter->argument = arg;
    return 0;
  }

  /* Argument and option are separately */
  if (get_next(argc, argv, parameter, p))
  {
    /* There is no more parameters */
    if (option->enable == 1)
    {
      return -1;
    }
    return 0;
  }

  /* Parameter is missing if next start with "-" but next time this
   * option is OK so we must fix parameter->cnt */
  if (strncmp("-", *p,  1) == 0)
  {
    parameter->cnt--;
    if (option->enable == 1)
    {
      return -1;
    }
    return 0;
  }

  /* There should be no argument */
  if (option->enable == 0)
  {
    return -1;
  }

  parameter->enable = 1;
  parameter->argument = *p;

  return 0;
}

/*------------------------------------------------------------------------------
  get_next
------------------------------------------------------------------------------*/
i32 get_next(i32 argc, char **argv, struct parameter *parameter, char **p)
{
  /* End of options */
  if ((parameter->cnt >= argc) || (parameter->cnt < 0))
  {
    return -1;
  }
  *p = argv[parameter->cnt];
  parameter->cnt++;

  return 0;
}
