/*-----------------------------------------------------------------------------*/
Date: 		2011-08-12
Author: 	ayakashi
Version:	3.0.3
Notes: 
BugFixed:
	1).add IM_Strnpy, IM_Strchr,IM_Strrchr,IM_Strcasecmp, IM_Strncasecmp
	but in wince, these are NULL

/*-----------------------------------------------------------------------------*/
Date: 		2011-06-29
Author: 	ayakashi
Version:	3.0.2
Notes: 
BugFixed:
	1).Killing thread is not supported by Android,so In function forceTerminated, do nothing

/*-----------------------------------------------------------------------------*/
Date: 		2011-05-30
Author: 	Leo
Version:	3.0.1
Notes: 
BugFixed:
	1). In oswl, modify IM_CharString2TCharString() and IM_TCharString2CharString();
	2). In IM_kvpair.h, replace new/delete to malloc/free to avoid compire warnning;

/*-----------------------------------------------------------------------------*/
Date: 		2011-05-27
Author: 	Leo
Version:	3.0.0
Notes: 
NewFeatures:
	1). In oswl, add IM_CharString2TCharString() and IM_TCharString2CharString() to
		support these convert. 

/*-----------------------------------------------------------------------------*/
Date: 		2011-05-05
Author: 	Leo
Version:	2.0.0
Notes: 
	1. In IM_refpointer.h, modified the constructor.
		IM_RefPointer(T *p, IM_INT32 pTypeKey, REFPOINTER_RELEASE_CALLBACK release);
		add a pTypeKey to indicate p type.
	2. In IM_refpointer.h, add 2 functions.
		IM_INT32 getPointerTypeKey();
		void * getPointer();

/*-----------------------------------------------------------------------------*/
Date: 		2011-04-23
Author: 	Leo
Version:	1.0.0
Notes: first commit.
/*-----------------------------------------------------------------------------*/
