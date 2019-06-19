#define _FILE_OFFSET_BITS 64

#include <stdlib.h>
#include <unistd.h>
#include <sys/file.h>
#include <stdint.h>
#include <math.h> // for ceil, floor
#include <stdio.h>

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

// Read: Pack Rat Zero (PR0)
static int packrat_read_zero(int pri, int bitsPos, int bitsLen, const char *pathPrd, const uint64_t id, char **data) {
	if (pri < 0) return -1;

	const int infoBytes = ceil((bitsPos + bitsLen) / (double)8);

	// Pack Rat Index: Position and Length
	char info[infoBytes];
	int bytesRead = pread(pri, info, infoBytes, 5 + id * infoBytes);
	if (bytesRead != infoBytes) {
		close(pri);
		return -2;
	}

	const uint64_t pos = simpleUint_toInt(info, 0,       bitsPos);
	const uint64_t len = simpleUint_toInt(info, bitsPos, bitsLen);
	close(pri);

	if (len < 1) return -2;

	// Pack Rat Data: File contents
	int prd = open(pathPrd, O_RDONLY);
	if (prd == -1) return -3;

	*data = malloc(len + 1);
	if (*data == NULL) return -4;

	bytesRead = pread(prd, *data, len, pos);
	close(prd);

	if (bytesRead != len) {
		free(*data);
		return -5;
	}

	return len;
}

static uint64_t getPos(int pri, const int infoBytes, const int id, const int bitsPos) {
	char info[infoBytes];
	const int bytesRead = pread(pri, info, infoBytes, 5 + id * infoBytes);
	return (bytesRead != infoBytes) ? -1 : simpleUint_toInt(info, 0, bitsPos);
}

// Read: Pack Rat Compact (PRC)
static int packrat_read_compact(int pri, const int bitsPos, const char *pathPrd, const uint64_t id, char **data) {
	if (pri < 0) return -1;

	const off_t endPri = lseek(pri, 0, SEEK_END);
	const int infoBytes = ceil(bitsPos / (double)8);

	uint64_t len;
	int prd;

	const uint64_t pos = getPos(pri, infoBytes, id, bitsPos);
	if (pos < 0) return -2;

	// Pack Rat Data: File contents
	if ((5 + (id + 1) * infoBytes) >= endPri) {
		// Request is for the last file stored
		close(pri);

		prd = open(pathPrd, O_RDONLY);
		if (prd == -1) return -3;

		// Length is eof - start
		const uint64_t endPrd = lseek(prd, 0, SEEK_END);
		len = endPrd - pos;
	} else {
		const uint64_t end = getPos(pri, infoBytes, id + 1, bitsPos);
		len = end - pos;
		close(pri);

		if (len < 1) return -2;

		prd = open(pathPrd, O_RDONLY);
		if (prd == -1) return -3;
	}

	*data = malloc(len + 1);
	if (*data == NULL) return -4;

	const int bytesRead = pread(prd, *data, len, pos);
	close(prd);

	if (bytesRead != len) {
		free(*data);
		return -5;
	}

	return len;
}

// Read: Main function - chooses format automatically.
// Returns length of file on success, or a negative error code on failure
int packrat_read(const char *pathPri, const char *pathPrd, const uint64_t id, char **data) {
	int pri = open(pathPri, O_RDONLY);
	if (pri == -1) return -1;

	char header[5];
	ssize_t bytesRead = read(pri, header, 5);
	if (bytesRead != 5) return -2;

	if (header[0] != 'P' || header[1] != 'R') return -3;

	const int bitsPos = header[3];
	const int bitsLen = header[4];

	int ret = -4;
	if (header[2] == 'C') ret = packrat_read_compact(pri, bitsPos, pathPrd, id, data);
	if (header[2] == '0') ret = packrat_read_zero(pri, bitsPos, bitsLen, pathPrd, id, data);

	close(pri);
	return ret;
}

int main(int argc, char *argv[]) {
	if (argc < 5) {
		puts("Usage: packrat-reader file.pri file.prd number outfile");
		return 1;
	}

	char *data;
	const int len = packrat_read(argv[1], argv[2], strtol(argv[3], NULL, 10), &data);
	if (len < 1) return 2;

	int fd = open(argv[4], O_WRONLY | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
	if (fd < 0) {free(data); return 3;}
	write(fd, data, len);
	close(fd);

	return 0;
}
