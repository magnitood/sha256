#include <stdio.h>
#include <stdint.h>

/* a single interation of SHA on a block of 512 bits */
void sha256_round(uint8_t arr[]);

/* computes sha256 of file */
void sha256(FILE *);

/* prints the digest */
void print_digest();

#define w 32 /* word size */

#define ROTR(x, n) (((x) >> (n)) | ((x) << ((w) - (n)))) /* Rotate Right x by n */
#define SHR(x, n) ((x) >> (n)) /* Shift Right x by n */
#define MOD(x, n) (((x)%(n) >= 0) ? (x)%(n) : (n)+((x)%(n))) /* Congruent Modulo */

#define Ch(x, y, z) (((x) & (y)) ^ (~(x) & (z))) /* Choose */
#define Maj(x, y, z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z))) /* Majority */
#define S0(x) (ROTR((x), 2) ^ ROTR((x), 13) ^ ROTR((x), 22))
#define S1(x) (ROTR((x), 6) ^ ROTR((x), 11) ^ ROTR((x), 25))
#define s0(x) (ROTR((x), 7) ^ ROTR((x), 18) ^ SHR((x), 3))
#define s1(x) (ROTR((x), 17) ^ ROTR((x), 19) ^ SHR((x), 10))
