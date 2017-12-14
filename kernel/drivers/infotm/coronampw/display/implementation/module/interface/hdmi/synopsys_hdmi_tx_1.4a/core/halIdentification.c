/*
 * @file:halIdentification.c
 *
 *  Created on: Jul 1, 2010
 *      Author: klabadi & dlopo
 */
#include "halIdentification.h"
#include "access.h"
#include "log.h"

/* register offsets */
static const u8 DESIGN_ID = 0x00;
static const u8 REVISION_ID = 0x01;
static const u8 PRODUCT_ID0 = 0x02;
static const u8 PRODUCT_ID1 = 0x03;

u8 halIdentification_Design(u16 baseAddr)
{
	LOG_TRACE();
	return access_CoreReadByte((baseAddr + DESIGN_ID));
}

u8 halIdentification_Revision(u16 baseAddr)
{
	LOG_TRACE();
	return access_CoreReadByte((baseAddr + REVISION_ID));
}

u8 halIdentification_ProductLine(u16 baseAddr)
{
	LOG_TRACE();
	return access_CoreReadByte((baseAddr + PRODUCT_ID0));
}

u8 halIdentification_ProductType(u16 baseAddr)
{
	LOG_TRACE();
	return access_CoreReadByte((baseAddr + PRODUCT_ID1));
}

