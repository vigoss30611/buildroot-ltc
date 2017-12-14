/*! 
******************************************************************************
@file   image.cpp
@brief  Base class for holding a single image in memory + set of derived classes
        for reading/writing the image in various file formats.
@Author Juraj Mlynar
@date   27/11/2012
        <b>Copyright 2012 by Imagination Technologies Limited.</b>\n
        All rights reserved.  No part of this software, either
        material or conceptual may be copied or distributed,
        transmitted, transcribed, stored in a retrieval system
        or translated into any human or computer language in any
        form by any means, electronic, mechanical, manual or
        other-wise, or disclosed to third parties without the
        express written permission of Imagination Technologies
        Limited, Unit 8, HomePark Industrial Estate,
        King's Langley, Hertfordshire, WD4 8LZ, U.K.

******************************************************************************/
#include <img_types.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include "image.h"

#define SizeofArray(x)  ( sizeof(x)/sizeof((x)[0]) )
#define ASSERT IMG_ASSERT

#ifdef _WIN32
	#include <io.h>
	//#define INT64PPFX    "I64"  //prefix used in printf/scanf before 'd' or 'x' to manipulate IMG_INT64
#else
	#include <unistd.h>         //for access()
	//#define INT64PPFX    "ll"   //prefix used in printf/scanf before 'd' or 'x' to manipulate IMG_INT64
	// Linux equivalents of Windows function names
	#define stricmp      strcasecmp
	#define strnicmp     strncasecmp
	#define _snprintf    snprintf
	#define _vsnprintf   vsnprintf
	#define _access      access
	#if defined(__APPLE__) || defined(ANDROID)
		// these are already 64bit on Apple
		#define ftello64 ftello
		#define fseeko64 fseeko
		#define __off64_t off_t
	#endif
#endif

#include <img_defs.h>
#define INT64PPFX IMG_I64PR

//bayer pattern layouts
//1st two numbers are horiz.& vert.pattern size, then channel numbers for each pattern row (in raster scan order)
const IMG_INT8 bayerLayout2x2_0123[]={ 2,2, 0,1, 2,3 };  //i.e. RGGB
const IMG_INT8 bayerLayout2x2_1032[]={ 2,2, 1,0, 3,2 };  //i.e. GRBG
const IMG_INT8 bayerLayout2x2_2301[]={ 2,2, 2,3, 0,1 };  //i.e. GBRG
const IMG_INT8 bayerLayout2x2_3210[]={ 2,2, 3,2, 1,0 };  //i.e. BGGR

template <class T>
T Max3(T a,T b,T c) { return  (a>b)?( (a>c)?a:c ):( (b>c)?b:c ); }

static bool myfseeki64(FILE *f,IMG_INT64 seekPos) {
  if (seekPos<0x7fffffff) { return (fseek(f,(long)seekPos,SEEK_SET)!=0) ? false: true; }
  return (fsetpos(f,(fpos_t*)&seekPos)!=0) ? false: true;
}

static IMG_INT64 myftelli64(FILE *f) {
  //fpos_t pos;
  //if (fgetpos(f,&pos)!=0) return 0;
  //return *(IMG_INT64*)&pos;
#ifdef _WIN32
  return _ftelli64(f);
#else
  return ftello64(f);
#endif
}

static IMG_INT64 myfilelengthi64(FILE *f)
{
#ifdef _WIN32
  return filelength(fileno(f));
#else
  __off64_t pos,size;
  pos = ftello64(f);
  fseeko64(f,0,SEEK_END);  //int fseeko64(FILE *stream, off64_t offset, int whence);
  size = ftello64(f);
  fseeko64(f,pos,SEEK_SET);  //off64_t ftello64(FILE *stream);
  return size;
#endif
}

static IMG_INT32 RoundUpToMultiple(IMG_INT32 val,IMG_INT32 multipleOf) {
  IMG_INT32 mod;
  if ((mod = val % multipleOf) == 0) return val;
  return val + multipleOf - mod;
}

static inline IMG_INT32 RoundUpDiv(IMG_INT32 val,IMG_INT32 div) {
  return (val + div - 1) / div;
}

static IMG_INT32 CalcGCD(IMG_INT32 x,IMG_INT32 y) {
//calculate greatest common divisor of two numbers
   while ((x > 0) && (y > 0)) {  //repeat until either x or y is 0; then the non-zero one is GCD
     if (x > y) x %= y;
     else y %= x;
     }
   return (x > 0) ? x : y;
}

static void swapbytes(void *p1,void *p2,IMG_INT32 Nbytes) {
  IMG_UINT8 *q1,*q2,tmp;
  q1 = (IMG_UINT8*)p1;
  q2 = (IMG_UINT8*)p2;
  while (Nbytes--) {
    tmp = *q1;  *q1 = *q2;  *q2 = tmp;
    q1++;  q2++;
    }
}

/*-------------------------------------------------------------------------------
image meta data storage*/

CMetaData::CMetaData() {
  list = NULL;
}

void CMetaData::Unload() {
//remove all meta data items (free any memory)
  while (list != NULL) {
    MDITEM *mdi;
    mdi = list;
    list = list->next;
    if (mdi->name != NULL) delete [](mdi->name);
    if (mdi->sVal != NULL) delete [](mdi->sVal);
    delete mdi;
    }
}

IMG_INT32 CMetaData::GetNItems() {
//returns number of meta data items
  MDITEM *mdi;
  IMG_INT32 count;
  count = 0;
  for (mdi = list; mdi != NULL; mdi = mdi->next) count++;
  return count;
}

bool CMetaData::Add(const char *name,IMG_INT32 nameLen,const char *value,IMG_INT32 valueLen,IFEX_xxx whatIfExists,const char *separatorOnAppend) {
//add meta data item
//nameLen: name length in characters (if not 0-terminated), -1 to use strlen(name)
//valueLen: value length in characters (if not 0-terminated), -1 to use strlen(value)
//whatIfExists: what to do if the item exists
//separatorOnAppend: string to to insert between already existing item value and the new value appended to the end (when appending this value to existing)
  MDITEM *mdi;
  if (nameLen < 0) nameLen = strlen(name);
  if (valueLen < 0) valueLen = strlen(value);
  //is this item already defined?
  mdi=((whatIfExists==IFEX_IGNORE)?NULL:Find(name,nameLen));
  if (mdi == NULL) {  //apparently not - create a new one
    mdi = new MDITEM;
	if (mdi == NULL) { return false; }
    mdi->name = new char[nameLen + 1];
	if (mdi->name == NULL) {
		delete mdi;
		return false;
	}
    mdi->sVal = new char[valueLen + 1];
	if (mdi->sVal == NULL) {
		delete [](mdi->name);
		delete mdi;
		return false;
	}
    memcpy(mdi->name,name,nameLen);
    mdi->name[nameLen] = '\0';
    memcpy(mdi->sVal,value,valueLen);
    mdi->sVal[valueLen] = '\0';
    //add to end of list
    if (list == NULL) list = mdi;
    else {
      MDITEM *pLast;
      for (pLast = list; pLast->next != NULL; pLast=pLast->next) ;
      pLast->next = mdi;
      }
    mdi->next = NULL;
    }
  else {  //already exists
    char *newVal;
    if (whatIfExists == IFEX_FAIL) return false;
    if (whatIfExists == IFEX_REPLACE) {  //replace
      if ((valueLen == (int)strlen(mdi->sVal)) && (strncmp(mdi->sVal,value,valueLen) == 0)) return true;  //no change - do nothing
      newVal=new char[valueLen + 1];
      if (newVal == NULL) { return false; }
      memcpy(newVal,value,valueLen);
      newVal[valueLen] = '\0';
      if (mdi->sVal != NULL) delete [](mdi->sVal);
      mdi->sVal = newVal;
      }
    else {  //IFEX_APPEND append
      IMG_INT32 oldLen,sepLen;
      if (separatorOnAppend == NULL) separatorOnAppend = "";
      oldLen = strlen(mdi->sVal);
      sepLen = strlen(separatorOnAppend);
      newVal = new char[oldLen + sepLen + valueLen + 1];
      if (newVal == NULL) { return false; }
      memcpy(newVal,mdi->sVal,oldLen);
      memcpy(newVal + oldLen, separatorOnAppend, sepLen);
      memcpy(newVal + oldLen + sepLen, value, valueLen);
      newVal[oldLen + sepLen + valueLen] = '\0';
      delete [](mdi->sVal);
      mdi->sVal = newVal;
      }
    }
  return true;
}

bool CMetaData::UpdateSubItem(const char *name,IMG_INT32 nameLen,IMG_INT subIndex,const char *value,IMG_INT32 valueLen,char separator) {
//modify the subIndex-th value of item with given name to 'value'
//if the item does not exist it is created
//if the item does not have as many values as subIndex assumes, value is replicated to fill in any previous missing ones
  MDITEM *mdi;
  char *pEndBefore,*pStartAfter,*snew,*c;
  IMG_INT idx;
  if (nameLen < 0) nameLen = strlen(name);
  if (valueLen < 0) valueLen = strlen(value);
  mdi = Find(name,nameLen);
  if (mdi == NULL) {
    char sep[2];
    sep[0] = separator;  sep[1] = '\0';
    if (!Add(name,nameLen,"",0,IFEX_IGNORE,sep)) return false;
    mdi = Find(name,nameLen);
    }
  //find end of items before subIndex
  for (pEndBefore = mdi->sVal; *pEndBefore && (*pEndBefore == separator); pEndBefore++) ;  //skip initial spaces
  idx = 0;
  for (c = pEndBefore; idx < subIndex; c++) {
    if (*c == '\0') { pEndBefore = c;  idx++;  break; }
    if (*c == separator) { pEndBefore = c;  idx++; }
    }
  if (idx < subIndex) {  //subIndex too large - not enough previous values - replicate the value at the end
    //need space for original string + separator + (subIndex-idx+1) copies of value (with separator in between)
    snew = new char[strlen(mdi->sVal) + 1 + (valueLen + 1) * (subIndex - idx + 1) + 1];
    if (snew == NULL) { return false; }
    if (idx > 0) { strcpy(snew,mdi->sVal);  c = snew + strlen(mdi->sVal);  *c++ = separator; }
    else c = snew;
    for (; idx <= subIndex; idx++) {
      memcpy(c,value,valueLen);
      c += valueLen;
      *c++ = separator;
      }
    c[-1] = '\0';
    }
  else {  //value exists - replace and copy all remaining values
    for (pStartAfter = pEndBefore; *pStartAfter && (*pStartAfter == separator); pStartAfter++);  //find start of value being replaced
    c = pStartAfter;
    for (;*pStartAfter && (*pStartAfter != separator); pStartAfter++);  //find end of value being replaced
    if (pStartAfter - c == valueLen) {  //the old value has same length as new value - just overwrite in-place and go away
      memcpy(c,value,valueLen);
      return true;
      }
    //need to re-allocate the buffer
    //need space for original string up to pEndBefore + separator + new value + remainder (from pStartAfter onwards)
    //pStartAfter is either empty string or starts with a separator
    snew = new char[(pEndBefore - mdi->sVal) + 1 + valueLen + strlen(pStartAfter) + 1];
    if (snew == NULL) { return false; }
    if (pEndBefore > mdi->sVal) {
      memcpy(snew,mdi->sVal,(pEndBefore - mdi->sVal));
      c = snew + (pEndBefore - mdi->sVal);
      *c++ = separator;
      }
    else c = snew;
    memcpy(c,value,valueLen);
    strcpy(c + valueLen, pStartAfter);
    }
  //replace old value with new one
  if (mdi->sVal != NULL) delete []mdi->sVal;
  mdi->sVal = snew;
  return true;
}

bool CMetaData::Add(MDITEM *metaDataItem) {
//Add or replace meta data item with another meta data item
  if (metaDataItem == NULL) return false;
  return CMetaData::Add(metaDataItem->name,strlen(metaDataItem->name),metaDataItem->sVal,strlen(metaDataItem->sVal),IFEX_REPLACE," ");
}

void CMetaData::Del(MDITEM *metaDataItem) {
//delete the given meta data item
  if (metaDataItem == NULL) return;
  if (list==metaDataItem) list=list->next;
  else {
    MDITEM *pPrev;
    for (pPrev=list;(pPrev!=NULL) && (pPrev->next!=metaDataItem);pPrev=pPrev->next) ;  //find previous item
    if (pPrev!=NULL) pPrev->next=metaDataItem->next;
    }
  delete [](metaDataItem->name);
  delete [](metaDataItem->sVal);
  delete metaDataItem;
}

bool CMetaData::Rename(MDITEM *metaDataItem,const char *newName) {
//change name of meta data item
  char *newn;
  newn=new char[strlen(newName)+1];
  if (newn) { return false; }
  strcpy(newn,newName);
  delete []metaDataItem->name;
  metaDataItem->name=newn;
  return true;
}

bool CMetaData::UpdateStr(const char *fieldName, const char *newValue,IFEX_xxx whatIfExists) {
//Add/replace meta data field of given name with new string value.
  return CMetaData::Add(fieldName,strlen(fieldName),newValue,strlen(newValue),whatIfExists," ");
}

bool CMetaData::UpdateStrFmt(const char *fieldName,IFEX_xxx whatIfExists,const char *newValueFmt,...) {
//Add/replace meta data field of given name with new string value, with string formatting support.
  char buf[500];
  va_list va;
  va_start(va,newValueFmt);
  _vsnprintf(buf,sizeof(buf),newValueFmt,va);
  va_end(va);
  buf[sizeof(buf)-1] = '\0';
  return UpdateStr(fieldName,buf,whatIfExists);
}

bool CMetaData::UpdateInt(const char *fieldName, IMG_INT newValue,IFEX_xxx whatIfExists) {
//Replace meta data field of given name with new value.
  char newValueStr[25]; // 64 bit integer has max. value of 2^64-1 = 1.8447e+019 (20 characters expanded) plus 1 char for '\0' and 1 for sign
  //newValueStr = itoa(newValue,newValueStr,10);
  sprintf(newValueStr,"%d",newValue);
  return UpdateStr(fieldName,newValueStr,whatIfExists);
}

bool CMetaData::UpdateDouble(const char *fieldName, double newValue,IFEX_xxx whatIfExists) {
//Replace meta data field of given name with new value.
  char newValueStr[50],*c;
  _snprintf(newValueStr,sizeof(newValueStr),"%.10lf",newValue);
  newValueStr[sizeof(newValueStr)-1]='\0';
  if (strchr(newValueStr,'.')!=NULL) {  //trim 0s in fractional part
    for (c=newValueStr+strlen(newValueStr);(c>newValueStr) && (c[-1]=='0');) *(--c)='\0';
    if ((c>newValueStr) && (c[-1]=='.')) *(--c)='\0';
    if (*newValueStr=='\0') newValueStr[0]='0';  //in case it started with a '.' make it a '0'
    }
  return UpdateStr(fieldName,newValueStr,whatIfExists);
}

bool CMetaData::UpdateSubStr(const char *fieldName,IMG_INT32 subIndex,const char *newValue,char separator) {
//replace one of the values in a multi-value item
  return UpdateSubItem(fieldName,-1,subIndex,newValue,-1,separator);
}

bool CMetaData::UpdateSubStrFmt(const char *fieldName,IMG_INT32 subIndex,char separator,const char *newValueFmt,...) {
  char buf[500];
  va_list va;
  va_start(va,newValueFmt);
  _vsnprintf(buf,sizeof(buf),newValueFmt,va);
  va_end(va);
  buf[sizeof(buf)-1] = '\0';
  return UpdateSubItem(fieldName,-1,subIndex,buf,-1,separator);
}

bool CMetaData::UpdateSubInt(const char *fieldName,IMG_INT32 subIndex,IMG_INT newValue,char separator) {
  char newValueStr[25]; // 64 bit integer has max. value of 2^64-1 = 1.8447e+019 (20 characters expanded) plus 1 char for '\0' and 1 for sign
  //newValueStr = itoa(newValue,newValueStr,10);
  sprintf(newValueStr,"%d",newValue);
  return UpdateSubItem(fieldName,-1,subIndex,newValueStr,-1,separator);
}

bool CMetaData::UpdateSubDouble(const char *fieldName,IMG_INT32 subIndex,double newValue,char separator) {
  char newValueStr[50],*c;
  _snprintf(newValueStr,sizeof(newValueStr),"%.10lf",newValue);
  newValueStr[sizeof(newValueStr)-1]='\0';
  if (strchr(newValueStr,'.')!=NULL) {  //trim 0s in fractional part
    for (c=newValueStr+strlen(newValueStr);(c>newValueStr) && (c[-1]=='0');) *(--c)='\0';
    if ((c>newValueStr) && (c[-1]=='.')) *(--c)='\0';
    if (*newValueStr=='\0') newValueStr[0]='0';  //in case it started with a '.' make it a '0'
    }
  return UpdateSubItem(fieldName,-1,subIndex,newValueStr,-1,separator);
}

bool CMetaData::Rename(const char *fieldName,const char *newName) {
//rename all meta data items with given fieldName to newName
  MDITEM *mdi;
  bool ok;
  ok = true;
  for (mdi=list; mdi!=NULL; mdi=mdi->next) {
    if (stricmp(mdi->name,fieldName)!=0) continue;
    if (!Rename(mdi,newName)) ok=false;
    }
  return ok;
}

const char *CMetaData::AddFromString(const char *str,IFEX_xxx whatIfExists,const char *separatorOnAppend) {
//add meta data item from a string in the format
//  itemName=itemValue
//where itemValue is terminated by end of string or '\n'.
//separatorOnAppend: string to to insert between already existing item value and the new value appended to the end (when appending this value to existing)
//returns pointer to the terminating '\0' or just after terminating '\n'
//returns NULL on error (string badly formated or out of memory)
  const char *pEq,*pNL;
  //find end of item
  pEq = NULL;
  for (pNL = str; *pNL && (*pNL != '\n'); pNL++) {
    if (*pNL == '=') pEq = pNL;
    }
  if (pEq == NULL) return NULL;
  if (!Add(str,pEq-str,pEq+1,pNL-pEq-1,whatIfExists,separatorOnAppend)) return NULL;
  if (*pNL == '\n') pNL++;
  return pNL;
}

const char *CMetaData::GetMetaStr(const char *name,const char *defVal) {
//return value of meta data item as string
  MDITEM *mdi;
  if ((mdi = Find(name)) == NULL) return defVal;
  return mdi->sVal;
}

const char *CMetaData::GetMetaSubStr(const char *name,const char *defVal,IMG_INT32 index,IMG_INT32 *pLen,char separator) {
//return value of meta data item substring as string
  MDITEM *mdi;
  const char *pSta,*pEnd;
  if ((mdi = Find(name)) == NULL) {
    if (pLen != NULL) *pLen = ((defVal == NULL)? 0: strlen(defVal));
    return defVal;
    }
  pSta = mdi->sVal;
  while (1) {
    while (*pSta && isspace(*pSta)) pSta++;  //skip leading spaces
    for (pEnd = pSta;*pEnd && (*pEnd != separator);pEnd++) ;  //find end
    if (((--index)<0) || (*pSta == '\0')) break;
    pSta = pEnd + 1;  //skip value and separator
    }
  if (*pSta == '\0') return NULL;
  if (pLen != NULL) *pLen = pEnd - pSta;
  return pSta;
}

IMG_INT32 CMetaData::GetMetaInt(const char *name,IMG_INT32 defVal,IMG_INT32 index,bool extendLast) {
//returns value of meta data item as integer
//the value may define multiple (space-separated) integers, in that case return the index-th integer value
//in case of error (value not found, it is not an integer) returns defVal
//extendLast: false = if index>=number of values available, returns defVal
//            true = if index>=number of values available, returns value at the highest index available
  const char *c,*pLast;
  MDITEM *mdi;
  IMG_INT32 val;
  int n;
  if ((mdi = Find(name)) == NULL) return defVal;
  pLast = mdi->sVal;
  for (c = mdi->sVal; index > 0; index--) {
    while (*c && isspace(*c)) c++;  //skip to value
    if (*c == '\0') break;
    pLast = c;  //remember beginning of last found value
    while (*c && !isspace(*c)) c++;  //skip value
    }
  if (extendLast && (*c == '\0')) c = pLast;
  while (*c && isspace(*c)) c++;  //skip to value
  n=-1;  sscanf(c,"%d%n",&val,&n);
  if (n<1) return defVal;
  return val;
}

// Overloaded GetMetaInt that is simplified - requires only 1 parameter
IMG_INT32 CMetaData::GetMetaInt(const char *name) {
  return CMetaData::GetMetaInt(name, 0x7FFFFFFF, 0);
}

double CMetaData::GetMetaDouble(const char *name,double defVal,IMG_INT32 index,bool extendLast) {
//returns value of meta data item as double
//the value may define multiple (space-separated) values, in that case return the index-th integer value
//in case of error (value not found, it is not numeric) returns defVal
//extendLast: false = if index>=number of values available, returns defVal
//            true = if index>=number of values available, returns value at the highest index available
  const char *c,*pLast;
  MDITEM *mdi;
  double val;
  int n;
  if ((mdi = Find(name)) == NULL) return defVal;
  pLast = mdi->sVal;
  for (c = mdi->sVal; index > 0; index--) {
    while (*c && isspace(*c)) c++;  //skip to value
    if (*c == '\0') break;
    pLast = c;  //remember beginning of last found value
    while (*c && !isspace(*c)) c++;  //skip value
    }
  if (extendLast && (*c == '\0')) c = pLast;
  while (*c && isspace(*c)) c++;  //skip to value
  n=-1;  sscanf(c,"%lf%n",&val,&n);
  if (n<1) return defVal;
  return val;
}

IMG_INT32 CMetaData::GetMetaValCount(const char *name) {
//get number of space-separated values provided for a meta data item of given name
  const char *c;
  MDITEM *mdi;
  IMG_INT32 count;
  if ((mdi = Find(name)) == NULL) return 0;
  if (*mdi->sVal == '\0') return 0;
  count = (isspace(*mdi->sVal) ? 0 : 1);
  for (c = mdi->sVal + 1; *c; c++) {
    if (!isspace(*c) && isspace(c[-1])) count++;
    }
  return count;
}

IMG_INT32 CMetaData::GetMetaEnum(const char *name,IMG_INT32 defVal,const char *enumStrings) {
//get meta data item as string, match it against one of the strings in enumStrings and return its 0-based index in the list
//enumStrings: \0-separated, \0\0-terminated list of strings to match against (not case sensitive)
  const char *c;
  MDITEM *mdi;
  IMG_INT32 idx;
  if ((mdi = Find(name)) == NULL) return defVal;
  for (c = enumStrings, idx = 0; *c; c += strlen(c) + 1, idx++) {
    if (stricmp(c,mdi->sVal) == 0) return idx;
    }
  return defVal;
}

const char *CMetaData::GetMetaName(IMG_INT32 index) {
//get name of meta data item at given index
  MDITEM *mdi;
  if ((mdi = GetMetaAt(index)) == NULL) return NULL;
  return mdi->name;
}

CMetaData::MDITEM *CMetaData::GetMetaAt(IMG_INT32 index) {
//return index-th item of meta data
  MDITEM *mdi;
  for (mdi = list; (mdi != NULL) && (index > 0); mdi = mdi->next) index--;
  return mdi;
}

CMetaData::MDITEM *CMetaData::Find(const char *name,IMG_INT32 prefixLen) {
//search for meta data item by name
//prefixLen: applies to the name parameter, not meta data items; -1=use entire name
  MDITEM *mdi;
  if (prefixLen < 0) prefixLen = strlen(name);
  for (mdi = list; mdi != NULL; mdi = mdi->next) {
    if (strnicmp(mdi->name,name,prefixLen) != 0) continue;
    if (strlen(mdi->name) != (size_t)prefixLen) continue;
    return mdi;
    }
  return NULL;
}

bool CMetaData::CopyFrom(CMetaData *pOther) {
//make this object a copy of the other one
  IMG_INT32 Nitems,i;
  if (pOther == NULL) return false;
  Unload();
  Nitems = pOther->GetNItems();
  for (i = 0; i < Nitems; i++) {
    if (!Add(pOther->GetMetaAt(i))) return false;
    }
  return true;
}

bool CMetaData::MergeFrom(CMetaData *pOther,IFEX_xxx whatIfExists) {
//merge meta data from other object into this one
  MDITEM *md,*mdOther;
  IMG_INT32 Nitems,i;
  Nitems = pOther->GetNItems();
  if (whatIfExists == IFEX_REPLACE) {  //delete all items which exist in pOther (may be multiple items with same name)
    for (i = 0; i < Nitems; i++) {
      mdOther = pOther->GetMetaAt(i);
      while ((md = Find(mdOther->name)) != NULL) Del(md);
      }
    whatIfExists = IFEX_IGNORE;
    }
  for (i = 0; i < Nitems; i++) {
    md = pOther->GetMetaAt(i);
    Add(md->name,-1,md->sVal,-1,whatIfExists," ");
    }
  return true;
}

void CMetaData::SwapWith(CMetaData *pOther) {
//swap all items with another meta data object
  MDITEM *tmp;
  tmp = list;  list = pOther->list;  pOther->list = tmp;
}

/*-------------------------------------------------------------------------------
bit reader/writer class - reads/writes a bit stream*/

class CBitStreamRW {
protected:
  IMG_UINT8 *pStream;
  IMG_UINT8 bitPos;  //next read/write bit position at *pStream (0-7)
public:
  CBitStreamRW() { pStream = NULL;  bitPos = 0; }
  void SetPos(IMG_UINT8 *pStreamData);
  IMG_UINT32 ReadBitsUns(IMG_INT8 Nbits);
  IMG_INT32 ReadBitsSig(IMG_INT8 Nbits);
  void WriteBitsUns(IMG_UINT32 val,IMG_INT8 Nbits);
  void WriteBitsSig(IMG_INT32 val,IMG_INT8 Nbits);
  void AdvanceToByteBoundary();
  };

void CBitStreamRW::SetPos(IMG_UINT8 *pStreamData) {
//set position for reading/writing bits
  pStream = pStreamData;
  bitPos = 0;
}

IMG_UINT32 CBitStreamRW::ReadBitsUns(IMG_INT8 Nbits) {
//unpack up to 32-bit unsigned number from a bit stream
  IMG_UINT32 val;
  IMG_INT8 bitsDone;
  if (8 - bitPos >= Nbits) {  //entire value is within the current byte of the bitstream
    val = ((*pStream) >> bitPos) & ((1 << Nbits) - 1);
    if ((bitPos += Nbits) > 7) { bitPos = 0;  pStream++; }
    }
  else {   //value is split between multiple bytes
    val = (*pStream >> bitPos);  bitsDone = 8 - bitPos;
    pStream++;
    while (Nbits - bitsDone >= 8) { val |= (*pStream++) << bitsDone;  bitsDone += 8; }
    if (Nbits > bitsDone) val |= ((*pStream) & ((1 << (Nbits - bitsDone)) - 1)) << bitsDone;
    bitPos = Nbits - bitsDone;  //#of bits already used in the last byte
    }
  return val;
}

IMG_INT32 CBitStreamRW::ReadBitsSig(IMG_INT8 Nbits) {
//unpack up to 32-bit signed number from a bit stream
  IMG_UINT32 val;
  val = ReadBitsUns(Nbits);
  if (val & (1 << (Nbits - 1))) val |= (0xfffffffful << Nbits);  //need to sign-extend
  return *(IMG_INT32*)&val;
}

void CBitStreamRW::WriteBitsUns(IMG_UINT32 val,IMG_INT8 Nbits) {
//pack up to 32-bit unsigned number to a bit stream
  IMG_INT8 bitsDone;
  val &= ((1 << Nbits) - 1);
  if (!bitPos) *pStream = 0;
  if (8 - bitPos >= Nbits) {  //entire value fits into the current byte of the bitstream
    (*pStream) |= (val << bitPos);
    if ((bitPos += Nbits) > 7) { bitPos = 0;  pStream++; }
    }
  else {  //value must be split between multiple bytes
    bitsDone = 8 - bitPos;
    (*pStream) |= ((val & ((1 << bitsDone) - 1)) << bitPos);
    pStream++;
    while (Nbits - bitsDone >= 8) { *pStream++ = (IMG_UINT8)((val >> bitsDone) & 0xff);  bitsDone += 8; }
    if (Nbits > bitsDone) *pStream = (IMG_UINT8)(val >> bitsDone);
    bitPos = Nbits - bitsDone;  //#of bits left unused in the last byte
    }
}

void CBitStreamRW::WriteBitsSig(IMG_INT32 val,IMG_INT8 Nbits) {
  WriteBitsUns(*(IMG_UINT32*)&val,Nbits);
}

void CBitStreamRW::AdvanceToByteBoundary() {
//align position to the beginning of the next byte
  if (bitPos > 0) { pStream++;  bitPos = 0; }
}

/*-------------------------------------------------------------------------------
base class for image data storage*/

static const char *channelNamesGray[] = {"Y"};
static const char *channelNamesRGBA[] = {"R","G","B","A"};
static const char *channelNamesRGGB[] = {"R","G1","G2","B"};
static const char *channelNamesRGBW[] = {"R","G","B","W"};
static const char *channelNamesYUVA[] = {"Y","U","V","A"};
const CImageBase::COLORMODELINFO colorModelsInfo[] = {
  { CImageBase::CM_GRAY,  1, false, false, channelNamesGray,},
  { CImageBase::CM_RGB,   3, false, false, channelNamesRGBA,},
  { CImageBase::CM_RGBA,  4, false, false, channelNamesRGBA },
  { CImageBase::CM_RGGB,  4, true,  true, channelNamesRGGB,},
  { CImageBase::CM_RGBW,  4, true,  true, channelNamesRGBW,},
  { CImageBase::CM_YUV,   3, false, true, channelNamesYUVA,},
  { CImageBase::CM_YUVA,  4, false, true, channelNamesYUVA,},
  };

CImageBase::CImageBase() {
  IMG_UINT32 ch;
  for (ch = 0; ch < SizeofArray(chnl); ch++) chnl[ch].data = NULL;
  fileName = NULL;
  Unload();
}

CImageBase::~CImageBase() {
  Unload();
}

/*static*/ const CImageBase::COLORMODELINFO *CImageBase::GetColorModelInfo(CImageBase::CM_xxx model) {
  IMG_UINT32 i;
  for (i = 0; i < SizeofArray(colorModelsInfo); i++) {
    if (colorModelsInfo[i].model == model) return colorModelsInfo + i;
    }
  return NULL;
}

/*static*/ IMG_UINT8 CImageBase::GetBayerFlip(CImageBase::SUBS_xxx bayerSubsampling) {
//get the flip value for given 2x2 bayer CFA pattern to convert it into RGGB/RGBW
//returned value: &1=horizontal flip, &2=vertical flip
  switch (bayerSubsampling) {
    case SUBS_RGGB: return 0;
    case SUBS_GRBG: return 1;
    case SUBS_GBRG: return 2;
    case SUBS_BGGR: return 3;
    default: return 0;
    }
}

/*static*/ IMG_UINT8 CImageBase::GetBayerFlip(CImageBase::SUBS_xxx srcBayerSubsampling,CImageBase::SUBS_xxx dstBayerSubsampling) {
//get the flip value for converting a given source 2x2 RGGB/RGBW bayer pattern into given destination 2x2 bayer pattern
  return GetBayerFlip(srcBayerSubsampling)^GetBayerFlip(dstBayerSubsampling);
}

bool CImageBase::IsDataLoaded() {
//returns true if image data is loaded (false if nothing or just header)
  IMG_INT32 Nchannels,i;
  if ((Nchannels = GetNColChannels()) < 1) return false;
  for (i = 0; i < Nchannels; i++) {
    if (chnl[i].data == NULL) return false;
    }
  return true;
}

void CImageBase::Unload() {
  UnloadHeader();
  UnloadData();
}

void CImageBase::UnloadHeader() {
  IMG_UINT32 ch;
  if (fileName != NULL) { delete []fileName;  fileName = NULL; }
  width = height = 0;
  colorModel = CM_UNDEF;
  subsMode = SUBS_UNDEF;
  for (ch = 0; ch < SizeofArray(chnl); ch++) {
    chnl[ch].chnlWidth = 0;
    chnl[ch].chnlHeight = 0;
    chnl[ch].bitDepth = 0;
    chnl[ch].isSigned = false;
    }
}

void CImageBase::UnloadData() {
  IMG_UINT32 ch;
  for (ch = 0; ch < SizeofArray(chnl); ch++) {
    if (chnl[ch].data != NULL) { delete [](chnl[ch].data);  chnl[ch].data = NULL; }
    }
}

IMG_INT32 CImageBase::GetNColChannels() const {
//returns #of color channels based on image color model
  const COLORMODELINFO *cmi;
  if ((cmi = GetColorModelInfo(colorModel)) == NULL) return 0;
  return cmi->Nchannels;
}

IMG_INT32 CImageBase::GetXSampling(IMG_INT32 colChannel) const {
//returns #of pixels to advance horizontally to get to the next sample in the same color channel
  switch (subsMode) {
    case SUBS_UNDEF: return 1;
    case SUBS_422:
    case SUBS_420: return (colChannel == 0) ? 1: 2;
    case SUBS_RGGB:
    case SUBS_GRBG:
    case SUBS_GBRG:
    case SUBS_BGGR: return 2;
    case SUBS_444:
    default: return 1;
    }
}

IMG_INT32 CImageBase::GetYSampling(IMG_INT32 colChannel) const {
//returns #of pixels to advance vertically to get to the next sample in the same color channel
  switch (subsMode) {
    case SUBS_UNDEF: return 1;
    case SUBS_422: return 1;
    case SUBS_420: return (colChannel == 0) ? 1: 2;
    case SUBS_RGGB:
    case SUBS_GRBG:
    case SUBS_GBRG:
    case SUBS_BGGR: return 2;
    case SUBS_444:
    default: return 1;
    }
}

const char *CImageBase::LoadFileHeader(const char *filename,void *pExtra) { return "Internal error"; }
//loads file header (this function must be called before image data can be loaded)
//pExtra: either NULL or points to a format-specific xxxLOADFORMAT structure used to override format of file being loaded
//returns NULL=ok, !NULL=pointer to error message

const char *CImageBase::LoadFileData(IMG_INT32 frameIndex) { return "Internal error"; }
//loads one image from file
//the header must have been loaded already
//frameIndex: index of the image in file (can be >0 if image is a video with multiple frames)
//returns NULL=ok, !NULL=pointer to error message

const char *CImageBase::SaveFileStart(const char *filename,void *pExtra,void **pSC) { return "Internal error"; }
//initiates saving of a file; allocates a format-specific internal save state structure and sets (*pSC) to it
//pSC: uninitialised pointer, set by this function, must be passed to remaining save functions
//pExtra: must point to a format-specific xxxSAVEFORMAT structure, which must remain valid during the entire save process
//returns: NULL=ok, !NULL=error message string

const char *CImageBase::SaveFileHeader(void *pSC) { return "Internal error"; }
//saves the file header, such as meta data
//Unless a file format supports header after data, SaveFileHeader() must be called before SaveFileData().

const char *CImageBase::SaveFileData(void *pSC) { return "Internal error"; }
//save one image/frame to file at the current file position
//SaveFileData must be called once for each frame if file should contain multiple frames (if format supports it)
//Unless a file format supports header after data, SaveFileHeader() must be called before SaveFileData().

const char *CImageBase::SaveFileEnd(void *pSC) { return "Internal error"; }
//finishes saving the image file - writes any remaining data, closes the file and deallocates memory used by pSC structure
//if SaveFileStart() succeeds, this function must be called (even if SaveFileHeader() or SaveFileData() fail) to properly tidy up

const char *CImageBase::SaveSingleFrameImage(const char *filename,void *pExtra) {
//do the save procedure for a special (but common) case where a file contains the header followed by one image
//returns NULL=ok, !NULL=error message string
  const char *pErr;
  void *pSaveContext;
  pErr = SaveFileStart(filename,pExtra,&pSaveContext);
  if (pErr == NULL) {
    pErr = SaveFileHeader(pSaveContext);
    if (pErr == NULL) pErr = SaveFileData(pSaveContext);
    if (pErr == NULL) pErr = SaveFileEnd(pSaveContext);
    else SaveFileEnd(pSaveContext);
    }
  return pErr;
}

bool CImageBase::CreateNewImage(IMG_INT32 width,IMG_INT32 height,CM_xxx colorModel,SUBS_xxx subsampling,IMG_INT8 *chnlBitDepths,PIXEL *chnlDefVal) {
//creates a blank image of the given dimensions and format
//chnlBitDepths: array of bitdepths, one per color channel (as implied by colorModel)
//               a negative bitdepth value indicates signed color channel, positive bitdepth indicates unsigned color channel
//chnlDefVal: array of default values one for each color channel; may be NULL in which case 0 is used as default 
  IMG_INT32 ch,i;
  const COLORMODELINFO *cmi;
  Unload();
  this->width = width;
  this->height = height;
  this->colorModel = colorModel;
  subsMode = subsampling;
  cmi = GetColorModelInfo(colorModel);
  for (ch = 0; ch < cmi->Nchannels; ch++) {
    COLCHANNEL *pChnl = chnl + ch;
    PIXEL defVal;
    i = GetXSampling(ch);
    pChnl->chnlWidth = (width + i - 1) / i;
    i = GetYSampling(ch);
    pChnl->chnlHeight = (height + i - 1) / i;
    pChnl->data = new PIXEL[pChnl->chnlWidth * pChnl->chnlHeight];
    if (pChnl->data == NULL) {
      Unload();
      return false;
      }
    if (chnlBitDepths[ch] < 0) {
      pChnl->bitDepth = (IMG_UINT8)(-chnlBitDepths[ch]);
      pChnl->isSigned = true;
      }
    else {
      pChnl->bitDepth = (IMG_UINT8)(chnlBitDepths[ch]);
      pChnl->isSigned = false;
      }
    defVal = ((chnlDefVal == NULL) ? 0 : chnlDefVal[ch]);
    for (i = pChnl->chnlWidth*pChnl->chnlHeight - 1; i >= 0; i--) pChnl->data[i] = defVal;
    }
  return true;
}

IMG_INT32 CImageBase::ConvertFrom(CImageBase *pSrc,CM_xxx newColorModel,SUBS_xxx newSubsMode,bool onlyCheckIfAvail) {
//convert pSrc into a different color model and store result in this object
//only very basic conversions are supported
//onlyCheckIfAvail: true=only check whether conversion is available from the current format, but don't actually do it
//returns: 0=ok,1=not supported,2=out of memory
  if (!onlyCheckIfAvail) Unload();
  if ((pSrc->colorModel == CM_GRAY) && (newColorModel == CM_RGGB)) {
    //re-interpret a bayer image stored as grayscale as a bayer image
    IMG_UINT8 flip;
    IMG_INT32 x,y,ch;
    IMG_INT8 newBd[4];
    PIXEL *pSrcPix,*pDstPix[4];
    if (onlyCheckIfAvail) return 0;
    switch (newSubsMode) {
      case SUBS_RGGB: flip = 0; break;
      case SUBS_GRBG: flip = 1; break;
      case SUBS_GBRG: flip = 2; break;
      case SUBS_BGGR: flip = 3; break;
      default: return 1;  //unsupported subsampling mode (bayer type)
      }
    newBd[0] = newBd[1] = newBd[2] = newBd[3] = pSrc->chnl[0].bitDepth*(pSrc->chnl[0].isSigned?-1:1);
    //allocate buffers
    if (!CreateNewImage(pSrc->width,pSrc->height,newColorModel,newSubsMode,newBd,NULL)) return 2;  //out of mem
    //convert
    pSrcPix=pSrc->chnl[0].data;
    for (ch=0;ch<4;ch++) pDstPix[ch]=chnl[ch].data;
    for (y=0;y<pSrc->height;y++) {
      for (x=0;x<pSrc->width;x++) {
        ch=(((y&1)<<1)|(x&1))^flip;
        *(pDstPix[ch])++=*pSrcPix++;
        }
      }
    }
  else return 1;  //not supported
  return 0;
}

void CImageBase::SwapWith(CImageBase *pOther) {
//swap contents of this image with another image
#define SWAPM(m)  swapbytes(&(m),&(pOther->m),sizeof(m))
  IMG_UINT32 ch;
  SWAPM(width);
  SWAPM(height);
  SWAPM(colorModel);
  SWAPM(subsMode);
  SWAPM(fileName);
  for (ch = 0; ch < SizeofArray(chnl); ch++) {
    SWAPM(chnl[ch].data);
    SWAPM(chnl[ch].chnlWidth);
    SWAPM(chnl[ch].chnlHeight);
    SWAPM(chnl[ch].bitDepth);
    SWAPM(chnl[ch].isSigned);
    }
#undef SWAPM
}

IMG_INT32 CImageBase::ChBitDepth(IMG_INT32 pixel,IMG_INT8 oldBitDepth,IMG_INT8 newBitDepth) {
//change bit depth of one pixel value
//oldBitDepth,newBitDepth: negative indicates signed value
//this function does not clip values
  IMG_INT shift;
  if ((shift = newBitDepth - oldBitDepth) == 0) return pixel;
  if (oldBitDepth > 0) {  //old unsigned
    if (newBitDepth < 0)  //new signed
      shift = (-newBitDepth - 1) - oldBitDepth;
    }
  else {  //old signed
    if (newBitDepth < 0)  //new signed
      shift = -shift;
    else shift = newBitDepth + oldBitDepth + 1;
    }
  if (shift < 0) return pixel >> (-shift);  //assuming sign extension if pSrc values are negative
  return pixel << shift;
}

void CImageBase::ChBitDepth(IMG_INT32 *pSrc,IMG_INT32 *pDst,IMG_INT32 Npixels,IMG_INT8 oldBitDepth,IMG_INT8 newBitDepth) {
//change bit depth of pixel value(s)
//oldBitDepth,newBitDepth: negative indicates signed value
//pSrc: array of source pixels
//pDst: destination array, can be = pSrc for in-place conversion
//this function does not clip values
  IMG_INT shift;
  if ((shift = newBitDepth - oldBitDepth) == 0) return;
  if (oldBitDepth > 0) {  //old unsigned
    if (newBitDepth < 0)  //new signed
      shift = (-newBitDepth - 1) - oldBitDepth;
    }
  else {  //old signed
    if (newBitDepth < 0)  //new signed
      shift = -shift;
    else shift = newBitDepth + oldBitDepth + 1;
    }
  if (shift < 0) {
    while (Npixels--) *pDst++ = (*pSrc++) >> (-shift);  //assuming sign extension if pSrc values are negative
    }
  else {
    while (Npixels--) *pDst++ = (*pSrc++) << shift;
    }
}

void CImageBase::ChangeChannelBitDepth(IMG_INT32 channel,IMG_INT8 newBitDepth) {
//change color bit depth of one or all channels of the image
//channel: channel index, -1 to apply to all channels
//newBitDepth: negative indicates signed values
  const COLORMODELINFO *cmi;
  IMG_INT32 channelSize,ch,i;
  IMG_INT8 oldBitDepth;
  COLCHANNEL *pChnl;
  PIXEL *p;
  if ((cmi = GetColorModelInfo(colorModel)) == NULL) { ASSERT(false);  return; }
  if (sizeof(PIXEL) > 2) {
    if ((newBitDepth < -32) || (newBitDepth > 31)) { ASSERT(false);  return; }
    }
  else {
    if ((newBitDepth < -16) || (newBitDepth > 15)) { ASSERT(false);  return; }
    }
  for (ch = ((channel < 0) ? 0 : channel); ch < cmi->Nchannels; ch++) {
    pChnl = chnl + ch;
    channelSize = pChnl->chnlWidth * pChnl->chnlHeight;
    p = pChnl->data;
    oldBitDepth = (chnl[ch].isSigned?(-chnl[ch].bitDepth):(chnl[ch].bitDepth));
    if (newBitDepth == oldBitDepth) continue;
    for (i = channelSize - 1; i >= 0; i--, p++)
      *p = (PIXEL)ChBitDepth(*p, oldBitDepth, newBitDepth);  //!!! should probably do clipping to valid range here
    pChnl->bitDepth = ((newBitDepth > 0)?newBitDepth:(-newBitDepth));
    pChnl->isSigned = ((newBitDepth > 0)?false:true);
    if (channel >= 0) break;  //only do the conversion for one channel
    }
}

void CImageBase::OffsetChannelValues(IMG_INT32 channel,IMG_INT32 addValue) {
//add a fixed value to all pixel values of one or all channels of the image
//channel: channel index, -1 to apply to all channels
  const COLORMODELINFO *cmi;
  IMG_INT32 channelSize,ch,i;
  COLCHANNEL *pChnl;
  PIXEL *p;
  if ((cmi = GetColorModelInfo(colorModel)) == NULL) { ASSERT(false);  return; }
  for (ch = ((channel < 0) ? 0 : channel); ch < cmi->Nchannels; ch++) {
    pChnl = chnl + ch;
    channelSize = pChnl->chnlWidth * pChnl->chnlHeight;
    p = pChnl->data;
    for (i = channelSize - 1; i >= 0; i--, p++) *p += (PIXEL)addValue;
    if (channel >= 0) break;  //only do the conversion for one channel
    }
}

IMG_UINT32 CImageBase::ClipChannelValues(IMG_INT32 channel,IMG_INT32 minVal,IMG_INT32 maxVal,bool countClipOnly) {
//clip all pixel values of one or all channels of the image to given or default range
//channel: channel index, -1 to apply to all channels
//minVal,maxVal: 0,0 to use default range (based on bit depth and signed/unsigned properties of channel)
//countClipOnly: false=apply clipping, true=don't apply clipping, only count number of pixels which would be clipped
//returns #of pixels out of the clipping range
  const COLORMODELINFO *cmi;
  IMG_INT32 channelSize,ch,i;
  IMG_UINT32 NoutOfRange;
  COLCHANNEL *pChnl;
  PIXEL *p;
  if ((cmi = GetColorModelInfo(colorModel)) == NULL) { ASSERT(false);  return 0; }
  NoutOfRange = 0;
  for (ch = ((channel < 0) ? 0 : channel); ch < cmi->Nchannels; ch++) {
    pChnl = chnl + ch;
    channelSize = pChnl->chnlWidth * pChnl->chnlHeight;
    p = pChnl->data;
    if ((minVal == 0) && (maxVal == 0)) {  //use default min & max
      if (chnl[ch].isSigned) {
        minVal = -(1 << (pChnl->bitDepth - 1));
        maxVal = +(1 << (pChnl->bitDepth - 1)) - 1;
        }
      else {
        minVal = 0;
        maxVal = +(1 << pChnl->bitDepth) - 1;
        }
      }
    for (i = channelSize - 1; i >= 0; i--, p++) {
      if (*p < minVal) {
        NoutOfRange++;
        if (!countClipOnly) *p = (PIXEL)minVal;
        }
      else if (*p > maxVal) {
        NoutOfRange++;
        if (!countClipOnly) *p = (PIXEL)maxVal;
        }
      }
    if (channel >= 0) break;  //only do the conversion for one channel
    }
  return NoutOfRange;
}

IMG_UINT32 CImageBase::ClipChannelValuesToBD(IMG_INT32 channel,bool countClipOnly) {
//clip channel values to the current range of the channel as given by bit depth and isSigned
//in a valid FLX file values should never be outside the bit depth in the header
//channel: channel index, -1 to apply to all channels
  IMG_INT32 ch;
  IMG_UINT32 cnt;
  ASSERT((channel>=-1) && (channel<GetNColChannels()));
  cnt=0;
  for (ch=(channel<0)?0:channel;ch<GetNColChannels();ch++) {
    if (chnl[ch].isSigned)
      cnt+=(IMG_UINT32)ClipChannelValues(ch,-(1l<<(chnl[ch].bitDepth-1)),+(1l<<(chnl[ch].bitDepth-1))-1,countClipOnly);
    else cnt+=(IMG_UINT32)ClipChannelValues(ch,0,(1l<<(chnl[ch].bitDepth))-1,countClipOnly);
    if (channel>=0) break;  //only one channel
    }
  return cnt;
}

/*-------------------------------------------------------------------------------
Felix file format*/

#define MAX_HDR_TABLE_SIZE   20  //max #of entries supported in FLX header table (each pair of entries=one section)

static const char *colorModelsEnum = "\x1\0""Grayscale\0""RGB\0""RGBA\0""RGGB\0""RGBW\0""YUV\0""YUVA\0\0";
static const CImageBase::CM_xxx colorModelsCodes[] = {
  CImageBase::CM_UNDEF,CImageBase::CM_GRAY,CImageBase::CM_RGB,CImageBase::CM_RGBA,CImageBase::CM_RGGB,CImageBase::CM_RGBW,CImageBase::CM_YUV,CImageBase::CM_YUVA
  };

CImageFlx::CImageFlx() {
  pSegInfo = NULL;
  Unload();
}

CImageFlx::~CImageFlx() {
  Unload();
}

IMG_UINT32 CImageFlx::GetNFilePlanes(FLXSAVEFORMAT *pFormat) {
//get number of file planes (based on plane format)
//pFormat: NULL=use meta data for the current image, current segment
//         !NULL=calculate for the specified format using current image or other image (if pFormat->pImData!=NULL)
  const COLORMODELINFO *cmi;
  IMG_UINT32 Nplanes;
  if (pFormat == NULL) {
    if ((Nplanes = GetMeta()->GetMetaValCount(FLXMETA_PLANE_FORMAT)) > 0) return Nplanes;
    //plane format is not specified -> default to planar
    if ((cmi = GetColorModelInfo(colorModel)) == NULL) return 0;
    return cmi->Nchannels;
    }
  else {
    CImageBase *pImData = ((pFormat->pImData == NULL)? this: pFormat->pImData);
    IMG_INT32 Nchnl;
    if ((cmi = GetColorModelInfo(pImData->colorModel)) == NULL) return 0;
    for (Nchnl = Nplanes = 0; Nchnl < cmi->Nchannels; Nplanes++) Nchnl += pFormat->planeFormat[Nplanes];
    return Nplanes;
    }
}

IMG_INT32 CImageFlx::GetNChannelsInPlane(IMG_INT32 plane,FLXSAVEFORMAT *pFormat) {
//get number of color channels interleaved in given file plane
//pFormat: NULL=use meta data for the current image, current segment
//         !NULL=calculate for the specified format using current image or other image (if pFormat->pImData!=NULL)
  if (pFormat != NULL) return pFormat->planeFormat[plane];
  if (GetMeta()->IsDefined(FLXMETA_PLANE_FORMAT)) return GetMeta()->GetMetaInt(FLXMETA_PLANE_FORMAT,1,plane,true);
  //plane format is not specified -> default to planar
  return 1;
}

IMG_INT32 CImageFlx::GetPlaneBaseChannel(IMG_INT32 plane,FLXSAVEFORMAT *pFormat) {
//get color channel number of the first channel stored in given file plane
//pFormat: NULL=use meta data for the current image, current segment
//         !NULL=calculate for the specified format using current image or other image (if pFormat->pImData!=NULL)
  IMG_INT32 ch,i;
  for (i = ch = 0; i < plane; i++) ch += GetNChannelsInPlane(i,pFormat);
  return ch;
}

CImageBase::SUBS_xxx CImageFlx::GetSubsampling(CMetaData *pMD) {
  if (pMD == NULL) pMD = GetMeta();
  return DetectFlxSubsampling(this,pMD);
}

/*static*/ CImageBase::SUBS_xxx CImageFlx::DetectFlxSubsampling(CImageBase *pImData,CMetaData *pMD) {
//detect subsampling mode for the specified image based on specified FLX type meta data
//recognize common combinations of subsampling and phase offset
  if (pMD == NULL) { ASSERT(false); return SUBS_UNDEF; }
  if (pMD->IsDefined(FLXMETA_SUBS_H) || pMD->IsDefined(FLXMETA_SUBS_V)) {
    IMG_UINT32 sh,sv,ph,pv;  //4 bits per channel LSBit->MSBit
    IMG_INT32 ch;
    if ((ch = pImData->GetNColChannels()) > 8) return SUBS_UNDEF;  //only up to 8 channels * 4 bits can fit into sh,sv,ph,pv
    sh = sv = ph = pv = 0;
    while ((--ch) >= 0) {
      sh |= (pMD->GetMetaInt(FLXMETA_SUBS_H,1,ch,true) & 0xf) << (ch*4);
      sv |= (pMD->GetMetaInt(FLXMETA_SUBS_V,1,ch,true) & 0xf) << (ch*4);
      ph |= ((IMG_INT)(pMD->GetMetaDouble(FLXMETA_PHASE_OFF_H,0,ch,true)*2+0.5) & 0xf) << (ch*4);  //in half-pixel units
      pv |= ((IMG_INT)(pMD->GetMetaDouble(FLXMETA_PHASE_OFF_V,0,ch,true)*2+0.5) & 0xf) << (ch*4);  //in half-pixel units
      }
    if ((pImData->colorModel == CM_RGGB) && (sh == 0x2222) && (sv == 0x2222)) {
      if ((ph == 0x2020) && (pv == 0x2200)) return SUBS_RGGB;
      if ((ph == 0x0202) && (pv == 0x2200)) return SUBS_GRBG;
      if ((ph == 0x2020) && (pv == 0x0022)) return SUBS_GBRG;
      if ((ph == 0x0202) && (pv == 0x0022)) return SUBS_BGGR;
      }
    else if ((pImData->colorModel == CM_YUV) || (pImData->colorModel == CM_YUVA)) {
      if ((sh == 0x111) && (sv == 0x111) && (ph == 0x000) && (pv == 0x000)) return SUBS_444;
      if ((sh == 0x221) && (sv == 0x111) && (ph == 0x000) && (pv == 0x000)) return SUBS_422;  //4:2:2 co-sited
      if ((sh == 0x221) && (sv == 0x111) && (ph == 0x110) && (pv == 0x000)) return SUBS_422;  //4:2:2 horizontally interstitial
      if ((sh == 0x221) && (sv == 0x221) && (ph == 0x000) && (pv == 0x000)) return SUBS_420;  //4:2:0 horizontally and vertically co-sited
      if ((sh == 0x221) && (sv == 0x221) && (ph == 0x000) && (pv == 0x110)) return SUBS_420;  //4:2:0 horizontally co-sited, vertically interstitial
      if ((sh == 0x221) && (sv == 0x221) && (ph == 0x110) && (pv == 0x110)) return SUBS_420;  //4:2:0 horizontally and vertically interstitial
      }
    }
#if FLXMETA_SUPPORT_DEPRECATED
  else {
    const char *oldSubs;
    if ((oldSubs = pMD->GetMetaStr(FLXMETA_SUBSAMPLING,NULL)) == NULL) return SUBS_UNDEF;
    if (strcmp(oldSubs,"RGGB") == 0) return SUBS_RGGB;
    if (strcmp(oldSubs,"GRBG") == 0) return SUBS_GRBG;
    if (strcmp(oldSubs,"GBRG") == 0) return SUBS_GBRG;
    if (strcmp(oldSubs,"BGGR") == 0) return SUBS_BGGR;
    if (strcmp(oldSubs,"444") == 0) return SUBS_444;
    if (strcmp(oldSubs,"422") == 0) return SUBS_422;
    if (strcmp(oldSubs,"420") == 0) return SUBS_420;
    }
#endif
  return SUBS_UNDEF;
}

IMG_INT32 CImageFlx::GetLineSize(IMG_INT32 plane,FLXSAVEFORMAT *pFormat) {
//calculates size in bytes of one line of pixels in file in the given plane
//plane: 0-based index
//pFormat: NULL=use meta data for the current image, current segment
//         !NULL=calculate for the specified format using current image or other image (if pFormat->pImData!=NULL)
  const COLORMODELINFO *cmi;
  CImageBase *pImData;
  IMG_INT32 size,NchannelsInPlane,planeBaseChnl,lineAlignSize,xSampling,NsamplesHoriz,i;
  PACKM_xxx aPixelFormat;
  /**/
  pImData = ((pFormat == NULL) || (pFormat->pImData == NULL))? this: pFormat->pImData;
  if ((cmi = GetColorModelInfo(pImData->colorModel)) == NULL) return 0;
  NchannelsInPlane = GetNChannelsInPlane(plane,pFormat);  //#of color channels interleaved in this plane
  planeBaseChnl = GetPlaneBaseChannel(plane,pFormat);  //index of lowest color channel stored in this plane
  //sanity checking
  if (planeBaseChnl + NchannelsInPlane > cmi->Nchannels) return 0;  //invalid image meta data
  xSampling = pImData->GetXSampling(planeBaseChnl);  //horizontal subsampling factor for the 1st channel in this plane
  for (i = 1; i < NchannelsInPlane; i++) {
    if (pImData->GetXSampling(planeBaseChnl + i) != xSampling) return 0;  //trying to store differently sampled channels in the same plane
    }
  //if image width or height is not a multiple of subsampling step then assume padding to nearest multiple with 0s
  NsamplesHoriz = RoundUpDiv(pImData->width,xSampling);
  aPixelFormat = ((pFormat == NULL) ? pixelFormat : pFormat->pixelFormat);
  switch (aPixelFormat) {
    case PACKM_UNPACKEDR:
    case PACKM_UNPACKEDL: {
      IMG_INT32 groupSize;
      groupSize = 0;  //#of bytes required to store one value for all channels in this plane
      for (i=0;i<NchannelsInPlane;i++) {
        if (pImData->chnl[planeBaseChnl + i].bitDepth > 16) groupSize += 4;
        else if (pImData->chnl[planeBaseChnl + i].bitDepth > 8) groupSize += 2;
        else groupSize++;
        }
      size = groupSize * NsamplesHoriz;
      } break;
    case PACKM_GROUPPACK: {
      //For group-packed formats the calculation is based on the pattern of repeating color channels stored in unique groups.
      //A pattern is a sequence of color components packed in one or more groups until a group starts again with the first color channel.
      IMG_INT32 groupPacking,patternLength,NvaluesHoriz;
      IMG_INT32 patternSize,lastPatternSize;  //size of one full repeating pattern and the last (incomplete) pattern in line [bits]
      groupPacking = ((pFormat == NULL) ? pixelFormatParam[plane] : pFormat->pixelFormatParam[plane]);  //#of color component values stored in one bit-packed group
      if (groupPacking < 1) { ASSERT(false); groupPacking = 1; }
      patternLength = (NchannelsInPlane*groupPacking)/CalcGCD(NchannelsInPlane,groupPacking);  //[#of color channel values]
      patternSize = lastPatternSize = 0;  //size of pattern in bits required for storage
      NvaluesHoriz = NsamplesHoriz * NchannelsInPlane;  //#of color component values per line
      for (i = 0; i < patternLength; i++) {  //loop through color components in one entire repeating pattern
        //add #of bits in next color component value to total size of the pattern
        patternSize += pImData->chnl[planeBaseChnl + (i % NchannelsInPlane)].bitDepth;
        if ((i % groupPacking) == groupPacking - 1) {  //end of packing group
          patternSize = RoundUpToMultiple(patternSize,8);  //alignment bits at the end of group
          //remember size of incomplete pattern (at the end of line)
          //a line is always padded with color channel values '0' to complete a packing group
          if (i == RoundUpToMultiple(NvaluesHoriz % patternLength, groupPacking) - 1)
            lastPatternSize = patternSize;
          }
        }
      ASSERT((patternSize % 8) == 0);  //sanity check - patternLength is always a multiple of group size, i.e. pattern is byte-aligned
      size = (NvaluesHoriz / patternLength) * (patternSize / 8);
      if (NvaluesHoriz % patternLength) size += lastPatternSize / 8;
      } break;
    default: ASSERT(false);  return 0;
    }
  //apply line alignment
  lineAlignSize = ((pFormat == NULL) ? GetMeta()->GetMetaInt(FLXMETA_LINE_ALIGN,1,plane,true) : pFormat->lineAlign);
  if (lineAlignSize > 1) size = RoundUpToMultiple(size,lineAlignSize);
  return size;
}

IMG_INT32 CImageFlx::GetFrameSize(FLXSAVEFORMAT *pFormat) {
//calculate frame size based on the current meta data or optional pFormat override
//the size is of one image (frame) in bytes
//pFormat: NULL=use meta data for the current image, current segment
//         !NULL=calculate for the specified format using current image or other image (if pFormat->pImData!=NULL)
  IMG_INT32 Nplanes,frameSize,ch,i;
  CImageBase *pImData;
  pImData = (((pFormat == NULL) || (pFormat->pImData == NULL))? this: pFormat->pImData);
  Nplanes = GetNFilePlanes(pFormat);
  if (Nplanes < 1) return 0;
  frameSize = ch = 0;
  for (i = 0; i < Nplanes; i++) {
    frameSize += GetLineSize(i,pFormat) * pImData->chnl[ch].chnlHeight;
    ch += GetNChannelsInPlane(i,pFormat);
    }
  return frameSize;
}

/*static*/ CImageFlx::FLXSEGMENTINFO *CImageFlx::NewSegment() {
//allocate a new blank segment structure
  FLXSEGMENTINFO *pSeg;
  pSeg = new FLXSEGMENTINFO;
  if (pSeg == NULL) { return NULL; }
  pSeg->frameOffsets = NULL;
  pSeg->NframeOffsets = 0;
  pSeg->Nframes = 0;
  pSeg->segmentOffset = 0;
  pSeg->segmentSize = 0;
  pSeg->next = NULL;
  return pSeg;
}

void CImageFlx::AddSegment(FLXSEGMENTINFO *pSeg) {
//add segment structure to the end of list of segments
  if (pSegInfo == NULL) pSegInfo = pSeg;
  else {
    FLXSEGMENTINFO *pLast;
    for (pLast = pSegInfo; pLast->next != NULL; pLast = pLast->next) ;
    pLast->next = pSeg;
    }
  pSeg->next = NULL;
}

CImageFlx::FLXSEGMENTINFO *CImageFlx::GetSegment(IMG_INT32 index,bool isFrameIndex) {
//get segment identified by index
//isFrameIndex: false=index is 0-based segment index, true=index is 0-based global frame index
  FLXSEGMENTINFO *pSeg;
  if (index < 0) return NULL;
  if (isFrameIndex) {
    for (pSeg = pSegInfo; (pSeg != NULL) && (index >= pSeg->Nframes) ; pSeg = pSeg->next) index -= pSeg->Nframes;
    }
  else {
    for (pSeg = pSegInfo; (pSeg != NULL) && (index > 0); pSeg = pSeg->next) index--;
    }
  return pSeg;
}

IMG_INT32 CImageFlx::GetSegmentIndex(IMG_INT32 frameIndex,IMG_INT32 *pFrameWithinSeg,FLXSEGMENTINFO **ppSeg) {
//frameIndex: 0-based global frame index
//pFrameWithinSeg: if !NULL will be set to 0-based index of the requested frame within its segment
//returns index of segment inside which a given frame is stored; -1 if invalid frameIndex
  FLXSEGMENTINFO *pSeg;
  IMG_INT32 segIndex;
  if (frameIndex < 0) return -1;
  for (segIndex = 0, pSeg = pSegInfo; (pSeg != NULL) && (frameIndex >= pSeg->Nframes); pSeg = pSeg->next) {
    frameIndex -= pSeg->Nframes;
    segIndex++;
    }
  if (pFrameWithinSeg != NULL) (*pFrameWithinSeg) = frameIndex;
  if (ppSeg != NULL) (*ppSeg) = pSeg;
  return (pSeg == NULL)? -1: segIndex;
}

CMetaData *CImageFlx::GetMetaForFrame(IMG_INT32 frameIndex) {
//get meta data of segment containing specified frame
//frameIndex: 0-based global frame number
  FLXSEGMENTINFO *pSeg;
  if (frameIndex < 0) return NULL;
  for (pSeg = pSegInfo; (frameIndex >= pSeg->Nframes) && (pSeg != NULL); pSeg = pSeg->next) frameIndex -= pSeg->Nframes;
  return (pSeg == NULL)? NULL: &(pSeg->meta);
}

const char *CImageFlx::ApplyMetaForFrame(IMG_INT32 frameIndex,bool force) {
//apply meta data for the specified frame (i.e. set the object members accordingly and set current segment & meta data pointer)
//frameIndex: 0-based global frame number
//force: true=always, false=only apply meta data if the current segment is different than segment for specified frame
//returns NULL=ok,!NULL=error message
  FLXSEGMENTINFO *pSeg;
  if ((pSeg = GetSegment(frameIndex,true)) == NULL) return "Invalid frame number";
  if ((pCurrSeg == pSeg) && !force) return NULL;
  return ApplyMetaData(pSeg);	//will set pCurrSeg=pSeg
}

const char *CImageFlx::ApplyMetaData(FLXSEGMENTINFO *pSeg) {
//read the given segment's meta data structure and set this object's members related to file format accordingly
//if the applied meta data changes requirements for channel buffers, the existing buffers will be deallocated
//pSeg becomes the current segment (also in case of error, in which case it may be partially loaded)
//returns: NULL=ok, !0=error message string
  const COLORMODELINFO *cmi,*cmiOld;
  CMetaData *pMD;
  const char *c;
  IMG_INT32 ch;
  bool deallocBuffers;
  int n;
  pCurrSeg = pSeg;
  pMD = &(pSeg->meta);
  deallocBuffers = false;
  //image dimensions [pixels]
  width = pMD->GetMetaInt(FLXMETA_WIDTH,0);
  height = pMD->GetMetaInt(FLXMETA_HEIGHT,0);
  //color format
  cmiOld = GetColorModelInfo(colorModel);
  if (pMD->IsDefined(FLXMETA_COLOR_FORMAT)) {
    colorModel = colorModelsCodes[pMD->GetMetaEnum(FLXMETA_COLOR_FORMAT,CM_UNDEF,colorModelsEnum)];
#if FLXMETA_SUPPORT_DEPRECATED
    if ((colorModel == CM_UNDEF) && (strcmp(pMD->GetMetaStr(FLXMETA_COLOR_FORMAT,""),"Bayer") == 0))
      colorModel = CM_RGGB;
#endif
    }
  else colorModel = CM_RGB;
  if ((cmi = GetColorModelInfo(colorModel)) == NULL) return "Invalid colour format";
  if (((cmi != NULL) && (cmiOld != NULL) && (cmi->Nchannels != cmiOld->Nchannels)) || (cmiOld == NULL))
    deallocBuffers = true;
  //subsampling
  //NOTE: if FLXMETA_SUBS_H/_V is given, subMode may still end up SUBS_UNDEF if pattern is not recognized
  if (pMD->IsDefined(FLXMETA_SUBS_H) || pMD->IsDefined(FLXMETA_SUBS_V)) subsMode = GetSubsampling();
#if FLXMETA_SUPPORT_DEPRECATED
  else if (pMD->IsDefined(FLXMETA_SUBSAMPLING)) subsMode = GetSubsampling();
#endif
  else {  //set default if not specified
    if ((colorModel == CM_YUV) || (colorModel == CM_YUVA)) subsMode = SUBS_444;
    else if (colorModel == CM_RGGB) subsMode = SUBS_RGGB;
    }
  //packing mode
  c = pMD->GetMetaStr(FLXMETA_PIXEL_FORMAT);
  if (c == NULL) pixelFormat = PACKM_UNPACKEDR;  //default
  else if (stricmp(c,"unpackedR") == 0) pixelFormat = PACKM_UNPACKEDR;
  else if (stricmp(c,"unpackedL") == 0) pixelFormat = PACKM_UNPACKEDL;
  else if (strnicmp(c,"grouppacked ",12) == 0) {
    IMG_UINT filePlane,tmp;
    c += 12;
    pixelFormat = PACKM_GROUPPACK;
    for (filePlane = 0; filePlane < SizeofArray(pixelFormatParam); filePlane++) {
      n=-1;  sscanf(c,"%d%n",&tmp,&n);
      if (((filePlane == 0) && (n < 1)) || (tmp < 1)) return "Invalid group packing value";
      if (n < 1) break;
      pixelFormatParam[filePlane] = tmp;
      c += n;
    }
    for (; filePlane < SizeofArray(pixelFormatParam); filePlane++)  //replicate the last parameter to fill the array
      pixelFormatParam[filePlane] = pixelFormatParam[filePlane - 1];
    }
  else return "Invalid pixel format";
  //color channel information
  for (ch = 0; ch < cmi->Nchannels; ch++) {
    IMG_INT32 sampling,oldW,oldH;
    oldW = chnl[ch].chnlWidth;
    oldH = chnl[ch].chnlHeight;
    chnl[ch].bitDepth = (IMG_INT8)pMD->GetMetaInt(FLXMETA_BITDEPTH,8,ch,true);
    chnl[ch].isSigned = (pMD->GetMetaInt(FLXMETA_SIGNED,0,ch,true) ? true : false);
    sampling = GetXSampling(ch);
    chnl[ch].chnlWidth = RoundUpDiv(width,sampling);
    sampling = GetYSampling(ch);
    chnl[ch].chnlHeight = RoundUpDiv(height,sampling);
    if ((oldW != chnl[ch].chnlWidth) || (oldH != chnl[ch].chnlHeight))
      deallocBuffers = true;
    }
  //deallocate all color channel buffers if the existing ones do not match the new memory requirement
  if (deallocBuffers) UnloadData();
  return NULL;
}

void CImageFlx::UpgradeMetaData(CMetaData *pMD) {
//replace any deprecated items with newer equivalents
//don't worry about items which are explicitly constructed during saving process
//NOTE: some items may need to be filled using current image properties (such as channel ranges), so the appropriate
//      segment may need to be selected first to get meta data appropriate for that segment.
//returns: NULL=ok, !0=error message string
#if FLXMETA_SUPPORT_DEPRECATED
  CMetaData::MDITEM *mdi;
  //if ISO_GAIN is present, set SENSOR_BASEISO and CAPTURE_ISO
  if (pMD->IsDefined(FLXMETA_ISO_GAIN)) {
    double gain;
    if ((gain=pMD->GetMetaInt(FLXMETA_ISO_GAIN,0))>0.01) {
      pMD->UpdateInt(FLXMETA_SENSOR_BASEISO,100,CMetaData::IFEX_FAIL);  //set only if it doesn't exist
      pMD->UpdateInt(FLXMETA_CAPT_ISO,(IMG_INT32)(pMD->GetMetaInt(FLXMETA_SENSOR_BASEISO,100)*gain),CMetaData::IFEX_FAIL);
      }
    }
  //if SENSOR_DENSITY and SENSOR_PIXEL_SIZE is present, calculate SENSOR_WELL_DEPTH
  if (pMD->IsDefined(FLXMETA_SENSOR_DENSITY) && pMD->IsDefined(FLXMETA_SENSOR_PIXSIZE)) {
    double welldepth;
    welldepth=pMD->GetMetaDouble(FLXMETA_SENSOR_PIXSIZE,1)-pMD->GetMetaDouble(FLXMETA_SENSOR_PIXDEAD,0);
    welldepth*=pMD->GetMetaDouble(FLXMETA_SENSOR_DENSITY,0);
    if (welldepth>1) pMD->UpdateDouble(FLXMETA_SENSOR_WELLD,welldepth,CMetaData::IFEX_FAIL);  //set only if it doesn't exist
    }
  //if GAMMA_BLACK_PT and GAMMA_WHITE_PT are present, set CHANNEL_DISPLAY_MIN/MAX
  if ((pMD->IsDefined(FLXMETA_GAMMA_BLACK_PT) || pMD->IsDefined(FLXMETA_GAMMA_WHITE_PT))
      && (!pMD->IsDefined(FLXMETA_CH_DISP_MIN) || !pMD->IsDefined(FLXMETA_CH_DISP_MAX))) {
    IMG_INT32 blackLevel,whiteLevel,gammaCh,ch;
    double val,gammaChGain;
    //figure out the gamma channel = channel to which the gamma white/black point apply
    if ((colorModel==CM_RGB) || (colorModel==CM_RGBA) || (colorModel==CM_RGGB) || (colorModel==CM_RGBW))
      gammaCh=1;
    else gammaCh=0;
    blackLevel=pMD->GetMetaInt(FLXMETA_GAMMA_BLACK_PT,GetChMin(gammaCh));
    whiteLevel=pMD->GetMetaInt(FLXMETA_GAMMA_WHITE_PT,GetChMax(gammaCh));
    gammaChGain=pMD->GetMetaDouble(FLXMETA_CHANNEL_GAIN,1,gammaCh);
    if (gammaChGain<0.001) gammaChGain=1;
    if (!pMD->IsDefined(FLXMETA_CH_DISP_MIN) && (blackLevel || whiteLevel)) {
      for (ch=0;ch<GetNColChannels();ch++) {
        val=blackLevel;  //*pMD->GetMetaDouble(FLXMETA_CHANNEL_GAIN,1,ch)/gammaChGain;
        if (chnl[ch].bitDepth>chnl[gammaCh].bitDepth) val*=(1<<(chnl[ch].bitDepth-chnl[gammaCh].bitDepth));
        else val/=(1<<(chnl[gammaCh].bitDepth-chnl[ch].bitDepth));
        pMD->UpdateInt(FLXMETA_CH_DISP_MIN,(IMG_INT32)val,CMetaData::IFEX_APPEND);
        }
      }
    if (!pMD->IsDefined(FLXMETA_CH_DISP_MAX) && (blackLevel || whiteLevel)) {
      for (ch=0;ch<GetNColChannels();ch++) {
        val=whiteLevel;  //*pMD->GetMetaDouble(FLXMETA_CHANNEL_GAIN,1,ch)/gammaChGain;
        if (chnl[ch].bitDepth>chnl[gammaCh].bitDepth) val*=(1<<(chnl[ch].bitDepth-chnl[gammaCh].bitDepth));
        else val/=(1<<(chnl[gammaCh].bitDepth-chnl[ch].bitDepth));
        pMD->UpdateInt(FLXMETA_CH_DISP_MAX,(IMG_INT32)val,CMetaData::IFEX_APPEND);
        }
      }
    }
  //HOR/VER_PHASE_OFFSET -> PHASE_OFFSET_HOR/VER
  if ((mdi=pMD->Find("HOR_PHASE_OFFSET"))!=NULL) {
    if (!pMD->IsDefined(FLXMETA_PHASE_OFF_H)) pMD->Rename("HOR_PHASE_OFFSET",FLXMETA_PHASE_OFF_H);
    else pMD->Del(mdi);
    }
  if ((mdi=pMD->Find("VER_PHASE_OFFSET"))!=NULL) {
    if (!pMD->IsDefined(FLXMETA_PHASE_OFF_V)) pMD->Rename("VER_PHASE_OFFSET",FLXMETA_PHASE_OFF_V);
    else pMD->Del(mdi);
    }
  //[CHANNEL_]NOMINAL_RANGE -> FLXMETA_CH_FULL_MIN/MAX
  pMD->Rename("NOMINAL_RANGE",FLXMETA_NOMINAL_RANGE);
  if (pMD->IsDefined(FLXMETA_NOMINAL_RANGE)) {
    IMG_INT ch,Nvals,tmp;
    Nvals=pMD->GetMetaValCount(FLXMETA_NOMINAL_RANGE);  //floats <-1,1> as fraction of bit depth
    if (!pMD->IsDefined(FLXMETA_CH_FULL_MIN)) {
      for (ch=0;ch<Nvals/2;ch++) {
        tmp=(IMG_INT)(pMD->GetMetaDouble(FLXMETA_NOMINAL_RANGE,0,ch*2,true)*(1<<chnl[ch].bitDepth));
        pMD->UpdateInt(FLXMETA_CH_FULL_MIN,tmp,CMetaData::IFEX_APPEND);
        }
      }
    if (!pMD->IsDefined(FLXMETA_CH_FULL_MAX)) {
      for (ch=0;ch<(Nvals+1)/2;ch++) {
        tmp=(IMG_INT)(pMD->GetMetaDouble(FLXMETA_NOMINAL_RANGE,0,ch*2+1,true)*(1<<chnl[ch].bitDepth));
        pMD->UpdateInt(FLXMETA_CH_FULL_MAX,tmp,CMetaData::IFEX_APPEND);
        }
      }
    pMD->Del(pMD->Find(FLXMETA_NOMINAL_RANGE));
    }
#endif
}

bool CImageFlx::CheckValidMetaPlaneFormat(FLXSAVEFORMAT *pFormat) {
//check validity of the plane format in given meta data (or current segment's if pMD==NULL) based on current image
//pFormat: NULL=use meta data for the current image, current segment
//         !NULL=use specified format, meta data and current image or other image (if pFormat->pImData!=NULL)
  const COLORMODELINFO *cmi;
  IMG_INT totalChnls,planeIdx,nch,i;
  CImageBase *pImData;
  CMetaData *pMD;
  pImData = (((pFormat == NULL) || (pFormat->pImData == NULL))? this: pFormat->pImData);
  pMD = ((pFormat == NULL) || (pFormat->pMD == NULL))? GetMeta(): pFormat->pMD;
  if ((pFormat == NULL) && !pMD->IsDefined(FLXMETA_PLANE_FORMAT)) return false;
  if ((cmi = GetColorModelInfo(pImData->colorModel)) == NULL) return false;
  for (totalChnls = planeIdx = 0; totalChnls < cmi->Nchannels;) {
    if (pFormat == NULL) nch = pMD->GetMetaInt(FLXMETA_PLANE_FORMAT,0,planeIdx);
    else nch = pFormat->planeFormat[planeIdx];
    if ((nch < 1) || (totalChnls + nch > cmi->Nchannels)) return false;
    //check that all channels in this plane have the same size
    for (i = 1; i < nch; i++) {
      if (pImData->chnl[totalChnls].chnlWidth != pImData->chnl[totalChnls + i].chnlWidth) return false;
      if (pImData->chnl[totalChnls].chnlHeight != pImData->chnl[totalChnls + i].chnlHeight) return false;
      }
    totalChnls += nch;
    planeIdx++;
    }
  return true;
}

void CImageFlx::RebuildBaseMetaData(FLXSAVEFORMAT *pFormat,CMetaData *pMD) {
//[re-]create values of meta data items which are important for describing format of data
//updates meta data object given by whichever is first not NULL in this order: pMD, pFormat->pMD, GetMeta()
//pFormat: NULL=use the current image, current segment
//         !NULL=use specified format and image or other image (if pFormat->pImData!=NULL)
//pMD: if !NULL then has priority over pFormat->pMD
  const COLORMODELINFO *cmi;
  const char *enumName;
  char buf[100],*c;
  CImageBase *pImData;
  IMG_UINT32 NfilePlanes = 0, i = 0;
  /**/
  pImData = (((pFormat == NULL) || (pFormat->pImData == NULL))? this: pFormat->pImData);
  if (pMD == NULL) {
    pMD = ((pFormat == NULL) || (pFormat->pMD == NULL))? GetMeta(): pFormat->pMD;
    if (pMD == NULL) { ASSERT(false); return; }
    }
  buf[sizeof(buf)-1] = '\0';
  if ((cmi = GetColorModelInfo(pImData->colorModel)) == NULL) return;  //invalid color model
  pMD->UpdateInt(FLXMETA_WIDTH,pImData->width);
  pMD->UpdateInt(FLXMETA_HEIGHT,pImData->height);
  //color model
  for (i = 0, enumName = colorModelsEnum; i < SizeofArray(colorModelsCodes); i++, enumName += strlen(enumName) + 1) {
    if (colorModelsCodes[i] == pImData->colorModel) break;
    }
  pMD->UpdateStr(FLXMETA_COLOR_FORMAT,enumName);
  //subsampling
  //if current subsampling mode is set then construct meta data items, otherwise just use the current ones (preserve them)
  {const char *subH,*subV,*phaH,*phaV;
  if (!cmi->isSubsSupport) subH = subV = phaH = phaV = NULL;
  else {
    subH = pMD->GetMetaStr(FLXMETA_SUBS_H,NULL);
    subV = pMD->GetMetaStr(FLXMETA_SUBS_V,NULL);
    phaH = pMD->GetMetaStr(FLXMETA_PHASE_OFF_H,NULL);
    phaV = pMD->GetMetaStr(FLXMETA_PHASE_OFF_V,NULL);
    }
  if (pImData->subsMode != SUBS_UNDEF) {
    ASSERT(cmi->isSubsSupport);
    switch (pImData->colorModel) {
      case CM_RGGB:
        subH = subV = "2 2 2 2";
        switch (pImData->subsMode) {
          case SUBS_RGGB: phaH = "0 1 0 1";  phaV = "0 0 1 1"; break;
          case SUBS_GRBG: phaH = "1 0 1 0";  phaV = "0 0 1 1"; break;
          case SUBS_GBRG: phaH = "0 1 0 1";  phaV = "1 1 0 0"; break;
          case SUBS_BGGR: phaH = "1 0 1 0";  phaV = "1 1 0 0"; break;
          default: break; // do nothing
          }
        break;
      case CM_YUV:
        switch (pImData->subsMode) {
          case SUBS_444: subH = "1 1 1";  subV = "1 1 1"; break;
          case SUBS_422: subH = "1 2 2";  subV = "1 1 1"; break;
          case SUBS_420: subH = "1 2 2";  subV = "1 2 2"; break;
          default: break; // do nothing
          }
        break;
      default: break; // do nothing
      }//switch
    }
  if (subH != NULL) pMD->UpdateStr(FLXMETA_SUBS_H,subH);
  if (subV != NULL) pMD->UpdateStr(FLXMETA_SUBS_V,subV);
  if (phaH != NULL) pMD->UpdateStr(FLXMETA_PHASE_OFF_H,phaH);
  if (phaV != NULL) pMD->UpdateStr(FLXMETA_PHASE_OFF_V,phaV);
  }
  //color channel bit depths
  c = buf;
  for (i = 0; i < cmi->Nchannels; i++) { _snprintf(c,sizeof(buf)-(c-buf)-1," %d",(IMG_INT)(pImData->chnl[i].bitDepth));  c+=strlen(c); }
  pMD->UpdateStr(FLXMETA_BITDEPTH,buf + 1);  //+1 because buf starts with space
  //color channel signed/unsigned
  c = buf;
  for (i = 0; i < cmi->Nchannels; i++) { _snprintf(c,sizeof(buf)-(c-buf)-1," %d",(IMG_INT)(pImData->chnl[i].isSigned?1:0));  c+=strlen(c); }
  pMD->UpdateStr(FLXMETA_SIGNED,buf + 1);  //+1 because buf starts with space
  //plane format
  if (pFormat != NULL) {
    NfilePlanes = GetNFilePlanes(pFormat);
    c = buf;
    for (i = 0; i < NfilePlanes; i++) { sprintf(c," %d",(IMG_INT)GetNChannelsInPlane(i,pFormat));  c += strlen(c); }
    pMD->UpdateStr(FLXMETA_PLANE_FORMAT,buf + 1);  //+1 because buf starts with space
    }
  else if (!pMD->IsDefined(FLXMETA_PLANE_FORMAT) || !CheckValidMetaPlaneFormat(pFormat/*=NULL*/)) {
    //the current one is not valid - replace it with planar
    NfilePlanes = cmi->Nchannels;
    c = buf;
    for (i = 0; i < NfilePlanes; i++) { *c++ = ' '; *c++ = '1'; }
    pMD->UpdateStr(FLXMETA_PLANE_FORMAT,buf + 1);  //+1 because buf starts with space
    }
  //pixel format
  {CImageFlx::PACKM_xxx fmt;
  IMG_INT32 fmtParam[MAX_COLOR_CHANNELS];
  if (pFormat == NULL) {
    fmt = pixelFormat;
    for (i = 0; i < SizeofArray(fmtParam); i++) fmtParam[i] = pixelFormatParam[i];
    }
  else {
    fmt = pFormat->pixelFormat;
    for (i = 0; i < SizeofArray(fmtParam); i++) fmtParam[i] = pFormat->pixelFormatParam[i];
    }
  switch (fmt) { 
    case PACKM_UNPACKEDR: pMD->UpdateStr(FLXMETA_PIXEL_FORMAT,"unpackedR"); break;
    case PACKM_UNPACKEDL: pMD->UpdateStr(FLXMETA_PIXEL_FORMAT,"unpackedL"); break;
    case PACKM_GROUPPACK: {
      sprintf(buf,"grouppacked");  c = buf + strlen(buf);
      for (i = 0; i < NfilePlanes; i++) { sprintf(c," %d",(IMG_INT)(fmtParam[i]%100));  c += strlen(c); }  //% to prevent buf overflow
      pMD->UpdateStr(FLXMETA_PIXEL_FORMAT,buf);
      } break;
    default: ASSERT(false);
    }
  }
  //frame size in bytes
  pMD->UpdateInt(FLXMETA_FRAME_SIZE,GetFrameSize(pFormat));
  //line alignment
  if ((pFormat != NULL) && (pFormat->lineAlign > 1)) pMD->UpdateInt(FLXMETA_LINE_ALIGN,pFormat->lineAlign);
  else pMD->Del(pMD->Find(FLXMETA_LINE_ALIGN));
}

IMG_INT32 CImageFlx::ConvertFrom(CImageFlx *pSrc,CM_xxx newColorModel,SUBS_xxx newSubsMode,bool onlyCheckIfAvail) {
//converts image data from pSrc (currently loaded frame) to the new color model and subsampling and store it in this object
//to copy meta data appropriately, a 'current' segment must be set in pSrc and in this object; if this object does not have
//any segments then a new segment will be created.
//Any frame offset information in the target segment will likely no longer be valid.
//onlyCheckIfAvail: true=only check whether conversion is available from the current format, but don't actually do it
//returns: 0=ok,1=not supported,2=out of memory
  CMetaData *pMD;
  IMG_INT32 ret;
  //run base class conversion
  ret = CImageBase::ConvertFrom(pSrc,newColorModel,newSubsMode,onlyCheckIfAvail);
  if (ret || onlyCheckIfAvail) return ret;
  //create a segment if we need one - after ConvertFrom() because it does Unload()
  if (pSegInfo == NULL) { if ((pSegInfo = NewSegment()) == NULL) return 2; }
  if (pCurrSeg == NULL) pCurrSeg = pSegInfo;
  //copy the additional data not in base class
  pixelFormat = pSrc->pixelFormat;
  memcpy(pixelFormatParam,pSrc->pixelFormatParam,sizeof(pixelFormatParam));
  //copy meta data
  pMD = GetMeta();  //(for current segment)
  pMD->CopyFrom(pSrc->GetMeta());
  RebuildBaseMetaData();  //will rebuild current segment's meta data using current image
  return 0;
}

void CImageFlx::SwapWith(CImageFlx *pOther) {
//swap contents of this image with another image
#define SWAPM(m)  swapbytes(&(m),&(pOther->m),sizeof(m))
  CImageBase::SwapWith(pOther);
  SWAPM(pixelFormat);
  SWAPM(pixelFormatParam);
  SWAPM(pSegInfo);  //this will include all meta data blocks
  SWAPM(pCurrSeg);
#undef SWAPM
}

IMG_INT32 CImageFlx::GetNSegments() {
//return #of segments in file
  FLXSEGMENTINFO *pSeg;
  IMG_INT32 count;
  for (count = 0, pSeg = pSegInfo; pSeg != NULL; pSeg = pSeg->next) count++;
  return count;
}

IMG_INT32 CImageFlx::GetNFrames(IMG_INT32 segmentIndex,IMG_INT32 *pFirstFrame) {
//returns #of frames in specified segment, or total #of frames across all segments (if segmentIndex=-1)
//pFirstFrame: if !NULL then sets it to index of the first frame in specified segment (0 if segmentIndex=-1)
  FLXSEGMENTINFO *pSeg;
  IMG_INT32 count;
  if (segmentIndex < 0) {
    for (count = 0, pSeg = pSegInfo; pSeg != NULL; pSeg = pSeg->next) count+=pSeg->Nframes;
    if (pFirstFrame != NULL) (*pFirstFrame) = 0;
    }
  else {
    for (count = 0, pSeg = pSegInfo; (pSeg != NULL) && (segmentIndex > 0); pSeg = pSeg->next) {
      count += pSeg->Nframes;
      segmentIndex--;
      }
    if (pSeg == NULL) return 0;
    if (pFirstFrame != NULL) (*pFirstFrame) = count;
    count = pSeg->Nframes;
    }
  return count;
}

IMG_INT64 CImageFlx::GetFrameFilePos(IMG_INT32 frameIndex,IMG_INT32 *pFrameSize) {
//get absolute file offset, and optionally size, of specified frame
//frameIndex: 0-based global frame index
//returns -1 if invalid frame index
  FLXSEGMENTINFO *pSeg;
  IMG_INT32 frameWithinSeg,frameSize;
  IMG_INT64 off;
  if (GetSegmentIndex(frameIndex,&frameWithinSeg,&pSeg) < 0) return -1;
  ASSERT((frameWithinSeg >= 0) && (frameWithinSeg < pSeg->Nframes));
  if (frameWithinSeg >= pSeg->NframeOffsets) {
    //higher frame index than #of frame sizes in array - use last frame size in array and assume remaining frames have that size
    frameSize = (IMG_INT32)(pSeg->frameOffsets[pSeg->NframeOffsets] - pSeg->frameOffsets[pSeg->NframeOffsets - 1]);
    off = pSeg->frameOffsets[pSeg->NframeOffsets] + (frameWithinSeg - pSeg->NframeOffsets) * frameSize;
    }
  else {
    frameSize = (IMG_INT32)(pSeg->frameOffsets[frameWithinSeg + 1] - pSeg->frameOffsets[frameWithinSeg]);
    off = pSeg->frameOffsets[frameWithinSeg];
    }
  if (pFrameSize != NULL) (*pFrameSize) = frameSize;
  return pSeg->segmentOffset + off;
}

bool CImageFlx::AddNewSegment(IMG_INT32 Nframes) {
//add a new segment for specified number of frames and make it current
//(mainly to have a segment to store meta data in, when creating a new image from scratch)
  FLXSEGMENTINFO *pSeg;
  if ((pSeg = NewSegment()) == NULL) return false;
  pSeg->Nframes = Nframes;
  AddSegment(pSeg);
  pCurrSeg = pSeg;
  return true;
}

IMG_INT32 CImageFlx::GetProperty(PROPERTY_xxx prop) {
//check whether a specified feature is supported by this file format
  switch (prop) {
    case PROPERTY_SUPPORT_VIDEO: return true;
    case PROPERTY_IS_VIDEO:
      if (pSegInfo == NULL) return false;
      if ((pSegInfo->Nframes > 1) || (pSegInfo->next != NULL)) return true;	//assume each segment has at least 1 frame
      break;
    }
  return false;
}

void CImageFlx::UnloadHeader() {
//unload all headers (all segments)
  pixelFormat = PACKM_UNDEF;
  memset(pixelFormatParam,0,sizeof(pixelFormatParam));
  //deallocate all segment info, including meta data
  while (pSegInfo != NULL) {
    FLXSEGMENTINFO *pSeg;
    pSeg = pSegInfo;  pSegInfo = pSegInfo->next;
    if (pSeg->frameOffsets != NULL) delete [](pSeg->frameOffsets);
    delete pSeg;
    }
  pCurrSeg = NULL;
  CImageBase::UnloadHeader();
}

const char *CImageFlx::LoadFlxSegmentHeader(FILE *f,FLXSEGMENTINFO *pSeg,IMG_INT64 fsize) {
//loads flx segment header into specified segment structure, at the current file position
//file position when the function returns is not defined
//pSeg: segment structure to load into
//fsize: for checking consistency
//returns NULL=ok, !NULL=pointer to error message
  IMG_INT64 soTable[MAX_HDR_TABLE_SIZE];  //sizes and offsets table loaded from the header
  IMG_INT32 soTableSize;  //number of items in soTable[]
  char segHeader[20];  //FLXv + NL + tableSize + space
  char *pMetaDataBuf;
  const char *errMsg,*pItem;
  IMG_INT64 *offsTable;
  IMG_INT32 i;
  int n;
  /**/
  pSeg->segmentOffset = myftelli64(f);
  pSeg->frameOffsets = NULL;
  //read the segment header
  // FLX<version>      - version is 1 digit
  // tableSize metaOffset metaSize pixelsOffset pixelsSize     - tableSize is 2 decimal digits, all others 13 digits each
  if ((fsize-pSeg->segmentOffset < 8) || !fread(segHeader,8,1,f)) {
    if (pSeg->segmentOffset == 0) return "File not valid";	//can not read first few bytes of the file
    return "Can't read segment header in multi-segment file (extra data at end of file?)";
    }
  if ((segHeader[0] != 'F') || (segHeader[1] != 'L') || (segHeader[2] != 'X') || (segHeader[4] != '\n')
      || !isdigit(segHeader[5]) || !isdigit(segHeader[6]) || (segHeader[7] != ' ')) {
    return "Invalid segment header";
    }
  if (segHeader[3] != '1') return "Unsupported file version";
  soTableSize = (segHeader[5] - '0') * 10 + (segHeader[6] - '0');
  if (soTableSize < 4) return "Unexpected header (table size too small)";
  //read the offsets and sizes
  for (i = 0; i < soTableSize; i++) {
    if (!fread(segHeader,14,1,f)) return "Error reading header";
    segHeader[13] = '\0';
    n = -1;  sscanf(segHeader,"%"INT64PPFX"d%n",soTable+i,&n);
    if (n < 13) return "Error in header (table format)";
    }
  //fill in file size if size is 0 for an item
  if (soTable[SOTE_METASIZE] == 0) return "Missing meta data section";
  if (soTable[SOTE_PIXELSSIZE] == 0) return "Missing pixel data section";
  //sanity check sizes and offsets
  if (pSeg->segmentOffset + soTable[SOTE_METAOFFS] + soTable[SOTE_METASIZE] > fsize) return "Invalid meta data offset and/or size";
  if (pSeg->segmentOffset + soTable[SOTE_PIXELSOFFS] + soTable[SOTE_PIXELSSIZE] > fsize) return "Invalid image data offset and/or size";
  if (soTable[SOTE_METASIZE] > (1<<28)) return "Meta data size too large";  //just a sanity check - we load meta data to memory

  //allocate memory for entire meta data block and load it
  pMetaDataBuf = new char[(IMG_INT32)soTable[SOTE_METASIZE] + 1];
  if (pMetaDataBuf == NULL) { return "Out of memory"; }
  myfseeki64(f,pSeg->segmentOffset + soTable[SOTE_METAOFFS]);
  if (!fread(pMetaDataBuf,(IMG_INT32)soTable[SOTE_METASIZE],1,f)) { delete []pMetaDataBuf;  return "Error reading header"; }
  pMetaDataBuf[soTable[SOTE_METASIZE]] = '\0';

  //parse pHeaderBuf and fill the meta data structure (.meta)
  pSeg->meta.Unload();
  pItem = pMetaDataBuf;
  while (1) {
    while (*pItem == '\n') pItem++;  //skip any empty header items
    if (*pItem == '\0') break;
    if ((pItem = pSeg->meta.AddFromString(pItem,CMetaData::IFEX_IGNORE," ")) == NULL) { delete []pMetaDataBuf;  return "Error loading meta data"; }
    }
  delete []pMetaDataBuf;

  //figure out length of the segment
  n = 0;  //index of the last segment's offset in soTable (in file storage order)
  for (i = 2; i < soTableSize; i+=2) { if (soTable[i] > soTable[n]) n = i; }
  pSeg->segmentSize = soTable[n] + soTable[n+1];

  //read frame sizes and translate them into frame offsets table
  pSeg->NframeOffsets = pSeg->meta.GetMetaValCount(FLXMETA_FRAME_SIZE);
  if (pSeg->NframeOffsets < 1) {
    //calculate frame size
    //GetFrameSize() is based on the current value of class members, so create a temporary object, set it up from
    //segment meta data and use it to calculate the frame size. I guess I could just call ApplyMetaData() on this object...?
    CImageFlx tmpFlx;
    if ((errMsg = tmpFlx.ApplyMetaData(pSeg)) != NULL) { goto _error; }
    i = tmpFlx.GetFrameSize(NULL);
    offsTable = new IMG_INT64[2];
    if (offsTable == NULL) { errMsg = "Out of memory";  goto _error; }
    offsTable[0] = soTable[SOTE_PIXELSOFFS];
    offsTable[1] = offsTable[0] + i;
    pSeg->NframeOffsets = 1;
    }
  else {  //read frame sizes
    offsTable = new IMG_INT64[pSeg->NframeOffsets + 1];
	if (offsTable == NULL) { errMsg = "Out of memory";  goto _error; }
    offsTable[0] = soTable[SOTE_PIXELSOFFS];
    for (i = 0;i < pSeg->NframeOffsets; i++) {
      IMG_INT32 size;
      size = pSeg->meta.GetMetaInt(FLXMETA_FRAME_SIZE,0,i,true);
      if (size < 1) { errMsg = "Invalid frame size";  goto _error; }
      offsTable[i + 1] = offsTable[i] + size;
      }
    }
  pSeg->frameOffsets = offsTable;
  //calculate number of frames - there may be more frames than frame sizes provided, in which case last provided
  //frame size is assumed for the remaining frames (i.e. only one size is required if all frames have same size)
  pSeg->Nframes = pSeg->NframeOffsets;
  n = (IMG_INT32)(pSeg->frameOffsets[pSeg->NframeOffsets] - pSeg->frameOffsets[pSeg->NframeOffsets - 1]);  //size of last frame provided in meta
  pSeg->Nframes += (IMG_INT32)((pSeg->segmentSize - pSeg->frameOffsets[pSeg->NframeOffsets]) / n);
  return NULL;  //ok

_error:  /****label****/
  if (pSeg->frameOffsets != NULL) delete []pSeg->frameOffsets;
  pSeg->frameOffsets = NULL;
  return errMsg;
}

const char *CImageFlx::LoadFileHeader(const char *filename,void *pExtra) {
//loads flx file header (this function must be called before image data can be loaded)
//pExtra: not used (NULL)
//returns NULL=ok, !NULL=pointer to error message
  FLXSEGMENTINFO *pSeg;
  const char *errMsg;
  FILE *f;
  IMG_INT64 fsize;
  Unload();  //trash everything - meta data, any old header and/or data
  if ((f = fopen(filename,"rb")) == NULL) return "Error opening file";
  fsize = myfilelengthi64(f);
  if (fsize < 8) { errMsg = "Invalid FLX file"; goto _error; }  //FLXv + NL + tableSize + space
  //load info for all segments
  while (1) {
    //allocate a segment struct and load segment from file
    if ((pSeg = NewSegment()) == NULL) { errMsg = "Out of memory";  goto _error; }
    errMsg = LoadFlxSegmentHeader(f,pSeg,fsize);
    if (errMsg != NULL) { delete pSeg;  goto _error; }
    AddSegment(pSeg);
    //any more segments?
    if (fsize < pSeg->segmentOffset + pSeg->segmentSize) { errMsg = "File appears truncated";  goto _error; }
    if (fsize == pSeg->segmentOffset + pSeg->segmentSize) break;  //no more segments
    //loop around to read more segments
    myfseeki64(f,pSeg->segmentOffset + pSeg->segmentSize);  //seek to end of the segment just read in
    }
  fclose(f);
  f = NULL;
  //remember file name
  fileName = new char[strlen(filename)+1];
  if (fileName == NULL) { errMsg = "Out of memory"; goto _error; }
  strcpy(fileName,filename);
  //by default make the 1st segment and frame current
  if ((errMsg = ApplyMetaForFrame(0,true)) != NULL) goto _error;
  return NULL;
_error:  /****label****/
  if (f != NULL) fclose(f);
  Unload();
  return errMsg;
}

const char *CImageFlx::LoadFileData(IMG_INT32 frameIndex) {
//loads one image from flx file
//the header must have been loaded already
//frameIndex: index of the image in file (can be >0 if image is a video with multiple frames)
//returns NULL=ok, !NULL=pointer to error message
#define FIX_OLD_IMAGES    0  //if enabled, the function will sign-extend pixels in images created with older FlxView
                             //version which were saved improperly (had no sign-extended pixels)
                             //it will still work with correct images, just do unnecessary checking in that case
  const char *errMsg;
  const COLORMODELINFO *cmi;
  FLXSEGMENTINFO *pSeg;
  PIXEL *pWr[MAX_COLOR_CHANNELS];  //write pointers into color channels in the currently decoded plane
  IMG_UINT8 *lineBuf;   //buffer for one line of data in a plane
  IMG_INT32 lineBufSize,lineSize;  //allocated size of lineBuf[] and actual size of data [bytes]
  IMG_INT32 frameInSeg;
  IMG_INT32 Nplanes,plane,Nchnls,chBase,ch,x,y;
  FILE *f;
  if (!IsHeaderLoaded()) return "File header not loaded";
  if ((cmi = GetColorModelInfo(colorModel)) == NULL) return "Invalid color model";
  if ((Nplanes = GetNFilePlanes()) < 1) return "Invalid plane format";
  if ((GetSegmentIndex(frameIndex,&frameInSeg,&pSeg)) < 0) return "Invalid frame number";
  //open file
  if ((f = fopen(fileName,"rb")) == NULL) return "Error opening file";
  //seek to index-th frame (if this is a video/multi-image file)
  //if the current segment is different than the one we seek to, make the new segment current and apply its meta data
  if (pSeg != pCurrSeg) {
    if ((errMsg = ApplyMetaData(pSeg)) != NULL) { fclose(f);  return errMsg; }
    ASSERT(pCurrSeg == pSeg);
    }
  {IMG_INT64 foff;
  foff = GetFrameFilePos(frameIndex,NULL);
  if (!myfseeki64(f,foff)) { fclose(f);  return "Invalid image data offset"; }
  }
  //allocate and decode color planes
  lineBuf = NULL;
  lineBufSize = 0;
  chBase = 0;
  for (plane = 0; plane < Nplanes; plane++) {
    if (((Nchnls = GetNChannelsInPlane(plane)) < 1) || (chBase + Nchnls > cmi->Nchannels)) {
      errMsg = "Invalid plane format";  goto _abort;
      }
    //get horizontal and vertical sub-sampling step
    //all color channels within plane must have same subsampling - get subsampling for 1st channel, check other channels
    for (ch = 1; ch < Nchnls; ch++) {
      if ((GetXSampling(chBase + 0) != GetXSampling(chBase + ch))
        || (GetYSampling(chBase + 0) != GetYSampling(chBase + ch))) { errMsg = "Inconsistent subsampling within plane";  goto _abort; }
      }
    //if needed, allocate buffers for color channels
    for (ch = 0; ch < Nchnls; ch++) {
      COLCHANNEL *pChnl = chnl + (chBase + ch);
      if (pChnl->data == NULL) {
        pChnl->data = new PIXEL[pChnl->chnlWidth * pChnl->chnlHeight];
        if (pChnl->data == NULL) { errMsg = "Out of memory";  goto _abort; }
        }
      pWr[ch] = pChnl->data;
      }

    //[re-]allocate lineBuf[] to hold one entire line of data in this plane
    lineSize = GetLineSize(plane);
    if (lineSize > lineBufSize) {
      if (lineBuf != NULL) delete []lineBuf;
        lineBuf = new IMG_UINT8[lineSize];
        if (lineBuf == NULL) { errMsg = "Out of memory";  goto _abort; }
      lineBufSize = lineSize;
      }

    //read and unpack data for this plane
    for (y = 0; y < chnl[chBase].chnlHeight; y++) {
      if (!fread(lineBuf,lineSize,1,f)) { errMsg = "Error reading file";  goto _abort; }
      switch (pixelFormat) {
        case PACKM_UNPACKEDR:  //value stored in LSBits with sign extension
        case PACKM_UNPACKEDL: {  //value stored in MSBits
          IMG_UINT8 *pRd;
          IMG_INT32 val;
          pRd = lineBuf;
          //this could probably be optimized for speed, but I can't be bothered now
          //NOTE: in file, signed values are sign-extended to 8, 16 or 32 bits
          for (x = 0; x < chnl[chBase].chnlWidth; x++) {
            for (ch = 0; ch < Nchnls; ch++) {
              IMG_UINT8 bd;
              bool isSigned;
              bd = chnl[chBase + ch].bitDepth;
              isSigned = chnl[chBase + ch].isSigned;
              if (bd > 16) { 
#if FIX_OLD_IMAGES
                if (isSigned && (*(IMG_UINT32*)pRd & (1<<(bd-1))) && (*(IMG_INT32*)pRd>0))
                  *(IMG_UINT32*)pRd |= ~((1<<bd)-1);
#endif
                val = *(IMG_INT32*)pRd;
                pRd += 4;
                if (pixelFormat == PACKM_UNPACKEDL) val >>= 32 - bd;
                }
              else if (bd > 8) { 
#if FIX_OLD_IMAGES
                if (isSigned && (*(IMG_UINT16*)pRd & (1<<(bd-1))) && (*(IMG_INT16*)pRd>0))
                  *(IMG_UINT16*)pRd |= ~((1<<bd)-1);
#endif
                if (isSigned) val = (IMG_INT32)(*(IMG_INT16*)pRd);
                else val = (IMG_INT32)(*(IMG_UINT16*)pRd);
                pRd += 2;
                if (pixelFormat == PACKM_UNPACKEDL) val >>= 16 - bd;
                }
              else {
#if FIX_OLD_IMAGES
                if (isSigned && (*(IMG_UINT8*)pRd & (1<<(bd-1))) && (*(IMG_INT8*)pRd>0))
                  *(IMG_UINT8*)pRd |= ~((1<<bd)-1);
#endif
                if (isSigned) val = (IMG_INT32)(*(IMG_INT8*)pRd);
                else val = (IMG_INT32)(*(IMG_UINT8*)pRd);
                pRd++;
                if (pixelFormat == PACKM_UNPACKEDL) val >>= 8 - bd;
                }
              *(pWr[ch])++ = (PIXEL)val;
              }
            }
          } break;
        case PACKM_GROUPPACK: {
          CBitStreamRW bitReader;
          IMG_INT32 packCounter,groupSize,val;
          bitReader.SetPos(lineBuf);
          packCounter = 0;
          groupSize = pixelFormatParam[plane];
          for (x = 0; x < chnl[chBase].chnlWidth; x++) {
            for (ch = 0; ch < Nchnls; ch++) {
              if (chnl[chBase + ch].isSigned) val = bitReader.ReadBitsSig(chnl[chBase + ch].bitDepth);
              else val = bitReader.ReadBitsUns(chnl[chBase + ch].bitDepth);
              *(pWr[ch])++ = (PIXEL)val;
              if (++packCounter >= groupSize) { packCounter = 0;  bitReader.AdvanceToByteBoundary(); }
              }
            }
          } break;
        default:
          errMsg = "Invalid packing mode";
          goto _abort;
        }
      } //for (y ...)
    //move on to the next plane
    chBase += Nchnls;
    }
  errMsg = NULL;
  _abort:  /****label****/
  fclose(f);
  if (lineBuf != NULL) delete []lineBuf;
  if (errMsg != NULL) UnloadData();
  return errMsg;
#undef FIX_OLD_IMAGES
}

const char *CImageFlx::SaveFlxMetaData(FLXSAVECONTEXT *pCtx) {
//write FLX segment header - meta data block
//pCtx->pExtra: if NULL, current object members and meta data will be saved
//              otherwise specified meta data and (if pCtx->pExtra->pImData!=NULL) specified image data will be saved
//Meta data is saved as-is, without any checking for consistency/validity. Higher level should make sure meta data is consistent with image data.
//Within each segment this can be called either before or after all calls to SaveFileData() (meta data can be before or after pixel data)
  static const char *dontSaveMetaItems[] = {
    //these are deprecated and not saved at all
#if FLXMETA_SUPPORT_DEPRECATED
    FLXMETA_SUBSAMPLING, FLXMETA_ISO_GAIN, FLXMETA_SENSOR_DENSITY,
    FLXMETA_GAMMA_BLACK_PT, FLXMETA_GAMMA_WHITE_PT, FLXMETA_NOMINAL_RANGE
#endif
    };
  const COLORMODELINFO *cmi;
  IMG_UINT32 i;
  CImageBase *pImData;
  CMetaData *pMD;
  const CMetaData::MDITEM *mdi;
  pImData = ((pCtx->pExtra == NULL) || (pCtx->pExtra->pImData == NULL))? this: pCtx->pExtra->pImData;
  pMD = (((pCtx->pExtra == NULL) || (pCtx->pExtra->pMD == NULL))? GetMeta(): pCtx->pExtra->pMD);
  if ((cmi = GetColorModelInfo(pImData->colorModel)) == NULL) return "Invalid color model";
  //update header table (offset)
  pCtx->soTableSave[SOTE_METAOFFS] = myftelli64(pCtx->f) - pCtx->seg.segmentOffset;
  //now save the meta data
  for (mdi = pMD->list; mdi != NULL; mdi = mdi->next) {
    for (i = 0; i < SizeofArray(dontSaveMetaItems); i++) {
      if (stricmp(mdi->name,dontSaveMetaItems[i]) == 0) break;
      }
    if (i < SizeofArray(dontSaveMetaItems)) continue;  //don't save this one
    fprintf(pCtx->f,"%s=%s\n",mdi->name,mdi->sVal);
    }
  //update header table (size)
  pCtx->soTableSave[SOTE_METASIZE] = (myftelli64(pCtx->f) - pCtx->seg.segmentOffset) - pCtx->soTableSave[SOTE_METAOFFS];
  pCtx->seg.segmentSize = pCtx->soTableSave[SOTE_METASIZE] + pCtx->soTableSave[SOTE_METAOFFS];
  return NULL;
}

const char *CImageFlx::SaveFlxFrameData(FLXSAVECONTEXT *pCtx) {
//write FLX data for one image (see CImageBase::SaveFileData() for more info)
//pCtx->pExtra: if NULL, current object members and meta data will be saved
//              otherwise specified meta data and (if pCtx->pExtra->pImData!=NULL) specified image data will be saved
  FLXSAVEFORMAT *pFormat;
  IMG_UINT8 *lineBuf;   //buffer for one line of data in a plane
  IMG_INT32 lineBufSize,lineSize;  //allocated size of lineBuf[] and actual size of data [bytes]
  IMG_INT32 Nplanes,plane,Nchnls,chBase,ch,x,y;
  IMG_INT32 chMin[MAX_COLOR_CHANNELS],chMax[MAX_COLOR_CHANNELS];
  PIXEL *pRd[MAX_COLOR_CHANNELS];  //read pointers into color channels of the plane currently being saved
  const COLORMODELINFO *cmi;
  const char *errMsg;
  PACKM_xxx savPixelFormat;
  CImageBase *pImData;
  /**/
  pFormat = pCtx->pExtra;
  pImData = (((pFormat == NULL) || (pFormat->pImData == NULL))? this: pFormat->pImData);
  if ((Nplanes = GetNFilePlanes(pFormat)) < 1) return "Invalid plane format";
  if ((cmi = GetColorModelInfo(pImData->colorModel)) == NULL) return "Invalid color model";
  savPixelFormat = ((pFormat == NULL)? pixelFormat: pFormat->pixelFormat);
  //remember the file offset if this is the first frame being saved
  if (pCtx->soTableSave[SOTE_PIXELSOFFS] < 0)
    pCtx->soTableSave[SOTE_PIXELSOFFS] = myftelli64(pCtx->f) - pCtx->seg.segmentOffset;
  //allocate and decode color planes
  lineBuf = NULL;
  lineBufSize = 0;
  chBase = 0;
  for (plane = 0; plane < Nplanes; plane++) {
    if (((Nchnls = GetNChannelsInPlane(plane,pFormat)) < 1) || (chBase + Nchnls > cmi->Nchannels)) {
      errMsg = "Invalid plane format";  goto _abort;
      }
    //get horizontal and vertical sub-sampling step
    //all color channels within plane must have same subsampling - get subsampling for 1st channel, check other channels
    for (ch = 1; ch < Nchnls; ch++) {
      if ((pImData->GetXSampling(chBase + 0) != pImData->GetXSampling(chBase + ch))
          || (pImData->GetYSampling(chBase + 0) != pImData->GetYSampling(chBase + ch))) {
        errMsg = "Inconsistent subsampling within plane";  goto _abort;
        }
      }
    //init color channel read pointers   and clipping limits
    for (ch = 0; ch < Nchnls; ch++) {
      pRd[ch] = pImData->chnl[chBase + ch].data;
      chMin[ch] = pImData->GetChMin(chBase + ch);
      chMax[ch] = pImData->GetChMax(chBase + ch);
      }

    //[re-]allocate lineBuf[] to hold one entire line of data in this plane
    lineSize = GetLineSize(plane,pFormat);
    if (lineSize > lineBufSize) {
      if (lineBuf != NULL) delete []lineBuf;
        lineBuf = new IMG_UINT8[lineSize];
        if (lineBuf == NULL) { errMsg = "Out of memory";  goto _abort; }
      lineBufSize = lineSize;
      }

    //write data for this plane
    //NOTE: in file, signed values need to be sign-extended to 8, 16 or 32 bits
    for (y = 0; y < pImData->chnl[chBase].chnlHeight; y++) {
      switch (savPixelFormat) {
        case PACKM_UNPACKEDR:  //value stored in LSBits with sign extension
        case PACKM_UNPACKEDL: {  //value stored in MSBits
          IMG_UINT8 *pWr;
          IMG_INT32 val,bd;
          pWr = lineBuf;
          //this could probably be optimized for speed, but I can't be bothered now
          for (x = 0; x < pImData->chnl[chBase].chnlWidth; x++) {
            for (ch = 0; ch < Nchnls; ch++) {
              bd = pImData->chnl[chBase + ch].bitDepth;
              val = (IMG_INT32)(*pRd[ch]);
              if (val<chMin[ch]) val = chMin[ch];
              else if (val>chMax[ch]) val = chMax[ch];
              if (bd > 16) {  //this should only be the case if PIXEL=int32 rather than int16
                if (savPixelFormat == PACKM_UNPACKEDL) val <<= 32 - bd;
                *(IMG_UINT32*)pWr = *(IMG_UINT32*)(&val);  pWr += 4;
                }
              else if (bd > 8) {
                if (savPixelFormat == PACKM_UNPACKEDL) val <<= 16 - bd;
                *(IMG_UINT16*)pWr = *(IMG_UINT16*)(&val);  pWr += 2;
                }
              else {
                if (savPixelFormat == PACKM_UNPACKEDL) val <<= 8 - bd;
                *(IMG_UINT8*)pWr = *(IMG_UINT8*)(&val);  pWr++;
                }
              pRd[ch]++;
              }
            }
          } break;
        case PACKM_GROUPPACK: {
          //!!!JML  NOT TESTED!
          CBitStreamRW bitWriter;
          IMG_INT32 packCounter,groupSize;
          bitWriter.SetPos(lineBuf);
          packCounter = 0;
          groupSize = ((pFormat == NULL)? pixelFormatParam[plane]: pFormat->pixelFormatParam[plane]);
          for (x = 0; x < pImData->chnl[chBase].chnlWidth; x++) {
            for (ch = 0; ch < Nchnls; ch++) {
              if (pImData->chnl[chBase + ch].isSigned)
                bitWriter.WriteBitsSig(*(IMG_INT32*)pRd[ch],pImData->chnl[chBase + ch].bitDepth);
              else bitWriter.WriteBitsUns(*(IMG_UINT32*)pRd[ch],pImData->chnl[chBase + ch].bitDepth);
              pRd[ch]++;
              if (++packCounter >= groupSize) { packCounter = 0;  bitWriter.AdvanceToByteBoundary(); }
              }
            }
          } break;
        default:
          errMsg = "Invalid packing mode";
          goto _abort;
        }
      if (!fwrite(lineBuf,lineSize,1,pCtx->f)) { errMsg = "Error writing file";  goto _abort; }
      } //for (y ...)
    //move on to the next plane
    chBase += Nchnls;
    }
  errMsg = NULL;
  pCtx->soTableSave[SOTE_PIXELSSIZE] = (myftelli64(pCtx->f) - pCtx->seg.segmentOffset) - pCtx->soTableSave[SOTE_PIXELSOFFS];
  pCtx->seg.segmentSize = pCtx->soTableSave[SOTE_PIXELSOFFS] + pCtx->soTableSave[SOTE_PIXELSSIZE];
  pCtx->seg.Nframes++;
  _abort:  /****label****/
  if (lineBuf != NULL) delete []lineBuf;
  return errMsg;
}

const char *CImageFlx::SaveFlxSegmentStart(FLXSAVECONTEXT *pCtx) {
//start writing a new segment
  IMG_UINT8 tmp[8],i;
  //reset segment info
  ASSERT((pCtx->seg.frameOffsets == NULL) && (pCtx->seg.NframeOffsets == 0));
  pCtx->seg.Nframes = 0;
  pCtx->seg.segmentOffset = myftelli64(pCtx->f);
  pCtx->seg.segmentSize = 0;
  //write dummy space for segment header to be overwritten by SaveFlxSegmentEnd()
  i = 4 + 1 + 2 + SizeofArray(pCtx->soTableSave)*14 + 1;  //FLX + NL + tableSize + tableSize*(space+13-digit-int) + NL
  for (; i >= sizeof(tmp); i-=sizeof(tmp)) fwrite(tmp,8,1,pCtx->f);
  if (i > 0) fwrite(tmp,i,1,pCtx->f);
  for (i = 0; i < SizeofArray(pCtx->soTableSave); i++) pCtx->soTableSave[i] = -1;
  return NULL;
}

const char *CImageFlx::SaveFlxSegmentEnd(FLXSAVECONTEXT *pCtx) {
//finish writing FLX file (see CImageBase::SaveFileEnd() for more info)
//this overwrites the dummy space in segment header with actual data, such as valid offsets & sizes table information
  char buf[20];
  IMG_UINT32 i;
  if (pCtx->soTableSave[SOTE_METAOFFS] < 0) return "Meta data not saved";
  if (pCtx->soTableSave[SOTE_PIXELSOFFS] < 0) return "Pixel data not saved";
  myfseeki64(pCtx->f,pCtx->seg.segmentOffset);
  sprintf(buf,"FLX1\n%02d",(IMG_INT)SizeofArray(pCtx->soTableSave));
  fwrite(buf,7,1,pCtx->f);
  for (i = 0; i < SizeofArray(pCtx->soTableSave); i++) {
    sprintf(buf," %013"INT64PPFX"d",pCtx->soTableSave[i]);
    fwrite(buf,14,1,pCtx->f);
    }
  fwrite("\n",1,1,pCtx->f);
  myfseeki64(pCtx->f,pCtx->seg.segmentOffset + pCtx->seg.segmentSize);  //prepare to write new segment
  return NULL;
}

const char *CImageFlx::SaveFileStart(const char *filename,void *pExtra,void **pSC) {
//initiates saving of an FLX file (see CImageBase::SaveFileStart() for more info)
//pExtra: must point to FLXSAVEFORMAT structure, which must be valid during the entire save process
//        if NULL, current object data and members will be saved
//returns: NULL=ok, !NULL=error message string
  FLXSAVECONTEXT *pCtx;
  FLXSAVEFORMAT *pFormat;
  CImageBase *pImData;
  /**/
  pFormat = (FLXSAVEFORMAT*)pExtra;
  pImData = (((pFormat == NULL) || (pFormat->pImData == NULL))? this: pFormat->pImData);
  if (!pImData->IsHeaderLoaded() || !pImData->IsDataLoaded()) return "No image data to save";
  (*pSC) = NULL;
  //create a new segment if there is none - this is required so that there is a current segment and GetMeta() works
  if (pSegInfo == NULL) { if ((pSegInfo = NewSegment()) == NULL) return "Out of memory"; }
  if (pCurrSeg == NULL) pCurrSeg = pSegInfo;
  //create save context struct and open output file
  pCtx = new FLXSAVECONTEXT;
  if (pCtx == NULL) { return "Out of memory"; }
  if ((pCtx->f = fopen(filename,"wb")) == NULL) { delete pCtx;  return "Error creating file"; }
  pCtx->pExtra = (FLXSAVEFORMAT*)pExtra;
  pCtx->seg.segmentOffset = 0;
  pCtx->seg.segmentSize = 0;
  pCtx->seg.frameOffsets = NULL;  //not used, at least not so far - all frames are always same size within a segment
  pCtx->seg.NframeOffsets = 0;
  pCtx->seg.Nframes = 0;
  pCtx->seg.next = NULL;  //not used
  (*pSC) = pCtx;
  return NULL;
}

const char *CImageFlx::StartSegment(void *pSC,bool inherit,bool force) {
//starts saving a new segment
//meta data will be taken at this point
//force: true=force start a new segment, false=only start a segment if meta data changed (check FLXSAVEFORMAT.pMD vs last saved)
//inherit: true=use META_INHERIT and inherit any unchanged meta data from the previous segment, false=do not use META_INHERIT
  FLXSAVECONTEXT *pCtx;
  FLXSAVEFORMAT *pFormat;
  CMetaData *pMD,newMD;
  const char *msg;
  pCtx = (FLXSAVECONTEXT*)pSC;
  pFormat = (FLXSAVEFORMAT*)(pCtx->pExtra);
  //make a local copy of the meta data
  pMD = ((pFormat == NULL) || (pFormat->pMD == NULL))? GetMeta(): pFormat->pMD;
  pCtx->seg.meta.CopyFrom(pMD);

  //!!! - implement comparison, inherit and force
  force = true;
  if (!force) return NULL;
  
  //flush currently opened segment (if any)
  if (pCtx->seg.segmentSize > 0) {
    if ((msg = SaveFlxSegmentEnd(pCtx)) != NULL) return msg;
    }
  //start a new segment
  if ((msg = SaveFlxSegmentStart(pCtx)) == NULL) return msg;
  return NULL;
}

const char *CImageFlx::SaveFileHeader(void *pSC) {
//write meta data block in the segment currently being written
//For FLX format this can be called either before or after all calls to SaveFileData() within each segment. The FLX format can
//store meta data at the beginning or at the end of a segment.
  FLXSAVECONTEXT *pCtx;
  FLXSAVEFORMAT *pFormat;
  const char *msg;
  CMetaData saveMD,*pMDSav;
  pCtx = (FLXSAVECONTEXT*)pSC;
  pFormat = (FLXSAVEFORMAT*)(pCtx->pExtra);
  //make a temporary copy of the meta data and validate it
  saveMD.CopyFrom(&(pCtx->seg.meta));
  RebuildBaseMetaData(pFormat,&saveMD);
  //save it - temporarily replace pMD pointer in pFormat with pointer to saveMD (a validated equivalent)
  if (pFormat != NULL) { pMDSav = pFormat->pMD;  pFormat->pMD = &saveMD; }
  msg = SaveFlxMetaData(pCtx);
  if (pFormat != NULL) pFormat->pMD = pMDSav;
  return msg;
}

const char *CImageFlx::SaveFileData(void *pSC) {
//write FLX data for one image (see CImageBase::SaveFileData() for more info)
  FLXSAVECONTEXT *pCtx;
  pCtx = (FLXSAVECONTEXT*)pSC;
  return SaveFlxFrameData(pCtx);
}

const char *CImageFlx::SaveFileEnd(void *pSC) {
//finish writing FLX file (see CImageBase::SaveFileEnd() for more info)
  FLXSAVECONTEXT *pCtx;
  const char *msg;
  pCtx = (FLXSAVECONTEXT*)pSC;
  //finish last segment
  if ((msg = SaveFlxSegmentEnd(pCtx)) != NULL) return msg;
  //terminate saving process
  fclose(pCtx->f);
  delete pCtx;
  return NULL;
}

const char *CImageFlx::GetSaveFormat(CImageFlx::FLXSAVEFORMAT *pFmt,CImageBase *pIm) {
//fills in the save format structure based on format of given image pIm
//returns NULL=ok, otherwise error message string
  IMG_UINT i;
  pFmt->lineAlign = 1;
  pFmt->pixelFormat = PACKM_UNPACKEDR;
  memset(pFmt->pixelFormatParam,0,sizeof(pFmt->pixelFormatParam));
  for (i = 0; i < SizeofArray(pFmt->planeFormat); i++) pFmt->planeFormat[i] = 1;  //planar
  pFmt->pMD = NULL;
  pFmt->pImData = pIm;
  return NULL;
}

const char *CImageFlx::GetSaveFormat(CImageFlx::FLXSAVEFORMAT *pFmt,CImageFlx *pIm) {
//fills in the save format structure based on format of given image pIm (current segment)
//returns NULL=ok, otherwise error message string
  IMG_UINT32 i;
  pFmt->lineAlign = pIm->GetMeta()->GetMetaInt(FLXMETA_LINE_ALIGN,1);
  pFmt->pixelFormat = pIm->pixelFormat;
  for (i = 0; i < SizeofArray(pFmt->pixelFormatParam); i++) pFmt->pixelFormatParam[i] = pIm->pixelFormatParam[i];
  for (i = 0; i < pIm->GetNFilePlanes(); i++) pFmt->planeFormat[i] = (IMG_INT8)pIm->GetNChannelsInPlane(i);
  pFmt->pMD = pIm->GetMeta();
  pFmt->pImData = pIm;
  return NULL;
}

const char *CImageFlx::GetSaveFormat(CImageFlx::FLXSAVEFORMAT *pFmt,CImagePxm *pIm) {
//fills in the save format structure based on format of given image pIm
//returns NULL=ok, otherwise error message string
  return GetSaveFormat(pFmt,(CImageBase*)pIm);
}

const char *CImageFlx::GetSaveFormat(FLXSAVEFORMAT *pFmt,CImageYuv *pIm) {
//fills in the save format structure based on format of given image pIm
//returns NULL=ok, otherwise error message string
  IMG_UINT i;
  pFmt->lineAlign = 1;
  pFmt->pixelFormat = PACKM_UNPACKEDR;
  for (i = 0; i < SizeofArray(pFmt->pixelFormatParam); i++) pFmt->pixelFormatParam[i] = 0;
  if ((pIm->planeFormat == CImageYuv::PLANEFMT_PLANAR) || (pIm->subsMode != SUBS_444)) {
    for (i = 0; i < SizeofArray(pFmt->planeFormat); i++) pFmt->planeFormat[i] = 1;  //planar
    }
  else  //PLANEFMT_INTER_YUV, PLANEFMT_INTER_YUYV/YVYU/UYVY/VYUY
    pFmt->planeFormat[0] = (IMG_INT8)pIm->GetNColChannels();  //interleaved
  pFmt->pMD = NULL;
  pFmt->pImData = pIm;
  return NULL;
}

const char *CImageFlx::GetSaveFormat(FLXSAVEFORMAT *pFmt,CImagePlRaw *pIm) {
//fills in the save format structure based on format of given image pIm
//returns NULL=ok, otherwise error message string
  return GetSaveFormat(pFmt,(CImageBase*)pIm);
}

const char *CImageFlx::GetSaveFormat(FLXSAVEFORMAT *pFmt,CImageNRaw *pIm) {
//fills in the save format structure based on format of given image pIm
//returns NULL=ok, otherwise error message string
  return GetSaveFormat(pFmt,(CImageBase*)pIm);
}

const char *CImageFlx::GetSaveFormat(FLXSAVEFORMAT *pFmt,CImageAptinaRaw *pIm) {
//fills in the save format structure based on format of given image pIm
//returns NULL=ok, otherwise error message string
  return GetSaveFormat(pFmt,(CImageBase*)pIm);
}

const char *CImageFlx::GetSaveFormat(FLXSAVEFORMAT *pFmt,CImageBmp *pIm) {
//fills in the save format structure based on format of given image pIm
//returns NULL=ok, otherwise error message string
  return GetSaveFormat(pFmt,(CImageBase*)pIm);
}

/*-------------------------------------------------------------------------------
PGM/PPM file format

Officially:
- the PGM format is grayscale, i.e. it is a single 'color' plane; there are width*height values in file
- the PPM format is color, i.e. it has width*height*3 values in file assuming three values per pixel (R,G,B)

Here we blend the two together and allow an override (through function parameter) to support other
non-standard color models (with not necessarily 1 or 3 color channels as given by format spec). In particular,
we have some images which are grayscale, but actually contain Bayer data, i.e. every 2x2 pixels represent
4 values of 4 different color channels. The format does not have much space to record the color space used
(it does not support variety in the first place) and so it is up to the user to know how to interpret an image.

Regardless of whether the file is PGM or PPM, we generalize that the number of values (per image) is
    width * height * numChannels
where numChannels is the number of color channels in the image (as appropriate for the color model) and it
1) defaults to 1 for PGM and to 3 for PPM
2) can be overriden using function parameter when loading a file (by specifying a different color model)
*/

CImagePxm::CImagePxm() {
  Unload();
}

CImagePxm::~CImagePxm() {
  Unload();
}

void CImagePxm::UnloadHeader() {
  fileFormat = PPTYPE_UNDEF;
  foffsetData = -1;
  pixelOnLoadResize = 0;
  CImageBase::UnloadHeader();
}

void CImagePxm::ParseComment(FILE *f,char **ppMetaItemBuf) {
//parse one comment in PGM/PPM, store any items which look like meta data
//initial file position is just after reading '#'
//final file position is just after reading '\n'
#define META_BUFSIZE  1000   //size of buffer for one meta data item (1 line of text)
  char *pName,*pNameEnd,*pValue,c;
  if ((*ppMetaItemBuf) == NULL) {
    (*ppMetaItemBuf) = new char[META_BUFSIZE];
    if ((*ppMetaItemBuf) == NULL) { return; }
  }
  pName = (*ppMetaItemBuf);
  //read until end of line ('\n' is part of the comment)
  //store comment in buffer
  while (!feof(f) && ((c = fgetc(f)) != '\n')) {
    if ((pName!=NULL) && (pName-(*ppMetaItemBuf)<META_BUFSIZE-1)) *pName++=c;
    }
  if ((*ppMetaItemBuf) == NULL) return;
  //split buffer into: first word = name, space, whatever is left = value
  *pName = '\0';
  for (pName=(*ppMetaItemBuf);*pName==' ';pName++) ;		//ptr to name
  for (pNameEnd=pName;*pNameEnd && (*pNameEnd!=' ');pNameEnd++) ;	//ptr to just after the name
  for (pValue=pNameEnd;*pValue==' ';pValue++) ;
  if (*pValue!='\0') meta.Add(pName,pNameEnd-pName,pValue,-1,CMetaData::IFEX_APPEND,"\n");
#undef META_BUFSIZE
}

const char *CImagePxm::LoadFileHeader(const char *filename,void *pExtra) {
//load file header
//pExtra: NULL or ptr to a PXMLOADFORMAT (which must be valid during the entire load process)
//returns NULL=ok, !NULL=error message string
  const COLORMODELINFO *cmi;
  FILE *f;
  const char *errMsg;
  CM_xxx aColorModel;
  SUBS_xxx aSubsMode;
  IMG_INT c;
  IMG_INT32 headerValues[3],NintsRead,bitsPerVal;
  char *pMetaItemBuf;  //for one line of meta data
  /**/
  Unload();  //trash everything - meta data, any old header and/or data
  pMetaItemBuf = NULL;
  if ((f = fopen(filename,"rb")) == NULL) return "Error opening file";
  if (fgetc(f) != 'P') { errMsg = "Invalid file header";  goto _error; }
  c = (char)fgetc(f);
  aSubsMode = SUBS_UNDEF;
  switch (c) {
    case '6': fileFormat = PPTYPE_PPMB;  aColorModel = CM_RGB; break;
    case '3': fileFormat = PPTYPE_PPMT;  aColorModel = CM_RGB; break;
    case '5': fileFormat = PPTYPE_PGMB;  aColorModel = CM_GRAY; break;
    case '2': fileFormat = PPTYPE_PGMT;  aColorModel = CM_GRAY; break;
    default: errMsg = "Unrecognized PPM/PGM format";  goto _error;
    }
  //NintsRead indicates how many integers have been read from the header
  for (NintsRead = 0; NintsRead < 3;) {
    while (!feof(f) && isspace(c = fgetc(f))) ;   //skip spaces
    if (feof(f)) { errMsg = "Incomplete file";  goto _error; }
    if (c == '#') { ParseComment(f,&pMetaItemBuf); continue; }  //a comment
    if (!isdigit(c)) { errMsg = "Invalid file header (number expected)";  goto _error; }
    headerValues[NintsRead] = c - '0';
    while (!feof(f) && isdigit(c = fgetc(f))) headerValues[NintsRead] = headerValues[NintsRead] * 10 + c - '0';
    NintsRead++;
    ungetc(c,f);  //c is most likely a space, but could be a comment I suppose...
    }
  //read any comments just before data - this can only happen if there is a comment immediately after the last
  //integer (otherwise a space (usually '\n') after the last integer indicates start of data)
  while (!feof(f) && ((c = fgetc(f)) == '#')) ParseComment(f,&pMetaItemBuf);
  if (feof(f) || !isspace(c)) { errMsg = "Invalid file format (space expected before data)";  goto _error; }
  //now at the beginning of data
  foffsetData = ftell(f);
  fclose(f);
  if (pMetaItemBuf != NULL) delete []pMetaItemBuf;
    
  //decode what has been read in - make sense of the file header and combine it with
  //any overrides in the structure passed as parameter
  if (headerValues[2] > 65535) { errMsg = "Invalid maxval in header";  goto _error; }
  width = headerValues[0];
  height = headerValues[1];
  for (bitsPerVal = 1; (1 << bitsPerVal) <= headerValues[2]; bitsPerVal++) ;
  this->pExtra = pExtra;
  if (pExtra != NULL) {
    PXMLOADFORMAT *pFormat;
    SUBS_xxx subsModeFromHdr;
    pFormat = (PXMLOADFORMAT*)pExtra;
    if (pFormat->forceColorModel != CM_UNDEF) aColorModel = pFormat->forceColorModel;
    //check whether the header has subsampling mode (CFA)
    //auto-detect RGGB color model if CFA is present in header, unless forceColorModel says grayscale
    subsModeFromHdr = SUBS_UNDEF;
    if ((aColorModel == CM_RGGB) || ((aColorModel == CM_GRAY) && (pFormat->forceColorModel != CM_GRAY))) {
      const char *c2;
      if ((c2=meta.GetMetaStr("CFA"))!=NULL) {
        if (strcmp(c2,"RGGB") == 0) subsModeFromHdr = SUBS_RGGB;
        else if (strcmp(c2,"GRBG") == 0) subsModeFromHdr = SUBS_GRBG;
        else if (strcmp(c2,"GBRG") == 0) subsModeFromHdr = SUBS_GBRG;
        else if (strcmp(c2,"BGGR") == 0) subsModeFromHdr = SUBS_BGGR;
        }
      }
    if ((aColorModel == CM_RGGB) || (aColorModel == CM_GRAY)) {
      if (pFormat->forceSubsampling != SUBS_UNDEF) aSubsMode = pFormat->forceSubsampling;
      else aSubsMode = subsModeFromHdr;
      if ((aColorModel == CM_RGGB) && (aSubsMode == SUBS_UNDEF)) aSubsMode = SUBS_RGGB;
      if ((aColorModel == CM_GRAY) && (aSubsMode != SUBS_UNDEF)) aColorModel = CM_RGGB;
      }
    pixelOnLoadResize = 0;
    }
  if (bitsPerVal >= 16) {
    if ((pExtra != NULL) && (((PXMLOADFORMAT*)pExtra)->loadBd == LOADBD_DOWNSIZE)) {
      #if PIXELSIZE==2	//we can only handle 16-bit signed or 15-bit unsigned, not 16-bit unsigned, so downsize
      pixelOnLoadResize = (IMG_INT8)(15 - bitsPerVal);  //downsize to (unsigned) 15-bit
      bitsPerVal = 15;
      #else		//we can handle 16-bit unsigned, don't do anything and load as-is
      #endif
      }
    else bitsPerVal=-16;  //load as signed 16-bit
    }
  if ((cmi = GetColorModelInfo(aColorModel)) == NULL) { errMsg = "Invalid color model";  goto _error; }
  colorModel = aColorModel;
  subsMode = aSubsMode;
  for (c = 0; c < cmi->Nchannels; c++) {
    COLCHANNEL *pChnl = chnl + c;
    IMG_INT32 xSampling,ySampling;
    pChnl->bitDepth = (IMG_UINT8)((bitsPerVal<0)?(-bitsPerVal):bitsPerVal);
    //all channels are assumed to be interleaved, so must have same subsampling (same number of values per color plane)
    if (c == 0) {
      xSampling = GetXSampling(c);
      ySampling = GetYSampling(c);
      }
    else {
      if ((GetXSampling(c) != xSampling) || (GetYSampling(c) != ySampling)) { errMsg = "Subsampling not supported for this file type";  goto _error; }
      }
    pChnl->chnlWidth = (width + xSampling - 1) / xSampling;
    pChnl->chnlHeight = (height + ySampling - 1) / ySampling;
    pChnl->isSigned = (bitsPerVal<0)?true:false;
    pChnl->data = NULL;
    }
  fileName = new char[strlen(filename)+1];
  if (fileName == NULL) { errMsg = "Out of memory";  goto _error; }
  strcpy(fileName,filename);
  return NULL;

  _error:  /****label****/
  fclose(f);
  UnloadHeader();
  if (pMetaItemBuf != NULL) delete []pMetaItemBuf;
  return errMsg;
}

const char *CImagePxm::LoadFileData(IMG_INT32 frameIndex) {
//read (next) image/frame from the file
  const IMG_INT8 *bayerLayoutUsed;
  const COLORMODELINFO *cmi;
  PXMLOADFORMAT *pFormat;
  PIXEL *pData[MAX_COLOR_CHANNELS];
  IMG_INT32 chWidth,chHeight,chBitdepth,chWidthDiv2,ch,x,y;
  bool isBinary;
  FILE *f;
  if (!IsHeaderLoaded()) return "File header not loaded";
  if ((frameIndex != 0) && (fileFormat != PPTYPE_PPMB) && (fileFormat != PPTYPE_PGMB))
    return "Multiple images in this file type are not supported";  //only binary PGM/PPM supports multiple frames
  if ((cmi = GetColorModelInfo(colorModel)) == NULL) return "Invalid color model";
  if ((f = fopen(fileName,"rb")) == NULL) return "Error opening file";
  //all channels are assumed to be interleaved, which means that each channel must have the same number of values
  //that implies that each channel must use the same subsampling, i.e. same color channel buffer dimensions
  chWidth = chnl[0].chnlWidth;
  chHeight = chnl[0].chnlHeight;
  chBitdepth = chnl[0].bitDepth;
  chWidthDiv2 = chWidth / 2;
#ifdef _DEBUG
  for (ch = 0; ch < cmi->Nchannels; ch++) {
    ASSERT((chWidth == chnl[ch].chnlWidth) && (chHeight == chnl[ch].chnlHeight) && (chBitdepth == chnl[ch].bitDepth));
    }
#endif
  if (!myfseeki64(f,foffsetData + frameIndex * chWidth * chHeight * cmi->Nchannels * ((chBitdepth > 8) ? 2: 1)))
    return "Failed to fseek to required frame";
  for (ch = 0; ch < cmi->Nchannels; ch++) {
    if (chnl[ch].data == NULL) {  //need to allocate buffer for this color channel
      chnl[ch].data = new PIXEL[chWidth * chHeight];
      if (chnl[ch].data == NULL) { fclose(f);  UnloadData();  return "Out of memory"; }
      }
    pData[ch] = chnl[ch].data;
    }
  pFormat = (PXMLOADFORMAT*)pExtra;
  isBinary = ((fileFormat == PPTYPE_PPMB) || (fileFormat == PPTYPE_PGMB)) ? true: false;
  //bayer format override
  bayerLayoutUsed = NULL;
  if ((colorModel == CM_RGGB) && (pFormat != NULL) && (pFormat->useBayerCells)) {
    switch (subsMode) {
      default: ASSERT(false);  //use RGGB
      case SUBS_RGGB: bayerLayoutUsed = bayerLayout2x2_0123; break;
      case SUBS_GRBG: bayerLayoutUsed = bayerLayout2x2_1032; break;
      case SUBS_GBRG: bayerLayoutUsed = bayerLayout2x2_2301; break;
      case SUBS_BGGR: bayerLayoutUsed = bayerLayout2x2_3210; break;
      }
    for (y = 0; y < height; y++) {
      for (x = 0; x < width; x++) {
        //always unsigned
        IMG_INT32 val;
        if (isBinary) {  //binary
          val = fgetc(f);
          if (chBitdepth > 8) val = (val << 8) | fgetc(f);  //file stores values in big endian
          }
        else {  //text
          int c;
          for (c = fgetc(f); !feof(f) && isspace(c); c = fgetc(f)) ;
          val = 0;
          for (; !feof(f) && isdigit(c); c = fgetc(f)) val = val * 10 + (c - '0');
          }
        if (pixelOnLoadResize) {
          if (pixelOnLoadResize > 0) val <<= pixelOnLoadResize;
          else val >>= (-pixelOnLoadResize);
          }
        //convert ch into channel based on location
        ch = bayerLayoutUsed[2 + (y%bayerLayoutUsed[1])*bayerLayoutUsed[0]+(x%bayerLayoutUsed[0])];
        *(pData[ch])++ = *(PIXEL*)(&val);  //will be <0 if val>32767
        }
      }
    }
  else {  //RGB/grayscale
    for (y = 0; y < chHeight; y++) {  //width must be the same across all channels (ASSERT-ed above)
      for (x = 0; x < chWidth; x++) {
        for (ch = 0; ch < cmi->Nchannels; ch++) {
          //read a value and store it into plane ch at x,y
          //always unsigned
          IMG_INT32 val;
          if (isBinary) {  //binary
            val = fgetc(f);
            if (chBitdepth > 8) val = (val << 8) | fgetc(f);  //file stores values in big endian
            }
          else {  //text
            int c;
            for (c = fgetc(f); !feof(f) && isspace(c); c = fgetc(f)) ;
            val = 0;
            for (; !feof(f) && isdigit(c); c = fgetc(f)) val = val * 10 + (c - '0');
            }
          if (pixelOnLoadResize) {
            if (pixelOnLoadResize > 0) val <<= pixelOnLoadResize;
            else val >>= (-pixelOnLoadResize);
            }
          *(pData[ch])++ = *(PIXEL*)(&val);  //will be <0 if val>32767
          }
        }
      }
    }
  fclose(f);
  return NULL;
}

const char *CImagePxm::SaveFileStart(const char *filename,void *pExtra,void **pSC) {
//initiates saving of a PGM/PPM file (see CImageBase::SaveFileStart() for more info)
//pExtra: must point to PXMSAVEFORMAT structure, which must be valid during the entire save process
//NOTE!! the caller must properly match the file extension PGM or PPM with PXMSAVEFORMAT.fileFormat
//returns: NULL=ok, !NULL=error message string
  PXMSAVECONTEXT *pCtx;
  (*pSC) = NULL;
  if (!IsHeaderLoaded() || !IsDataLoaded()) return "No image data to save";
  pCtx = new PXMSAVECONTEXT;
  if (pCtx == NULL) { return "Out of memory"; }
  if ((pCtx->f = fopen(filename,"wb")) == NULL) { delete pCtx;  return "Error creating file"; }
  pCtx->pExtra = (PXMSAVEFORMAT*)pExtra;
  (*pSC) = pCtx;
  return NULL;
}

const char *CImagePxm::SaveFileHeader(void *pSC) {
//write PGM/PPM file header (see CImageBase::SaveFileHeader() for more info)
  PXMSAVECONTEXT *pCtx;
  PXMSAVEFORMAT *pFormat;
  const COLORMODELINFO *cmi;
  char buf[100],formatType;
  IMG_INT32 ch,bitDepth;
  /**/
  pCtx = (PXMSAVECONTEXT*)pSC;
  pFormat = pCtx->pExtra;
  if ((cmi = GetColorModelInfo(colorModel)) == NULL) return "Invalid color model";
  switch (pFormat->fileFormat) {
    case PPTYPE_PPMB: formatType = '6'; break;
    case PPTYPE_PPMT: formatType = '3'; break;
    case PPTYPE_PGMB: formatType = '5'; break;
    case PPTYPE_PGMT: formatType = '2'; break;
    default: return "File format not supported";
    }
  //get the bit depth - use max. bitdepth across all channels
  bitDepth = chnl[0].bitDepth;
  for (ch = 1; ch < cmi->Nchannels; ch++) { if (chnl[ch].bitDepth > bitDepth) bitDepth = chnl[ch].bitDepth; }
  pCtx->maxVal = (1 << bitDepth) - 1;
  //write the header
  sprintf(buf,"P%c\n%d %d\n%d\n",formatType,width,height,pCtx->maxVal);
  if (!fwrite(buf,strlen(buf),1,pCtx->f)) return "Error writing file header";
  return NULL;
}

const char *CImagePxm::SaveFileData(void *pSC) {
//write PGM/PPM data for one image (see CImageBase::SaveFileData() for more info)
  PXMSAVECONTEXT *pCtx;
  PXMSAVEFORMAT *pFormat;
  const COLORMODELINFO *cmi;
  PIXEL *pData[MAX_COLOR_CHANNELS];
  IMG_INT32 chWidth,chHeight,chBitdepth,chWidthDiv2,NvalsInRow,ch,x,y;
  const IMG_INT8 *bayerLayoutUsed;
  /**/
  pCtx = (PXMSAVECONTEXT*)pSC;
  pFormat = pCtx->pExtra;
  if ((cmi = GetColorModelInfo(colorModel)) == NULL) return "Invalid color model";
  //all channels are assumed to be interleaved, which means that each channel must have the same number of values
  //that implies that each channel must use the same subsampling, i.e. same color channel buffer dimensions
  chWidth = chnl[0].chnlWidth;
  chHeight = chnl[0].chnlHeight;
  chBitdepth = chnl[0].bitDepth;
  chWidthDiv2 = chWidth / 2;
#ifdef _DEBUG
  for (ch = 0; ch < cmi->Nchannels; ch++) {
    ASSERT((chWidth == chnl[ch].chnlWidth) && (chHeight == chnl[ch].chnlHeight) && (chBitdepth == chnl[ch].bitDepth));
    }
#endif
  for (ch = 0; ch < cmi->Nchannels; ch++) pData[ch] = chnl[ch].data;
  bayerLayoutUsed = NULL;
  if ((colorModel == CM_RGGB) && (pFormat->useBayerCells)) {
    switch (subsMode) {
      case SUBS_RGGB: bayerLayoutUsed = bayerLayout2x2_0123; break;
      case SUBS_GRBG: bayerLayoutUsed = bayerLayout2x2_1032; break;
      case SUBS_GBRG: bayerLayoutUsed = bayerLayout2x2_2301; break;
      case SUBS_BGGR: bayerLayoutUsed = bayerLayout2x2_3210; break;
      default: ASSERT(false); break;
      }
    }
  if (bayerLayoutUsed != NULL) {
    //use bayer cells layout
    IMG_INT32 xs[MAX_COLOR_CHANNELS],ys[MAX_COLOR_CHANNELS];
    for (ch = 0; ch < cmi->Nchannels; ch++) {
      xs[ch] = GetXSampling(ch);
      ys[ch] = GetYSampling(ch);
      }
    for (y = 0; y < height; y++) {
      for (ch = 0; ch < cmi->Nchannels; ch++)
        pData[ch] = chnl[ch].data + (y/ys[ch])*chnl[ch].chnlWidth;
      NvalsInRow = 0;
      for (x = 0; x < width; x++) {
        PIXEL pix;
        //figure out the channel from position
        ch = bayerLayoutUsed[2 + (y%bayerLayoutUsed[1])*bayerLayoutUsed[0]+(x%bayerLayoutUsed[0])];
        pix = pData[ch][x/xs[ch]];
        if ((pFormat->fileFormat == PPTYPE_PPMB) || (pFormat->fileFormat == PPTYPE_PGMB)) {
          //store a value from plane ch at x,y
          //always unsigned
          union u { PIXEL p; IMG_UINT8 b[3]; } val;
          val.p = pix;
          //write 1 or 2 bytes (MSByte first)
          if (chBitdepth > 8) {
            val.b[2]=val.b[0];  //reverse byte order [0],[1] -> [1],[2]
            fwrite(val.b+1,2,1,pCtx->f);
            }
          else fwrite(val.b+0,1,1,pCtx->f);
          }
        else {  //text
          char buf[6],*pBuf;  //16-bit unsigned int = max 5 decimal digits + ' '
          IMG_INT32 val;
          val = (IMG_INT32)pix;
          ASSERT(sizeof(PIXEL) == 2);  //7fff below
          if (val < 0) val = *(IMG_UINT16*)(&val);  //wrap around as unsigned (esp. for 16-bit)
          buf[5] = ' ';
          if (++NvalsInRow >= 10) { buf[5] = '\n';  NvalsInRow = 0; }
          for (pBuf = buf+4; pBuf >= buf; pBuf--) {
            *pBuf = (val % 10) + '0';
            if ((val /= 10) == 0) break;
            }
          fwrite(pBuf,sizeof(buf) - (pBuf-buf),1,pCtx->f);
          }
        }
      if ((pFormat->fileFormat == PPTYPE_PPMT) || (pFormat->fileFormat == PPTYPE_PGMT))
        fputc('\n',pCtx->f);
      }
    }
  else {
    //use layout with interleaved channels in raster scan order
    if ((pFormat->fileFormat == PPTYPE_PPMB) || (pFormat->fileFormat == PPTYPE_PGMB)) {
      //binary format
      for (y = 0; y < chHeight; y++) {  //width must be the same across all channels (ASSERT-ed above)
        for (x = 0; x < chWidth; x++) {
          for (ch = 0; ch < cmi->Nchannels; ch++) {
            //store a value from plane ch at x,y
            //always unsigned
            union u { PIXEL p; IMG_UINT8 b[3]; } val;
            val.p = *(pData[ch])++;
            //write 1 or 2 bytes (MSByte first)
            if (chBitdepth > 8) {
              val.b[2]=val.b[0];  //reverse byte order [0],[1] -> [1],[2]
              fwrite(val.b+1,2,1,pCtx->f);
              }
            else fwrite(val.b+0,1,1,pCtx->f);
            }
          }
        }
      }
    else {
      //text format
      for (y = 0; y < chHeight; y++) {
        NvalsInRow = 0;
        for (x = 0; x < chWidth; x++) {
          for (ch = 0; ch < cmi->Nchannels; ch++) {
            //store a value from plane ch at x,y
            //always unsigned
            char buf[6],*pBuf;  //16-bit unsigned int = max 5 decimal digits + ' '
            IMG_INT32 val;
            val = *(pData[ch])++;
            ASSERT(sizeof(PIXEL) == 2);  //7fff below
            if (val < 0) val = *(IMG_UINT16*)(&val);  //wrap around as unsigned (esp. for 16-bit)
            buf[5] = ' ';
            if (++NvalsInRow >= 10) { buf[5] = '\n';  NvalsInRow = 0; }
            for (pBuf = buf+4; pBuf >= buf; pBuf--) {
              *pBuf = (val % 10) + '0';
              if ((val /= 10) == 0) break;
              }
            fwrite(pBuf,sizeof(buf) - (pBuf-buf),1,pCtx->f);
            }
          }
        fputc('\n',pCtx->f);
        }
      }//text format
    }
  return NULL;
}

const char *CImagePxm::SaveFileEnd(void *pSC) {
//finish writing PGM/PPM file (see CImageBase::SaveFileEnd() for more info)
  PXMSAVECONTEXT *pCtx = (PXMSAVECONTEXT*)pSC;
  if (pCtx->f != NULL) fclose(pCtx->f);
  delete pCtx;
  return NULL;
}

const char *CImagePxm::GetSaveFormat(CImagePxm::PXMSAVEFORMAT *pFmt,CImageBase *pIm) {
//fills in the save format structure based on format of given image pIm
//returns NULL=ok, otherwise error message string
  pFmt->useBayerCells = false;
  switch (pIm->colorModel) {
    case CM_GRAY:
    case CM_RGBW:  //once CFA cells are defined this should move down to be with CM_RGGB
      pFmt->fileFormat = PPTYPE_PGMB;
      break;
    case CM_RGGB:
      pFmt->fileFormat = PPTYPE_PGMB;
      pFmt->useBayerCells = true;
      break;
    case CM_RGB:
      pFmt->fileFormat = PPTYPE_PPMB;
      break;
    case CM_YUV:
    case CM_RGBA:
    case CM_YUVA:
    default: return "Unsupported color model";
    }
  return NULL;
}

const char *CImagePxm::GetSaveFormat(PXMSAVEFORMAT *pFmt,CImageFlx *pIm) {
//fills in the save format structure based on format of given image pIm
//returns NULL=ok, otherwise error message string
  return GetSaveFormat(pFmt,(CImageBase*)pIm);
}

const char *CImagePxm::GetSaveFormat(PXMSAVEFORMAT *pFmt,CImagePxm *pIm) {
//fills in the save format structure based on format of given image pIm
//returns NULL=ok, otherwise error message string
  return GetSaveFormat(pFmt,(CImageBase*)pIm);
}

const char *CImagePxm::GetSaveFormat(PXMSAVEFORMAT *pFmt,CImageYuv *pIm) {
//fills in the save format structure based on format of given image pIm
//returns NULL=ok, otherwise error message string
  return "Unsupported color model";  //PGM/PPM files do not support YUV data
}

const char *CImagePxm::GetSaveFormat(PXMSAVEFORMAT *pFmt,CImagePlRaw *pIm) {
//fills in the save format structure based on format of given image pIm
//returns NULL=ok, otherwise error message string
  return GetSaveFormat(pFmt,(CImageBase*)pIm);
}

const char *CImagePxm::GetSaveFormat(PXMSAVEFORMAT *pFmt,CImageNRaw *pIm) {
//fills in the save format structure based on format of given image pIm
//returns NULL=ok, otherwise error message string
  return GetSaveFormat(pFmt,(CImageBase*)pIm);
}

const char *CImagePxm::GetSaveFormat(PXMSAVEFORMAT *pFmt,CImageAptinaRaw *pIm) {
//fills in the save format structure based on format of given image pIm
//returns NULL=ok, otherwise error message string
  return GetSaveFormat(pFmt,(CImageBase*)pIm);
}

const char *CImagePxm::GetSaveFormat(PXMSAVEFORMAT *pFmt,CImageBmp *pIm) {
//fills in the save format structure based on format of given image pIm
//returns NULL=ok, otherwise error message string
  return GetSaveFormat(pFmt,(CImageBase*)pIm);
}

/*-------------------------------------------------------------------------------
YUV file format*/

typedef struct yuvFormatSpecStr {
  const char *format;
  CImageBase::SUBS_xxx subsMode;
  CImageYuv::PLANEFMT_xxx planeFmt;
  bool msbAligned;
  IMG_UINT8 defaultBd;
  } YUVFORMATSPEC;
static const YUVFORMATSPEC yuvFormatSpecs[] = {
  //!! NOTE !! this must be in order such that shorter prefixes come after longer unique strings
  //p=planar, i=interleaved, p12=2 planes (Y and U+V)
  //r=reversed U & V, c=compressed, u=l=uncompressed using LSBits, m=uncompressed using MSBits 
  {"444pl",    CImageBase::SUBS_444, CImageYuv::PLANEFMT_PLANAR, false, 10 },
  {"444pu",    CImageBase::SUBS_444, CImageYuv::PLANEFMT_PLANAR, false, 10 },
  {"444pm",    CImageBase::SUBS_444, CImageYuv::PLANEFMT_PLANAR, true, 10 },
  {"444p",     CImageBase::SUBS_444, CImageYuv::PLANEFMT_PLANAR, false, 8 },
  {"444il",    CImageBase::SUBS_444, CImageYuv::PLANEFMT_INTER_YUV, false, 10 },
  {"444iu",    CImageBase::SUBS_444, CImageYuv::PLANEFMT_INTER_YUV, false, 10 },
  {"444im",    CImageBase::SUBS_444, CImageYuv::PLANEFMT_INTER_YUV, true, 10 },
  {"444i",     CImageBase::SUBS_444, CImageYuv::PLANEFMT_INTER_YUV, false, 8 },
  {"444",      CImageBase::SUBS_444, CImageYuv::PLANEFMT_INTER_YUV, false, 8 },
  //{"420p12rl", CImageBase::SUBS_420, CImageYuv::PLANEFMT_PL12VU, false, 10 },  //not supported
  //{"420p12ru", CImageBase::SUBS_420, CImageYuv::PLANEFMT_PL12VU, false, 10 },  //not supported
  //{"420p12rm", CImageBase::SUBS_420, CImageYuv::PLANEFMT_PL12VU, true, 10 },  //not supported
  //{"420p12r",  CImageBase::SUBS_420, CImageYuv::PLANEFMT_PL12VU, false, 8 },  //not supported
  //{"420p12l",  CImageBase::SUBS_420, CImageYuv::PLANEFMT_PL12UV, false, 10 },  //not supported
  //{"420p12u",  CImageBase::SUBS_420, CImageYuv::PLANEFMT_PL12UV, false, 10 },  //not supported
  //{"420p12m",  CImageBase::SUBS_420, CImageYuv::PLANEFMT_PL12UV, true, 10 },  //not supported
  //{"420p12",   CImageBase::SUBS_420, CImageYuv::PLANEFMT_PL12UV, false, 8 },  //not supported
  {"420pl",    CImageBase::SUBS_420, CImageYuv::PLANEFMT_PLANAR, false, 10 },
  {"420pu",    CImageBase::SUBS_420, CImageYuv::PLANEFMT_PLANAR, false, 10 },
  {"420pm",    CImageBase::SUBS_420, CImageYuv::PLANEFMT_PLANAR, true, 10 },
  {"420p",     CImageBase::SUBS_420, CImageYuv::PLANEFMT_PLANAR, false, 8 },
  {"420",      CImageBase::SUBS_420, CImageYuv::PLANEFMT_PLANAR, false, 8 },
  //{"422p12rl", CImageBase::SUBS_422, CImageYuv::PLANEFMT_PL12VU, false, 10 },  //not supported
  //{"422p12ru", CImageBase::SUBS_422, CImageYuv::PLANEFMT_PL12VU, false, 10 },  //not supported
  //{"422p12rm", CImageBase::SUBS_422, CImageYuv::PLANEFMT_PL12VU, true, 10 },  //not supported
  //{"422p12r",  CImageBase::SUBS_422, CImageYuv::PLANEFMT_PL12VU, false, 8 },  //not supported
  //{"422p12l",  CImageBase::SUBS_422, CImageYuv::PLANEFMT_PL12UV, false, 10 },  //not supported
  //{"422p12u",  CImageBase::SUBS_422, CImageYuv::PLANEFMT_PL12UV, false, 10 },  //not supported
  //{"422p12m",  CImageBase::SUBS_422, CImageYuv::PLANEFMT_PL12UV, true, 10 },  //not supported
  //{"422p12",   CImageBase::SUBS_422, CImageYuv::PLANEFMT_PL12UV, false, 8 },  //not supported
  {"422pl",    CImageBase::SUBS_422, CImageYuv::PLANEFMT_PLANAR, false, 10 },
  {"422pu",    CImageBase::SUBS_422, CImageYuv::PLANEFMT_PLANAR, false, 10 },
  {"422pm",    CImageBase::SUBS_422, CImageYuv::PLANEFMT_PLANAR, true, 10 },
  {"422p",     CImageBase::SUBS_422, CImageYuv::PLANEFMT_PLANAR, false, 8 },
  {"422i",     CImageBase::SUBS_422, CImageYuv::PLANEFMT_INTER_YUYV, false, 8 },
  {"422",      CImageBase::SUBS_422, CImageYuv::PLANEFMT_PLANAR, false, 8 },
  {"yuyv",     CImageBase::SUBS_422, CImageYuv::PLANEFMT_INTER_YUYV, false, 8 },
  {"yvyu",     CImageBase::SUBS_422, CImageYuv::PLANEFMT_INTER_YVYU, false, 8 },
  {"uyvy",     CImageBase::SUBS_422, CImageYuv::PLANEFMT_INTER_UYVY, false, 8 },
  {"vyuy",     CImageBase::SUBS_422, CImageYuv::PLANEFMT_INTER_VYUY, false, 8 },
  };

CImageYuv::CImageYuv() {
  Unload();
}

CImageYuv::~CImageYuv() {
  Unload();
}

void CImageYuv::UnloadHeader() {
  planeFormat = PLANEFMT_PLANAR;  //whatever
  frameSize=0;
  Nframes=0;
  CImageBase::UnloadHeader();
}

IMG_INT32 CImageYuv::GetProperty(PROPERTY_xxx prop) {
  switch (prop) {
    case PROPERTY_SUPPORT_VIDEO: return 1;
    case PROPERTY_IS_VIDEO: return (Nframes > 1)? 1: 0;
    }
  return 0;
}

const char *CImageYuv::LoadFileHeader(const char *filename,void *pExtra) {
//load file header
//A YUV file does not have a header, all the 'header' info is in pExtra
//pExtra: NULL or ptr to a YUVLOADFORMAT (which must be valid during the entire load process)
//returns NULL=ok, !NULL=error message string
  const COLORMODELINFO *cmi;
  YUVLOADFORMAT *pFmt;
  IMG_INT ch,tmp;
  FILE *f;
  pFmt = (YUVLOADFORMAT*)pExtra;
  if ((pFmt->bitDepth < 1) || (pFmt->bitDepth >= ((sizeof(PIXEL) == 2)?15:16)))
    return "Unsupported bit depth";  //max 15 if PIXEL is signed 16-bit, max 16 if PIXEL is 32-bit (we support YUV formats <=16 bit)
  colorModel = CM_YUV;
  if ((cmi = GetColorModelInfo(colorModel)) == NULL) return "Invalid color model";
  width = pFmt->width;
  height = pFmt->height;
  subsMode = pFmt->subsMode;
  planeFormat = pFmt->planeFormat;
  msbAligned = pFmt->msbAligned;
  frameSize = 0;
  for (ch = 0; ch < cmi->Nchannels; ch++) {
    chnl[ch].bitDepth = pFmt->bitDepth;
    tmp = GetXSampling(ch);
    chnl[ch].chnlWidth = (pFmt->width + tmp - 1) / tmp;
    tmp = GetYSampling(ch);
    chnl[ch].chnlHeight = (pFmt->height + tmp - 1) / tmp;
    chnl[ch].isSigned = false;
    chnl[ch].data = NULL;
    frameSize += chnl[ch].chnlWidth * chnl[ch].chnlHeight * ((chnl[ch].bitDepth + 7) / 8);
    }
  //figure out number of frames
  if ((f = fopen(filename,"rb")) == NULL) { return "Error opening file"; }
  Nframes = (IMG_INT32)(myfilelengthi64(f)/frameSize);
  fclose(f);
  if (Nframes < 1) { Unload();  return "File too short for a single frame"; }
  //save a copy of file name
  fileName = new char[strlen(filename)+1];
  if (fileName == NULL) { return "Out of memory"; }
  strcpy(fileName,filename);
  return NULL;
}

const char *CImageYuv::LoadFileData(IMG_INT32 frameIndex) {
//read (next) image/frame from the file
  const COLORMODELINFO *cmi;
  PIXEL *pData[4];  //3 for YUV, 4 for YUVA (perhaps one day...)
  FILE *f;
  IMG_INT8 rshift;
  IMG_INT32 ch,x,y;
  const char *msg;
  if (!IsHeaderLoaded()) return "File header not loaded";
  if ((frameIndex < 0) || (frameIndex >= Nframes)) return "Invalid frame index";
  if ((cmi = GetColorModelInfo(colorModel)) == NULL) return "Invalid color model";
  if ((f = fopen(fileName,"rb")) == NULL) return "Error opening file";
  myfseeki64(f,frameIndex * frameSize);
  //allocate buffers if required
  for (ch = 0; ch < cmi->Nchannels; ch++) {
    if (chnl[ch].data == NULL) {  //need to allocate buffer for this color channel
      chnl[ch].data = new PIXEL[chnl[ch].chnlWidth * chnl[ch].chnlHeight];
	  if (chnl[ch].data == NULL) { msg = "Out of memory";  goto _abort; }
      }
    pData[ch] = chnl[ch].data;
    }
  //load frame
  rshift = 0;
  if (msbAligned) rshift = (8 - (chnl[0].bitDepth&7))&7;  //assume same bit depth on all channels
  switch (planeFormat) {
    case PLANEFMT_PLANAR: {  //should work for 444,422 and 420
      char ok=1,bytes;
      bytes = (chnl[0].bitDepth + 7) / 8;  //assume same bit depth on all channels
      for (ch = 0; ch < cmi->Nchannels; ch++) {
        for (y = 0; (y < chnl[ch].chnlHeight) && ok; y++) {
          for (x = 0; (x < chnl[ch].chnlWidth) && ok; x++) {
            IMG_INT32 val = 0;
            if (!fread(&val,bytes,1,f)) ok = 0;
            *(pData[ch])++ = (PIXEL)(val >> rshift);
            }
          }
        }
      if (!ok) { msg = "Error reading frame data";  goto _abort; };
      } break;
    case PLANEFMT_INTER_YUV: {
      char ok=1,bytes;
      ASSERT(subsMode == SUBS_444);
      ASSERT((chnl[0].chnlWidth == chnl[1].chnlWidth) && (chnl[0].chnlWidth == chnl[2].chnlWidth));
      ASSERT((chnl[0].chnlHeight == chnl[1].chnlHeight) && (chnl[0].chnlHeight == chnl[2].chnlHeight));
      bytes = (chnl[0].bitDepth + 7) / 8;  //assume same bit depth on all channels
      for (y = 0; (y < height) && ok; y++) {
        for (x = 0; (x < width) && ok; x++) {
          for (ch = 0; ch < cmi->Nchannels; ch++) {
            IMG_INT32 val = 0;
            if (!fread(&val,bytes,1,f)) ok = 0;
            *(pData[ch])++ = (PIXEL)(val >> rshift);
            }
          }
        }
      if (!ok) { msg = "Error reading frame data";  goto _abort; };
      } break;
    case PLANEFMT_INTER_YUYV:
    case PLANEFMT_INTER_YVYU:
    case PLANEFMT_INTER_UYVY:
    case PLANEFMT_INTER_VYUY: {
      IMG_INT32 widthDiv2;
      IMG_UINT8 channels[4];
      char ok=1;
      if (width & 1) { msg = "Error: odd width not supported";  goto _abort; };  //could support it for YUYV and YVYU but I am lazy
      ASSERT(subsMode == SUBS_422);
      switch (planeFormat) {
        case PLANEFMT_INTER_YUYV: channels[0] = channels[2] = 0;  channels[1] = 1;  channels[3] = 2; break;
        case PLANEFMT_INTER_YVYU: channels[0] = channels[2] = 0;  channels[1] = 2;  channels[3] = 1; break;
        case PLANEFMT_INTER_UYVY: channels[1] = channels[3] = 0;  channels[0] = 1;  channels[2] = 2; break;
        case PLANEFMT_INTER_VYUY: channels[1] = channels[3] = 0;  channels[0] = 2;  channels[2] = 1; break;
        default: break; // do nothing, unreachable anyway
        }
      widthDiv2 = width / 2;
      for (y = 0; (y < height) && ok; y++) {
        //in each loop read 4 values = 2 Ys, U and V
        if (chnl[0].bitDepth <= 8) {
          IMG_UINT8 buf[4];
          for (x = 0; x < widthDiv2; x++) {
            if (!fread(&buf,4,1,f)) { ok = 0;  break; }
            *(pData[channels[0]])++ = (PIXEL)(buf[0] >> rshift);
            *(pData[channels[1]])++ = (PIXEL)(buf[1] >> rshift);
            *(pData[channels[2]])++ = (PIXEL)(buf[2] >> rshift);
            *(pData[channels[3]])++ = (PIXEL)(buf[3] >> rshift);
            }
          }
        else {
          IMG_UINT16 buf[4];
          for (x = 0; x < widthDiv2; x++) {
            if (!fread(&buf,4*2,1,f)) { ok = 0;  break; }
            *(pData[channels[0]])++ = (PIXEL)(buf[0] >> rshift);
            *(pData[channels[1]])++ = (PIXEL)(buf[1] >> rshift);
            *(pData[channels[2]])++ = (PIXEL)(buf[2] >> rshift);
            *(pData[channels[3]])++ = (PIXEL)(buf[3] >> rshift);
            }
          }
        }
      if (!ok) { msg = "Error reading frame data";  goto _abort; }
      } break;
    }
  msg = NULL;
  _abort:  /****label****/
  fclose(f);
  if (msg != NULL) UnloadData();
  return msg;
}

const char *CImageYuv::SaveFileStart(const char *filename,void *pExtra,void **pSC) {
//initiates saving of a YUV file (see CImageBase::SaveFileStart() for more info)
//pExtra: must point to YUVSAVEFORMAT structure, which must be valid during the entire save process
//returns: NULL=ok, !NULL=error message string
  YUVSAVECONTEXT *pCtx;
  (*pSC) = NULL;
  if (!IsHeaderLoaded() || !IsDataLoaded()) return "No image data to save";
  if (colorModel != CM_YUV) return "Invalid color model";
  if (chnl[0].bitDepth > 16) return "Unsupported bit depth";
  pCtx = new YUVSAVECONTEXT;
  if (pCtx == NULL) { return "Out of memory"; }
  if ((pCtx->f = fopen(filename,"wb")) == NULL) { delete pCtx;  return "Error creating file"; }
  pCtx->pExtra = (YUVSAVEFORMAT*)pExtra;
  (*pSC) = pCtx;
  return NULL;
}

const char *CImageYuv::SaveFileHeader(void *pSC) {
//write YUV file header (see CImageBase::SaveFileHeader() for more info)
//there is no file header for YUV so do nothing
  return NULL;
}

const char *CImageYuv::SaveFileData(void *pSC) {
//write YUV data for one image (see CImageBase::SaveFileData() for more info)
  YUVSAVECONTEXT *pCtx;
  YUVSAVEFORMAT *pFormat;
  const COLORMODELINFO *cmi;
  PIXEL *pData[MAX_COLOR_CHANNELS],valOff[MAX_COLOR_CHANNELS];
  IMG_INT32 ch,x,y;
  IMG_INT8 lshift;
  /**/
  pCtx = (YUVSAVECONTEXT*)pSC;
  pFormat = pCtx->pExtra;
  if ((cmi = GetColorModelInfo(colorModel)) == NULL) return "Invalid color model";
  //save frame
  lshift = 0;
  if (pFormat->msbAligned) lshift = (8 - (chnl[0].bitDepth&7))&7;  //assume same bit depth on all channels
  for (ch = 0; ch < cmi->Nchannels; ch++) {
    pData[ch] = chnl[ch].data;
    valOff[ch] = (chnl[ch].isSigned ? (1<<(chnl[0].bitDepth-1)) : 0);
    }
  switch (pFormat->planeFormat) {
    case PLANEFMT_PLANAR: {  //should work for 444,422 and 420
      char ok=1,bytes;
      bytes = (chnl[0].bitDepth + 7) / 8;  //assume same bit depth on all channels
      for (ch = 0; ch < cmi->Nchannels; ch++) {
        for (y = 0; (y < chnl[ch].chnlHeight) && ok; y++) {
          for (x = 0; (x < chnl[ch].chnlWidth) && ok; x++) {
            IMG_INT32 val;
            val = *(pData[ch])++;
            val = (IMG_INT32)(val + valOff[ch]) << lshift;
            if (!fwrite(&val,bytes,1,pCtx->f)) ok = 0;
            }
          }
        }
      if (!ok) return "Error writing frame data";
      } break;
    case PLANEFMT_INTER_YUV: {
      char ok=1,bytes;
      ASSERT(subsMode == SUBS_444);
      ASSERT((chnl[0].chnlWidth == chnl[1].chnlWidth) && (chnl[0].chnlWidth == chnl[2].chnlWidth));
      ASSERT((chnl[0].chnlHeight == chnl[1].chnlHeight) && (chnl[0].chnlHeight == chnl[2].chnlHeight));
      bytes = (chnl[0].bitDepth + 7) / 8;  //assume same bit depth on all channels
      for (y = 0; (y < height) && ok; y++) {
        for (x = 0; (x < width) && ok; x++) {
          for (ch = 0; ch < cmi->Nchannels; ch++) {
            IMG_INT32 val;
            val = *(pData[ch])++;
            val = (IMG_INT32)(val + valOff[ch]) << lshift;
            if (!fwrite(&val,bytes,1,pCtx->f)) ok = 0;
            }
          }
        }
      if (!ok) return "Error writing frame data";
      } break;
    case PLANEFMT_INTER_YUYV:
    case PLANEFMT_INTER_YVYU:
    case PLANEFMT_INTER_UYVY:
    case PLANEFMT_INTER_VYUY: {
      IMG_INT32 widthDiv2;
      IMG_UINT8 channels[4];
      char ok=1;
      if (width & 1) return "Error: odd width not supported";  //could support it for YUYV and YVYU but I am lazy
      ASSERT(subsMode == SUBS_422);
      switch (pFormat->planeFormat) {
        case PLANEFMT_INTER_YUYV: channels[0] = channels[2] = 0;  channels[1] = 1;  channels[3] = 2; break;
        case PLANEFMT_INTER_YVYU: channels[0] = channels[2] = 0;  channels[1] = 2;  channels[3] = 1; break;
        case PLANEFMT_INTER_UYVY: channels[1] = channels[3] = 0;  channels[0] = 1;  channels[2] = 2; break;
        case PLANEFMT_INTER_VYUY: channels[1] = channels[3] = 0;  channels[0] = 2;  channels[2] = 1; break;
        default: break; // do nothing, unreachable anyway
        }
      widthDiv2 = width / 2;
      for (y = 0; (y < height) && ok; y++) {
        //in each loop write 4 values = 2 Ys, U and V
        if (chnl[0].bitDepth <= 8) {
          IMG_UINT8 buf[4];
          for (x = 0; x < widthDiv2; x++) {
            buf[0] = (IMG_UINT8)( *(pData[channels[0]])++ + valOff[channels[0]] ) << lshift;
            buf[1] = (IMG_UINT8)( *(pData[channels[1]])++ + valOff[channels[1]] ) << lshift;
            buf[2] = (IMG_UINT8)( *(pData[channels[2]])++ + valOff[channels[2]] ) << lshift;
            buf[3] = (IMG_UINT8)( *(pData[channels[3]])++ + valOff[channels[3]] ) << lshift;
            if (!fwrite(&buf,4,1,pCtx->f)) { ok = 0;  break; }
            }
          }
        else {
          IMG_UINT16 buf[4];
          for (x = 0; x < widthDiv2; x++) {
            buf[0] = (IMG_UINT16)( *(pData[channels[0]])++ + valOff[channels[0]] ) << lshift;
            buf[1] = (IMG_UINT16)( *(pData[channels[1]])++ + valOff[channels[1]] ) << lshift;
            buf[2] = (IMG_UINT16)( *(pData[channels[2]])++ + valOff[channels[2]] ) << lshift;
            buf[3] = (IMG_UINT16)( *(pData[channels[3]])++ + valOff[channels[3]] ) << lshift;
            if (!fwrite(&buf,4*2,1,pCtx->f)) { ok = 0;  break; }
            }
          }
        }
      if (!ok) return "Error writing frame data";
      } break;
    }
  return NULL;
}

const char *CImageYuv::SaveFileEnd(void *pSC) {
//finish writing PGM/PPM file (see CImageBase::SaveFileEnd() for more info)
  YUVSAVECONTEXT *pCtx = (YUVSAVECONTEXT*)pSC;
  if (pCtx->f != NULL) fclose(pCtx->f);
  delete pCtx;
  return NULL;
}

const char *CImageYuv::GetLoadFormat(YUVLOADFORMAT *pFmt,const char *filename,const char **pFormatPart) {
//construct a YUVLOADFORMAT structure from YUV file name a'la YUVViewer:
//  <name>_<width>x<height>_<format>_<bitdepth>.yuv
//pFormatPart: if !NULL will be set to beginning of the format information in filename (or to NULL if not found)
//returns NULL=ok, otherwise error message string
  const YUVFORMATSPEC *pFmtType;
  char *fncopy,*pFnOnly,*c;
  int formatOff;
  IMG_UINT i;
  memset(pFmt,0,sizeof(YUVLOADFORMAT));
  if (pFormatPart!=NULL) (*pFormatPart)=NULL;
  //need a copy so that the name can be modified
  fncopy = new char[strlen(filename)+1];
  if (fncopy == NULL) { return "Out of memory"; }
  strcpy(fncopy,filename);
  for (c = fncopy; *c; c++) *c = tolower(*c);	// strlwr(fncopy); not avail in Linux apparently
  //look for "<width>x<height>" and "<N>-bit" or "<N>bit" in filename
  formatOff = -1;
  for (c=fncopy;*c;) {
    if (isdigit(*c)) {
      IMG_INT w,h,n;
      char *start = c;
      n=-1; sscanf(c,"%d%n",&w,&n);
      if (n<1) continue;
      c+=n;
      if ((strncmp(c,"bit",3) == 0) || (strncmp(c,"-bit",4) == 0)) {  //looks like bit depth
        if ((w > 0) && (w < 16)) pFmt->bitDepth = w;
        if ((formatOff < 0) || (formatOff > start - fncopy)) formatOff = start - fncopy;
        continue;
        }
      if (((*c!='x') && (*c!='X')) || !isdigit(c[1])) continue;
      n=-1; sscanf(++c,"%d%n",&h,&n);
      if ((n<1) || (w<1) || (h<1) || (w>10000) || (h>10000)) continue;  //sanity
      c+=n;
      //we have width & height
      pFmt->width = w;
      pFmt->height = h;
      //overwrite width x height with spaces to prevent false format match with a width/height of 444,422,420
      memset(start,' ',c-start);
      if ((formatOff < 0) || (formatOff > start - fncopy)) formatOff = start - fncopy;
      continue;
      }
    c++;
    }
  if ((pFmt->width < 1) || (pFmt->height < 1)) {
    delete []fncopy;
    return "Unable to detect YUV format from file name";
    }
  //look for format
  pFmtType=NULL;
  if ((pFnOnly=strrchr(fncopy,'\\'))==NULL) pFnOnly=strrchr(fncopy,'/');
  if (pFnOnly==NULL) pFnOnly=fncopy; else pFnOnly++;
  for (i=0;i<SizeofArray(yuvFormatSpecs);i++) {
    if ((c=strstr(pFnOnly,yuvFormatSpecs[i].format))!=NULL) {
      pFmtType=yuvFormatSpecs+i;
      if ((formatOff < 0) || (formatOff > c - fncopy)) formatOff = c - fncopy;
      break;
      }
    }
  //bit depth has either been found in file name or is still 0 in which case set default
  if (pFmt->bitDepth < 1) {
    pFmt->bitDepth = ((pFmtType == NULL) ? 8: pFmtType->defaultBd);  //default bit depth
    }
  if (pFmtType == NULL) {  //set default format
    pFmt->planeFormat = PLANEFMT_PLANAR;
    pFmt->subsMode = SUBS_420;
    pFmt->msbAligned = false;
    }
  else {
    pFmt->planeFormat = pFmtType->planeFmt;
    pFmt->subsMode = pFmtType->subsMode;
    pFmt->msbAligned = pFmtType->msbAligned;
    }
  if ((pFormatPart != NULL) && (formatOff >= 0)) (*pFormatPart) = filename + formatOff;
  delete []fncopy;
  return NULL;
}

const char *CImageYuv::GetFormatString(YUVSAVEFORMAT *pFmt) {
//returns a string identifying YUV format (for filename during saving)
  static char buf[50];
  IMG_UINT32 i;
  for (i = 0; i < SizeofArray(yuvFormatSpecs); i++) {
    if ((yuvFormatSpecs[i].subsMode == subsMode)
        && (yuvFormatSpecs[i].planeFmt == pFmt->planeFormat)
        && (yuvFormatSpecs[i].msbAligned == pFmt->msbAligned)
        && (yuvFormatSpecs[i].defaultBd == ((chnl[0].bitDepth <= 8) ? 8 : 10))) break;
    }
  _snprintf(buf,sizeof(buf)-20,"%dx%d_%dbit",(IMG_INT)width,(IMG_INT)height,(IMG_INT)chnl[0].bitDepth);
  buf[sizeof(buf)-20] = '\0';  //leave 20 space for the format identifier
  if (i <= SizeofArray(yuvFormatSpecs)) {
    strcat(buf,"_");
    strcat(buf,yuvFormatSpecs[i].format);
    }
  return buf;
}

const char *CImageYuv::GetSaveFormat(YUVSAVEFORMAT *pFmt,CImageBase *pIm) {
//fills in the save format structure based on format of given image pIm
//returns NULL=ok, otherwise error message string
  if (pIm->colorModel != CM_YUV) return "Unsupported color model";
  pFmt->planeFormat = PLANEFMT_PLANAR;
  pFmt->msbAligned = false;
  return NULL;
}

const char *CImageYuv::GetSaveFormat(YUVSAVEFORMAT *pFmt,CImageFlx *pIm) {
//fills in the save format structure based on format of given image pIm
//returns NULL=ok, otherwise error message string
  IMG_INT32 tmp;
  if (pIm->colorModel != CM_YUV) return "Unsupported color model";
  //choose planar or interleaved format - if the source FLX was interleaved, prefer interleaved
  pFmt->planeFormat = PLANEFMT_PLANAR;
  tmp = pIm->GetNFilePlanes();
  if ((pIm->subsMode == SUBS_444) && (tmp == 1)) pFmt->planeFormat = PLANEFMT_INTER_YUV;
  else if ((pIm->subsMode == SUBS_422) && (tmp == 1)) pFmt->planeFormat = PLANEFMT_INTER_YUYV;
  //save aligned in LSBits
  pFmt->msbAligned = false;
  return NULL;
}

const char *CImageYuv::GetSaveFormat(YUVSAVEFORMAT *pFmt,CImagePxm *pIm) {
//fills in the save format structure based on format of given image pIm
//returns NULL=ok, otherwise error message string
  return "Unsupported color model";  //YUV doesn't support PGM/PPM data (grayscale,bayer or RGB)
}

const char *CImageYuv::GetSaveFormat(YUVSAVEFORMAT *pFmt,CImageYuv *pIm) {
//fills in the save format structure based on format of given image pIm
//returns NULL=ok, otherwise error message string
  pFmt->planeFormat = pIm->planeFormat;
  pFmt->msbAligned = pIm->msbAligned;
  return NULL;
}

const char *CImageYuv::GetSaveFormat(YUVSAVEFORMAT *pFmt,CImagePlRaw *pIm) {
//fills in the save format structure based on format of given image pIm
//returns NULL=ok, otherwise error message string
  return GetSaveFormat(pFmt,(CImageBase*)pIm);
}

const char *CImageYuv::GetSaveFormat(YUVSAVEFORMAT *pFmt,CImageNRaw *pIm) {
//fills in the save format structure based on format of given image pIm
//returns NULL=ok, otherwise error message string
  return GetSaveFormat(pFmt,(CImageBase*)pIm);
}

const char *CImageYuv::GetSaveFormat(YUVSAVEFORMAT *pFmt,CImageAptinaRaw *pIm) {
//fills in the save format structure based on format of given image pIm
//returns NULL=ok, otherwise error message string
  return GetSaveFormat(pFmt,(CImageBase*)pIm);
}

const char *CImageYuv::GetSaveFormat(YUVSAVEFORMAT *pFmt,CImageBmp *pIm) {
//fills in the save format structure based on format of given image pIm
//returns NULL=ok, otherwise error message string
  return GetSaveFormat(pFmt,(CImageBase*)pIm);
}

/*-------------------------------------------------------------------------------
plain raw file format*/

typedef struct plrawFormatSpecStr {
  const char *format;
  CImageBase::CM_xxx colorModel;
  CImageBase::SUBS_xxx subsMode;
  IMG_UINT8 defaultBd;
  } PLRAWFORMATSPEC;
static const PLRAWFORMATSPEC plrawFormatSpecs[] = {
  //!! NOTE !! this must be in order such that shorter prefixes come after longer unique strings
  {"rggb",    CImageBase::CM_RGGB, CImageBase::SUBS_RGGB, 8 },
  {"grbg",    CImageBase::CM_RGGB, CImageBase::SUBS_GRBG, 8 },
  {"gbrg",    CImageBase::CM_RGGB, CImageBase::SUBS_GBRG, 8 },
  {"bggr",    CImageBase::CM_RGGB, CImageBase::SUBS_BGGR, 8 },
  {"rgb",     CImageBase::CM_RGB, CImageBase::SUBS_UNDEF, 8 },
  };

CImagePlRaw::CImagePlRaw() {
  Unload();
}

CImagePlRaw::~CImagePlRaw() {
  Unload();
}

void CImagePlRaw::UnloadHeader() {
  frameSize = 0;
  CImageBase::UnloadHeader();
}

const char *CImagePlRaw::LoadFileHeader(const char *filename,void *pExtra) {
//load file header
//A plain raw file does not have a header (surprise!), all the 'header' info is in pExtra
//pExtra: NULL or ptr to a PLRAWLOADFORMAT (which must be valid during the entire load process)
//returns NULL=ok, !NULL=error message string
  const COLORMODELINFO *cmi;
  PLRAWLOADFORMAT *pFmt;
  IMG_INT ch,tmp;
  pFmt = (PLRAWLOADFORMAT*)pExtra;
  if ((pFmt->bitDepth < 1) || (pFmt->bitDepth >= ((sizeof(PIXEL) == 2)?15:31)))
    return "Unsupported bit depth";
  colorModel = pFmt->colorModel;
  if ((cmi = GetColorModelInfo(colorModel)) == NULL) return "Invalid color model";
  width = pFmt->width;
  height = pFmt->height;
  subsMode = pFmt->subsMode;
  frameSize = 0;
  for (ch = 0; ch < cmi->Nchannels; ch++) {
    chnl[ch].bitDepth = pFmt->bitDepth;
    tmp = GetXSampling(ch);
    chnl[ch].chnlWidth = (pFmt->width + tmp - 1) / tmp;
    tmp = GetYSampling(ch);
    chnl[ch].chnlHeight = (pFmt->height + tmp - 1) / tmp;
    chnl[ch].isSigned = false;
    chnl[ch].data = NULL;
    frameSize += chnl[ch].chnlWidth * chnl[ch].chnlHeight * ((chnl[ch].bitDepth + 7) / 8);
    }
  fileName = new char[strlen(filename)+1];
  if (fileName == NULL) { return "Out of memory"; }
  strcpy(fileName,filename);
  return NULL;
}

const char *CImagePlRaw::LoadFileData(IMG_INT32 frameIndex) {
//read (next) image/frame from the file
  const COLORMODELINFO *cmi;
  PIXEL *pData[4];
  FILE *f;
  IMG_INT32 ch,x,y;
  const char *msg;
  if (!IsHeaderLoaded()) return "File header not loaded";
  if ((cmi = GetColorModelInfo(colorModel)) == NULL) return "Invalid color model";
  if ((f = fopen(fileName,"rb")) == NULL) return "Error opening file";
  myfseeki64(f,frameIndex * frameSize);
  //allocate buffers if required
  for (ch = 0; ch < cmi->Nchannels; ch++) {
    if (chnl[ch].data == NULL) {  //need to allocate buffer for this color channel
      chnl[ch].data = new PIXEL[chnl[ch].chnlWidth * chnl[ch].chnlHeight];
      if (chnl[ch].data == NULL) { msg = "Out of memory";  goto _abort; }
      }
    pData[ch] = chnl[ch].data;
    }
  //load frame
  switch (colorModel) {
    case CM_RGB: {
      char ok=1,bytes;
      bytes = (chnl[0].bitDepth + 7) / 8;  //assume same bit depth on all channels
      for (y = 0; (y < height) && ok; y++) {
        for (x = 0; (x < width) && ok; x++) {
          for (ch = 0; ch < cmi->Nchannels; ch++) {
            IMG_INT32 val = 0;
            if (!fread(&val,bytes,1,f)) ok = 0;
            *(pData[ch])++ = (PIXEL)val;
            }
          }
        }
      if (!ok) { msg = "Error reading frame data";  goto _abort; };
      } break;
    case CM_RGGB: {
      char ok=1,bytes;
      IMG_UINT8 flip;
      ASSERT((chnl[0].chnlWidth == chnl[1].chnlWidth) && (chnl[0].chnlWidth == chnl[2].chnlWidth) && (chnl[0].chnlWidth == chnl[3].chnlWidth));
      ASSERT((chnl[0].chnlHeight == chnl[1].chnlHeight) && (chnl[0].chnlHeight == chnl[2].chnlHeight) && (chnl[0].chnlHeight == chnl[3].chnlHeight));
      bytes = (chnl[0].bitDepth + 7) / 8;  //assume same bit depth on all channels
      switch (subsMode) {
        case SUBS_RGGB: flip=0; break;
        case SUBS_GRBG: flip=1; break;
        case SUBS_GBRG: flip=2; break;
        case SUBS_BGGR: flip=3; break;
        default: ASSERT(false); msg = "Unsupported pattern"; goto _abort;
        }
      for (y = 0; (y < height) && ok; y++) {
        for (x = 0; x < width; x++) {
          IMG_INT32 val = 0;
          ch=(((y&1)<<1)|(x&1))^flip;
          if (!fread(&val,bytes,1,f)) { ok = 0;  break; }
          *(pData[ch])++ = (PIXEL)val;
          }
        }
      if (!ok) { msg = "Error reading frame data";  goto _abort; };
      } break;
    default: ASSERT(false); msg = "Unsupported color model"; goto _abort;
    }
  msg = NULL;
  _abort:  /****label****/
  fclose(f);
  if (msg != NULL) UnloadData();
  return msg;
}

const char *CImagePlRaw::SaveFileStart(const char *filename,void *pExtra,void **pSC) {
//initiates saving of a plain raw file (see CImageBase::SaveFileStart() for more info)
//pExtra: must point to PLRAWSAVEFORMAT structure, which must be valid during the entire save process
//returns: NULL=ok, !NULL=error message string
  PLRAWSAVECONTEXT *pCtx;
  (*pSC) = NULL;
  if (!IsHeaderLoaded() || !IsDataLoaded()) return "No image data to save";
  if ((colorModel != CM_RGB) && (colorModel != CM_RGGB)) return "Unsupported color model";
  pCtx = new PLRAWSAVECONTEXT;
  if (pCtx == NULL) { return "Out of memory"; }
  if ((pCtx->f = fopen(filename,"wb")) == NULL) { delete pCtx;  return "Error creating file"; }
  pCtx->pExtra = (PLRAWSAVEFORMAT*)pExtra;
  (*pSC) = pCtx;
  return NULL;
}

const char *CImagePlRaw::SaveFileHeader(void *pSC) {
//write plain raw file header (see CImageBase::SaveFileHeader() for more info)
//there is no file header for plain raw so do nothing
  return NULL;
}

const char *CImagePlRaw::SaveFileData(void *pSC) {
//write plain raw data for one image (see CImageBase::SaveFileData() for more info)
  PLRAWSAVECONTEXT *pCtx;
  PLRAWSAVEFORMAT *pFormat;
  const COLORMODELINFO *cmi;
  PIXEL *pData[MAX_COLOR_CHANNELS],valOff[MAX_COLOR_CHANNELS];
  double valMul[MAX_COLOR_CHANNELS];
  IMG_INT32 ch,x,y,valMax;
  /**/
  pCtx = (PLRAWSAVECONTEXT*)pSC;
  pFormat = pCtx->pExtra;
  if ((cmi = GetColorModelInfo(colorModel)) == NULL) return "Invalid color model";
  //save frame
  valMax=(1l<<pFormat->bitDepth)-1;
  for (ch = 0; ch < cmi->Nchannels; ch++) {
    pData[ch] = chnl[ch].data;
    valOff[ch] = -pFormat->rangeMin[ch];
    valMul[ch] = valMax/(pFormat->rangeMax[ch]-pFormat->rangeMin[ch]);
    }
  switch (colorModel) {
    case CM_RGB: {
      char ok=1,bytes;
      bytes = (pFormat->bitDepth + 7) / 8;  //assume same bit depth on all channels
      for (y = 0; (y < height) && ok; y++) {
        for (x = 0; (x < width) && ok; x++) {
          for (ch = 0; ch < cmi->Nchannels; ch++) {
            IMG_INT32 val;
            val = *(pData[ch])++;
            val = (IMG_INT32)((val + valOff[ch])*valMul[ch]);
            if (val<0) val=0;
            if (val>valMax) val=valMax;
            if (!fwrite(&val,bytes,1,pCtx->f)) ok = 0;
            }
          }
        }
      if (!ok) return "Error writing frame data";
      } break;
    case CM_RGGB: {
      char ok=1,bytes;
      IMG_UINT8 flip;
      ASSERT((chnl[0].chnlWidth == chnl[1].chnlWidth) && (chnl[0].chnlWidth == chnl[2].chnlWidth) && (chnl[0].chnlWidth == chnl[3].chnlWidth));
      ASSERT((chnl[0].chnlHeight == chnl[1].chnlHeight) && (chnl[0].chnlHeight == chnl[2].chnlHeight) && (chnl[0].chnlHeight == chnl[3].chnlHeight));
      bytes = (chnl[0].bitDepth + 7) / 8;  //assume same bit depth on all channels
      switch (subsMode) {
        case SUBS_RGGB: flip=0; break;
        case SUBS_GRBG: flip=1; break;
        case SUBS_GBRG: flip=2; break;
        case SUBS_BGGR: flip=3; break;
        default: ASSERT(false); return "Unsupported pattern";
        }
      for (y = 0; (y < height) && ok; y++) {
        for (x = 0; x < width; x++) {
          IMG_INT32 val;
          ch=(((y&1)<<1)|(x&1))^flip;
          val = *(pData[ch])++;
          val = (IMG_INT32)((val + valOff[ch])*valMul[ch]);
          if (val<0) val=0;
          if (val>valMax) val=valMax;
          if (!fwrite(&val,bytes,1,pCtx->f)) { ok = 0;  break; }
          }
        }
      if (!ok) return "Error writing frame data";
      } break;
    default: ASSERT(false); return "Unsupported color model";
    }
  return NULL;
}

const char *CImagePlRaw::SaveFileEnd(void *pSC) {
//finish writing PGM/PPM file (see CImageBase::SaveFileEnd() for more info)
  PLRAWSAVECONTEXT *pCtx = (PLRAWSAVECONTEXT*)pSC;
  if (pCtx->f != NULL) fclose(pCtx->f);
  delete pCtx;
  return NULL;
}

const char *CImagePlRaw::GetLoadFormat(PLRAWLOADFORMAT *pFmt,const char *filename,const char **pFormatPart) {
//construct a PLRAWLOADFORMAT structure from plain raw file name
//  <name>_<width>x<height>_<format>_<bitdepth>.raw
//pFormatPart: if !NULL will be set to beginning of the format information in filename (or to NULL if not found)
//returns NULL=ok, otherwise error message string
  const PLRAWFORMATSPEC *pFmtType;
  char *fncopy,*pFnOnly,*c;
  int formatOff;
  IMG_UINT i;
  memset(pFmt,0,sizeof(PLRAWLOADFORMAT));
  if (pFormatPart!=NULL) (*pFormatPart)=NULL;
  //need a copy so that the name can be modified
  fncopy = new char[strlen(filename)+1];
  if (fncopy == NULL) { return "Out of memory"; }
  strcpy(fncopy,filename);
  for (c=fncopy;*c;c++) *c=tolower(*c);  //strlwr(fncopy); is not avail on Linux
  //look for "<width>x<height>" and "<N>-bit" or "<N>bit" in filename
  formatOff = -1;
  for (c=fncopy;*c;) {
    if (isdigit(*c)) {
      IMG_INT w,h,n;
      char *start = c;
      n=-1; sscanf(c,"%d%n",&w,&n);
      if (n<1) continue;
      c+=n;
      if ((strncmp(c,"bit",3) == 0) || (strncmp(c,"-bit",4) == 0)) {  //looks like bit depth
        if ((w > 0) && (w < 16)) pFmt->bitDepth = w;
        if ((formatOff < 0) || (formatOff > start - fncopy)) formatOff = start - fncopy;
        continue;
        }
      if (((*c!='x') && (*c!='X')) || !isdigit(c[1])) continue;
      n=-1; sscanf(++c,"%d%n",&h,&n);
      if ((n<1) || (w<1) || (h<1) || (w>10000) || (h>10000)) continue;  //sanity
      c+=n;
      //we have width & height
      pFmt->width = w;
      pFmt->height = h;
      //overwrite width x height with spaces to prevent false format match with a width/height of 444,422,420
      memset(start,' ',c-start);
      if ((formatOff < 0) || (formatOff > start - fncopy)) formatOff = start - fncopy;
      continue;
      }
    c++;
    }
  if ((pFmt->width < 1) || (pFmt->height < 1)) {
    delete []fncopy;
    return "Unable to detect plain raw format from file name";
    }
  //look for format
  pFmtType=NULL;
  //if it's .RGB extension look up rgb format else scan file name
  if ((pFnOnly=strrchr(fncopy,'\\'))==NULL) pFnOnly=strrchr(fncopy,'/');
  if (pFnOnly==NULL) pFnOnly=fncopy; else pFnOnly++;
  if (((c=strrchr(pFnOnly,'.'))!=NULL) && (stricmp(c,".rgb")==0)) {
    for (i=0;i<SizeofArray(plrawFormatSpecs);i++) { if (plrawFormatSpecs[i].colorModel==CM_RGB) break; }
    }
  else {
    for (i=0;i<SizeofArray(plrawFormatSpecs);i++) {
      if (((c=strstr(pFnOnly,plrawFormatSpecs[i].format))!=NULL)
          && (c>pFnOnly) && !isalpha(c[-1]) && !isalpha(c[strlen(plrawFormatSpecs[i].format)])) {  //check for whole word only
        if ((formatOff < 0) || (formatOff > c - fncopy)) formatOff = c - fncopy;
        break;
        }
      }
    }
  if (i<SizeofArray(plrawFormatSpecs)) pFmtType=plrawFormatSpecs+i;
  //bit depth has either been found in file name or is still 0 in which case set default
  if (pFmt->bitDepth < 1) {
    pFmt->bitDepth = ((pFmtType == NULL) ? 8: pFmtType->defaultBd);  //default bit depth
    }
  if (pFmtType == NULL) {  //set default format
    pFmt->colorModel=CM_RGB;
    pFmt->subsMode=SUBS_UNDEF;
    }
  else {
    pFmt->colorModel=pFmtType->colorModel;
    pFmt->subsMode=pFmtType->subsMode;
    }
  if ((pFormatPart != NULL) && (formatOff >= 0)) (*pFormatPart) = filename + formatOff;
  delete []fncopy;
  return NULL;
}

const char *CImagePlRaw::GetFormatString(PLRAWSAVEFORMAT *pFmt) {
//returns a string identifying plain raw format (for filename during saving)
  static char buf[50];
  IMG_UINT32 i;
  for (i = 0; i < SizeofArray(plrawFormatSpecs); i++) {
    if ((plrawFormatSpecs[i].colorModel == colorModel)
        && (plrawFormatSpecs[i].subsMode == subsMode)) break;
    }
  _snprintf(buf,sizeof(buf)-20,"%dx%d_%dbit",(IMG_INT)width,(IMG_INT)height,(IMG_INT)pFmt->bitDepth);
  buf[sizeof(buf)-20] = '\0';  //leave 20 space for the format identifier
  if (i <= SizeofArray(plrawFormatSpecs)) {
    strcat(buf,"_");
    strcat(buf,plrawFormatSpecs[i].format);
    }
  return buf;
}

const char *CImagePlRaw::GetSaveFormat(PLRAWSAVEFORMAT *pFmt,CImageBase *pIm) {
//fills in the save format structure based on format of given image pIm
//returns NULL=ok, otherwise error message string
  IMG_INT32 ch;
  if ((pIm->colorModel != CM_RGB) && (pIm->colorModel != CM_RGGB)) return "Unsupported color model";
  //figure out maximum bit depth across channels
  pFmt->bitDepth=pIm->chnl[0].bitDepth;
  for (ch=1; ch<pIm->GetNColChannels(); ch++) {
    if (pIm->chnl[ch].bitDepth>pFmt->bitDepth) pFmt->bitDepth = pIm->chnl[ch].bitDepth;
    pFmt->rangeMin[ch]=pIm->GetChMin(ch);
    pFmt->rangeMax[ch]=pIm->GetChMax(ch);
    }
  return NULL;
}

const char *CImagePlRaw::GetSaveFormat(PLRAWSAVEFORMAT *pFmt,CImageFlx *pIm) {
//fills in the save format structure based on format of given image pIm
//returns NULL=ok, otherwise error message string
  const char *msg;
  IMG_INT32 ch;
  msg = GetSaveFormat(pFmt,(CImageBase*)pIm);
  if (msg != NULL) return msg;
  //try to use display ranges for min & max
  for (ch=0; ch<pIm->GetNColChannels(); ch++) {
    pFmt->rangeMin[ch] = pIm->GetMeta()->GetMetaInt(FLXMETA_CH_DISP_MIN,pFmt->rangeMin[ch],ch,true);
    pFmt->rangeMin[ch] = pIm->GetMeta()->GetMetaInt(FLXMETA_CH_DISP_MAX,pFmt->rangeMin[ch],ch,true);
    }
  return NULL;
}

const char *CImagePlRaw::GetSaveFormat(PLRAWSAVEFORMAT *pFmt,CImagePxm *pIm) {
//fills in the save format structure based on format of given image pIm
//returns NULL=ok, otherwise error message string
  return GetSaveFormat(pFmt,(CImageBase*)pIm);
}

const char *CImagePlRaw::GetSaveFormat(PLRAWSAVEFORMAT *pFmt,CImageYuv *pIm) {
//fills in the save format structure based on format of given image pIm
//returns NULL=ok, otherwise error message string
  return GetSaveFormat(pFmt,(CImageBase*)pIm);
}

const char *CImagePlRaw::GetSaveFormat(PLRAWSAVEFORMAT *pFmt,CImagePlRaw *pIm) {
//fills in the save format structure based on format of given image pIm
//returns NULL=ok, otherwise error message string
  return GetSaveFormat(pFmt,(CImageBase*)pIm);
}

const char *CImagePlRaw::GetSaveFormat(PLRAWSAVEFORMAT *pFmt,CImageNRaw *pIm) {
//fills in the save format structure based on format of given image pIm
//returns NULL=ok, otherwise error message string
  return GetSaveFormat(pFmt,(CImageBase*)pIm);
}

const char *CImagePlRaw::GetSaveFormat(PLRAWSAVEFORMAT *pFmt,CImageBmp *pIm) {
//fills in the save format structure based on format of given image pIm
//returns NULL=ok, otherwise error message string
  return GetSaveFormat(pFmt,(CImageBase*)pIm);
}

/*-------------------------------------------------------------------------------
NRAW file format*/

CImageNRaw::CImageNRaw() {
  Unload();
}

CImageNRaw::~CImageNRaw() {
  Unload();
}

void CImageNRaw::UnloadHeader() {
  CImageBase::UnloadHeader();
  meta.Unload();
  fileOffsetFrame1 = 0;
  frameSize = 0;
  nrawImageType = IT_UNDEF;
  bytesPerSample = 0;
}

bool CImageNRaw::DetectFormat(const char *filename) {
//test whether specified file is in this format
  FILE *f;
  IMG_UINT8 hdr[4];
  if ((f = fopen(filename,"rb")) == NULL) return false;
  if (!fread(hdr,sizeof(hdr),1,f)) memset(hdr,0,sizeof(hdr));
  fclose(f);
  if (*(IMG_UINT32*)hdr == *(IMG_UINT32*)"NRAW") return true;
  return false;
}

const char *CImageNRaw::LoadFileHeader(const char *filename,void *pExtra) {
//pExtra=NULL
//NRAW file has a 1024-byte header which we load here and translate into FLX-style meta data (because that one is most flexible)
  NRAWHEADER nrawHeader;
  FILE *f;
  IMG_INT32 Nchnl,ch;
  IMG_UINT8 *bitDepths;
  if ((f=fopen(filename,"rb")) == NULL) return "Error opening file";
  if (!fread(&nrawHeader,sizeof(nrawHeader),1,f)) { fclose(f);  return "Error loading header"; }
  fclose(f);
  //check the NRAW header and store basic info in members
  if (*(IMG_UINT32*)nrawHeader.signature!=*(IMG_UINT32*)"NRAW")
    return "Invalid NRAW file header";
  if ((nrawHeader.headerVersion != 0x100) && (nrawHeader.headerVersion != 0x200))
    return "Unsupported header version";
  if ((nrawHeader.bitDepth > 31) || (nrawHeader.bytesPerSample < 1) || (nrawHeader.bytesPerSample > 4))
    return "Unsupported bit depth - above 31-bit signed";
  nrawImageType = (IT_xxx)nrawHeader.imageType;
  bytesPerSample = (IMG_UINT8)nrawHeader.bytesPerSample;
  width = nrawHeader.width;
  height = nrawHeader.height;
  fileOffsetFrame1 = 1024;
  //figure out color model and subsampling
  //for RGB override bitDepth with bit depth implied by image type
  subsMode = SUBS_UNDEF;
  bitDepths = NULL;
  switch (nrawImageType) {
    case IT_BAYER:
      switch (nrawHeader.bayerType) {
        case BT_RGGB: colorModel = CM_RGGB;  subsMode = SUBS_RGGB; break;
        case BT_GRBG: colorModel = CM_RGGB;  subsMode = SUBS_GRBG; break;
        case BT_GBRG: colorModel = CM_RGGB;  subsMode = SUBS_GBRG; break;
        case BT_BGGR: colorModel = CM_RGGB;  subsMode = SUBS_BGGR; break;
        case BT_UNDEF: colorModel = CM_RGGB;  subsMode = SUBS_RGGB; break;  //default to RGGB, better than failing to load
        default: return "Unsupported bayer type";
        }
      Nchnl = 4;
      break;
    case IT_YUV422:
      colorModel = CM_YUV;
      subsMode = SUBS_422;
      Nchnl = 3;
      break;
    case IT_RGB565:
      colorModel = CM_RGB;  //for simplicity upscale to RGB888 all three, don't bother with bit packing
      Nchnl = 3;
      bitDepths = (IMG_UINT8*)"\x5\x6\x5";
      break;
    case IT_RGB444:
      colorModel = CM_RGB;  //for simplicity upscale to RGB888 all three, don't bother with bit packing
      Nchnl = 3;
      bitDepths = (IMG_UINT8*)"\x4\x4\x4";
      break;
    case IT_RGB888:
      colorModel = CM_RGB;  //for simplicity upscale to RGB888 all three, don't bother with bit packing
      Nchnl = 3;
      bitDepths = (IMG_UINT8*)"\x8\x8\x8";
      break;
    default: return "Unsupported image type";
    }
  frameSize = 0;
  for (ch = 0; ch < Nchnl; ch++) {
    chnl[ch].data = NULL;
    chnl[ch].chnlWidth = (width + GetXSampling(ch) - 1) / GetXSampling(ch);
    chnl[ch].chnlHeight = (height + GetYSampling(ch) - 1) / GetYSampling(ch);
    if (bitDepths == NULL) chnl[ch].bitDepth = (IMG_UINT8)nrawHeader.bitDepth;
    else chnl[ch].bitDepth = bitDepths[ch];
    chnl[ch].isSigned = false;
    frameSize += chnl[ch].chnlWidth * chnl[ch].chnlHeight * bytesPerSample;
    }
  fileName = new char[strlen(filename) + 1];
  if (fileName == NULL) { UnloadHeader();  return "Out of memory"; }
  strcpy(fileName,filename);
  //store any extra info from NRAW header as meta data items
  if (nrawHeader.sensorCfgNum > 0) meta.UpdateInt(NRAWMETA_SENSOR_CFG,nrawHeader.sensorCfgNum);
  if (nrawHeader.chipID > 0) meta.UpdateInt(NRAWMETA_CHIP_ID,nrawHeader.chipID);
  if (nrawHeader.fwVersion[0] > 0) meta.Add(NRAWMETA_FW_VERSION,-1,nrawHeader.fwVersion,sizeof(nrawHeader.fwVersion),CMetaData::IFEX_REPLACE,NULL);
  if (nrawHeader.fwVersion[0] > 0) meta.Add(NRAWMETA_CAMERA_MAKE,-1,nrawHeader.make,sizeof(nrawHeader.make),CMetaData::IFEX_REPLACE,NULL);
  if (nrawHeader.fwVersion[0] > 0) meta.Add(NRAWMETA_CAMERA_MODEL,-1,nrawHeader.model,sizeof(nrawHeader.model),CMetaData::IFEX_REPLACE,NULL);
  if (nrawHeader.numFrames > 0) meta.UpdateInt(NRAWMETA_NFRAMES,nrawHeader.numFrames);
  if (nrawHeader.opticalBlack[0] || nrawHeader.opticalBlack[1] || nrawHeader.opticalBlack[2] || nrawHeader.opticalBlack[3]) {
    char buf[100];
    _snprintf(buf,sizeof(buf),"%d %d %d %d",(IMG_INT)nrawHeader.opticalBlack[0],(IMG_INT)nrawHeader.opticalBlack[1],
        (IMG_INT)nrawHeader.opticalBlack[2],(IMG_INT)nrawHeader.opticalBlack[3]);
    meta.UpdateStr(NRAWMETA_OPT_BLACK,buf);
    }
  return 0;
}

const char *CImageNRaw::LoadFileData(IMG_INT32 frameIndex) {
//read (next) image/frame from the file
  static IMG_UINT8 bayerLayout2x2_1234[] = {2,2, 0,1,2,3};  //RGGB  pattern size x,y, channel numbers
  static IMG_UINT8 bayerLayout2x2_2143[] = {2,2, 1,0,3,2};  //GRBG
  static IMG_UINT8 bayerLayout2x2_3412[] = {2,2, 2,3,0,1};  //GBRG
  static IMG_UINT8 bayerLayout2x2_4321[] = {2,2, 3,2,1,0};  //BGGR
  IMG_UINT8 *bayerLayoutUsed,*bayerLayoutUsedBase;
  const COLORMODELINFO *cmi;
  IMG_INT32 ch,x,y,valMask[MAX_COLOR_CHANNELS];
  PIXEL *pData[4];  //3 for YUV, 4 for YUVA (perhaps one day...)
  FILE *f;
  const char *msg;
  if (!IsHeaderLoaded()) return "File header not loaded";
  if ((cmi = GetColorModelInfo(colorModel)) == NULL) return "Invalid color model";
  if ((f = fopen(fileName,"rb")) == NULL) return "Error opening file";
  myfseeki64(f,fileOffsetFrame1 + frameIndex * frameSize);
  //allocate buffers if required
  for (ch = 0; ch < cmi->Nchannels; ch++) {
    if (chnl[ch].data == NULL) {  //need to allocate buffer for this color channel
      chnl[ch].data = new PIXEL[chnl[ch].chnlWidth * chnl[ch].chnlHeight];
	  if (chnl[ch].data == NULL) { msg = "Out of memory";  goto _abort; }
      }
    pData[ch] = chnl[ch].data;
    valMask[ch] = (1<<chnl[ch].bitDepth) - 1;
    }
  //load frame
  bayerLayoutUsed = NULL;
  msg = "Error reading frame data";  //default error if we fail below
  switch (nrawImageType) {
    case IT_BAYER:
      switch (subsMode) {
        case SUBS_RGGB: bayerLayoutUsed = bayerLayout2x2_1234; break;
        case SUBS_GRBG: bayerLayoutUsed = bayerLayout2x2_2143; break;
        case SUBS_GBRG: bayerLayoutUsed = bayerLayout2x2_3412; break;
        case SUBS_BGGR: bayerLayoutUsed = bayerLayout2x2_4321; break;
        default: msg = "Unsupported bayer type";  goto _abort;
        }
      for (y = 0; y < height; y++) {
        bayerLayoutUsedBase = bayerLayoutUsed + (y%bayerLayoutUsed[1])*bayerLayoutUsed[0] + 2;
        for (x = 0; x < width; x++) {
          IMG_INT32 val;
          ch = bayerLayoutUsedBase[x%bayerLayoutUsed[0]];
          if (!fread(&val,bytesPerSample,1,f)) goto _abort;
          *(pData[ch])++ = (val & valMask[ch]);
          }
        }
      break;
    case IT_YUV422:
      //422 can't be all interleaved, so assume planar
      for (ch = 0; ch < cmi->Nchannels; ch++) {
        for (y = 0; y < chnl[ch].chnlHeight; y++) {
          for (x = 0; x < chnl[ch].chnlWidth; x++) {
            IMG_INT32 val;
            if (!fread(&val,bytesPerSample,1,f)) goto _abort;
            *(pData[ch])++ = (val & valMask[ch]);
            }//x
          }//y
        }//ch
      break;
    case IT_RGB565:  //assume interleaved 16 bits per R,G,B pixel, R in bottom bits
      for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
          IMG_UINT16 val;
          if (!fread(&val,2,1,f)) goto _abort;
          *(pData[0])++ = (val & 0x1f);
          *(pData[1])++ = ((val >> 5) & 0x3f);
          *(pData[2])++ = ((val >> 11) & 0x1f);
          }//x
        }//y
      break;
    case IT_RGB444:  //probably interleaved 12 bits per pixel bit-packed (i.e. bytes [R,G],[B,R],[G,B])
      for (y = 0; y < height; y++) {
        for (x = 0; x < width/2; x++) {
          IMG_UINT8 pix[3];
          if (!fread(pix,3,1,f)) goto _abort;
          *(pData[0])++ = pix[0] & 0xf;
          *(pData[1])++ = pix[0] >> 4;
          *(pData[2])++ = pix[1] & 0xf;
          *(pData[0])++ = pix[1] >> 4;
          *(pData[1])++ = pix[2] & 0xf;
          *(pData[2])++ = pix[2] >> 4;
          }//x
        if (width & 1) {
          IMG_UINT8 pix[2];
          if (!fread(pix,2,1,f)) goto _abort;
          *(pData[0])++ = pix[0] & 0xf;
          *(pData[1])++ = pix[0] >> 4;
          *(pData[2])++ = pix[1] & 0xf;
          }//x
        }//y
      break;
    case IT_RGB888:
      for (y = 0; y < height; y++) {
        for (x = 0; x < width; x++) {
          IMG_UINT8 pix[3];
          if (!fread(pix,3,1,f)) goto _abort;
          *(pData[0])++ = pix[0];
          *(pData[1])++ = pix[1];
          *(pData[2])++ = pix[2];
          }//x
        }//y
      break;
    default: msg = "Unsupported image type";  goto _abort;
    }
  msg = NULL;
  _abort:  /****label****/
  fclose(f);
  if (msg != NULL) UnloadData();
  return msg;
}

/*-------------------------------------------------------------------------------
Aptina RAW file format*/

CImageAptinaRaw::CImageAptinaRaw() {
  Unload();
}

CImageAptinaRaw::~CImageAptinaRaw() {
  Unload();
}

void CImageAptinaRaw::UnloadHeader() {
  CImageBase::UnloadHeader();
  frameSize = 0;
  Nframes = 0;
  pixelFormat = RPF_UNDEF;
}

IMG_INT32 CImageAptinaRaw::GetProperty(PROPERTY_xxx prop) {
  switch (prop) {
    case PROPERTY_SUPPORT_VIDEO: return 1;
    case PROPERTY_IS_VIDEO: return (Nframes > 1)? 1: 0;
    }
  return 0;
}

bool CImageAptinaRaw::DetectFormat(const char *filename) {
//test whether specified file is in this format
  char infoFileName[300],*c;
  strncpy(infoFileName,filename,sizeof(infoFileName)-6);  infoFileName[sizeof(infoFileName)-6]='\0';
  if ((c=strrchr(infoFileName,'.'))==NULL) return false;
  if (stricmp(c,".raw")!=0) return false;	//not ".raw"
  strcpy(c,"_info.txt");
  if (_access(infoFileName,0)==0) return true;
  return false;
}

const char *CImageAptinaRaw::LoadFileHeader(const char *filename,void *pExtra) {
//pExtra=NULL
  typedef enum infoFileSectionsEnum { IFS_UNDEF,IFS_INITIAL,IFS_RAWFORMAT } IFS_xxx;
  char lineBuf[300],*c;
  IFS_xxx currSection;
  IMG_INT64 fileSize;
  IMG_INT32 Nchnl,ch,n;
  IMG_UINT8 *bitDepths;
  FILE *f;
  //init
  UnloadHeader();
  pixelFormat = RPF_UNDEF;
  //scan INFO file
  strncpy(lineBuf,filename,sizeof(lineBuf)-6);  lineBuf[sizeof(lineBuf)-6]='\0';
  if ((c=strrchr(lineBuf,'.'))==NULL) return "Error loading header";
  if (stricmp(c,".raw")!=0) return "Error loading header";	//not ".raw"
  strcpy(c,"_info.txt");
  if ((f=fopen(lineBuf,"rb")) == NULL) return "Error reading _info file";
  currSection=IFS_INITIAL;
  while (!feof(f)) {
    if (fgets(lineBuf,sizeof(lineBuf),f)==NULL) break;
    lineBuf[strcspn(lineBuf,"\r\n")]='\0';
    if (stricmp(lineBuf,"[Raw Image Format]")==0) { currSection=IFS_RAWFORMAT;  continue; }
    if (lineBuf[0]=='[') { currSection=IFS_UNDEF;  continue; }  //start of a section we didn't recognise above
    if (currSection==IFS_RAWFORMAT) {
      if (strnicmp(lineBuf,"IMAGE=",6)==0) {
        //image dimensions
        for (c=lineBuf;*c;c++) { if (*c==',') *c=' '; }
        n=-1;  c=lineBuf+6;  sscanf(c,"%d%d%n",&width,&height,&n);
        if (n<3) { width=height=0;  continue; }
        //format
        for (c+=n;*c && isspace(*c);c++) ;
        if (stricmp(c,"BAYER-10")==0) {
          pixelFormat = RPF_BAYER10;
          bitDepths = (IMG_UINT8*)"\xA\xA\xA\xA";
          Nchnl = 4;
          colorModel = CM_RGGB;
          subsMode = SUBS_GRBG;
          }
        continue;
        }
      }
    else if (currSection==IFS_INITIAL) {
#if 0  //looks like it's better if I calculate the number of frames myself from file size, this item is 1 less
      if (((c=strstr(lineBuf+1,"Frames"))!=NULL) && !isalnum(c[-1]) && !isalnum(c[6])) {
        if ((c=strchr(c,'='))==NULL) continue;
        n=-1;  sscanf(++c,"%d%n",&Nframes,&n);
        if (n<1) Nframes=0;
        continue;
        }
#endif
      }
    }
  fclose(f);
  //check whether we found all the information we need
  if ((pixelFormat==RPF_UNDEF) || (width<1) || (height<1) || (colorModel==CM_UNDEF) || (subsMode==SUBS_UNDEF)) return "Incomplete header";
  switch (pixelFormat) {
    case RPF_BAYER10:
      frameSize=((width*10+7)/8)*height;
      break;
	default: break; // do nothing, unreachable anyway
    }
  if (Nframes<1) {
    if ((f=fopen(filename,"rb"))==NULL) { UnloadHeader();  return "Error opening file"; }
    fileSize=myfilelengthi64(f);
    fclose(f);
    Nframes=(IMG_INT32)(fileSize/frameSize);
    }
  for (ch = 0; ch < Nchnl; ch++) {
    chnl[ch].data = NULL;
    chnl[ch].chnlWidth = (width + GetXSampling(ch) - 1) / GetXSampling(ch);
    chnl[ch].chnlHeight = (height + GetYSampling(ch) - 1) / GetYSampling(ch);
    chnl[ch].bitDepth = bitDepths[ch];
    chnl[ch].isSigned = false;
    }
  fileName = new char[strlen(filename) + 1];
  if (fileName == NULL) { UnloadHeader();  return "Out of memory"; }
  strcpy(fileName,filename);
  return 0;
}

const char *CImageAptinaRaw::LoadFileData(IMG_INT32 frameIndex) {
//read (next) image/frame from the file
  static IMG_UINT8 bayerLayout2x2_1234[] = {2,2, 0,1,2,3};  //RGGB  pattern size x,y, channel numbers
  static IMG_UINT8 bayerLayout2x2_2143[] = {2,2, 1,0,3,2};  //GRBG
  static IMG_UINT8 bayerLayout2x2_3412[] = {2,2, 2,3,0,1};  //GBRG
  static IMG_UINT8 bayerLayout2x2_4321[] = {2,2, 3,2,1,0};  //BGGR
  IMG_UINT8 *bayerLayoutUsed,*bayerLayoutUsedBase;
  const COLORMODELINFO *cmi;
  IMG_INT32 ch,x,y,valMask[MAX_COLOR_CHANNELS];
  PIXEL *pData[4];  //3 for YUV, 4 for YUVA (perhaps one day...)
  IMG_UINT8 *pLineBuf;
  IMG_INT32 lineSizeInFile;
  FILE *f;
  const char *msg;
  msg = NULL;
  pLineBuf = NULL;
  if (!IsHeaderLoaded()) return "File header not loaded";
  if ((cmi = GetColorModelInfo(colorModel)) == NULL) return "Invalid color model";
  if (fileName == NULL) return "File not loaded";  //could be an edited image
  if ((f = fopen(fileName,"rb")) == NULL) return "Error opening file";
  myfseeki64(f,frameIndex * frameSize);
  //allocate buffers if required
  for (ch = 0; ch < cmi->Nchannels; ch++) {
    if (chnl[ch].data == NULL) {  //need to allocate buffer for this color channel
      chnl[ch].data = new PIXEL[chnl[ch].chnlWidth * chnl[ch].chnlHeight];
      if (chnl[ch].data == NULL) { msg = "Out of memory";  goto _abort; }
      }
    pData[ch] = chnl[ch].data;
    valMask[ch] = (1<<chnl[ch].bitDepth) - 1;
    }
  //load frame
  bayerLayoutUsed = NULL;
  switch (subsMode) {
    case SUBS_RGGB: bayerLayoutUsed = bayerLayout2x2_1234; break;
    case SUBS_GRBG: bayerLayoutUsed = bayerLayout2x2_2143; break;
    case SUBS_GBRG: bayerLayoutUsed = bayerLayout2x2_3412; break;
    case SUBS_BGGR: bayerLayoutUsed = bayerLayout2x2_4321; break;
    default: msg = "Unsupported bayer type";  goto _abort;
    }
  if (msg == NULL) {
    lineSizeInFile = (width*10+7)/8;	//[bytes]  assume 10 bits per pixel, tightly packed (all bits used)
    pLineBuf = new IMG_UINT8[lineSizeInFile];
    if (pLineBuf == NULL) { msg = "Out of memory"; }
    }
  if (msg != NULL) goto _abort;
  //decode frame
  msg = "Error reading frame data";  //default error if we fail below
  for (y = 0; y < height; y++) {
    bayerLayoutUsedBase = bayerLayoutUsed + (y%bayerLayoutUsed[1])*bayerLayoutUsed[0] + 2;
    if (!fread(pLineBuf,lineSizeInFile,1,f)) goto _abort;
    for (x = 0; x < width; x++) {
      IMG_INT32 val;
      ch = bayerLayoutUsedBase[x%bayerLayoutUsed[0]];
      val = ((*(IMG_UINT32*)(pLineBuf+(x*10)/8)) >> ((x*10)%8)) & 0x3ff;
      *(pData[ch])++ = (val & valMask[ch]);
      }
    }

  msg = NULL;
  _abort:  /****label****/
  fclose(f);
  if (msg != NULL) UnloadData();
  if (pLineBuf != NULL) delete []pLineBuf;
  return msg;
}

/*-------------------------------------------------------------------------------
BMP file format*/

CImageBmp::CImageBmp() {
  pPalette=NULL;
  Unload();
}

CImageBmp::~CImageBmp() {
  Unload();
}

bool CImageBmp::GetChannelMapToRGB(CM_xxx colorModel,IMG_UINT8 *pMap3) {
//get mapping of current color model channel indexes to R,G,B
  IMG_UINT8 chMapGray[3]={0,0,0};
  IMG_UINT8 chMapRGBA[3]={0,1,2};
  switch (colorModel) {
    case CM_GRAY: memcpy(pMap3,chMapGray,3); break;
    case CM_RGB: memcpy(pMap3,chMapRGBA,3); break;
    case CM_RGBA: memcpy(pMap3,chMapRGBA,3); break;
    default: return false;
    }
  return true;
}

void CImageBmp::UnloadHeader() {
  memset(&fhdr,0,sizeof(fhdr));
  memset(&ihdr,0,sizeof(ihdr));
  if (pPalette!=NULL) { delete []pPalette;  pPalette=NULL; }
  CImageBase::UnloadHeader();
}

const char *CImageBmp::LoadFileHeader(const char *filename,void *pExtra) {
//loads file header
//pExtra=NULL or BMPLOADFORMAT
//returns NULL=ok, !NULL=pointer to error message
#define BMPERROR(emsg) { msg=emsg; goto _error; }
  const char *msg;
  FILE *f;
  IMG_UINT i;
  Unload();
  if ((f=fopen(filename,"rb"))==NULL) return "Error opening file";
  if (!fread(&fhdr,sizeof(fhdr),1,f)) BMPERROR("Error reading file header")
  if (fhdr.wType!=0x4d42) BMPERROR("Invalid file header")  //check header
  if (!fread(&ihdr,sizeof(ihdr),1,f)) BMPERROR("Error reading info header")
  if (ihdr.wPlanes!=1) BMPERROR("Unsupported plane format")
  if ((ihdr.wBitCount!=1) && (ihdr.wBitCount!=4) && (ihdr.wBitCount!=8) && (ihdr.wBitCount!=24))
    BMPERROR("Unsupported bit depth");
  fseek(f,ihdr.dwSize-sizeof(ihdr),SEEK_CUR);
  if (ihdr.dwSizeImage==0) ihdr.dwSizeImage=(ihdr.dwWidth*ihdr.dwHeight*ihdr.wBitCount)/8;
  //read palette
  if (ihdr.wBitCount<24) {
    IMG_UINT8 palQuad[4];  /*blue,green,red,reserved*/
    if (ihdr.dwClrUsed==0) ihdr.dwClrUsed=(1u<<ihdr.wBitCount);  //all colors are important
    pPalette=new IMG_UINT8[ihdr.dwClrUsed*3];
    if (pPalette == NULL) { BMPERROR("Out of memory") }
    for (i=0;i<ihdr.dwClrUsed;i++) {
      if (!fread(palQuad,4,1,f)) BMPERROR("Error reading palette")
      pPalette[i*3+0]=palQuad[2];  //R
      pPalette[i*3+1]=palQuad[1];  //G
      pPalette[i*3+2]=palQuad[0];  //B
      }
    }
  //set base class members
  width=ihdr.dwWidth;
  height=ihdr.dwHeight;
  for (i=0;i<3;i++) {
    chnl[i].data=NULL;
    chnl[i].chnlWidth=width;
    chnl[i].chnlHeight=height;
    chnl[i].bitDepth=8;
    chnl[i].isSigned=false;
    }
  colorModel=CM_RGB;
  subsMode=SUBS_UNDEF;
  fileName=new char[strlen(filename)+1];
  if (fileName == NULL) { BMPERROR("Out of memory") }
  strcpy(fileName,filename);
  //done
  msg=NULL;
_error:  /****label****/
  fclose(f);
  if (msg!=NULL) UnloadHeader();
  return msg;
#undef BMPERROR
}

const char *CImageBmp::LoadFileData(IMG_INT32 frameIndex) {
//loads image data from file (the header must have been loaded already)
//frameIndex: must be 0
//returns NULL=ok, !NULL=pointer to error message
#define BMPERROR(emsg) { msg=emsg; goto _error; }
  const char *msg;
  IMG_UINT32 x,y,ch;
  PIXEL *pRGB[3];
  FILE *f;
  msg=NULL;
  if (!IsHeaderLoaded()) return "Header not loaded";
  if ((ihdr.wBitCount<24) && (pPalette==NULL)) return "Palette not loaded";
  if ((f=fopen(fileName,"rb"))==NULL) return "Error opening file (2)";
  //allocate pixel buffers
  for (ch=0;ch<3;ch++) {
    chnl[ch].data=new PIXEL[chnl[ch].chnlWidth*chnl[ch].chnlHeight];
    if (chnl[ch].data == NULL) { BMPERROR("Out of memory") }
    }
  //read pixels
  fseek(f,fhdr.dwOffBits,SEEK_SET);
  if (ihdr.dwCompression==COMPR_NONE) {  //uncompressed
    IMG_INT32 msk,shift;
    for (y=0;y<ihdr.dwHeight;y++) {
      //bitmap data is upside down, so set the buffer pointers accordingly
      for (ch=0;ch<3;ch++)
        pRGB[ch]=chnl[ch].data+chnl[ch].chnlWidth*(ihdr.dwHeight-1-y);
      //read one scanline
      if (ihdr.wBitCount<24) {   //1,4,8-bit indexed bitmap
        IMG_UINT8 data,pad,pixel;
        msk=(1<<ihdr.wBitCount)-1;
        shift=8;
        pad=0;
        for (x=0;x<ihdr.dwWidth;x++) {
          if (shift==8) {
            if (!fread(&data,1,1,f)) BMPERROR("Error reading image data (1)")
            if (!pad) pad=3; else pad--;
            }
          shift-=ihdr.wBitCount;
          pixel=(data>>shift)&msk;
          if (shift<=0) shift=8;
          //pixel is an index into palette - translate to RGB and store
          *(pRGB[0])++=pPalette[pixel*3+0];
          *(pRGB[1])++=pPalette[pixel*3+1];
          *(pRGB[2])++=pPalette[pixel*3+2];
          }
        while (pad--) { if (!fread(&data,1,1,f)) BMPERROR("Error reading image data (2)") }  //skip scanline padding (to 32bits)
        }
      else {  //24bit RGB
        IMG_UINT8 buff[300],*pBuf;  //buffer size must be multiple of 3
        IMG_UINT32 NpixLeft,NpixNow;
        for (NpixLeft=ihdr.dwWidth;NpixLeft>0;) {
          if ((NpixNow=sizeof(buff)/3)>NpixLeft) NpixNow=NpixLeft;
          if (!fread(buff,NpixNow*3,1,f)) BMPERROR("Error reading image data (3)")
          pBuf=buff;
          for (x=0;x<NpixNow;x++) {
            *(pRGB[2])++=*pBuf++;  //B
            *(pRGB[1])++=*pBuf++;  //G
            *(pRGB[0])++=*pBuf++;  //R
            }
          NpixLeft-=NpixNow;
          }
        //skip scanline padding (to 32bits)
        NpixLeft=(ihdr.dwWidth*3)&3;  //actually bytes
        if (NpixLeft) { if (!fread(buff,4-NpixLeft,1,f)) BMPERROR("Error reading image data (4)") }
        }
      }
    }
  else if ((ihdr.dwCompression==COMPR_RLE4) || (ihdr.dwCompression==COMPR_RLE8)) {  //4,8-bit RLE indexed
    //This part of the code reads data almost byte by byte which is horribly inefficient.
    //However, the RLE-compressed formats are uncommon so I won't bother optimizing this.
    IMG_UINT8 buf[2],done=0,rle4bit=(ihdr.dwCompression==COMPR_RLE4);  //??? ==COMPR_RLE8 ???
    x=y=0;  //track position within bitmap
    for (ch=0;ch<3;ch++) pRGB[ch]=chnl[ch].data;
    while (!done) {
      if (!fread(buf,2,1,f)) BMPERROR("Error reading image data (5)")
      if (buf[0]) {  //RLE: data=pixel count (4bpp->#of nibbles), next byte=1 8-bit or 2 4-bit pixel(s)
        IMG_INT pixel,count;
        count=buf[0];
        if ((x+=count)>ihdr.dwWidth) x=ihdr.dwWidth;  //don't go beyond end of line
        if (rle4bit) {  //RLE4
          //expand pixels - alternate 2 pixel indexes in buf[1]>>4 and buf[1]&0xF
          while (count>=2) {
            pixel=(buf[1]>>4)*3;
            *(pRGB[0])++=pPalette[pixel+0];
            *(pRGB[1])++=pPalette[pixel+1];
            *(pRGB[2])++=pPalette[pixel+2];
            pixel=(buf[1]&0xF)*3;
            *(pRGB[0])++=pPalette[pixel+0];
            *(pRGB[1])++=pPalette[pixel+1];
            *(pRGB[2])++=pPalette[pixel+2];
            count-=2;
            }
          if (count) {
            pixel=(buf[1]>>4)*3;
            *(pRGB[0])++=pPalette[pixel+0];
            *(pRGB[1])++=pPalette[pixel+1];
            *(pRGB[2])++=pPalette[pixel+2];
            }
          }
        else {  //RLE8
          pixel=((IMG_INT)buf[1])*3;
          while (count--) {
            *(pRGB[0])++=pPalette[pixel+0];
            *(pRGB[1])++=pPalette[pixel+1];
            *(pRGB[2])++=pPalette[pixel+2];
            }
          }
        }//RLE
      else {  //ESC codes
        IMG_UINT8 esc;
        IMG_INT pixel;
        esc=buf[1];
        switch (esc) {
          case 0: {  //end of line
            pixel=0;
            for (;x<ihdr.dwWidth;x++) {  //expand 1st palette entry
              *(pRGB[0])++=pPalette[pixel+0];
              *(pRGB[1])++=pPalette[pixel+1];
              *(pRGB[2])++=pPalette[pixel+2];
              }
            x=0; y++;
            } break;
          case 1:  //end of bitmap
            done=1;
            break;
          case 2: {
            //delta: next 2 bytes = dx a dy - change position within bitmap; dx and dy are unsigned
            IMG_UINT8 dxdy[2];
            IMG_UINT32 countSkipped;
            pixel=0;
            if (!fread(dxdy,2,1,f)) BMPERROR("Error reading image data (6)")
            countSkipped=(dxdy[1]*ihdr.dwWidth+dxdy[0]);
            while (countSkipped--) {  //expand 1st palette entry
              *(pRGB[0])++=pPalette[pixel+0];
              *(pRGB[1])++=pPalette[pixel+1];
              *(pRGB[2])++=pPalette[pixel+2];
              }
            x+=dxdy[0];  y+=dxdy[1];
            while (x>=ihdr.dwWidth) { x-=ihdr.dwWidth; y++; }  //just in case wrap around end of line is required
            } break;
          default: {  //otherwise esc=#of pixels to be directly copied into bitmap
            IMG_UINT8 color,odd,shl=0,msk=rle4bit?0xf:0xff;
            if (rle4bit) odd=((esc+1)/2)&1;  //odd=(esc%4==1)||(esc%4==2);
            else odd=esc&1;
            if ((x+=esc)>ihdr.dwWidth) x=ihdr.dwWidth;  //don't go beyond end of line
            while (esc--) {
              if (rle4bit) shl=4-shl;   //shl for nibble
              if (!rle4bit || shl) {
                if (!fread(&color,1,1,f)) BMPERROR("Error reading image data (7)")
                }
              pixel=((IMG_INT)((color>>shl)&msk))*3;
              *(pRGB[0])++=pPalette[pixel+0];
              *(pRGB[1])++=pPalette[pixel+1];
              *(pRGB[2])++=pPalette[pixel+2];
              }
            if (odd) { if (!fread(&color,1,1,f)) BMPERROR("Error reading image data (8)") }  //#of bytes must be even
            } break;
          }//switch(esc)
        }//ESC codes
      }  //while(!done)
    //bitmap data is upside down, so need to flip all the data vertically
    for (ch=0;ch<3;ch++) {
      PIXEL *p1,*p2,tmp;
      for (y=0;y<(IMG_UINT32)chnl[ch].chnlHeight/2;y++) {
        p1=chnl[ch].data+chnl[ch].chnlWidth*y;
        p2=chnl[ch].data+chnl[ch].chnlWidth*(chnl[ch].chnlHeight-1-y);
        for (x=chnl[ch].chnlWidth;x>0;x--) {
          tmp=*p1;  *p1=*p2;  *p2=tmp;
          p1++;  p2++;
          }
        }
      }
    }  //4,8-bit RLE
  else BMPERROR("Unsupported compression")
  msg=NULL;
_error:  /****label****/
  fclose(f);
  if (pPalette!=NULL) { delete []pPalette;  pPalette=NULL; }  //will not be needed any more
  return msg;
#undef BMPERROR
}

const char *CImageBmp::SaveFileStart(const char *filename,void *pExtra,void **pSC) {
//initiates saving of a file; allocates a format-specific internal save state structure and sets (*pSC) to it
//pSC: uninitialised pointer, set by this function, must be passed to remaining save functions
//pExtra=BMPSAVEFORMAT
//returns: NULL=ok, !NULL=error message string
  BMPSAVECONTEXT *pCtx;
  (*pSC)=NULL;
  if (!IsHeaderLoaded() || !IsDataLoaded()) return "No image data to save";
  pCtx=new BMPSAVECONTEXT;
  if (pCtx == NULL) { return "Out of memory"; }
  if ((pCtx->f=fopen(filename,"wb"))==NULL) { delete pCtx;  return "Error creating file"; }
  pCtx->pExtra=(BMPSAVEFORMAT*)pExtra;
  (*pSC)=pCtx;
  return NULL;
}

const char *CImageBmp::SaveFileHeader(void *pSC) {
//saves the file header, such as meta data
//SaveFileHeader() must be called before SaveFileData()
//only 24-bit BMP format is supported (because internal data is direct RGB)
  BMPSAVECONTEXT *pCtx=(BMPSAVECONTEXT*)pSC;
  IMG_UINT32 lineSizeBytes;
  lineSizeBytes=width*3;
  while (lineSizeBytes&3) lineSizeBytes++;  //padding
  //bitmap file header
  fhdr.wType=0x4d42;  //'BM'
  fhdr.dwSize=sizeof(fhdr)+sizeof(ihdr)+lineSizeBytes*height;  //entire file size
  fhdr.wRes1=0;
  fhdr.wRes2=0;
  fhdr.dwOffBits=sizeof(fhdr)+sizeof(ihdr);		//offset where pixel data starts
  //bitmap info header
  ihdr.dwSize=sizeof(ihdr);
  ihdr.dwWidth=width;
  ihdr.dwHeight=height;
  ihdr.wPlanes=1;
  ihdr.wBitCount=24;
  ihdr.dwCompression=COMPR_NONE;
  ihdr.dwSizeImage=lineSizeBytes*height;  //size of pixel data only [bytes]
  ihdr.dwXPelsPerMeter=4000;
  ihdr.dwYPelsPerMeter=4000;
  ihdr.dwClrUsed=0;
  ihdr.dwClrImportant=0;
  //write headers to file
  if (!fwrite(&fhdr,sizeof(fhdr),1,pCtx->f)) return "Error writing header (1)";
  if (!fwrite(&ihdr,sizeof(ihdr),1,pCtx->f)) return "Error writing header (2)";
  //header done
  pPalette=NULL;
  return NULL;
}

const char *CImageBmp::SaveFileData(void *pSC) {
//save image data to file at the current file position, SaveFileHeader() must be called before this function
//only 24-bit BMP format is supported (because internal data is direct RGB)
  IMG_UINT8 chMap[3];
  BMPSAVECONTEXT *pCtx=(BMPSAVECONTEXT*)pSC;
  BMPSAVEFORMAT *pBsf=(pCtx->pExtra);
  IMG_UINT8 *lineBuf,*pBGR;
  PIXEL *pRGB[3];
  IMG_INT x,y;
  if (!GetChannelMapToRGB(colorModel,chMap)) return "Unsupported color model";
  lineBuf=new IMG_UINT8[width*3+4];
  if (lineBuf == NULL) { return "Out of memory"; }
  memset(lineBuf,0,width*3+4);  //pretty much just so that padding at the end of each line is 0s
  for (y=0;y<height;y++) {
    //bitmap data is upside down, so convert it going upwards
    pRGB[0]=chnl[chMap[0]].data+chnl[chMap[0]].chnlWidth*(ihdr.dwHeight-1-y);
    pRGB[1]=chnl[chMap[1]].data+chnl[chMap[1]].chnlWidth*(ihdr.dwHeight-1-y);
    pRGB[2]=chnl[chMap[2]].data+chnl[chMap[2]].chnlWidth*(ihdr.dwHeight-1-y);
    pBGR=lineBuf;
    for (x=0;x<width;x++) {
      IMG_INT val;
      //B
      val=IMG_INT32( (*(pRGB[2])++)*pBsf->mul[2]+pBsf->off[2] );
      if (val<0) val=0; else if (val>255) val=255;
      *pBGR++=(IMG_UINT8)val;
      //G
      val=IMG_INT32( (*(pRGB[1])++)*pBsf->mul[1]+pBsf->off[1] );
      if (val<0) val=0; else if (val>255) val=255;
      *pBGR++=(IMG_UINT8)val;
      //R
      val=IMG_INT32( (*(pRGB[0])++)*pBsf->mul[0]+pBsf->off[0] );
      if (val<0) val=0; else if (val>255) val=255;
      *pBGR++=(IMG_UINT8)val;
      }
    while ((pBGR-lineBuf)&3) pBGR++;  //skip padding to align to 4 bytes
    if (!fwrite(lineBuf,pBGR-lineBuf,1,pCtx->f)) return "Error writing image data";
    }
  delete []lineBuf;
  return NULL;
}

const char *CImageBmp::SaveFileEnd(void *pSC) {
//finishes saving the image file - writes any remaining data, closes the file and deallocates memory used by pSC structure
//if SaveFileStart() succeeds, this function must be called (even if SaveFileHeader() or SaveFileData() fail) to properly tidy up
  BMPSAVECONTEXT *pCtx=(BMPSAVECONTEXT*)pSC;
  if (pCtx->f!=NULL) fclose(pCtx->f);
  delete pCtx;
  return NULL;
}

const char *CImageBmp::GetSaveFormat(BMPSAVEFORMAT *pFmt,CImageBase *pIm) {
  IMG_UINT8 chMap[3];
  IMG_INT ch;
  if (!GetChannelMapToRGB(pIm->colorModel,chMap)) return "Unsupported color model";
  //figure out multipliers for bit depth adjustment
  //for signed channels scale the entire range (including negative half)
  for (ch=0;ch<3;ch++) {
    if (pIm->chnl[chMap[ch]].isSigned) {
      pFmt->mul[ch]=127.0/(1u<<(pIm->chnl[chMap[ch]].bitDepth-1));
      pFmt->off[ch]=128;
      }
    else {
      pFmt->mul[ch]=255.0/((1u<<pIm->chnl[chMap[ch]].bitDepth)-1);
      pFmt->off[ch]=0;
      }
    }
  return NULL;
}

const char *CImageBmp::GetSaveFormat(BMPSAVEFORMAT *pFmt,CImageFlx *pIm) {
  return GetSaveFormat(pFmt,(CImageBase*)pIm);
}

const char *CImageBmp::GetSaveFormat(BMPSAVEFORMAT *pFmt,CImagePxm *pIm) {
  return GetSaveFormat(pFmt,(CImageBase*)pIm);
}

const char *CImageBmp::GetSaveFormat(BMPSAVEFORMAT *pFmt,CImageYuv *pIm) {
  return GetSaveFormat(pFmt,(CImageBase*)pIm);
}

const char *CImageBmp::GetSaveFormat(BMPSAVEFORMAT *pFmt,CImagePlRaw *pIm) {
  return GetSaveFormat(pFmt,(CImageBase*)pIm);
}

const char *CImageBmp::GetSaveFormat(BMPSAVEFORMAT *pFmt,CImageNRaw *pIm) {
  return GetSaveFormat(pFmt,(CImageBase*)pIm);
}

const char *CImageBmp::GetSaveFormat(BMPSAVEFORMAT *pFmt,CImageAptinaRaw *pIm) {
  return GetSaveFormat(pFmt,(CImageBase*)pIm);
}
