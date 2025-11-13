/// Copyright 2010-2024 - ogarnd
#include "gpk_enum.h"

#ifndef GPK_AES_H_23627
#define GPK_AES_H_23627

namespace gpk
{
	GDEFINE_ENUM_TYPE(AES_LEVEL, u0_t);
	GDEFINE_ENUM_VALUE(AES_LEVEL, 128, 0);
	GDEFINE_ENUM_VALUE(AES_LEVEL, 192, 1);
	GDEFINE_ENUM_VALUE(AES_LEVEL, 256, 2);

	GDEFINE_ENUM_TYPE(AES_MODE, u0_t);
	GDEFINE_ENUM_VALUE(AES_MODE, ECB, 0);
	GDEFINE_ENUM_VALUE(AES_MODE, CBC, 1);
	GDEFINE_ENUM_VALUE(AES_MODE, CTR, 2);

#pragma pack(push, 1)
	stct SAESLevelProperties {
		int32_t						KeyLength ;
		int32_t						KeyExpSize;
	};
#pragma pack(pop)

	stxp	SAESLevelProperties	AES_LEVEL_PROPERTIES	[3]		=
		{ {16, 176}
		, {24, 208}
		, {32, 240}
		};

	stxp	u2_t				AES_SIZEBLOCK					= 16; // Block length in bytes AES is 128b block only
	stxp	u2_t				AES_SIZEIV						= 16;

	stct SAESContext {
		AES_LEVEL					Level							= {};
		au0_t						RoundKey						= {};
		u0_t						Iv			[AES_SIZEBLOCK]		= {};
	};

	void						aesInitCtx						(SAESContext * ctx, u0_c * key, AES_LEVEL level);
	void						aesInitCtxIV					(SAESContext * ctx, u0_c * key, AES_LEVEL level, u0_c * iv);
	void						aesCtxSetIV						(SAESContext * ctx, u0_c * iv);

	// ECB enables the basic ECB 16-byte block algorithm. All can be enabled simultaneously. Buffer size is exactly AES_SIZEBLOCK bytes; you need only AES_init_ctx as IV is not used in ECB.
	// NB: ECB is considered insecure for most uses
	void						aesECBEncrypt					(SAESContext * ctx, u0_t * buf);
	void						aesECBDecrypt					(SAESContext * ctx, u0_t * buf);

	// CBC enables AES encryption in CBC-mode of operation.
	// buffer size MUST be mutile of AES_SIZEBLOCK; Suggest https://en.wikipedia.org/wiki/Padding_(cryptography)#PKCS7 for padding scheme.
	// NOTES: you need to set IV in ctx via aesInitCtxIV() or aesCtxSetIV() no IV should ever be reused with the same key
	void						aesCBCEncryptBuffer				(SAESContext * ctx, u0_t * buf, u2_t length);
	void						aesCBCDecryptBuffer				(SAESContext * ctx, u0_t * buf, u2_t length);

	// CTR enables encryption in counter-mode.
	// Symmetrical operation: Same function for encrypting as for decrypting. IV is incremented for every block, and used after encryption as XOR-compliment for output; Suggesting https://en.wikipedia.org/wiki/Padding_(cryptography)#PKCS7 for padding scheme.
	// NOTES: you need to set IV in ctx with aesInitCtxIV() or aesCtxSetIV() no IV should ever be reused with the same key
	void						aesCTRXCryptBuffer				(SAESContext * ctx, u0_t * buf, u2_t length);

	err_t						aesEncode		(vcu0_c & messageToEncrypt, vcu0_c & iv, vcu0_c & encryptionKey, AES_LEVEL level, au0_t & outputEncrypted);
	err_t						aesDecode		(vcu0_c & messageEncrypted, vcu0_c & iv, vcu0_c & encryptionKey, AES_LEVEL level, au0_t & outputDecrypted);
	err_t						aesEncode		(vcu0_c & messageToEncrypt, vcu0_c & encryptionKey, AES_LEVEL level, au0_t & outputEncrypted);
	err_t						aesDecode		(vcu0_c & messageEncrypted, vcu0_c & encryptionKey, AES_LEVEL level, au0_t & outputDecrypted);

	stin	err_t				aesEncode		(u0_c * messageToEncrypt, u2_t dataLength, vcu0_c & encryptionKey, AES_LEVEL level, au0_t & outputEncrypted)							{ return aesEncode({messageToEncrypt, dataLength}, encryptionKey, level, outputEncrypted); }
	stin	err_t				aesDecode		(u0_c * messageEncrypted, u2_t dataLength, vcu0_c & encryptionKey, AES_LEVEL level, au0_t & outputDecrypted)							{ return aesDecode({messageEncrypted, dataLength}, encryptionKey, level, outputDecrypted); }
	stin	err_t				aesEncode		(u0_c * messageToEncrypt, u2_t dataLength, vcu0_c & iv, vcu0_c & encryptionKey, AES_LEVEL level, au0_t & outputEncrypted)	{ return aesEncode({messageToEncrypt, dataLength}, iv, encryptionKey, level, outputEncrypted); }
	stin	err_t				aesDecode		(u0_c * messageEncrypted, u2_t dataLength, vcu0_c & iv, vcu0_c & encryptionKey, AES_LEVEL level, au0_t & outputDecrypted)	{ return aesDecode({messageEncrypted, dataLength}, iv, encryptionKey, level, outputDecrypted); }

	stin	err_t				aesEncode		(vcu0_c & messageToEncrypt, vcs0_c & encryptionKey, AES_LEVEL level, au0_t & outputEncrypted)							{ return aesEncode(messageToEncrypt, *(vcu0_c*)&encryptionKey, level, outputEncrypted); }
	stin	err_t				aesDecode		(vcu0_c & messageEncrypted, vcs0_c & encryptionKey, AES_LEVEL level, au0_t & outputDecrypted)							{ return aesDecode(messageEncrypted, *(vcu0_c*)&encryptionKey, level, outputDecrypted); }
	stin	err_t				aesEncode		(vcu0_c & messageToEncrypt, vcu0_c & iv, vcs0_c & encryptionKey, AES_LEVEL level, au0_t & outputEncrypted)	{ return aesEncode(messageToEncrypt, iv, *(vcu0_c*)&encryptionKey, level, outputEncrypted); }
	stin	err_t				aesDecode		(vcu0_c & messageEncrypted, vcu0_c & iv, vcs0_c & encryptionKey, AES_LEVEL level, au0_t & outputDecrypted)	{ return aesDecode(messageEncrypted, iv, *(vcu0_c*)&encryptionKey, level, outputDecrypted); }

	stin	err_t				aesEncode		(vcs0_c & messageToEncrypt, vcs0_c & encryptionKey, AES_LEVEL level, au0_t & outputEncrypted)							{ return aesEncode(*(vcu0_c*)&messageToEncrypt, *(vcu0_t*)&encryptionKey, level, outputEncrypted); }
	stin	err_t				aesDecode		(vcu0_c & messageEncrypted, vcs0_c & encryptionKey, AES_LEVEL level, as0_t & outputDecrypted)							{ return aesDecode(messageEncrypted, *(vcu0_c*)&encryptionKey, level, *(au0_t*)&outputDecrypted); }
	stin	err_t				aesEncode		(vcs0_c & messageToEncrypt, vcu0_c & iv, vcs0_c & encryptionKey, AES_LEVEL level, au0_t & outputEncrypted)	{ return aesEncode(*(vcu0_c*)&messageToEncrypt, iv, *(vcu0_t*)&encryptionKey, level, outputEncrypted); }
	stin	err_t				aesDecode		(vcu0_c & messageEncrypted, vcu0_c & iv, vcs0_c & encryptionKey, AES_LEVEL level, as0_t & outputDecrypted)	{ return aesDecode(messageEncrypted, iv, *(vcu0_c*)&encryptionKey, level, *(au0_t*)&outputDecrypted); }

	stin	err_t				aesEncode		(vcsc_c & messageToEncrypt, vcs0_c & encryptionKey, AES_LEVEL level, au0_t & outputEncrypted)							{ return aesEncode(*(vcu0_c*)&messageToEncrypt, *(vcu0_t*)&encryptionKey, level, outputEncrypted); }
	stin	err_t				aesDecode		(vcu0_c & messageEncrypted, vcs0_c & encryptionKey, AES_LEVEL level, asc_t & outputDecrypted)							{ return aesDecode(messageEncrypted, *(vcu0_c*)&encryptionKey, level, *(au0_t*)&outputDecrypted); }
	stin	err_t				aesEncode		(vcsc_c & messageToEncrypt, vcu0_c & iv, vcs0_c & encryptionKey, AES_LEVEL level, au0_t & outputEncrypted)	{ return aesEncode(*(vcu0_c*)&messageToEncrypt, iv, *(vcu0_t*)&encryptionKey, level, outputEncrypted); }
	stin	err_t				aesDecode		(vcu0_c & messageEncrypted, vcu0_c & iv, vcs0_c & encryptionKey, AES_LEVEL level, asc_t & outputDecrypted)	{ return aesDecode(messageEncrypted, iv, *(vcu0_c*)&encryptionKey, level, *(au0_t*)&outputDecrypted); }

	stin	err_t				aesEncode		(vcu0_c & messageToEncrypt, vcsc_c & encryptionKey, AES_LEVEL level, au0_t & outputEncrypted)							{ return aesEncode(messageToEncrypt, *(vcu0_c*)&encryptionKey, level, outputEncrypted); }
	stin	err_t				aesDecode		(vcu0_c & messageEncrypted, vcsc_c & encryptionKey, AES_LEVEL level, au0_t & outputDecrypted)							{ return aesDecode(messageEncrypted, *(vcu0_c*)&encryptionKey, level, outputDecrypted); }
	stin	err_t				aesEncode		(vcu0_c & messageToEncrypt, vcu0_c & iv, vcsc_c & encryptionKey, AES_LEVEL level, au0_t & outputEncrypted)	{ return aesEncode(messageToEncrypt, iv, *(vcu0_c*)&encryptionKey, level, outputEncrypted); }
	stin	err_t				aesDecode		(vcu0_c & messageEncrypted, vcu0_c & iv, vcsc_c & encryptionKey, AES_LEVEL level, au0_t & outputDecrypted)	{ return aesDecode(messageEncrypted, iv, *(vcu0_c*)&encryptionKey, level, outputDecrypted); }

	stin	err_t				aesEncode		(vcs0_c & messageToEncrypt, vcsc_c & encryptionKey, AES_LEVEL level, au0_t & outputEncrypted)							{ return aesEncode(*(vcu0_c*)&messageToEncrypt, *(vcu0_t*)&encryptionKey, level, outputEncrypted); }
	stin	err_t				aesDecode		(vcu0_c & messageEncrypted, vcsc_c & encryptionKey, AES_LEVEL level, as0_t & outputDecrypted)							{ return aesDecode(messageEncrypted, *(vcu0_c*)&encryptionKey, level, *(au0_t*)&outputDecrypted); }
	stin	err_t				aesEncode		(vcs0_c & messageToEncrypt, vcu0_c & iv, vcsc_c & encryptionKey, AES_LEVEL level, au0_t & outputEncrypted)	{ return aesEncode(*(vcu0_c*)&messageToEncrypt, iv, *(vcu0_t*)&encryptionKey, level, outputEncrypted); }
	stin	err_t				aesDecode		(vcu0_c & messageEncrypted, vcu0_c & iv, vcsc_c & encryptionKey, AES_LEVEL level, as0_t & outputDecrypted)	{ return aesDecode(messageEncrypted, iv, *(vcu0_c*)&encryptionKey, level, *(au0_t*)&outputDecrypted); }

	stin	err_t				aesEncode		(vcsc_c & messageToEncrypt, vcsc_c & encryptionKey, AES_LEVEL level, au0_t & outputEncrypted)							{ return aesEncode(*(vcu0_c*)&messageToEncrypt, *(vcu0_t*)&encryptionKey, level, outputEncrypted); }
	stin	err_t				aesDecode		(vcu0_c & messageEncrypted, vcsc_c & encryptionKey, AES_LEVEL level, asc_t & outputDecrypted)							{ return aesDecode(messageEncrypted, *(vcu0_c*)&encryptionKey, level, *(au0_t*)&outputDecrypted); }
	stin	err_t				aesEncode		(vcsc_c & messageToEncrypt, vcu0_c & iv, vcsc_c & encryptionKey, AES_LEVEL level, au0_t & outputEncrypted)	{ return aesEncode(*(vcu0_c*)&messageToEncrypt, iv, *(vcu0_t*)&encryptionKey, level, outputEncrypted); }
	stin	err_t				aesDecode		(vcu0_c & messageEncrypted, vcu0_c & iv, vcsc_c & encryptionKey, AES_LEVEL level, asc_t & outputDecrypted)	{ return aesDecode(messageEncrypted, iv, *(vcu0_c*)&encryptionKey, level, *(au0_t*)&outputDecrypted); }
}

#endif // GPK_AES_H_23627
