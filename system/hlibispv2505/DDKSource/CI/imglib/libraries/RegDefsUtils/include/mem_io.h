/*!
******************************************************************************
 @file   : mem_io.h

 @brief        Memory structure access macros.

 @Author    Imagination Technologies

 @date        12/09/05

 <b>Copyright 2005 by Imagination Technologies Limited.</b>\n

 All rights reserved.  No part of this software, either material or conceptual
 may be copied or distributed, transmitted, transcribed, stored in a retrieval
 system or translated into any human or computer language in any form by any
 means, electronic, mechanical, manual or other-wise, or disclosed to third
 parties without the express written permission of Imagination Technologies
 Limited, Unit 8, HomePark Industrial Estate, King's Langley, Hertfordshire,
 WD4 8LZ, U.K.

 <b>Description:</b>\n

 This file contains a set of memory access macros for accessing packed memory
 structures.

 <b>Platform:</b>\n
 Platform Independent

 @Version
    -    1.0 First Release

******************************************************************************/
/*
******************************************************************************
 Modifications:

 $Log$
 Revision 1.21  2010/07/21 14:07:12  mbs
 Fixed doxygen warnings.

 Revision 1.20  2009/12/21 17:19:29  bnb
 these shouldn't have been removed - whoops!

 Revision 1.1  2009/12/21 16:04:06  bnb
 Added headers in eurasia folder to sync better with eurasia tree

 Revision 1.18  2009/04/23 15:30:18  tmh
 ensured MTX message memory is initialised

 Revision 1.17  2008/10/15 15:16:42  rml
 Change #if (__cplusplus) to #if defined (__cplusplus)

 Revision 1.16  2008/08/09 10:04:32  rml
 Fix 64 bit warnings

 Revision 1.15  2008/08/08 08:37:14  rml
 Re-fix line endings

 Revision 1.14  2008/08/08 07:43:44  rml
 Fix bad line endings

 Revision 1.13  2008/07/02 17:28:54  jmg
 Tidied up doxygen output.

 Revision 1.12  2008/03/06 17:10:12  bal
 Use the same MEMIO_CHECK_ALIGNMENT in linux as in WIN32.

 Revision 1.11  2006/11/03 17:48:44  jmg
 Added some macro parameter protection.

 Revision 1.10  2006/07/17 10:16:46  grc
 Added casts to IMG_UINT32 to allow pointers and NULL to be used with MEMIO macros.

 Revision 1.9  2006/05/30 09:57:56  grc
 Memio read and erite table macros now allow zero length tables.

 Revision 1.8  2006/02/14 15:37:08  mbs
 Added optimised REGIO/MEMIO macros and made changes to def2code to support this

 Revision 1.7  2006/02/09 09:59:52  kab
 Added parenthesis around some of the macro's parameters.

 Revision 1.6  2006/02/08 10:08:22  mbs
 Added compile time switch to verify memory alignment for debug build only.
 For release/test build IMG_ASSERT() has no value which caused an error
 before ',' in macro definition.

 Revision 1.5  2005/10/11 13:33:40  rml
 Add C braces

 Revision 1.4  2005/09/20 14:16:59  rml
 Test rep and table access macros

 Revision 1.3  2005/09/20 13:49:53  rml
 Change to mem_io macros

 Revision 1.2  2005/09/15 13:26:59  rml
 Modified for MTX

 Revision 1.1  2005/09/14 09:02:24  rml
 Added to CVS


*****************************************************************************/

#if !defined (__MEM_IO_H__)
#define __MEM_IO_H__

#if defined (__cplusplus)
extern "C" {
#endif

#include "img_types.h"

#define RND_TO_WORDS(size) (((size + 3) / 4) * 4)

#ifdef DOXYGEN_WILL_SEE_THIS
/*!
******************************************************************************

 @Function    MEMIO_READ_FIELD

 @Description

 This macro is used to extract a field from a packed memory based structure.

 @Input        vpMem : A pointer to the memory structure.

 @Input        field : The name of the field to be extracted.

 @Return    IMG_UINT : The value of the field - right aligned.
                       The actual type of the field depends on its size
                       and can be found in the header file defining the field.
                       The value read may need a cast to fit into storing
                       variable.

******************************************************************************/
IMG_UINT MEMIO_READ_FIELD(IMG_VOID * vpMem, IMG_VOID * field);

/*!
******************************************************************************

 @Function    MEMIO_READ_TABLE_FIELD

 @Description

 This macro is used to extract the value of a field in a table in a packed
 memory based structure.

 @Input          vpMem : A pointer to the memory structure.

 @Input          field : The name of the field to be extracted.

 @Input   ui32TabIndex : The table index of the field to be extracted.

 @Return      IMG_UINT : The value of the field - right aligned.
                         The actual type of the field depends on its size
                         and can be found in the header file defining the field.
                         The value read may need a cast to fit into storing
                         variable.

******************************************************************************/
IMG_UINT MEMIO_READ_TABLE_FIELD(IMG_VOID * vpMem, IMG_VOID * field, IMG_UINT32 ui32TabIndex);

/*!
******************************************************************************

 @Function    MEMIO_READ_REPEATED_FIELD

 @Description

 This macro is used to extract the value of a repeated field in a packed
 memory based structure.

 @Input          vpMem : A pointer to the memory structure.

 @Input          field : The name of the field to be extracted.

 @Input   ui32RepIndex : The repeat index of the field to be extracted.

 @Return      IMG_UINT : The value of the field - right aligned.
                         The actual type of the field depends on its size
                         and can be found in the header file defining the field.
                         The value read may need a cast to fit into storing
                         variable.

******************************************************************************/
IMG_UINT MEMIO_READ_REPEATED_FIELD(IMG_VOID * vpMem, IMG_VOID * field, IMG_UINT32 ui32RepIndex);

/*!
******************************************************************************

 @Function    MEMIO_READ_TABLE_REPEATED_FIELD

 @Description

 This macro is used to extract the value of a repeated field in a table
 in a packed memory based structure.

 @Input          vpMem : A pointer to the memory structure.

 @Input          field : The name of the field to be extracted.

 @Input   ui32TabIndex : The table index of the field to be extracted.

 @Input   ui32RepIndex : The repeat index of the field to be extracted.

 @Return      IMG_UINT : The value of the field - right aligned.
                         The actual type of the field depends on its size
                         and can be found in the header file defining the field.
                         The value read may need a cast to fit into storing
                         variable.

******************************************************************************/
IMG_UINT MEMIO_READ_TABLE_REPEATED_FIELD(IMG_VOID * vpMem, IMG_VOID * field,IMG_UINT32 ui32TabIndex, IMG_UINT32 ui32RepIndex);

/*!
******************************************************************************

 @Function    MEMIO_WRITE_FIELD

 @Description

 This macro is used to update the value of a field in a packed memory based
 structure.

 @Input     vpMem   : A pointer to the memory structure.

 @Input     field   : The name of the field to be updated.

 @Input     uiValue : The value to be written to the field - right aligned.
                      The actual type of the field depends on its size
                      and can be found in the header file defining the field.

 @Return    None.

******************************************************************************/
IMG_VOID MEMIO_WRITE_FIELD(IMG_VOID * vpMem, IMG_VOID * field, IMG_UINT uiValue);

/*!
******************************************************************************

 @Function    MEMIO_WRITE_TABLE_FIELD

 @Description

 This macro is used to update the field in a table in a packed memory
 based structure.

 @Input          vpMem : A pointer to the memory structure.

 @Input          field : The name of the field to be updated.

 @Input   ui32TabIndex : The table index of the field to be updated.

 @Input        uiValue : The value to be written to the field - right aligned.
                         The actual type of the field depends on its size
                         and can be found in the header file defining the field.

 @Return    None.

******************************************************************************/
IMG_VOID MEMIO_WRITE_TABLE_FIELD(IMG_VOID * vpMem, IMG_VOID * field, IMG_UINT32 ui32TabIndex, IMG_UINT uiValue);

/*!
******************************************************************************

 @Function    MEMIO_WRITE_REPEATED_FIELD

 @Description

 This macro is used to update a repeated field in a packed memory
 based structure.

 @Input          vpMem : A pointer to the memory structure.

 @Input          field : The name of the field to be updated.

 @Input   ui32RepIndex : The repeat index of the field to be updated.

 @Input        uiValue : The value to be written to the field - right aligned.
                         The actual type of the field depends on its size
                         and can be found in the header file defining the field.

 @Return    None.

******************************************************************************/
IMG_VOID MEMIO_WRITE_REPEATED_FIELD(IMG_VOID * vpMem, IMG_VOID * field, IMG_UINT32 ui32RepIndex, IMG_UINT uiValue);


/*!
******************************************************************************

 @Function    MEMIO_WRITE_TABLE_REPEATED_FIELD

 @Description

 This macro is used to update a repeated field in a table in a packed memory
 based structure.

 @Input          vpMem : A pointer to the memory structure.

 @Input          field : The name of the field to be updated.

 @Input   ui32TabIndex : The table index of the field to be updated.

 @Input   ui32RepIndex : The repeat index of the field to be updated.

 @Input        uiValue : The value to be written to the field - right aligned.
                         The actual type of the field depends on its size
                         and can be found in the header file defining the field.

 @Return    None.

******************************************************************************/
IMG_VOID MEMIO_WRITE_TABLE_REPEATED_FIELD(IMG_VOID * vpMem, IMG_VOID * field, IMG_UINT32 ui32TabIndex, IMG_UINT32 ui32RepIndex, IMG_UINT uiValue);

#else

#if defined(WIN32) || defined(__linux__)
#define MEMIO_CHECK_ALIGNMENT(vpMem)        \
    IMG_ASSERT((vpMem))
#else
#define MEMIO_CHECK_ALIGNMENT(vpMem)        \
    IMG_ASSERT(((IMG_UINTPTR)(vpMem) & 0x3) == 0)
#endif

/*!
******************************************************************************

 @Function    MEMIO_READ_FIELD

******************************************************************************/
#if defined __RELEASE_DEBUG__

#define MEMIO_READ_FIELD(vpMem, field)                                                                         \
    ( MEMIO_CHECK_ALIGNMENT(vpMem),                                                                            \
    ((((*((field##_TYPE *)(((IMG_UINTPTR)(vpMem)) + field##_OFFSET))) & field##_MASK) >> field##_SHIFT)) )

#else

#if 1
#define MEMIO_READ_FIELD(vpMem, field)                                                                         \
    ((((*((field##_TYPE *)(((IMG_UINTPTR)(vpMem)) + field##_OFFSET))) & field##_MASK) >> field##_SHIFT))
#else
#define MEMIO_READ_FIELD(vpMem, field)                                                                         \
    ((((*((field##_TYPE *)(((IMG_UINTPTR)(vpMem)) + field##_OFFSET))) >> field##_SHIFT) & field##_LSBMASK) )
#endif

#endif

/*!
******************************************************************************

 @Function    MEMIO_READ_TABLE_FIELD

******************************************************************************/
#if defined __RELEASE_DEBUG__

#define MEMIO_READ_TABLE_FIELD(vpMem, field, ui32TabIndex)                                                                                     \
    ( MEMIO_CHECK_ALIGNMENT(vpMem), IMG_ASSERT((ui32TabIndex < field##_NO_ENTRIES) || (field##_NO_ENTRIES == 0)),                              \
    ((((*((field##_TYPE *)(((IMG_UINTPTR)(vpMem)) + field##_OFFSET + (field##_STRIDE * ui32TabIndex)))) & field##_MASK) >> field##_SHIFT)) ) \

#else

#define MEMIO_READ_TABLE_FIELD(vpMem, field, ui32TabIndex)                                                                                     \
    ((((*((field##_TYPE *)(((IMG_UINTPTR)(vpMem)) + field##_OFFSET + (field##_STRIDE * ui32TabIndex)))) & field##_MASK) >> field##_SHIFT))   \

#endif


/*!
******************************************************************************

 @Function    MEMIO_READ_REPEATED_FIELD

******************************************************************************/
#if defined __RELEASE_DEBUG__

#define MEMIO_READ_REPEATED_FIELD(vpMem, field, ui32RepIndex)                                                                                                                     \
    ( MEMIO_CHECK_ALIGNMENT(vpMem),    IMG_ASSERT(ui32RepIndex < field##_NO_REPS),                                                                                                \
    ((((*((field##_TYPE *)(((IMG_UINTPTR)(vpMem)) + field##_OFFSET))) & (field##_MASK >> (ui32RepIndex * field##_SIZE))) >> (field##_SHIFT - (ui32RepIndex * field##_SIZE)))) ) \

#else

#define MEMIO_READ_REPEATED_FIELD(vpMem, field, ui32RepIndex)                                                                                                                     \
    ((((*((field##_TYPE *)(((IMG_UINTPTR)(vpMem)) + field##_OFFSET))) & (field##_MASK >> (ui32RepIndex * field##_SIZE))) >> (field##_SHIFT - (ui32RepIndex * field##_SIZE))) )  \

#endif
/*!
******************************************************************************

 @Function    MEMIO_READ_TABLE_REPEATED_FIELD

******************************************************************************/
#if defined __RELEASE_DEBUG__

#define MEMIO_READ_TABLE_REPEATED_FIELD(vpMem, field, ui32TabIndex, ui32RepIndex)                                                                                                                                  \
    ( MEMIO_CHECK_ALIGNMENT(vpMem), IMG_ASSERT((ui32TabIndex < field##_NO_ENTRIES) || (field##_NO_ENTRIES == 0)), IMG_ASSERT(ui32RepIndex < field##_NO_REPS), \
    ((((*((field##_TYPE *)(((IMG_UINTPTR)(vpMem)) + field##_OFFSET + (field##_STRIDE * ui32TabIndex)))) & (field##_MASK >> (ui32RepIndex * field##_SIZE))) >> (field##_SHIFT - (ui32RepIndex * field##_SIZE))))) \

#else

#define MEMIO_READ_TABLE_REPEATED_FIELD(vpMem, field, ui32TabIndex, ui32RepIndex)                                                                                                                                  \
    ((((*((field##_TYPE *)(((IMG_UINTPTR)(vpMem)) + field##_OFFSET + (field##_STRIDE * ui32TabIndex)))) & (field##_MASK >> (ui32RepIndex * field##_SIZE))) >> (field##_SHIFT - (ui32RepIndex * field##_SIZE))))  \

#endif

/*!
******************************************************************************

 @Function    MEMIO_WRITE_FIELD

******************************************************************************/
#define MEMIO_WRITE_FIELD(vpMem, field, uiValue)                                                       \
    MEMIO_CHECK_ALIGNMENT(vpMem);                                                                      \
    (*((field##_TYPE *)(((IMG_UINTPTR)(vpMem)) + field##_OFFSET))) =                                 \
    ((*((field##_TYPE *)(((IMG_UINTPTR)(vpMem)) + field##_OFFSET))) & (field##_TYPE)~field##_MASK) | \
        (field##_TYPE)(( (uiValue) << field##_SHIFT) & field##_MASK);

#define MEMIO_WRITE_FIELD_LITE(vpMem, field, uiValue)                                                  \
    MEMIO_CHECK_ALIGNMENT(vpMem);                                                                      \
     (*((field##_TYPE *)(((IMG_UINTPTR)(vpMem)) + field##_OFFSET))) =                                \
    ((*((field##_TYPE *)(((IMG_UINTPTR)(vpMem)) + field##_OFFSET))) |                                \
        (field##_TYPE) (( (uiValue) << field##_SHIFT)) );

/*!
******************************************************************************

 @Function    MEMIO_WRITE_TABLE_FIELD
******************************************************************************/
#define MEMIO_WRITE_TABLE_FIELD(vpMem, field, ui32TabIndex, uiValue)                                                                           \
    MEMIO_CHECK_ALIGNMENT(vpMem); IMG_ASSERT(((ui32TabIndex) < field##_NO_ENTRIES) || (field##_NO_ENTRIES == 0));                              \
    (*((field##_TYPE *)(((IMG_UINTPTR)(vpMem)) + field##_OFFSET + (field##_STRIDE * (ui32TabIndex))))) =                                     \
        ((*((field##_TYPE *)(((IMG_UINTPTR)(vpMem)) + field##_OFFSET + (field##_STRIDE * (ui32TabIndex))))) & (field##_TYPE)~field##_MASK) | \
        (field##_TYPE)(((uiValue) << field##_SHIFT) & field##_MASK);

/*!
******************************************************************************

 @Function    MEMIO_WRITE_REPEATED_FIELD

******************************************************************************/
#define MEMIO_WRITE_REPEATED_FIELD(vpMem, field, ui32RepIndex, uiValue)                                                                       \
    MEMIO_CHECK_ALIGNMENT(vpMem); IMG_ASSERT((ui32RepIndex) < field##_NO_REPS);                                                               \
    (*((field##_TYPE *)(((IMG_UINTPTR)(vpMem)) + field##_OFFSET))) =                                                                        \
    ((*((field##_TYPE *)(((IMG_UINTPTR)(vpMem)) + field##_OFFSET))) & (field##_TYPE)~(field##_MASK >> ((ui32RepIndex) * field##_SIZE)) |    \
        (field##_TYPE)(((uiValue) << (field##_SHIFT - ((ui32RepIndex) * field##_SIZE))) & (field##_MASK >> ((ui32RepIndex) * field##_SIZE))));

/*!
******************************************************************************

 @Function    MEMIO_WRITE_TABLE_REPEATED_FIELD

******************************************************************************/
#define MEMIO_WRITE_TABLE_REPEATED_FIELD(vpMem, field, ui32TabIndex, ui32RepIndex, uiValue)                                                                                         \
    MEMIO_CHECK_ALIGNMENT(vpMem); IMG_ASSERT(((ui32TabIndex) < field##_NO_ENTRIES) || (field##_NO_ENTRIES == 0)); IMG_ASSERT((ui32RepIndex) < field##_NO_REPS);                     \
    (*((field##_TYPE *)(((IMG_UINTPTR)(vpMem)) + field##_OFFSET + (field##_STRIDE * (ui32TabIndex))))) =                                                                          \
        ((*((field##_TYPE *)(((IMG_UINTPTR)(vpMem)) + field##_OFFSET + (field##_STRIDE * (ui32TabIndex))))) & (field##_TYPE)~(field##_MASK >> ((ui32RepIndex) * field##_SIZE))) | \
        (field##_TYPE)(((uiValue) << (field##_SHIFT - ((ui32RepIndex) * field##_SIZE))) & (field##_MASK >> ((ui32RepIndex) * field##_SIZE)));

#endif


#if defined (__cplusplus)
}
#endif

#endif


