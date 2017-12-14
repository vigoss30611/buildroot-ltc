/*
 * access.c
 * 
 *  Synopsys Inc.
 *  SG DWC PT02
 */
#include <linux/printk.h>
#include <asm/io.h>
#include "access.h"
#include "log.h"

#include "error.h"
#include "mutex.h"

static void * access_mMutex = NULL;
static u8 * access_mBaseAddr = 0;
int access_Initialize(u8 * baseAddr)
{
	//access_mBaseAddr = baseAddr;
	access_mBaseAddr = (u8 *)ioremap_nocache((u32)baseAddr, 0x8000);
	mutex_Initialize(access_mMutex);
	return TRUE;
}

int access_uninstall()
{
	iounmap(access_mBaseAddr);
	return TRUE;
}

u8 access_CoreReadByte(u16 addr)
{
	u8 data = 0;
	//mutex_Lock(access_mMutex);
	data = access_Read(addr);
	//mutex_Unlock(access_mMutex);
	return data;
}

u8 access_CoreRead(u16 addr, u8 shift, u8 width)
{
	if (width <= 0)
	{
		error_Set(ERR_DATA_WIDTH_INVALID);
		LOG_ERROR("Invalid parameter: width == 0");
		return 0;
	}
	return (access_CoreReadByte(addr) >> shift) & (BIT(width) - 1);
}

void access_CoreWriteByte(u8 data, u16 addr)
{
	//mutex_Lock(access_mMutex);
	access_Write(data, addr);
	//mutex_Unlock(access_mMutex);
}

void access_CoreWrite(u8 data, u16 addr, u8 shift, u8 width)
{
	u8 temp = 0;
	u8 mask = 0;
	if (width <= 0)
	{
		error_Set(ERR_DATA_WIDTH_INVALID);
		LOG_ERROR("Invalid parameter: width == 0");
		return;
	}
	mask = BIT(width) - 1;
	if (data > mask)
	{
		error_Set(ERR_DATA_GREATER_THAN_WIDTH);
		LOG_ERROR("Invalid parameter: data > width");
		return;
	}
	//mutex_Lock(access_mMutex);
	temp = access_Read(addr);
	temp &= ~(mask << shift);
	temp |= (data & mask) << shift;
	access_Write(temp, addr);
	//mutex_Unlock(access_mMutex);
}

u8 access_Read(u16 addr)
{
	u8 val;
	LOG_TRACE1(addr);
	if(!access_mBaseAddr) {
		printk(KERN_ERR "Warning: HDMI accessed before initializing!!\n");
		return 0;
	}

	val = *(volatile u8 *)((u32)access_mBaseAddr + (u32)addr);
	return val;
}

void access_Write(u8 data, u16 addr)
{
	//LOG_WRITE(addr, data);
//	LOG_TRACE2(data, addr);
	if(!access_mBaseAddr) {
		printk(KERN_ERR "Warning: HDMI accessed before initializing!!\n");
		return ;
	}

	*(volatile u8 *)((u32)access_mBaseAddr + (u32)addr) = data;
	return;
}
