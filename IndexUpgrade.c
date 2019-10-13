#define _FILE_OFFSET_BITS 64

#include <stdlib.h>
#include <unistd.h>
#include <sys/file.h>
#include <stdint.h>
#include <strings.h>
#include <math.h> // for ceil, floor
#include <stdio.h>

static const uint64_t pow2[] = {1,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,16384,32768,65536,131072,262144,524288,1048576,2097152,4194304,8388608,16777216,33554432,67108864,134217728,268435456,536870912,1073741824,2147483648,4294967296,8589934592,17179869184,34359738368,68719476736,137438953472,274877906944,549755813888,1099511627776,2199023255552,4398046511104,8796093022208,17592186044416,35184372088832,70368744177664,140737488355328,281474976710656,562949953421312,1125899906842624,2251799813685248,4503599627370496,9007199254740992,18014398509481984,36028797018963968,72057594037927936,144115188075855872,288230376151711744,576460752303423488,1152921504606846976,2305843009213693952,4611686018427387904,9223372036854775808ULL};

#define BIT_SET(a,b) ((a) |= (1ULL<<(b)))
#define BIT_CLEAR(a,b) ((a) &= ~(1ULL<<(b)))
#define BIT_FLIP(a,b) ((a) ^= (1ULL<<(b)))
#define BIT_CHECK(a,b) (!!((a) & (1ULL<<(b)))) // '!!' to make sure this returns 0 or 1

#define INT_2P63 9223372036854775808ULL
#define INT_2P62 4611686018427387904
#define INT_2P61 2305843009213693952
#define INT_2P60 1152921504606846976
#define INT_2P59 576460752303423488
#define INT_2P58 288230376151711744
#define INT_2P57 144115188075855872
#define INT_2P56 72057594037927936
#define INT_2P55 36028797018963968
#define INT_2P54 18014398509481984
#define INT_2P53 9007199254740992
#define INT_2P52 4503599627370496
#define INT_2P51 2251799813685248
#define INT_2P50 1125899906842624
#define INT_2P49 562949953421312
#define INT_2P48 281474976710656
#define INT_2P47 140737488355328
#define INT_2P46 70368744177664
#define INT_2P45 35184372088832
#define INT_2P44 17592186044416
#define INT_2P43 8796093022208
#define INT_2P42 4398046511104
#define INT_2P41 2199023255552
#define INT_2P40 1099511627776
#define INT_2P39 549755813888
#define INT_2P38 274877906944
#define INT_2P37 137438953472
#define INT_2P36 68719476736
#define INT_2P35 34359738368
#define INT_2P34 17179869184
#define INT_2P33 8589934592
#define INT_2P32 4294967296
#define INT_2P31 2147483648
#define INT_2P30 1073741824
#define INT_2P29 536870912
#define INT_2P28 268435456
#define INT_2P27 134217728
#define INT_2P26 67108864
#define INT_2P25 33554432
#define INT_2P24 16777216
#define INT_2P23 8388608
#define INT_2P22 4194304
#define INT_2P21 2097152
#define INT_2P20 1048576
#define INT_2P19 524288
#define INT_2P18 262144
#define INT_2P17 131072
#define INT_2P16 65536
#define INT_2P15 32768
#define INT_2P14 16384
#define INT_2P13 8192
#define INT_2P12 4096
#define INT_2P11 2048
#define INT_2P10 1024
#define INT_2P9 512
#define INT_2P8 256
#define INT_2P7 128
#define INT_2P6 64
#define INT_2P5 32
#define INT_2P4 16
#define INT_2P3 8
#define INT_2P2 4
#define INT_2P1 2
#define INT_2P0 1


#define bytesInBits_DOWN 0
#define bytesInBits_UP 1
static int bytesInBits(const int bits, const int dir) {
	const int modulus = bits % 8;
	return ((bits - modulus) / 8) + ((dir == bytesInBits_UP && modulus > 0) ? 1 : 0);
}

// Pack Rat v1 (PR)

// Get bit value, skipping first (skipBits) bits
static int bitCheck(const char *source, int skipBits) {
	int byteBegin = 0;
	int bitBegin = 0;

	for (;skipBits > 0; skipBits--) {
		bitBegin++;

		if (bitBegin > 7) {
			bitBegin = 0;
			byteBegin++;
		}
	}

	return BIT_CHECK(source[byteBegin], bitBegin);
}

// Get unsigned integer of 1-64 bits from a char array of 1-8 bytes
static uint64_t simpleUint_toInt(const char *c, const int skipBits, const int bitCount) {
	if (bitCount < 1 || bitCount > 64) return 0;

	uint64_t result = 0;

	switch(bitCount) {
		case 64: if (bitCheck(c, skipBits + 63)) result += INT_2P63;
		case 63: if (bitCheck(c, skipBits + 62)) result += INT_2P62;
		case 62: if (bitCheck(c, skipBits + 61)) result += INT_2P61;
		case 61: if (bitCheck(c, skipBits + 60)) result += INT_2P60;
		case 60: if (bitCheck(c, skipBits + 59)) result += INT_2P59;
		case 59: if (bitCheck(c, skipBits + 58)) result += INT_2P58;
		case 58: if (bitCheck(c, skipBits + 57)) result += INT_2P57;
		case 57: if (bitCheck(c, skipBits + 56)) result += INT_2P56;
		case 56: if (bitCheck(c, skipBits + 55)) result += INT_2P55;
		case 55: if (bitCheck(c, skipBits + 54)) result += INT_2P54;
		case 54: if (bitCheck(c, skipBits + 53)) result += INT_2P53;
		case 53: if (bitCheck(c, skipBits + 52)) result += INT_2P52;
		case 52: if (bitCheck(c, skipBits + 51)) result += INT_2P51;
		case 51: if (bitCheck(c, skipBits + 50)) result += INT_2P50;
		case 50: if (bitCheck(c, skipBits + 49)) result += INT_2P49;
		case 49: if (bitCheck(c, skipBits + 48)) result += INT_2P48;
		case 48: if (bitCheck(c, skipBits + 47)) result += INT_2P47;
		case 47: if (bitCheck(c, skipBits + 46)) result += INT_2P46;
		case 46: if (bitCheck(c, skipBits + 45)) result += INT_2P45;
		case 45: if (bitCheck(c, skipBits + 44)) result += INT_2P44;
		case 44: if (bitCheck(c, skipBits + 43)) result += INT_2P43;
		case 43: if (bitCheck(c, skipBits + 42)) result += INT_2P42;
		case 42: if (bitCheck(c, skipBits + 41)) result += INT_2P41;
		case 41: if (bitCheck(c, skipBits + 40)) result += INT_2P40;
		case 40: if (bitCheck(c, skipBits + 39)) result += INT_2P39;
		case 39: if (bitCheck(c, skipBits + 38)) result += INT_2P38;
		case 38: if (bitCheck(c, skipBits + 37)) result += INT_2P37;
		case 37: if (bitCheck(c, skipBits + 36)) result += INT_2P36;
		case 36: if (bitCheck(c, skipBits + 35)) result += INT_2P35;
		case 35: if (bitCheck(c, skipBits + 34)) result += INT_2P34;
		case 34: if (bitCheck(c, skipBits + 33)) result += INT_2P33;
		case 33: if (bitCheck(c, skipBits + 32)) result += INT_2P32;
		case 32: if (bitCheck(c, skipBits + 31)) result += INT_2P31;
		case 31: if (bitCheck(c, skipBits + 30)) result += INT_2P30;
		case 30: if (bitCheck(c, skipBits + 29)) result += INT_2P29;
		case 29: if (bitCheck(c, skipBits + 28)) result += INT_2P28;
		case 28: if (bitCheck(c, skipBits + 27)) result += INT_2P27;
		case 27: if (bitCheck(c, skipBits + 26)) result += INT_2P26;
		case 26: if (bitCheck(c, skipBits + 25)) result += INT_2P25;
		case 25: if (bitCheck(c, skipBits + 24)) result += INT_2P24;
		case 24: if (bitCheck(c, skipBits + 23)) result += INT_2P23;
		case 23: if (bitCheck(c, skipBits + 22)) result += INT_2P22;
		case 22: if (bitCheck(c, skipBits + 21)) result += INT_2P21;
		case 21: if (bitCheck(c, skipBits + 20)) result += INT_2P20;
		case 20: if (bitCheck(c, skipBits + 19)) result += INT_2P19;
		case 19: if (bitCheck(c, skipBits + 18)) result += INT_2P18;
		case 18: if (bitCheck(c, skipBits + 17)) result += INT_2P17;
		case 17: if (bitCheck(c, skipBits + 16)) result += INT_2P16;
		case 16: if (bitCheck(c, skipBits + 15)) result += INT_2P15;
		case 15: if (bitCheck(c, skipBits + 14)) result += INT_2P14;
		case 14: if (bitCheck(c, skipBits + 13)) result += INT_2P13;
		case 13: if (bitCheck(c, skipBits + 12)) result += INT_2P12;
		case 12: if (bitCheck(c, skipBits + 11)) result += INT_2P11;
		case 11: if (bitCheck(c, skipBits + 10)) result += INT_2P10;
		case 10: if (bitCheck(c, skipBits +  9)) result += INT_2P9;
		case  9: if (bitCheck(c, skipBits +  8)) result += INT_2P8;
		case  8: if (bitCheck(c, skipBits +  7)) result += INT_2P7;
		case  7: if (bitCheck(c, skipBits +  6)) result += INT_2P6;
		case  6: if (bitCheck(c, skipBits +  5)) result += INT_2P5;
		case  5: if (bitCheck(c, skipBits +  4)) result += INT_2P4;
		case  4: if (bitCheck(c, skipBits +  3)) result += INT_2P3;
		case  3: if (bitCheck(c, skipBits +  2)) result += INT_2P2;
		case  2: if (bitCheck(c, skipBits +  1)) result += INT_2P1;
		case  1: if (bitCheck(c, skipBits +  0)) result += INT_2P0;
	}

	return result;
}

// Pack Rat v2 (Pr)

void bitcpy(char * const target, const char * const source, const int targetBegin, const int sourceBegin, const int bits) {
	int targetBit = targetBegin % 8;
	int sourceBit = sourceBegin % 8;
	int targetByte = (targetBegin - targetBit) / 8;
	int sourceByte = (sourceBegin - sourceBit) / 8;

	for (int i = 0; i < bits; i++) {
		if (1 & (source[sourceByte] >> (7 - sourceBit))) {
			target[targetByte] |= (1 << (7 - targetBit)); // 1
		} else {
			target[targetByte] &= (UINT8_MAX ^ (1 << (7 - targetBit))); // 0
		}

		sourceBit++;
		if (sourceBit > 7) {
			sourceByte++;
			sourceBit = 0;
		}

		targetBit++;
		if (targetBit > 7) {
			targetByte++;
			targetBit = 0;
		}
	}
}

static void pruint_store(char * const target, uint64_t source, const int bitCount) {
	switch (bitCount) { // no breaks
		case 64: if (source >= pow2[63]) {target[7] |= 1 << 0; source -= pow2[63];} else target[7] &= (UINT8_MAX ^ 1 << 0);
		case 63: if (source >= pow2[62]) {target[7] |= 1 << 1; source -= pow2[62];} else target[7] &= (UINT8_MAX ^ 1 << 1);
		case 62: if (source >= pow2[61]) {target[7] |= 1 << 2; source -= pow2[61];} else target[7] &= (UINT8_MAX ^ 1 << 2);
		case 61: if (source >= pow2[60]) {target[7] |= 1 << 3; source -= pow2[60];} else target[7] &= (UINT8_MAX ^ 1 << 3);
		case 60: if (source >= pow2[59]) {target[7] |= 1 << 4; source -= pow2[59];} else target[7] &= (UINT8_MAX ^ 1 << 4);
		case 59: if (source >= pow2[58]) {target[7] |= 1 << 5; source -= pow2[58];} else target[7] &= (UINT8_MAX ^ 1 << 5);
		case 58: if (source >= pow2[57]) {target[7] |= 1 << 6; source -= pow2[57];} else target[7] &= (UINT8_MAX ^ 1 << 6);
		case 57: if (source >= pow2[56]) {target[7] |= 1 << 7; source -= pow2[56];} else target[7] &= (UINT8_MAX ^ 1 << 7);

		case 56: if (source >= pow2[55]) {target[6] |= 1 << 0; source -= pow2[55];} else target[6] &= (UINT8_MAX ^ 1 << 0);
		case 55: if (source >= pow2[54]) {target[6] |= 1 << 1; source -= pow2[54];} else target[6] &= (UINT8_MAX ^ 1 << 1);
		case 54: if (source >= pow2[53]) {target[6] |= 1 << 2; source -= pow2[53];} else target[6] &= (UINT8_MAX ^ 1 << 2);
		case 53: if (source >= pow2[52]) {target[6] |= 1 << 3; source -= pow2[52];} else target[6] &= (UINT8_MAX ^ 1 << 3);
		case 52: if (source >= pow2[51]) {target[6] |= 1 << 4; source -= pow2[51];} else target[6] &= (UINT8_MAX ^ 1 << 4);
		case 51: if (source >= pow2[50]) {target[6] |= 1 << 5; source -= pow2[50];} else target[6] &= (UINT8_MAX ^ 1 << 5);
		case 50: if (source >= pow2[49]) {target[6] |= 1 << 6; source -= pow2[49];} else target[6] &= (UINT8_MAX ^ 1 << 6);
		case 49: if (source >= pow2[48]) {target[6] |= 1 << 7; source -= pow2[48];} else target[6] &= (UINT8_MAX ^ 1 << 7);

		case 48: if (source >= pow2[47]) {target[5] |= 1 << 0; source -= pow2[47];} else target[5] &= (UINT8_MAX ^ 1 << 0);
		case 47: if (source >= pow2[46]) {target[5] |= 1 << 1; source -= pow2[46];} else target[5] &= (UINT8_MAX ^ 1 << 1);
		case 46: if (source >= pow2[45]) {target[5] |= 1 << 2; source -= pow2[45];} else target[5] &= (UINT8_MAX ^ 1 << 2);
		case 45: if (source >= pow2[44]) {target[5] |= 1 << 3; source -= pow2[44];} else target[5] &= (UINT8_MAX ^ 1 << 3);
		case 44: if (source >= pow2[43]) {target[5] |= 1 << 4; source -= pow2[43];} else target[5] &= (UINT8_MAX ^ 1 << 4);
		case 43: if (source >= pow2[42]) {target[5] |= 1 << 5; source -= pow2[42];} else target[5] &= (UINT8_MAX ^ 1 << 5);
		case 42: if (source >= pow2[41]) {target[5] |= 1 << 6; source -= pow2[41];} else target[5] &= (UINT8_MAX ^ 1 << 6);
		case 41: if (source >= pow2[40]) {target[5] |= 1 << 7; source -= pow2[40];} else target[5] &= (UINT8_MAX ^ 1 << 7);

		case 40: if (source >= pow2[39]) {target[4] |= 1 << 0; source -= pow2[39];} else target[4] &= (UINT8_MAX ^ 1 << 0);
		case 39: if (source >= pow2[38]) {target[4] |= 1 << 1; source -= pow2[38];} else target[4] &= (UINT8_MAX ^ 1 << 1);
		case 38: if (source >= pow2[37]) {target[4] |= 1 << 2; source -= pow2[37];} else target[4] &= (UINT8_MAX ^ 1 << 2);
		case 37: if (source >= pow2[36]) {target[4] |= 1 << 3; source -= pow2[36];} else target[4] &= (UINT8_MAX ^ 1 << 3);
		case 36: if (source >= pow2[35]) {target[4] |= 1 << 4; source -= pow2[35];} else target[4] &= (UINT8_MAX ^ 1 << 4);
		case 35: if (source >= pow2[34]) {target[4] |= 1 << 5; source -= pow2[34];} else target[4] &= (UINT8_MAX ^ 1 << 5);
		case 34: if (source >= pow2[33]) {target[4] |= 1 << 6; source -= pow2[33];} else target[4] &= (UINT8_MAX ^ 1 << 6);
		case 33: if (source >= pow2[32]) {target[4] |= 1 << 7; source -= pow2[32];} else target[4] &= (UINT8_MAX ^ 1 << 7);

		case 32: if (source >= pow2[31]) {target[3] |= 1 << 0; source -= pow2[31];} else target[3] &= (UINT8_MAX ^ 1 << 0);
		case 31: if (source >= pow2[30]) {target[3] |= 1 << 1; source -= pow2[30];} else target[3] &= (UINT8_MAX ^ 1 << 1);
		case 30: if (source >= pow2[29]) {target[3] |= 1 << 2; source -= pow2[29];} else target[3] &= (UINT8_MAX ^ 1 << 2);
		case 29: if (source >= pow2[28]) {target[3] |= 1 << 3; source -= pow2[28];} else target[3] &= (UINT8_MAX ^ 1 << 3);
		case 28: if (source >= pow2[27]) {target[3] |= 1 << 4; source -= pow2[27];} else target[3] &= (UINT8_MAX ^ 1 << 4);
		case 27: if (source >= pow2[26]) {target[3] |= 1 << 5; source -= pow2[26];} else target[3] &= (UINT8_MAX ^ 1 << 5);
		case 26: if (source >= pow2[25]) {target[3] |= 1 << 6; source -= pow2[25];} else target[3] &= (UINT8_MAX ^ 1 << 6);
		case 25: if (source >= pow2[24]) {target[3] |= 1 << 7; source -= pow2[24];} else target[3] &= (UINT8_MAX ^ 1 << 7);

		case 24: if (source >= pow2[23]) {target[2] |= 1 << 0; source -= pow2[23];} else target[2] &= (UINT8_MAX ^ 1 << 0);
		case 23: if (source >= pow2[22]) {target[2] |= 1 << 1; source -= pow2[22];} else target[2] &= (UINT8_MAX ^ 1 << 1);
		case 22: if (source >= pow2[21]) {target[2] |= 1 << 2; source -= pow2[21];} else target[2] &= (UINT8_MAX ^ 1 << 2);
		case 21: if (source >= pow2[20]) {target[2] |= 1 << 3; source -= pow2[20];} else target[2] &= (UINT8_MAX ^ 1 << 3);
		case 20: if (source >= pow2[19]) {target[2] |= 1 << 4; source -= pow2[19];} else target[2] &= (UINT8_MAX ^ 1 << 4);
		case 19: if (source >= pow2[18]) {target[2] |= 1 << 5; source -= pow2[18];} else target[2] &= (UINT8_MAX ^ 1 << 5);
		case 18: if (source >= pow2[17]) {target[2] |= 1 << 6; source -= pow2[17];} else target[2] &= (UINT8_MAX ^ 1 << 6);
		case 17: if (source >= pow2[16]) {target[2] |= 1 << 7; source -= pow2[16];} else target[2] &= (UINT8_MAX ^ 1 << 7);

		case 16: if (source >= pow2[15]) {target[1] |= 1 << 0; source -= pow2[15];} else target[1] &= (UINT8_MAX ^ 1 << 0);
		case 15: if (source >= pow2[14]) {target[1] |= 1 << 1; source -= pow2[14];} else target[1] &= (UINT8_MAX ^ 1 << 1);
		case 14: if (source >= pow2[13]) {target[1] |= 1 << 2; source -= pow2[13];} else target[1] &= (UINT8_MAX ^ 1 << 2);
		case 13: if (source >= pow2[12]) {target[1] |= 1 << 3; source -= pow2[12];} else target[1] &= (UINT8_MAX ^ 1 << 3);
		case 12: if (source >= pow2[11]) {target[1] |= 1 << 4; source -= pow2[11];} else target[1] &= (UINT8_MAX ^ 1 << 4);
		case 11: if (source >= pow2[10]) {target[1] |= 1 << 5; source -= pow2[10];} else target[1] &= (UINT8_MAX ^ 1 << 5);
		case 10: if (source >= pow2[9])  {target[1] |= 1 << 6; source -= pow2[9];}  else target[1] &= (UINT8_MAX ^ 1 << 6);
		case  9: if (source >= pow2[8])  {target[1] |= 1 << 7; source -= pow2[8];}  else target[1] &= (UINT8_MAX ^ 1 << 7);

		case  8: if (source >= pow2[7])  {target[0] |= 1 << 0; source -= pow2[7];}  else target[0] &= (UINT8_MAX ^ 1 << 0);
		case  7: if (source >= pow2[6])  {target[0] |= 1 << 1; source -= pow2[6];}  else target[0] &= (UINT8_MAX ^ 1 << 1);
		case  6: if (source >= pow2[5])  {target[0] |= 1 << 2; source -= pow2[5];}  else target[0] &= (UINT8_MAX ^ 1 << 2);
		case  5: if (source >= pow2[4])  {target[0] |= 1 << 3; source -= pow2[4];}  else target[0] &= (UINT8_MAX ^ 1 << 3);
		case  4: if (source >= pow2[3])  {target[0] |= 1 << 4; source -= pow2[3];}  else target[0] &= (UINT8_MAX ^ 1 << 4);
		case  3: if (source >= pow2[2])  {target[0] |= 1 << 5; source -= pow2[2];}  else target[0] &= (UINT8_MAX ^ 1 << 5);
		case  2: if (source >= pow2[1])  {target[0] |= 1 << 6; source -= pow2[1];}  else target[0] &= (UINT8_MAX ^ 1 << 6);
		case  1: if (source >= pow2[0])  {target[0] |= 1 << 7; source -= pow2[0];}  else target[0] &= (UINT8_MAX ^ 1 << 7);
	}
}

// Upgrade PRC to PrC (Pack Rat Compact v1 to v2)
int upgrade_compact2(const char * const pathOld, const char * const pathNew) {
	int priOld = open(pathOld, O_RDONLY);
	if (priOld == -1) return 1;

	char header[5];
	ssize_t bytesRead = read(priOld, header, 5);
	if (bytesRead != 5) return 1;

	if (header[0] != 'P' || header[1] != 'R' || header[2] != 'C') {
		puts("Not a Pack Rat Compact v1 (PRC) index file");
		return 1;
	}

	const uint8_t bitsPos = header[3];
	const int infoBytes = bytesInBits(bitsPos, bytesInBits_UP);

	const off_t fileBytes = lseek(priOld, 0, SEEK_END);
	lseek(priOld, 5, SEEK_SET);

	// Write new PRI header
	int priNew = open(pathNew, O_WRONLY | O_CREAT, 0644);
	if (priNew == -1) return 1;
	write(priNew, "PrC", 3);
	write(priNew, &bitsPos, 1);
	char zero = '\0';
	write(priNew, &zero, 1);

	off_t doneBytes = 5;
	while (doneBytes < fileBytes) {
		// Old (v1)
		char oldInfo[infoBytes];

		int bytesRead = read(priOld, oldInfo, infoBytes);
		if (bytesRead != infoBytes) {
			close(priOld);
			close(priNew);
			puts("Failed read()");
			return 1;
		}

		const uint64_t pos = simpleUint_toInt(oldInfo, 0, bitsPos);

		// New (v2)
		char cpr_pos[infoBytes];
		bzero(cpr_pos, infoBytes);
		pruint_store(cpr_pos, pos, bitsPos);

		const int ret = write(priNew, cpr_pos, infoBytes);
		if (ret != infoBytes) {
			close(priOld);
			close(priNew);
			puts("Failed write()");
			return 1;
		}

		doneBytes += infoBytes;
	}

	close(priOld);
	close(priNew);
	puts("Pack Rat Compact v1 to v2 upgrade successful.");
	return 0;
}

// Upgrade PR0 to Pr0 (Pack Rat Zero v1 to v2)
int upgrade_zero2(const char * const pathOld, const char * const pathNew) {
	int priOld = open(pathOld, O_RDONLY);
	if (priOld == -1) return 1;

	char header[5];
	ssize_t bytesRead = read(priOld, header, 5);
	if (bytesRead != 5) return 1;

	if (header[0] != 'P' || header[1] != 'R' || header[2] != '0') {
		puts("Not a Pack Rat Zero v1 (PR0) index file");
		return 1;
	}

	const uint8_t bitsPos = header[3];
	const uint8_t bitsLen = header[4];
	const int infoBytes = bytesInBits(bitsPos + bitsLen, bytesInBits_UP);

	const off_t fileBytes = lseek(priOld, 0, SEEK_END);
	lseek(priOld, 5, SEEK_SET);

	// Write new PRI header
	int priNew = open(pathNew, O_WRONLY | O_CREAT, 0644);
	if (priNew == -1) return 1;
	write(priNew, "Pr0", 3);
	write(priNew, &bitsPos, 1);
	write(priNew, &bitsLen, 1);

	off_t doneBytes = 5;
	while (doneBytes < fileBytes) {
		// Old (v1)
		char oldInfo[infoBytes];

		int bytesRead = read(priOld, oldInfo, infoBytes);
		if (bytesRead != infoBytes) {
			close(priOld);
			close(priNew);
			puts("Failed read()");
			return 1;
		}

		const uint64_t pos = simpleUint_toInt(oldInfo, 0,       bitsPos);
		const uint64_t len = simpleUint_toInt(oldInfo, bitsPos, bitsLen);

		// New (v2)
		char cpr_pos[infoBytes]; pruint_store(cpr_pos, pos, bitsPos);
		char cpr_len[infoBytes]; pruint_store(cpr_len, len, bitsLen);

		char cpr_full[infoBytes];
		bzero(cpr_full, infoBytes);
		bitcpy(cpr_full, cpr_pos, 0, 0, bitsPos);
		bitcpy(cpr_full, cpr_len, bitsPos, 0, bitsLen);

		const int ret = write(priNew, cpr_full, infoBytes);
		if (ret != infoBytes) {
			close(priOld);
			close(priNew);
			puts("Failed write()");
			return 1;
		}

		doneBytes += infoBytes;
	}

	close(priOld);
	close(priNew);
	puts("Pack Rat Zero v1 to v2 upgrade successful.");
	return 0;
}

int main(int argc, char *argv[]) {
	if (argc != 3) {
		puts("Usage: packrat-upgrade Old.pri New.pri");
		return 1;
	}

//	return upgrade_compact2(argv[1], argv[2]);
	return upgrade_zero2(argv[1], argv[2]);
}
