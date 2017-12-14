/*
 * halColorSpaceConverter.c
 *
 *  Created on: Jun 30, 2010
 *      Author: klabadi & dlopo
 */

#include "halColorSpaceConverter.h"
#include "access.h"
#include "log.h"
/* Color Space Converter register offsets */
static const u8 CSC_CFG = 0x00;
static const u8 CSC_SCALE = 0x01;
static const u8 CSC_COEF_A1_MSB = 0x02;
static const u8 CSC_COEF_A1_LSB = 0x03;
static const u8 CSC_COEF_A2_MSB = 0x04;
static const u8 CSC_COEF_A2_LSB = 0x05;
static const u8 CSC_COEF_A3_MSB = 0x06;
static const u8 CSC_COEF_A3_LSB = 0x07;
static const u8 CSC_COEF_A4_MSB = 0x08;
static const u8 CSC_COEF_A4_LSB = 0x09;
static const u8 CSC_COEF_B1_MSB = 0x0A;
static const u8 CSC_COEF_B1_LSB = 0x0B;
static const u8 CSC_COEF_B2_MSB = 0x0C;
static const u8 CSC_COEF_B2_LSB = 0x0D;
static const u8 CSC_COEF_B3_MSB = 0x0E;
static const u8 CSC_COEF_B3_LSB = 0x0F;
static const u8 CSC_COEF_B4_MSB = 0x10;
static const u8 CSC_COEF_B4_LSB = 0x11;
static const u8 CSC_COEF_C1_MSB = 0x12;
static const u8 CSC_COEF_C1_LSB = 0x13;
static const u8 CSC_COEF_C2_MSB = 0x14;
static const u8 CSC_COEF_C2_LSB = 0x15;
static const u8 CSC_COEF_C3_MSB = 0x16;
static const u8 CSC_COEF_C3_LSB = 0x17;
static const u8 CSC_COEF_C4_MSB = 0x18;
static const u8 CSC_COEF_C4_LSB = 0x19;

void halColorSpaceConverter_Interpolation(u16 baseAddr, u8 value)
{
	LOG_TRACE1(value);
	/* 2-bit width */
	access_CoreWrite(value, baseAddr + CSC_CFG, 4, 2);
}

void halColorSpaceConverter_Decimation(u16 baseAddr, u8 value)
{
	LOG_TRACE1(value);
	/* 2-bit width */
	access_CoreWrite(value, baseAddr + CSC_CFG, 0, 2);
}

void halColorSpaceConverter_ColorDepth(u16 baseAddr, u8 value)
{
	LOG_TRACE1(value);
	/* 4-bit width */
	access_CoreWrite(value, baseAddr + CSC_SCALE, 4, 4);
}

void halColorSpaceConverter_ScaleFactor(u16 baseAddr, u8 value)
{
	LOG_TRACE1(value);
	/* 2-bit width */
	access_CoreWrite(value, baseAddr + CSC_SCALE, 0, 2);
}

void halColorSpaceConverter_CoefficientA1(u16 baseAddr, u16 value)
{
	LOG_TRACE1(value);
	/* 15-bit width */
	access_CoreWriteByte((u8) (value), baseAddr + CSC_COEF_A1_LSB);
	access_CoreWrite((u8) (value >> 8), baseAddr + CSC_COEF_A1_MSB, 0, 7);
}

void halColorSpaceConverter_CoefficientA2(u16 baseAddr, u16 value)
{
	LOG_TRACE1(value);
	/* 15-bit width */
	access_CoreWriteByte((u8) (value), baseAddr + CSC_COEF_A2_LSB);
	access_CoreWrite((u8) (value >> 8), baseAddr + CSC_COEF_A2_MSB, 0, 7);
}

void halColorSpaceConverter_CoefficientA3(u16 baseAddr, u16 value)
{
	LOG_TRACE1(value);
	/* 15-bit width */
	access_CoreWriteByte((u8) (value), baseAddr + CSC_COEF_A3_LSB);
	access_CoreWrite((u8) (value >> 8), baseAddr + CSC_COEF_A3_MSB, 0, 7);
}

void halColorSpaceConverter_CoefficientA4(u16 baseAddr, u16 value)
{
	LOG_TRACE1(value);
	/* 15-bit width */
	access_CoreWriteByte((u8) (value), baseAddr + CSC_COEF_A4_LSB);
	access_CoreWrite((u8) (value >> 8), baseAddr + CSC_COEF_A4_MSB, 0, 7);
}

void halColorSpaceConverter_CoefficientB1(u16 baseAddr, u16 value)
{
	LOG_TRACE1(value);
	/* 15-bit width */
	access_CoreWriteByte((u8) (value), baseAddr + CSC_COEF_B1_LSB);
	access_CoreWrite((u8) (value >> 8), baseAddr + CSC_COEF_B1_MSB, 0, 7);
}

void halColorSpaceConverter_CoefficientB2(u16 baseAddr, u16 value)
{
	LOG_TRACE1(value);
	/* 15-bit width */
	access_CoreWriteByte((u8) (value), baseAddr + CSC_COEF_B2_LSB);
	access_CoreWrite((u8) (value >> 8), baseAddr + CSC_COEF_B2_MSB, 0, 7);
}

void halColorSpaceConverter_CoefficientB3(u16 baseAddr, u16 value)
{
	LOG_TRACE1(value);
	/* 15-bit width */
	access_CoreWriteByte((u8) (value), baseAddr + CSC_COEF_B3_LSB);
	access_CoreWrite((u8) (value >> 8), baseAddr + CSC_COEF_B3_MSB, 0, 7);
}

void halColorSpaceConverter_CoefficientB4(u16 baseAddr, u16 value)
{
	LOG_TRACE1(value);
	/* 15-bit width */
	access_CoreWriteByte((u8) (value), baseAddr + CSC_COEF_B4_LSB);
	access_CoreWrite((u8) (value >> 8), baseAddr + CSC_COEF_B4_MSB, 0, 7);
}

void halColorSpaceConverter_CoefficientC1(u16 baseAddr, u16 value)
{
	LOG_TRACE1(value);
	/* 15-bit width */
	access_CoreWriteByte((u8) (value), baseAddr + CSC_COEF_C1_LSB);
	access_CoreWrite((u8) (value >> 8), baseAddr + CSC_COEF_C1_MSB, 0, 7);
}

void halColorSpaceConverter_CoefficientC2(u16 baseAddr, u16 value)
{
	LOG_TRACE1(value);
	/* 15-bit width */
	access_CoreWriteByte((u8) (value), baseAddr + CSC_COEF_C2_LSB);
	access_CoreWrite((u8) (value >> 8), baseAddr + CSC_COEF_C2_MSB, 0, 7);
}

void halColorSpaceConverter_CoefficientC3(u16 baseAddr, u16 value)
{
	LOG_TRACE1(value);
	/* 15-bit width */
	access_CoreWriteByte((u8) (value), baseAddr + CSC_COEF_C3_LSB);
	access_CoreWrite((u8) (value >> 8), baseAddr + CSC_COEF_C3_MSB, 0, 7);
}

void halColorSpaceConverter_CoefficientC4(u16 baseAddr, u16 value)
{
	LOG_TRACE1(value);
	access_CoreWriteByte((u8) (value), baseAddr + CSC_COEF_C4_LSB);
	access_CoreWrite((u8) (value >> 8), baseAddr + CSC_COEF_C4_MSB, 0, 7);
}
