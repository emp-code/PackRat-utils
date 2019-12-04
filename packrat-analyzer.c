// Analyzer for Pack Rat Archival System files
// Updated to support Pack Rat v2 archives with file signature 'Pr'

#define _FILE_OFFSET_BITS 64

#include <stdlib.h>
#include <unistd.h>
#include <sys/file.h>
#include <stdint.h>
#include <math.h> // for ceil
#include <stdio.h>

static const uint64_t pow2[] = {1,2,4,8,16,32,64,128,256,512,1024,2048,4096,8192,16384,32768,65536,131072,262144,524288,1048576,2097152,4194304,8388608,16777216,33554432,67108864,134217728,268435456,536870912,1073741824,2147483648,4294967296,8589934592,17179869184,34359738368,68719476736,137438953472,274877906944,549755813888,1099511627776,2199023255552,4398046511104,8796093022208,17592186044416,35184372088832,70368744177664,140737488355328,281474976710656,562949953421312,1125899906842624,2251799813685248,4503599627370496,9007199254740992,18014398509481984,36028797018963968,72057594037927936,144115188075855872,288230376151711744,576460752303423488,1152921504606846976,2305843009213693952,4611686018427387904,9223372036854775808ULL};

static uint64_t pruint_fetch(const char * const source, const int skipBits, const int bits) {
	int bit = skipBits % 8;
	int byte = (skipBits - bit) / 8;
	uint64_t result = 0;

	for (int i = 0; i < bits; i++) {
		if (1 & (source[byte] >> (7 - bit))) result += pow2[i];

		bit++;
		if (bit > 7) {
			byte++;
			bit = 0;
		}
	}

	return result;
}

static uint64_t getPos(int pri, const int infoBytes, const int id, const int bitsPos) {
	char info[infoBytes];
	const int bytesRead = pread(pri, info, infoBytes, 5 + id * infoBytes);
	return (bytesRead != infoBytes) ? -1 : pruint_fetch(info, 0, bitsPos);
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
	puts("Pack Rat Analyzer. Supports Pack Rat v2 archives (file signature 'Pr').");

	if (argc < 2) {
		printf(
			"Usage:\n"
			"Short analysis (index only): %s index.pri\n"
			"Full analysis (index and data): %s index.pri data.prd\n"
		, argv[0], argv[0]);

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

	if (header[0] != 'P' || header[1] != 'r') {
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

	printf("Bits used for position: %d\n", bitsPos);
	printf("Bits used for length: %d%s\n", bitsLen, (type == 'C') ? " (N/A)" : "");

	const int infoBytes = (type == 'C') ? ceil(bitsPos / (double)8) : ceil((bitsPos + bitsLen) / (double)8);
	printf("Bytes used per entry: %d\n", infoBytes);

	puts("\n== PRI (Index): Entries ==");

	const uint64_t indexSize = lseek(pri, 0, SEEK_END) - 5;

	const int entryCount = indexSize / infoBytes;
	printf("Number of entries: %d\n", entryCount);

	int phCount = 0;
	uint64_t expectedSize = 0;
	if (type == 'C' && entryCount > 1) {
		uint64_t lastPos;
		for (int i = 1; (entryCount - i) > 0; i++) {
			lastPos = getPos(pri, infoBytes, entryCount - i, bitsPos);
			if (lastPos != 0) break;
		}

		double humanSize;
		char *humanUnit = humanReadableSize(lastPos, &humanSize);

		printf("Last file stored in the PRD begins at byte %ld (%.2f %s).\n", lastPos, humanSize, humanUnit);
		expectedSize = lastPos;
	} else if (type == '0') {
		for (int i = 0; i < entryCount; i++) {
			char info[infoBytes];
			int bytesRead = pread(pri, info, infoBytes, 5 + (i * infoBytes));
			if (bytesRead != infoBytes) {close(pri); return -2;}

			const uint64_t pos = pruint_fetch(info, 0,       bitsPos);
			const uint64_t len = pruint_fetch(info, bitsPos, bitsLen);
			expectedSize += len;
			if (len == 0) phCount++;
		}

		double humanSize;
		char *humanUnit = humanReadableSize(expectedSize, &humanSize);

		printf("Expected PRD size: %ld bytes (%.2f %s)\n", expectedSize, humanSize, humanUnit);

		printf("Number of placeholders: %d\n", phCount);

		double avgSize = expectedSize / (entryCount);
		humanUnit = humanReadableSize_d(avgSize, &humanSize);
		printf("Average file size, including placeholders: %f bytes (%.3f %s)\n", avgSize, humanSize, humanUnit);

		avgSize = expectedSize / (entryCount - phCount);
		humanUnit = humanReadableSize_d(avgSize, &humanSize);
		printf("Average file size, excluding placeholders: %f bytes (%.3f %s)\n", avgSize, humanSize, humanUnit);
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

	puts("\n== PRI (Index): Full scan ==");

	FILE* prif = fopen(argv[1], "rb"); // faster than open() method for large reads
	if (prif == NULL) {puts("Failed to open PRI file. Exiting."); return 1;}
	fseek(prif, 5, SEEK_SET); // skip header

	if (type == '0') {
		for (int i = 0; i < entryCount; i++) {
			char info[infoBytes];
			int bytesRead = fread(info, 1, infoBytes, prif);
			if (bytesRead != infoBytes) {close(pri); return -2;}

			const uint64_t pos = pruint_fetch(info, 0, bitsPos);
			if (pos > dataSize) printf("Entry %d is corrupt: position size (%ld) is higher than file size.\n", i, pos);
		}
	} else if (type == 'C') {
		uint64_t prevPos = 0;
		int validCount = 0;

		for (int i = 0; i < entryCount; i++) {
			char info[infoBytes];
			int bytesRead = fread(info, 1, infoBytes, prif);
			if (bytesRead != infoBytes) {close(pri); return -2;}

			const uint64_t pos = pruint_fetch(info, 0, bitsPos);
			if (pos != 0) {
				if (pos > dataSize) printf("Entry %d is corrupt: position size (%ld) is higher than file size.\n", i, pos);
				if (pos < prevPos) printf("Entry %d is corrupt: position size (%ld) is located before the position of the previous entry (%ld).\n", i, pos, prevPos);
				prevPos = pos;
				validCount++;
			}
		}

		printf("%d non-empty files after the first file.\n", validCount);
	}

	fclose(prif);

	puts("\nAnalysis done. Exiting.");
	return 0;
}
