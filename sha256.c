#include "sha256.h"
#include <assert.h>
#include <stdlib.h>


/* SHA256 constants */
uint32_t K[64] = {
	0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
	0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
	0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
	0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
	0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
	0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
	0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
	0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2,
};

/* Initial Hash Values */
uint32_t H[8] = {
	0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
	0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19,
};

void sha256_round(uint8_t msg[])
{
	uint32_t *arr = (uint32_t *) msg;
	for (int i = 0; i < 16; i++) {
		arr[i] = (msg[4*i] << 8*3) | (msg[4*i+1] << 8*2) | (msg[4*i+2] << 8*1) | (msg[4*i+3] << 8*0); // convert to big endian
	}

	uint32_t W[64];

	for (int t = 0; t <= 15; t++) {
		W[t] = arr[t];
#ifndef NDEBUG
		printf("W[%d] = %x\n", t, W[t]);
#endif
	}

	for (int t = 16; t <= 63; t++) {
		W[t] = s1(W[t-2]) + W[t-7] + s0(W[t-15]) + W[t-16];
#ifndef NDEBUG
		printf("W[%d] = %x\n", t, W[t]);
#endif
	}

	uint32_t a = H[0];
	uint32_t b = H[1];
	uint32_t c = H[2];
	uint32_t d = H[3];
	uint32_t e = H[4];
	uint32_t f = H[5];
	uint32_t g = H[6];
	uint32_t h = H[7];

	for (int t = 0; t <= 63; t++) {
		uint32_t T1 = h + S1(e) + Ch(e, f, g) + K[t] + W[t];
		uint32_t T2 = S0(a) + Maj(a, b, c);
		h = g;
		g = f;
		f = e;
		e = d + T1;
		d = c;
		c = b;
		b = a;
		a = T1 + T2;
#ifndef NDEBUG
		printf("%x %x %x %x %x %x %x %x\n", a, b, c, d, e, f, g, h);
#endif
	}

	H[0] += a;
	H[1] += b;
	H[2] += c;
	H[3] += d;
	H[4] += e;
	H[5] += f;
	H[6] += g;
	H[7] += h;
}

void sha256(FILE *fp)
{
	uint8_t msg_buf[64]; // 512 bit array
	size_t bytes_written;
	uint64_t total_bytes_read = 0;
	while ((bytes_written = fread(msg_buf, sizeof(uint8_t), 64, fp)) == 64) {
		total_bytes_read += sizeof(uint8_t) * 64;
		sha256_round(msg_buf);
	}

	if (ferror(fp)) {
		perror("Reading file caused error");
		exit(EXIT_FAILURE);
	}

	if (bytes_written == 0)
		return;
#ifndef NDEBUG
	printf("%lu\n", bytes_written);
#endif

	assert(feof(fp));
	if (bytes_written < 64) {
		total_bytes_read += bytes_written;
#ifndef NDEBUG
		printf("total: %lu\n", total_bytes_read);
#endif
		msg_buf[bytes_written] = 0x80; // appending 1000 0000 to the msg
		if (bytes_written < 56) { // 1 block padding
			for (size_t i = bytes_written+1; i < 56; i++) {
				msg_buf[i] = 0;
			}
			// uint64_t *len_bits = (uint64_t *) &msg_buf[56]; // wont work because of endianness
			// *len_bits = total_bytes_read * 8;

			uint64_t total_bits_read = total_bytes_read * 8;
			msg_buf[56] = (total_bits_read >> 7*8) & 0xFF;
			msg_buf[57] = (total_bits_read >> 6*8) & 0xFF;
			msg_buf[58] = (total_bits_read >> 5*8) & 0xFF;
			msg_buf[59] = (total_bits_read >> 4*8) & 0xFF;
			msg_buf[60] = (total_bits_read >> 3*8) & 0xFF;
			msg_buf[61] = (total_bits_read >> 2*8) & 0xFF;
			msg_buf[62] = (total_bits_read >> 1*8) & 0xFF;
			msg_buf[63] = (total_bits_read >> 0*8) & 0xFF;

#ifndef NDEBUG
			for (int i = 0; i < 64; i++) {
				printf("%x", msg_buf[i]);
			}
#endif
			sha256_round(msg_buf);

		} else { // 2 block padding

			for (size_t i = bytes_written + 1; i < 64; i++) {
				msg_buf[i] = 0;
			}
			sha256_round(msg_buf);
			for (size_t i = 0; i < 56; i++) {
				msg_buf[i] = 0;
			}

			uint64_t total_bits_read = total_bytes_read * 8;
			msg_buf[56] = (total_bits_read >> 7*8) & 0xFF;
			msg_buf[57] = (total_bits_read >> 6*8) & 0xFF;
			msg_buf[58] = (total_bits_read >> 5*8) & 0xFF;
			msg_buf[59] = (total_bits_read >> 4*8) & 0xFF;
			msg_buf[60] = (total_bits_read >> 3*8) & 0xFF;
			msg_buf[61] = (total_bits_read >> 2*8) & 0xFF;
			msg_buf[62] = (total_bits_read >> 1*8) & 0xFF;
			msg_buf[63] = (total_bits_read >> 0*8) & 0xFF;
			sha256_round(msg_buf);

		}
	}
}

void print_digest()
{
	printf("%x%x%x%x%x%x%x%x\n", H[0], H[1], H[2], H[3], H[4], H[5], H[6], H[7]);
}
