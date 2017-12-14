/*
 * @file:halIdentification.h
 *
 *  Created on: Jul 1, 2010
 *      Author: klabadi & dlopo
 */

#ifndef HALIDENTIFICATION_H_
#define HALIDENTIFICATION_H_

#include "types.h"

u8 halIdentification_Design(u16 baseAddr);

u8 halIdentification_Revision(u16 baseAddr);

u8 halIdentification_ProductLine(u16 baseAddr);

u8 halIdentification_ProductType(u16 baseAddr);

#endif /* HALIDENTIFICATION_H_ */
