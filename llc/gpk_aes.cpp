// Tiny AES in C (from https://github.com/kokke/tiny-AES-c). A small and portable implementation of the AES ECB, CTR and CBC encryption algorithms written in C.
// The API is very simple and looks like this (I am using C99 <stdint.h>-style annotated types):
//
// /* Initialize context calling one of: */
// void AES_init_ctx(struct AES_ctx* ctx, const uint8_t* key);
// void AES_init_ctx_iv(struct AES_ctx* ctx, const uint8_t* key, const uint8_t* iv);
//
// /* ... or reset IV at random point: */
// void AES_ctx_set_iv(struct AES_ctx* ctx, const uint8_t* iv);
//
// /* Then start encrypting and decrypting with the functions below: */
// void AES_ECB_encrypt(struct AES_ctx* ctx, uint8_t* buf);
// void AES_ECB_decrypt(struct AES_ctx* ctx, uint8_t* buf);
//
// void AES_CBC_encrypt_buffer(struct AES_ctx* ctx, uint8_t* buf, uint32_t length);
// void AES_CBC_decrypt_buffer(struct AES_ctx* ctx, uint8_t* buf, uint32_t length);
//
// /* Same function for encrypting as for decrypting in CTR mode */
// void AES_CTR_xcrypt_buffer(struct AES_ctx* ctx, uint8_t* buf, uint32_t length);
// Note:
//
// No padding is provided so for CBC and ECB all buffers should be multiples of 16 bytes. For padding PKCS7 is recommendable.
// ECB mode is considered unsafe for most uses and is not implemented in streaming mode. If you need this mode, call the function for every block of 16 bytes you need encrypted. See wikipedia's article on ECB for more details.
// You can choose to use any or all of the modes-of-operations, by defining the symbols CBC, CTR or ECB. See the header file for clarification.
//
// C++ users should #include aes.hpp instead of aes.h
//
// - There is no built-in error checking or protection from out-of-bounds memory access errors as a result of malicious input.
// - The module uses less than 200 bytes of RAM and 1-2K ROM when compiled for ARM, but YMMV depending on which modes are enabled.
// - It is one of the smallest implementations in C I've seen yet, but do contact me if you know of something smaller (or have improvements to the code here).
//
// I've successfully used the code on 64bit x86, 32bit ARM and 8 bit AVR platforms.
//
// GCC size output when only CTR mode is compiled for ARM:
//
// $ arm-none-eabi-gcc -Os -DCBC=0 -DECB=0 -DCTR=1 -c aes.c
// $ size aes.o
//    text    data     bss     dec     hex filename
//    1203       0       0    1203     4b3 aes.o
// .. and when compiling for the THUMB instruction set, we end up just below 1K in code size.
//
// $ arm-none-eabi-gcc -Os -mthumb -DCBC=0 -DECB=0 -DCTR=1 -c aes.c
// $ size aes.o
//    text    data     bss     dec     hex filename
//     955       0       0     955     3bb aes.o
// I am using the Free Software Foundation, ARM GCC compiler:
//
// $ arm-none-eabi-gcc --version
// arm-none-eabi-gcc (4.8.4-1+11-1) 4.8.4 20141219 (release)
// Copyright (C) 2013 Free Software Foundation, Inc. This is free software; see the source for copying conditions.
// There is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
// This implementation is verified against the data in:
// - National Institute of Standards and Technology Special Publication 800-38A 2001 ED Appendix F: Example Vectors for Modes of Operation of the AES.
// - The other appendices in the document are valuable for implementation details on e.g. padding, generation of IVs and nonces in CTR-mode etc.
//
// A heartfelt thank-you to all the nice people out there who have contributed to this project.
//
// All material in this repository is in the public domain.
//
// This is an implementation of the AES algorithm, specifically ECB, CTR and CBC mode. Block size can be chosen in aes.h - available choices are AES128, AES192, AES256.
// The implementation is verified against the test vectors in:
//   National Institute of Standards and Technology Special Publication 800-38A 2001 ED
//
// ECB-AES128
// ----------
//
//   plain-text:
//     6bc1bee22e409f96e93d7e117393172a
//     ae2d8a571e03ac9c9eb76fac45af8e51
//     30c81c46a35ce411e5fbc1191a0a52ef
//     f69f2445df4f9b17ad2b417be66c3710
//
//   key:
//     2b7e151628aed2a6abf7158809cf4f3c
//
//   resulting cipher
//     3ad77bb40d7a3660a89ecaf32466ef97
//     f5d3d58503b9699de785895a96fdbaaf
//     43b1cd7f598ece23881b00e3ed030688
//     7b0c785e27e8ad3f8223207104725dd4
//
//
// NOTE:   String length must be evenly divisible by 16byte (str_len % 16 == 0)
//         You should pad the end of the string with zeros if this is not the case. For AES192/256 the key size is proportionally larger.
#include "gpk_aes.h"
#include "gpk_noise.h"
#include "gpk_chrono.h"

// Defines: The number of columns comprising a state in AES. This is a constant in AES. Value=4
stacxpr	const uint32_t				AES_Nb									= 4;

// Private variables:
typedef	uint8_t						state_t[4][4];	// array holding the intermediate results during decryption.

// The lookup-tables are marked const so they can be placed in read-only storage instead of RAM
// The numbers below can be computed dynamically trading ROM for RAM -
// This can be useful in (embedded) bootloader applications, where ROM is often limited.
stacxpr	const uint8_t				sbox	[256]							= {
  //0     1    2      3     4    5     6     7      8    9     A      B    C     D     E     F
  0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
  0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
  0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
  0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
  0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
  0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
  0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
  0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
  0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
  0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
  0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
  0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
  0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
  0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
  0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
  0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16 };

stacxpr	const uint8_t				rsbox	[256]							= {
  0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
  0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
  0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
  0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
  0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
  0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
  0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
  0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
  0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
  0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
  0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
  0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
  0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
  0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
  0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
  0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d };

// The round constant word array, Rcon[i], contains the values given by x to the power (i-1) being powers of x (x is denoted as {02}) in the field GF(2^8)
stacxpr	uint8_t						Rcon	[11]							= {0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36 };

// Jordan Goulder points out in PR #12 (https://github.com/kokke/tiny-AES-C/pull/12), that you can remove most of the elements in the Rcon array, because they are unused.
// From Wikipedia's article on the Rijndael key schedule @ https://en.wikipedia.org/wiki/Rijndael_key_schedule#Rcon
// "Only the first some of these constants are actually used – up to rcon[10] for AES-128 (as 11 round keys are needed),
//  up to rcon[8] for AES-192, up to rcon[7] for AES-256. rcon[0] is not used in AES algorithm."
stainli	uint8_t						getSBoxValue							(uint8_t num)														{ return  sbox[num]; }
stainli	uint8_t						getSBoxInvert							(uint8_t num)														{ return rsbox[num]; }

// This function produces Nb(Nr+1) round keys. The round keys are used in each round to decrypt the states.
static	void						KeyExpansion							(uint8_t* RoundKey, const uint8_t* Key, ::gpk::AES_LEVEL level)		{
	uint32_t											Nk = 0, Nr = 0;
	switch(level) {
	case ::gpk::AES_LEVEL_128: Nk = 4; Nr = 10; break;
	case ::gpk::AES_LEVEL_192: Nk = 6; Nr = 12; break;
	case ::gpk::AES_LEVEL_256: Nk = 8; Nr = 14; break;
	}

	// The first round key is the key itself.
	for (uint32_t base = 0; base < Nk; ++base) {
		for(int idx = 0; idx < 4; ++idx)
			RoundKey[(base * 4) + idx]							= Key[(base * 4) + idx];
	}

	// All other round keys are found from the previous round keys.
	uint8_t												tempa[4]; // Used for the column/row operations
	for (uint32_t i = Nk; i < AES_Nb * (Nr + 1); ++i) {
		uint32_t											k										= (i - 1) * 4;
		for(int idx = 0; idx < 4; ++idx)
			tempa[idx]										= RoundKey[k + idx];

		if (i % Nk == 0) {
			{	// Function RotWord()	// This function shifts the 4 bytes in a word to the left once. [a0,a1,a2,a3] becomes [a1,a2,a3,a0]
				k												= tempa[0];
				tempa[0]										= tempa[1];
				tempa[1]										= tempa[2];
				tempa[2]										= tempa[3];
				tempa[3]										= k % 256;
			}

			// SubWord() is a function that takes a four-byte input word and applies the S-box to each of the four bytes to produce an output word.
			for(int idx = 0; idx < 4; ++idx)	// Function Subword()
				tempa[idx]										= getSBoxValue(tempa[idx]);

			tempa[0]										= tempa[0] ^ Rcon[i/Nk];
		}
		if(level == ::gpk::AES_LEVEL_256)
			if (i % Nk == 4) {
				for(int idx = 0; idx < 4; ++idx)	// Function Subword()
					tempa[idx]										= getSBoxValue(tempa[idx]);
			}
		uint32_t											j										= i * 4;
		k												= (i - Nk) * 4;
		for(int idx = 0; idx < 4; ++idx)
			RoundKey[j + idx]									= RoundKey[k + idx] ^ tempa[idx];
	}
}

void											gpk::aesInitCtx							(::gpk::SAESContext* ctx, const uint8_t* key, ::gpk::AES_LEVEL level)						{ ctx->Level = level; ctx->RoundKey.resize(::gpk::AES_LEVEL_PROPERTIES[level].KeyExpSize); KeyExpansion(ctx->RoundKey.begin(), key, level); }
void											gpk::aesInitCtxIV						(::gpk::SAESContext* ctx, const uint8_t* key, ::gpk::AES_LEVEL level, const uint8_t* iv)	{ ctx->Level = level; ctx->RoundKey.resize(::gpk::AES_LEVEL_PROPERTIES[level].KeyExpSize); KeyExpansion(ctx->RoundKey.begin(), key, level); memcpy(ctx->Iv, iv, AES_SIZEBLOCK); }
void											gpk::aesCtxSetIV						(::gpk::SAESContext* ctx, const uint8_t* iv)												{ memcpy(ctx->Iv, iv, AES_SIZEBLOCK); }

// This function adds the round key to state. The round key is added to	 the state by an XOR function.
static	void										AddRoundKey								(uint8_t round,state_t* state,uint8_t* RoundKey)											{
	for(uint8_t i = 0; i < 4; ++i)
	for(uint8_t j = 0; j < 4; ++j)
		(*state)[i][j]									^= RoundKey[(round * AES_Nb * 4) + (i * AES_Nb) + j];
}

// The SubBytes Function Substitutes the values in the state matrix with values in an S-box.
static	void										SubBytes								(state_t* state)																			{
	for(uint8_t i = 0; i < 4; ++i)
	for(uint8_t j = 0; j < 4; ++j)
		(*state)[j][i]									= getSBoxValue((*state)[j][i]);
}

// The ShiftRows() function shifts the rows in the state to the left. Each row is shifted with different offset. Offset = Row number. So the first row is not shifted.
static	void										ShiftRows								(state_t* state)																			{
	uint8_t												temp;
	// Rotate first row 1 columns to left
	temp											= (*state)[0][1];
	(*state)[0][1]									= (*state)[1][1];
	(*state)[1][1]									= (*state)[2][1];
	(*state)[2][1]									= (*state)[3][1];
	(*state)[3][1]									= temp;

	// Rotate second row 2 columns to left
	temp											= (*state)[0][2];
	(*state)[0][2]									= (*state)[2][2];
	(*state)[2][2]									= temp;

	temp											= (*state)[1][2];
	(*state)[1][2]									= (*state)[3][2];
	(*state)[3][2]									= temp;

	// Rotate third row 3 columns to left
	temp											= (*state)[0][3];
	(*state)[0][3]									= (*state)[3][3];
	(*state)[3][3]									= (*state)[2][3];
	(*state)[2][3]									= (*state)[1][3];
	(*state)[1][3]									= temp;
}

static uint8_t									xtime									(uint8_t x)														{ return ((x<<1) ^ (((x>>7) & 1) * 0x1b)); }

// MixColumns function mixes the columns of the state matrix
static	void										MixColumns								(state_t* state)												{
	for (uint8_t i = 0; i < 4; ++i) {
		const uint8_t										t										= (*state)[i][0];
		const uint8_t										Tmp										= (*state)[i][0] ^ (*state)[i][1] ^ (*state)[i][2] ^ (*state)[i][3] ;
		uint8_t												Tm;
		Tm												= (*state)[i][0] ^ (*state)[i][1] ; Tm = xtime(Tm);  (*state)[i][0] ^= Tm ^ Tmp ;
		Tm												= (*state)[i][1] ^ (*state)[i][2] ; Tm = xtime(Tm);  (*state)[i][1] ^= Tm ^ Tmp ;
		Tm												= (*state)[i][2] ^ (*state)[i][3] ; Tm = xtime(Tm);  (*state)[i][2] ^= Tm ^ Tmp ;
		Tm												= (*state)[i][3] ^ t ;              Tm = xtime(Tm);  (*state)[i][3] ^= Tm ^ Tmp ;
	}
}

// Multiply is used to multiply numbers in the field GF(2^8)
// jcallan@github points out that declaring Multiply as a function reduces code size considerably with the Keil ARM compiler.
// See this link for more information: https://github.com/kokke/tiny-AES-C/pull/3
static uint8_t									Multiply								(uint8_t x, uint8_t y)											{
	return
		( ((y		& 1) * x)
		^ ((y >> 1	& 1) * xtime(x))
		^ ((y >> 2	& 1) * xtime(xtime(x)))
		^ ((y >> 3	& 1) * xtime(xtime(xtime(x))))
		);
}

// MixColumns function mixes the columns of the state matrix. The method used to multiply may be difficult to understand for the inexperienced. Please use the references to gain more information.
static	void										InvMixColumns							(state_t* state)												{
	for (int i = 0; i < 4; ++i) {
		uint8_t											a										= (*state)[i][0];
		uint8_t											b										= (*state)[i][1];
		uint8_t											c										= (*state)[i][2];
		uint8_t											d										= (*state)[i][3];
		(*state)[i][0] = Multiply(a, 0x0e) ^ Multiply(b, 0x0b) ^ Multiply(c, 0x0d) ^ Multiply(d, 0x09);
		(*state)[i][1] = Multiply(a, 0x09) ^ Multiply(b, 0x0e) ^ Multiply(c, 0x0b) ^ Multiply(d, 0x0d);
		(*state)[i][2] = Multiply(a, 0x0d) ^ Multiply(b, 0x09) ^ Multiply(c, 0x0e) ^ Multiply(d, 0x0b);
		(*state)[i][3] = Multiply(a, 0x0b) ^ Multiply(b, 0x0d) ^ Multiply(c, 0x09) ^ Multiply(d, 0x0e);
	}
}

// The SubBytes Function Substitutes the values in the state matrix with values in an S-box.
static	void										InvSubBytes								(state_t* state)												{
	for (uint8_t i = 0; i < 4; ++i)
	for (uint8_t j = 0; j < 4; ++j)
		(*state)[j][i]									= getSBoxInvert((*state)[j][i]);
}

static	void										InvShiftRows							(state_t* state)												{
	uint8_t												temp;

	// Rotate first row 1 columns to right
	temp											= (*state)[3][1];
	(*state)[3][1]									= (*state)[2][1];
	(*state)[2][1]									= (*state)[1][1];
	(*state)[1][1]									= (*state)[0][1];
	(*state)[0][1]									= temp;

	// Rotate second row 2 columns to right
	temp											= (*state)[0][2];
	(*state)[0][2]									= (*state)[2][2];
	(*state)[2][2]									= temp;

	temp											= (*state)[1][2];
	(*state)[1][2]									= (*state)[3][2];
	(*state)[3][2]									= temp;

	// Rotate third row 3 columns to right
	temp											= (*state)[0][3];
	(*state)[0][3]									= (*state)[1][3];
	(*state)[1][3]									= (*state)[2][3];
	(*state)[2][3]									= (*state)[3][3];
	(*state)[3][3]									= temp;
}

// Cipher is the main function that encrypts the PlainText. Encrypts the PlainText with the Key using AES algorithm.
static	void									Cipher									(state_t* state, uint8_t* RoundKey, ::gpk::AES_LEVEL level)		{
	AddRoundKey(0, state, RoundKey);	// Add the First round key to the state before starting the rounds.
	uint8_t												Nr										= 0;
	switch(level) {
	case ::gpk::AES_LEVEL_128: Nr = 10; break;
	case ::gpk::AES_LEVEL_192: Nr = 12; break;
	case ::gpk::AES_LEVEL_256: Nr = 14; break;
	}
	for (uint8_t round = 1; round < Nr; ++round) {	// There will be Nr rounds. The first Nr-1 rounds are identical. These Nr-1 rounds are executed in the loop below.
		SubBytes(state);
		ShiftRows(state);
		MixColumns(state);
		AddRoundKey(round, state, RoundKey);
	}
	// The last round is given below. The MixColumns function is not here in the last round.
	SubBytes(state);
	ShiftRows(state);
	AddRoundKey(Nr, state, RoundKey);
}

// Decrypts the PlainText with the Key using AES algorithm.
static	void									InvCipher							(state_t* state, uint8_t* RoundKey, ::gpk::AES_LEVEL level)			{
	uint8_t												Nr									= 0;
	switch(level) {
	case ::gpk::AES_LEVEL_128: Nr = 10; break;
	case ::gpk::AES_LEVEL_192: Nr = 12; break;
	case ::gpk::AES_LEVEL_256: Nr = 14; break;
	}
	AddRoundKey(Nr, state, RoundKey);	// Add the First round key to the state before starting the rounds.
	for (uint8_t round = (Nr - 1); round > 0; --round) {	// There will be Nr rounds. The first Nr-1 rounds are identical. These Nr-1 rounds are executed in the loop below.
		InvShiftRows(state);
		InvSubBytes(state);
		AddRoundKey(round, state, RoundKey);
		InvMixColumns(state);
	}
	// The last round is given below. The MixColumns function is not here in the last round.
	InvShiftRows(state);
	InvSubBytes(state);
	AddRoundKey(0, state, RoundKey);
}

static	void									XorWithIv							(uint8_t* buf, uint8_t* Iv)											{
	for (uint8_t i = 0; i < ::gpk::AES_SIZEBLOCK; ++i) // The block in AES is always 128bit no matter the key size
		buf[i]											^= Iv[i];
}

/* Public functions:                                                         */
void											gpk::aesECBEncrypt					(::gpk::SAESContext *ctx, uint8_t* buf)								{ Cipher	((state_t*)buf, ctx->RoundKey.begin(), ctx->Level); } // Cipher encrypts the PlainText with the Key using AES algorithm.
void											gpk::aesECBDecrypt					(::gpk::SAESContext* ctx, uint8_t* buf)								{ InvCipher	((state_t*)buf, ctx->RoundKey.begin(), ctx->Level); } // InvCiper decrypts the PlainText with the Key using AES algorithm.
void											gpk::aesCBCEncryptBuffer			(::gpk::SAESContext *ctx,uint8_t* buf, uint32_t length)	{
	uint8_t												* Iv								= ctx->Iv;
	for (uintptr_t i = 0; i < length; i += ::gpk::AES_SIZEBLOCK) {
		XorWithIv(buf, Iv);
		Cipher((state_t*)buf, ctx->RoundKey.begin(), ctx->Level);
		Iv												= buf;
		buf												+= ::gpk::AES_SIZEBLOCK;
		//printf("Step %d - %d", i/16, i);
	}
	memcpy(ctx->Iv, Iv, ::gpk::AES_SIZEBLOCK);	// store Iv in ctx for next call */
}

void											gpk::aesCBCDecryptBuffer			(::gpk::SAESContext* ctx, uint8_t* buf, uint32_t length)			{
	uint8_t												storeNextIv	[::gpk::AES_SIZEBLOCK];
	for (uintptr_t i = 0; i < length; i += ::gpk::AES_SIZEBLOCK) {
		memcpy(storeNextIv, buf, ::gpk::AES_SIZEBLOCK);
		InvCipher((state_t*)buf, ctx->RoundKey.begin(), ctx->Level);
		XorWithIv(buf, ctx->Iv);
		memcpy(ctx->Iv, storeNextIv, ::gpk::AES_SIZEBLOCK);
		buf												+= ::gpk::AES_SIZEBLOCK;
	}
}

// Symmetrical operation: same function for encrypting as for decrypting. Note any IV/nonce should never be reused with the same key
void											gpk::aesCTRXCryptBuffer				(::gpk::SAESContext* ctx, uint8_t* buf, uint32_t length)			{
	uint8_t												buffer	[::gpk::AES_SIZEBLOCK];
	int													bi									= ::gpk::AES_SIZEBLOCK;
	for (uint32_t i = 0; i < length; ++i, ++bi) {
		if (bi == ::gpk::AES_SIZEBLOCK) {// we need to regen xor compliment in buffer */
			memcpy(buffer, ctx->Iv, ::gpk::AES_SIZEBLOCK);
			Cipher((state_t*)buffer, ctx->RoundKey.begin(), ctx->Level);
			for(bi = (::gpk::AES_SIZEBLOCK - 1); bi >= 0; --bi) {	// Increment Iv and handle overflow
				if (ctx->Iv[bi] == 255) {	/* inc will owerflow */
					ctx->Iv[bi]										= 0;
					continue;
				}
				ctx->Iv[bi]										+= 1;
				break;
			}
			bi												= 0;
		}
		buf[i]											= (buf[i] ^ buffer[bi]);
	}
}

::gpk::error_t										gpk::aesEncode							(const ::gpk::vcb & messageToEncrypt, const ::gpk::vcb & encryptionKey, ::gpk::AES_LEVEL level, ::gpk::apod<byte_t>& outputEncrypted)	{
	uint8_t													iv		[::gpk::AES_SIZEIV]				= {};
	const double											fraction								= (1.0 / (65535>>1)) * 255.0;
	for(uint32_t iVal = 0; iVal < ::gpk::AES_SIZEIV; ++iVal)
		iv[iVal]											= (uint8_t)(::gpk::noise1DBase(::gpk::timeCurrentInUs()) * fraction);

	gpk_necall(::gpk::aesEncode(messageToEncrypt, iv, encryptionKey, level, outputEncrypted), "%s", "Failed to encrypt message.");
	gpk_necall(outputEncrypted.resize(outputEncrypted.size() + ::gpk::AES_SIZEIV), "%s", "Out of memory?");
	memcpy(&outputEncrypted[outputEncrypted.size() - ::gpk::AES_SIZEIV], iv, ::gpk::AES_SIZEIV);
	return 0;
}

::gpk::error_t										gpk::aesDecode							(const ::gpk::vcb & messageEncrypted, const ::gpk::vcb & encryptionKey, ::gpk::AES_LEVEL level, ::gpk::apod<byte_t>& outputDecrypted)	{
	return ::gpk::aesDecode({messageEncrypted.begin(), messageEncrypted.size() - ::gpk::AES_SIZEIV}, {(uint8_t*)&messageEncrypted[messageEncrypted.size() - ::gpk::AES_SIZEIV], ::gpk::AES_SIZEIV}, encryptionKey, level, outputDecrypted);
}

::gpk::error_t										gpk::aesEncode						(const ::gpk::vcb & messageToEncrypt, const ::gpk::vcub& iv, const ::gpk::vcb & encryptionKey, ::gpk::AES_LEVEL level, ::gpk::apod<byte_t>& outputEncrypted) {
	ree_if(0 == messageToEncrypt.size(), "Cannot encode empty message at address %p.", messageToEncrypt);
	ree_if(encryptionKey.size() != 32, "Invalid key length! Key must be exactly 32 bytes long. Key size: %u.", encryptionKey.size());
	int8_t													excedent								= messageToEncrypt.size() % ::gpk::AES_SIZEBLOCK;
	int8_t													paddingRequired							= (int8_t)(::gpk::AES_SIZEBLOCK - excedent);
	outputEncrypted.clear();
	gpk_necall(outputEncrypted.resize(messageToEncrypt.size() + (paddingRequired ? paddingRequired : ::gpk::AES_SIZEBLOCK), (byte_t)0), "%s", "Out of memory?");
	for(int8_t iPad = 0; iPad < paddingRequired; ++iPad)
		outputEncrypted[messageToEncrypt.size() + iPad]		= paddingRequired;
	memcpy(outputEncrypted.begin(), messageToEncrypt.begin(), messageToEncrypt.size());

	::gpk::SAESContext										aes;
	::gpk::aesInitCtxIV(&aes, (const uint8_t*)encryptionKey.begin(), level, iv.begin());
	::gpk::aesCBCEncryptBuffer(&aes, (uint8_t*)outputEncrypted.begin(), outputEncrypted.size());
	return 0;
}

::gpk::error_t										gpk::aesDecode						(const ::gpk::vcb & messageEncrypted, const ::gpk::vcub& iv, const ::gpk::vcb & encryptionKey, ::gpk::AES_LEVEL level, ::gpk::apod<byte_t>& outputDecrypted) {
	ree_if(0 == messageEncrypted.size(), "Cannot encode empty message at address %p.", messageEncrypted.begin());
	ree_if(messageEncrypted.size() % ::gpk::AES_SIZEBLOCK, "Invalid data length: %u.", messageEncrypted.size());
	ree_if(encryptionKey.size() != 32, "Invalid key length! Key must be exactly 32 bytes long. Key size: %u.", encryptionKey.size());
	gpk_necall(outputDecrypted.resize(messageEncrypted.size()), "%s", "Out of memory?");
	memcpy(outputDecrypted.begin(), messageEncrypted.begin(), outputDecrypted.size());
	::gpk::SAESContext										aes;
	::gpk::aesInitCtxIV(&aes, (const uint8_t*)encryptionKey.begin(), level, iv.begin());
	::gpk::aesCBCDecryptBuffer(&aes, (uint8_t*)outputDecrypted.begin(), outputDecrypted.size());
	if(outputDecrypted[outputDecrypted.size() - 1] == 0)
		gpk_necall(outputDecrypted.resize(outputDecrypted.size() - ::gpk::AES_SIZEBLOCK), "%s", "Out of memory?");
	else {
		const int32_t											padCunt									= outputDecrypted[outputDecrypted.size() - 1];
		const int32_t											sizeOriginal							= outputDecrypted.size() - padCunt;
		gpk_necall(outputDecrypted.resize(sizeOriginal), "%s", "Out of memory?");
	}
	return 0;
}
