/* Copyright 2015 Verisilicon Inc. All Rights Reserved. */


#include "basetype.h"
#include "dwl.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>



/*------------------------------------------------------------------------------
    Function name   : DWLPrivateAeraReadByte
    Description     : Read one byte from a given pointer, which points to a virtual
                      address in a private buffer.

    Return type     : One byte data from the given address.

    Argument        : u8 *p - points to a virtual address in the private buffer.
------------------------------------------------------------------------------*/
u8 DWLPrivateAreaReadByte(const u8 *p)
{
    return *p;
}

/*------------------------------------------------------------------------------
    Function name   : DWLPrivateAeraWriteByte
    Description     : Write one byte into a given pointer, which points to a virtual
                      address in a private buffer.

    Return type     : void

    Argument        : u8 *p - points to a virtual address in the private buffer.
    Argument        : u8 data -  One byte data which will write into given address.
------------------------------------------------------------------------------*/
void DWLPrivateAreaWriteByte(u8 *p, u8 data)
{
    *p = data;
}


/*------------------------------------------------------------------------------
    Function name   : DWLPrivateAreaMemcpy
    Description     : Copies characters between private buffers.

    Return type     : The value of destination d

    Argument        : void *d - Destination buffer
    Argument        : const void *s - Buffer to copy from
    Argument        : u32 n - Number of bytes to copy
------------------------------------------------------------------------------*/
void *DWLPrivateAreaMemcpy(void *d, const void *s, u32 n)
{
    /*Here we believe d and s have valid value */
    u8 *ptmpd = d;
    u8 *ptmps = (u8 *)s;
    u32 i, tmp;
    for(i = 0; i < n; i++)
    {
        tmp = DWLPrivateAreaReadByte(ptmps);
        DWLPrivateAreaWriteByte(ptmpd, tmp);
        ptmps++;
        ptmpd++;
    }
    return d;
}

/*------------------------------------------------------------------------------
    Function name   : DWLPrivateAreaMemset
    Description     : Sets private buffers to a specified character.

    Return type     : The value of destination d

    Argument        : void *d - Pointer to destination
    Argument        : i32 c - Character to set
    Argument        : u32 n - Number of characters
------------------------------------------------------------------------------*/
void *DWLPrivateAreaMemset(void *d, i32 c, u32 n)
{
    u8 *ptmp = d;
    u32 i;
    for( i = 0; i < n; i++)
    {
        DWLPrivateAreaWriteByte(ptmp, c);
        ptmp++;
    }
    return d;
}
