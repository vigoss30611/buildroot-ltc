

#ifndef _IROM_ERRNO_H_
#define _IROM_ERRNO_H_

#define EFAILED			1
//#define ENODATA			2
#define EACCESS			3
#define EUNSUPPORT		4
#define ENOBUFFER		5

#define EBLBAD			11
#define EBLINV			12
#define EBLTOOLARGE		13

#define ENODRAM			21

#define EBADMAGIC		31
#define EBADCRC			32
#define EBADSIG			33
#define EUNTRUST		34
#define EWRONGTYPE		35

/* NAND */
#define ENOCFG			52
#define EBADBLK			53

#endif /* _IROM_ERRNO_H_ */

