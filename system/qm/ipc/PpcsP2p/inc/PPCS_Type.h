#if !defined(__INCLUDED_TYPE_DEFINE____H)
#define __INCLUDED_TYPE_DEFINE____H

//#if defined WIN32DLL || defined LINUX
typedef int INT_32;
typedef unsigned int UINT_32;
//#endif

typedef short INT_16;
typedef unsigned short UINT_16;

typedef char CHAR_8;
typedef signed char	 SCHAR_8;
typedef unsigned char UCHAR_8;

typedef long LONG_32;
typedef unsigned long ULONG_32;

#ifndef bool
#define bool	CHAR
#endif

#ifndef _ARC_COMPILER
#ifndef true
#define true	1
#endif

#ifndef false
#define false	0
#endif
#endif ////#ifndef _ARC_COMPILER
#endif  //__INCLUDED_TYPE_DEFINE_H