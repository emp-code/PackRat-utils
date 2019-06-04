// Analyzer for Pack Rat Archival System files

#define _FILE_OFFSET_BITS 64

#include <stdlib.h>
#include <unistd.h>
#include <sys/file.h>
#include <stdint.h>
#include <math.h> // for ceil
#include <stdio.h>

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

static uint64_t getPos(int pri, const int infoBytes, const int id, const int bitsPos) {
	char info[infoBytes];
	const int bytesRead = pread(pri, info, infoBytes, 5 + id * infoBytes);
	return (bytesRead != infoBytes) ? -1 : simpleUint_toInt(info, 0, bitsPos);
}

static char* humanReadableSize_d(const double bytes, double *humanSize) {
	*humanSize = bytes;

	char *humanUnit = "B";
	if (*humanSize > 1023) {*humanSize /= 1024; humanUnit = "KiB";}
	if (*humanSize > 1023) {*humanSize /= 1024; humanUnit = "MiB";}
	if (*humanSize > 1023) {*humanSize /= 1024; humanUnit = "GiB";}
	if (*humanSize > 1023) {*humanSize /= 1024; humanUnit = "TiB";}

	return humanUnit;
}

static char* humanReadableSize(const uint64_t bytes, double *humanSize) {
	*humanSize = bytes;

	char *humanUnit = "B";
	if (*humanSize > 1023) {*humanSize /= 1024; humanUnit = "KiB";}
	if (*humanSize > 1023) {*humanSize /= 1024; humanUnit = "MiB";}
	if (*humanSize > 1023) {*humanSize /= 1024; humanUnit = "GiB";}
	if (*humanSize > 1023) {*humanSize /= 1024; humanUnit = "TiB";}

	return humanUnit;
}

int main(int argc, char *argv[]) {
	puts("Pack Rat Analyzer by EMPcode.");

	if (argc < 2) {
		printf(
			"Usage:\nShort analysis (index only, fast): %s index.pri\n"
			"Full analysis (index and data, may be slow): %s index.pri data.prd\n",
		argv[0], argv[0]);

		return 0;
	}

	puts("\n== PRI (Index): Header ==");

	char analyze_type;
	int analyze_entryCount;

	int pri = open(argv[1], O_RDONLY);
	if (pri == -1) {puts("Failed to open PRI file. Exiting."); return 1;}

	char header[5];
	ssize_t bytesRead = read(pri, header, 5);
	if (bytesRead != 5) return -2;

	if (header[0] != 'P' || header[1] != 'R') {
		puts("File signature is wrong. Exiting.");
		return 0;
	}

	const char type = header[2];

	switch(type) {
		case '0': puts("Type: Zero."); break;
		case 'C': puts("Type: Compact."); break;
		default: puts("Invalid type. Exiting."); return 0;
	}

	const int bitsPos = header[3];
	const int bitsLen = header[4];

	printf("Bits used for position: %d.\n", bitsPos);
	printf("Bits used for length: %d%s.\n", bitsLen, (type == 'C') ? " (N/A)" : "");

	const int infoBytes = (type == 'C') ? ceil(bitsPos / (double)8) : ceil((bitsPos + bitsLen) / (double)8);
	printf("Bytes used per entry: %d.\n", infoBytes);

	puts("\n== PRI (Index): Entries ==");

	const uint64_t indexSize = lseek(pri, 0, SEEK_END) - 5;

	const int entryCount = indexSize / infoBytes;
	printf("Number of entries: %d.\n", entryCount);

	uint64_t expectedSize = 0;
	if (type == 'C') {
		uint64_t lastPos = getPos(pri, infoBytes, entryCount - 1, bitsPos);
		double humanSize;
		char *humanUnit = humanReadableSize(lastPos, &humanSize);

		printf("Last file stored in the PRD begins at byte %ld (%.2f %s).\n", lastPos, humanSize, humanUnit);
		expectedSize = lastPos;
	} else if (type == '0') {
		for (int i = 1; expectedSize == 0; i++) {
			char info[infoBytes];
			int bytesRead = pread(pri, info, infoBytes, 5 + (entryCount - i) * infoBytes);
			if (bytesRead != infoBytes) {close(pri); return -2;}

			const uint64_t pos = simpleUint_toInt(info, 0,       bitsPos);
			const uint64_t len = simpleUint_toInt(info, bitsPos, bitsLen);
			expectedSize = pos + len;
		}

		double humanSize;
		char *humanUnit = humanReadableSize(expectedSize, &humanSize);

		printf("Expected PRD size: %ld (%.2f %s).\n", expectedSize, humanSize, humanUnit);

		double avgSize = expectedSize / entryCount;
		humanUnit = humanReadableSize_d(avgSize, &humanSize);
		printf("Average file size, including placeholders: %.3f %s.\n", humanSize, humanUnit);
	}

	close(pri);

	if (argc < 3) {puts("\nPRD (Data) analysis not requested. Exiting."); return 0;}

	puts("\n== PRD (Data) ==");

	int prd = open(argv[2], O_RDONLY);
	if (prd == -1) {puts("Failed to open PRD file. Exiting."); return 1;}

	const uint64_t dataSize = lseek(pri, 0, SEEK_END);
	if (type == '0') {
		if (dataSize == expectedSize) {
			puts("Size matches.");
		} else {
			printf("Size (%ld) does not match expected size (%ld).\n", dataSize, expectedSize);
		}
	} else if (type == 'C') {
		if (dataSize > expectedSize) {
			puts("Size exceeds expected minimum size.");
		} else {
			printf("Size (%ld) is less than expected minimum size (%ld).\n", dataSize, expectedSize);
		}
	}

	close(prd);

	puts("\n== PRI (Index): Full scan - may be slow ==");
	uint64_t totalDataSize = 0;
	int phCount = 0;

	FILE* prif = fopen(argv[1], "rb"); // faster than open() method for large reads
	if (prif == NULL) {puts("Failed to open PRI file. Exiting."); return 1;}
	fseek(prif, 5, SEEK_SET); // skip header

	if (type == '0') {
		for (int i = 0; i < entryCount; i++) {
			char info[infoBytes];
			int bytesRead = fread(info, 1, infoBytes, prif);
			if (bytesRead != infoBytes) {close(pri); return -2;}

			const uint64_t pos = simpleUint_toInt(info, 0,       bitsPos);
			const uint64_t len = simpleUint_toInt(info, bitsPos, bitsLen);

			if (pos > dataSize) printf("Entry %d is corrupt: position size (%ld) is higher than file size.\n", i, pos);

			if (len == 0) phCount++;

			totalDataSize += len;
		}

		printf("Number of placeholders: %d\n", phCount);

		double avgSize = expectedSize / (entryCount - phCount);
		double humanSize;
		char *humanUnit = humanReadableSize_d(avgSize, &humanSize);
		printf("Average file size, excluding placeholders: %.3f %s.\n", humanSize, humanUnit);

		if (totalDataSize == dataSize)
			puts("Total size of files matches PRD size.");
		else
			printf("Total size of files (%ld) does not match PRD size (%ld).\n", totalDataSize, dataSize);
	} else if (type == 'C') {
		uint64_t prevPos = 0;

		for (int i = 0; i < entryCount; i++) {
			char info[infoBytes];
			int bytesRead = fread(info, 1, infoBytes, prif);
			if (bytesRead != infoBytes) {close(pri); return -2;}

			const uint64_t pos = simpleUint_toInt(info, 0, bitsPos);
			if (pos > dataSize) printf("Entry %d is corrupt: position size (%ld) is higher than file size.\n", i, pos);
			if (pos < prevPos) printf("Entry %d is corrupt: position size (%ld) is located before the position of the previous entry (%ld).\n", i, pos, prevPos);
			prevPos = pos;
		}
	}

	fclose(prif);

	puts("\nAnalysis done. Exiting.");
	return 0;
}
