#ifndef __LM75_H_
#define __LM75_H_

#define LM75_TEMP_FROM_INT(i)	(((i) >> 7) / 2.0)

#define LM75_TEMPERATURE	0
#define LM75_CONFIGURATION	1
#define LM75_THYST		2
#define LM75_TOS		3
#define LM75_ID			7

#define LM75_ID_HIGH_NIBBLE	0xa0
#define LM75_HYST_DEF_VALUE	0x4b00
#define LM75_TOS_DEF_VALUE	0x5000

#endif /* __LM75_H_ */
