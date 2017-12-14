/*
 * log.c
 *
 *  Created on: Jun 25, 2010
 * 
 * Synopsys Inc.
 * SG DWC PT02
 */

#include "log.h"

#include <linux/kernel.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>

verbose_t log_Verbose = VERBOSE_NOTICE;
static numeric_t log_Numeric = NUMERIC_DEC;
static unsigned log_VerboseWrite = 0;

void log_SetVerbose(verbose_t verbose)
{
	log_Verbose = verbose;
}

void log_SetNumeric(numeric_t numeric)
{
	log_Numeric = numeric;
}

void log_SetVerboseWrite(unsigned state)
{
	log_VerboseWrite = state;
}

void log_PrintWrite(unsigned a, unsigned b)
{
	if (log_VerboseWrite == 1)
	{
		if (log_Numeric == NUMERIC_DEC)
		{
			printk("%d, %d\n", a, b);
		}
		else
		{
			printk("0x%x, 0x%x\n", a, b);
		}
	}
}

void log_Print0(verbose_t verbose, const char* functionName)
{
	if (verbose == VERBOSE_ERROR)
	{
		printk("ERROR: ");
	}
	if (verbose == VERBOSE_WARN)
	{
		printk("WARNING: ");
	}
	if (verbose <= log_Verbose)
	{
		printk("%s\n", functionName);
	}
}

void log_Print1(verbose_t verbose, const char* functionName, const char* a)
{
	if (verbose == VERBOSE_ERROR)
	{
		printk("ERROR: ");
	}
	if (verbose == VERBOSE_WARN)
	{
		printk("WARNING: ");
	}
	if (verbose <= log_Verbose)
	{
		printk("%s: %s\n", functionName, a);
	}
}

void log_Print2(verbose_t verbose, const char* functionName, const char* a,
		unsigned b)
{
	if (verbose == VERBOSE_ERROR)
	{
		printk("ERROR: ");
	}
	if (verbose == VERBOSE_WARN)
	{
		printk("WARNING: ");
	}
	if (verbose <= log_Verbose)
	{
		if (log_Numeric == NUMERIC_DEC)
		{
			printk("%s: %s, %d\n", functionName, a, b);
		}
		else
		{
			printk("%s: %s, 0x%x\n", functionName, a, b);
		}
	}
}

void log_Print3(verbose_t verbose, const char* functionName, const char* a,
		unsigned b, unsigned c)
{
	if (verbose == VERBOSE_ERROR)
	{
		printk("ERROR: ");
	}
	if (verbose == VERBOSE_WARN)
	{
		printk("WARNING: ");
	}
	if (verbose <= log_Verbose)
	{
		if (log_Numeric == NUMERIC_DEC)
		{
			printk("%s: %s, %d, %d\n", functionName, a, b, c);
		}
		else
		{
			printk("%s: %s, 0x%x, 0x%x\n", functionName, a, b, c);
		}
	}
}

void log_PrintInt(verbose_t verbose, const char* functionName, unsigned a)
{
	if (verbose <= log_Verbose)
	{
		if (log_Numeric == NUMERIC_DEC)
		{
			printk("%s: %d\n", functionName, a);
		}
		else
		{
			printk("%s: 0x%x\n", functionName, a);
		}
	}
}

void log_PrintInt2(verbose_t verbose, const char* functionName, unsigned a,
		unsigned b)
{
	if (verbose <= log_Verbose)
	{
		if (log_Numeric == NUMERIC_DEC)
		{
			printk("%s: %d, %d\n", functionName, a, b);
		}
		else
		{
			printk("%s: 0x%x, 0x%x\n", functionName, a, b);
		}
	}
}

void log_Printk(verbose_t verbose, const char* a)
{
	if (verbose <= log_Verbose) {
		printk("%s", a);
	}
}
