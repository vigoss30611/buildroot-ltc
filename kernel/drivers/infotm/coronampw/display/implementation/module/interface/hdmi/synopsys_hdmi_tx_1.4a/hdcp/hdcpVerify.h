/*
 * hdcpVerify.h
 *
 *  Created on: Jul 20, 2010
 *      Author: klabadi & dlopo
 */

#ifndef HDCPVERIFY_H_
#define HDCPVERIFY_H_

#include "types.h"

typedef struct
{
	u8 mLength[8];
	u8 mBlock[64];
	int mIndex;
	int mComputed;
	int mCorrupted;
	unsigned mDigest[5];
} sha_t;

static const unsigned KSV_LEN = 5;
static const unsigned KSV_MSK = 0x7F;
static const unsigned VRL_LENGTH = 0x05;
static const unsigned VRL_HEADER = 5;
static const unsigned VRL_NUMBER = 3;
static const unsigned HEADER = 10;
static const unsigned SHAMAX = 20;
static const unsigned DSAMAX = 20;
static const unsigned INT_KSV_SHA1 = 1;

void sha_Reset(sha_t *sha);

int sha_Result(sha_t *sha);

void sha_Input(sha_t *sha, const u8 * data, size_t size);

void sha_ProcessBlock(sha_t *sha);

void sha_PadMessage(sha_t *sha);

int hdcpVerify_DSA(const u8 * M, size_t n, const u8 * r, const u8 * s);

int hdcpVerify_ArrayADD(u8 * r, const u8 * a, const u8 * b, size_t n);

int hdcpVerify_ArrayCMP(const u8 * a, const u8 * b, size_t n);

void hdcpVerify_ArrayCPY(u8 * dst, const u8 * src, size_t n);

int hdcpVerify_ArrayDIV(u8 * r, const u8 * D, const u8 * d, size_t n);

int hdcpVerify_ArrayMAC(u8 * r, const u8 * M, const u8 m, size_t n);

int hdcpVerify_ArrayMUL(u8 * r, const u8 * M, const u8 * m, size_t n);

void hdcpVerify_ArraySET(u8 * dst, const u8 src, size_t n);

int hdcpVerify_ArraySUB(u8 * r, const u8 * a, const u8 * b, size_t n);

void hdcpVerify_ArraySWP(u8 * r, size_t n);

int hdcpVerify_ArrayTST(const u8 * a, const u8 b, size_t n);

int hdcpVerify_ComputeEXP(u8 * c, const u8 * M, const u8 * e, const u8 * p,
		size_t n, size_t nE);

int hdcpVerify_ComputeINV(u8 * out, const u8 * z, const u8 * a, size_t n);

int hdcpVerify_ComputeMOD(u8 * dst, const u8 * src, const u8 * p, size_t n);

int hdcpVerify_ComputeMUL(u8 * p, const u8 * a, const u8 * b, const u8 * m,
		size_t n);

int hdcpVerify_KSV(const u8 * data, size_t size);

int hdcpVerify_SRM(const u8 * data, size_t size);

#endif /* HDCPVERIFY_H_ */
