#include "gpk_enum.h"

#ifndef AES_H_28937498273423049
#define AES_H_28937498273423049

namespace gpk
{
	GDEFINE_ENUM_TYPE(AES_LEVEL, uint8_t);
	GDEFINE_ENUM_VALUE(AES_LEVEL, 128, 0);
	GDEFINE_ENUM_VALUE(AES_LEVEL, 192, 1);
	GDEFINE_ENUM_VALUE(AES_LEVEL, 256, 2);

	GDEFINE_ENUM_TYPE(AES_MODE, uint8_t);
	GDEFINE_ENUM_VALUE(AES_MODE, ECB, 0);
	GDEFINE_ENUM_VALUE(AES_MODE, CBC, 1);
	GDEFINE_ENUM_VALUE(AES_MODE, CTR, 2);

#pragma pack(push, 1)
	struct SAESLevelProperties {
		int32_t						KeyLength ;
		int32_t						KeyExpSize;
	};
#pragma pack(pop)

	stacxpr	SAESLevelProperties	AES_LEVEL_PROPERTIES	[3]		=
		{ {16, 176}
		, {24, 208}
		, {32, 240}
		};

	stacxpr	uint32_t			AES_SIZEBLOCK					= 16; // Block length in bytes AES is 128b block only
	stacxpr	uint32_t			AES_SIZEIV						= 16;

	struct SAESContext {
		AES_LEVEL					Level							= {};
		::gpk::apod<uint8_t>		RoundKey						= {};
		uint8_t						Iv			[AES_SIZEBLOCK]		= {};
	};

	void						aesInitCtx						(SAESContext* ctx, const uint8_t* key, ::gpk::AES_LEVEL level);
	void						aesInitCtxIV					(SAESContext* ctx, const uint8_t* key, ::gpk::AES_LEVEL level, const uint8_t* iv);
	void						aesCtxSetIV						(SAESContext* ctx, const uint8_t* iv);

	// ECB enables the basic ECB 16-byte block algorithm. All can be enabled simultaneously. Buffer size is exactly AES_SIZEBLOCK bytes; you need only AES_init_ctx as IV is not used in ECB.
	// NB: ECB is considered insecure for most uses
	void						aesECBEncrypt					(SAESContext* ctx, uint8_t* buf);
	void						aesECBDecrypt					(SAESContext* ctx, uint8_t* buf);

	// CBC enables AES encryption in CBC-mode of operation.
	// buffer size MUST be mutile of AES_SIZEBLOCK; Suggest https://en.wikipedia.org/wiki/Padding_(cryptography)#PKCS7 for padding scheme.
	// NOTES: you need to set IV in ctx via aesInitCtxIV() or aesCtxSetIV() no IV should ever be reused with the same key
	void						aesCBCEncryptBuffer				(SAESContext* ctx, uint8_t* buf, uint32_t length);
	void						aesCBCDecryptBuffer				(SAESContext* ctx, uint8_t* buf, uint32_t length);

	// CTR enables encryption in counter-mode.
	// Symmetrical operation: Same function for encrypting as for decrypting. IV is incremented for every block, and used after encryption as XOR-compliment for output; Suggesting https://en.wikipedia.org/wiki/Padding_(cryptography)#PKCS7 for padding scheme.
	// NOTES: you need to set IV in ctx with aesInitCtxIV() or aesCtxSetIV() no IV should ever be reused with the same key
	void						aesCTRXCryptBuffer				(SAESContext* ctx, uint8_t* buf, uint32_t length);

	::gpk::error_t				aesEncode						(const ::gpk::vcu8 & messageToEncrypt, const ::gpk::vcu8 & iv, const ::gpk::vcu8 & encryptionKey, ::gpk::AES_LEVEL level, ::gpk::au8 & outputEncrypted);
	::gpk::error_t				aesDecode						(const ::gpk::vcu8 & messageEncrypted, const ::gpk::vcu8 & iv, const ::gpk::vcu8 & encryptionKey, ::gpk::AES_LEVEL level, ::gpk::au8 & outputDecrypted);
	::gpk::error_t				aesEncode						(const ::gpk::vcu8 & messageToEncrypt, const ::gpk::vcu8 & encryptionKey, ::gpk::AES_LEVEL level, ::gpk::au8 & outputEncrypted);
	::gpk::error_t				aesDecode						(const ::gpk::vcu8 & messageEncrypted, const ::gpk::vcu8 & encryptionKey, ::gpk::AES_LEVEL level, ::gpk::au8 & outputDecrypted);

	stainli	::gpk::error_t		aesEncode						(const uint8_t * messageToEncrypt, uint32_t dataLength	, const ::gpk::vcu8 & encryptionKey, ::gpk::AES_LEVEL level, ::gpk::au8 & outputEncrypted)							{ return ::gpk::aesEncode({messageToEncrypt, dataLength}, encryptionKey, level, outputEncrypted); }
	stainli	::gpk::error_t		aesDecode						(const uint8_t * messageEncrypted, uint32_t dataLength	, const ::gpk::vcu8 & encryptionKey, ::gpk::AES_LEVEL level, ::gpk::au8 & outputDecrypted)							{ return ::gpk::aesDecode({messageEncrypted, dataLength}, encryptionKey, level, outputDecrypted); }
	stainli	::gpk::error_t		aesEncode						(const uint8_t * messageToEncrypt, uint32_t dataLength	, const ::gpk::vcu8 & iv, const ::gpk::vcu8 & encryptionKey, ::gpk::AES_LEVEL level, ::gpk::au8 & outputEncrypted)	{ return ::gpk::aesEncode({messageToEncrypt, dataLength}, iv, encryptionKey, level, outputEncrypted); }
	stainli	::gpk::error_t		aesDecode						(const uint8_t * messageEncrypted, uint32_t dataLength	, const ::gpk::vcu8 & iv, const ::gpk::vcu8 & encryptionKey, ::gpk::AES_LEVEL level, ::gpk::au8 & outputDecrypted)	{ return ::gpk::aesDecode({messageEncrypted, dataLength}, iv, encryptionKey, level, outputDecrypted); }

	stainli	::gpk::error_t		aesEncode						(const ::gpk::vcu8 & messageToEncrypt, const ::gpk::vci8 & encryptionKey, ::gpk::AES_LEVEL level, ::gpk::au8 & outputEncrypted)							{ return ::gpk::aesEncode(messageToEncrypt, *(const ::gpk::vcu8*)&encryptionKey, level, outputEncrypted); }
	stainli	::gpk::error_t		aesDecode						(const ::gpk::vcu8 & messageEncrypted, const ::gpk::vci8 & encryptionKey, ::gpk::AES_LEVEL level, ::gpk::au8 & outputDecrypted)							{ return ::gpk::aesDecode(messageEncrypted, *(const ::gpk::vcu8*)&encryptionKey, level, outputDecrypted); }
	stainli	::gpk::error_t		aesEncode						(const ::gpk::vcu8 & messageToEncrypt, const ::gpk::vcu8 & iv, const ::gpk::vci8 & encryptionKey, ::gpk::AES_LEVEL level, ::gpk::au8 & outputEncrypted)	{ return ::gpk::aesEncode(messageToEncrypt, iv, *(const ::gpk::vcu8*)&encryptionKey, level, outputEncrypted); }
	stainli	::gpk::error_t		aesDecode						(const ::gpk::vcu8 & messageEncrypted, const ::gpk::vcu8 & iv, const ::gpk::vci8 & encryptionKey, ::gpk::AES_LEVEL level, ::gpk::au8 & outputDecrypted)	{ return ::gpk::aesDecode(messageEncrypted, iv, *(const ::gpk::vcu8*)&encryptionKey, level, outputDecrypted); }

	stainli	::gpk::error_t		aesEncode						(const ::gpk::vci8 & messageToEncrypt, const ::gpk::vci8 & encryptionKey, ::gpk::AES_LEVEL level, ::gpk::au8 & outputEncrypted)							{ return ::gpk::aesEncode(*(const ::gpk::vcu8*)&messageToEncrypt, *(::gpk::vcu8*)&encryptionKey, level, outputEncrypted); }
	stainli	::gpk::error_t		aesDecode						(const ::gpk::vcu8 & messageEncrypted, const ::gpk::vci8 & encryptionKey, ::gpk::AES_LEVEL level, ::gpk::ai8 & outputDecrypted)							{ return ::gpk::aesDecode(messageEncrypted, *(const ::gpk::vcu8*)&encryptionKey, level, *(::gpk::au8*)&outputDecrypted); }
	stainli	::gpk::error_t		aesEncode						(const ::gpk::vci8 & messageToEncrypt, const ::gpk::vcu8 & iv, const ::gpk::vci8 & encryptionKey, ::gpk::AES_LEVEL level, ::gpk::au8 & outputEncrypted)	{ return ::gpk::aesEncode(*(const ::gpk::vcu8*)&messageToEncrypt, iv, *(::gpk::vcu8*)&encryptionKey, level, outputEncrypted); }
	stainli	::gpk::error_t		aesDecode						(const ::gpk::vcu8 & messageEncrypted, const ::gpk::vcu8 & iv, const ::gpk::vci8 & encryptionKey, ::gpk::AES_LEVEL level, ::gpk::ai8 & outputDecrypted)	{ return ::gpk::aesDecode(messageEncrypted, iv, *(const ::gpk::vcu8*)&encryptionKey, level, *(::gpk::au8*)&outputDecrypted); }

	stainli	::gpk::error_t		aesEncode						(const ::gpk::vcc  & messageToEncrypt, const ::gpk::vci8 & encryptionKey, ::gpk::AES_LEVEL level, ::gpk::au8 & outputEncrypted)							{ return ::gpk::aesEncode(*(const ::gpk::vcu8*)&messageToEncrypt, *(::gpk::vcu8*)&encryptionKey, level, outputEncrypted); }
	stainli	::gpk::error_t		aesDecode						(const ::gpk::vcu8 & messageEncrypted, const ::gpk::vci8 & encryptionKey, ::gpk::AES_LEVEL level, ::gpk::ac & outputDecrypted)							{ return ::gpk::aesDecode(messageEncrypted, *(const ::gpk::vcu8*)&encryptionKey, level, *(::gpk::au8*)&outputDecrypted); }
	stainli	::gpk::error_t		aesEncode						(const ::gpk::vcc  & messageToEncrypt, const ::gpk::vcu8 & iv, const ::gpk::vci8 & encryptionKey, ::gpk::AES_LEVEL level, ::gpk::au8 & outputEncrypted)	{ return ::gpk::aesEncode(*(const ::gpk::vcu8*)&messageToEncrypt, iv, *(::gpk::vcu8*)&encryptionKey, level, outputEncrypted); }
	stainli	::gpk::error_t		aesDecode						(const ::gpk::vcu8 & messageEncrypted, const ::gpk::vcu8 & iv, const ::gpk::vci8 & encryptionKey, ::gpk::AES_LEVEL level, ::gpk::ac & outputDecrypted)	{ return ::gpk::aesDecode(messageEncrypted, iv, *(const ::gpk::vcu8*)&encryptionKey, level, *(::gpk::au8*)&outputDecrypted); }

	stainli	::gpk::error_t		aesEncode						(const ::gpk::vcu8 & messageToEncrypt, const ::gpk::vcc & encryptionKey, ::gpk::AES_LEVEL level, ::gpk::au8 & outputEncrypted)							{ return ::gpk::aesEncode(messageToEncrypt, *(const ::gpk::vcu8*)&encryptionKey, level, outputEncrypted); }
	stainli	::gpk::error_t		aesDecode						(const ::gpk::vcu8 & messageEncrypted, const ::gpk::vcc & encryptionKey, ::gpk::AES_LEVEL level, ::gpk::au8 & outputDecrypted)							{ return ::gpk::aesDecode(messageEncrypted, *(const ::gpk::vcu8*)&encryptionKey, level, outputDecrypted); }
	stainli	::gpk::error_t		aesEncode						(const ::gpk::vcu8 & messageToEncrypt, const ::gpk::vcu8 & iv, const ::gpk::vcc & encryptionKey, ::gpk::AES_LEVEL level, ::gpk::au8 & outputEncrypted)	{ return ::gpk::aesEncode(messageToEncrypt, iv, *(const ::gpk::vcu8*)&encryptionKey, level, outputEncrypted); }
	stainli	::gpk::error_t		aesDecode						(const ::gpk::vcu8 & messageEncrypted, const ::gpk::vcu8 & iv, const ::gpk::vcc & encryptionKey, ::gpk::AES_LEVEL level, ::gpk::au8 & outputDecrypted)	{ return ::gpk::aesDecode(messageEncrypted, iv, *(const ::gpk::vcu8*)&encryptionKey, level, outputDecrypted); }

	stainli	::gpk::error_t		aesEncode						(const ::gpk::vci8 & messageToEncrypt, const ::gpk::vcc & encryptionKey, ::gpk::AES_LEVEL level, ::gpk::au8 & outputEncrypted)							{ return ::gpk::aesEncode(*(const ::gpk::vcu8*)&messageToEncrypt, *(::gpk::vcu8*)&encryptionKey, level, outputEncrypted); }
	stainli	::gpk::error_t		aesDecode						(const ::gpk::vcu8 & messageEncrypted, const ::gpk::vcc & encryptionKey, ::gpk::AES_LEVEL level, ::gpk::ai8 & outputDecrypted)							{ return ::gpk::aesDecode(messageEncrypted, *(const ::gpk::vcu8*)&encryptionKey, level, *(::gpk::au8*)&outputDecrypted); }
	stainli	::gpk::error_t		aesEncode						(const ::gpk::vci8 & messageToEncrypt, const ::gpk::vcu8 & iv, const ::gpk::vcc & encryptionKey, ::gpk::AES_LEVEL level, ::gpk::au8 & outputEncrypted)	{ return ::gpk::aesEncode(*(const ::gpk::vcu8*)&messageToEncrypt, iv, *(::gpk::vcu8*)&encryptionKey, level, outputEncrypted); }
	stainli	::gpk::error_t		aesDecode						(const ::gpk::vcu8 & messageEncrypted, const ::gpk::vcu8 & iv, const ::gpk::vcc & encryptionKey, ::gpk::AES_LEVEL level, ::gpk::ai8 & outputDecrypted)	{ return ::gpk::aesDecode(messageEncrypted, iv, *(const ::gpk::vcu8*)&encryptionKey, level, *(::gpk::au8*)&outputDecrypted); }

	stainli	::gpk::error_t		aesEncode						(const ::gpk::vcc  & messageToEncrypt, const ::gpk::vcc & encryptionKey, ::gpk::AES_LEVEL level, ::gpk::au8 & outputEncrypted)							{ return ::gpk::aesEncode(*(const ::gpk::vcu8*)&messageToEncrypt, *(::gpk::vcu8*)&encryptionKey, level, outputEncrypted); }
	stainli	::gpk::error_t		aesDecode						(const ::gpk::vcu8 & messageEncrypted, const ::gpk::vcc & encryptionKey, ::gpk::AES_LEVEL level, ::gpk::ac & outputDecrypted)							{ return ::gpk::aesDecode(messageEncrypted, *(const ::gpk::vcu8*)&encryptionKey, level, *(::gpk::au8*)&outputDecrypted); }
	stainli	::gpk::error_t		aesEncode						(const ::gpk::vcc  & messageToEncrypt, const ::gpk::vcu8 & iv, const ::gpk::vcc & encryptionKey, ::gpk::AES_LEVEL level, ::gpk::au8 & outputEncrypted)	{ return ::gpk::aesEncode(*(const ::gpk::vcu8*)&messageToEncrypt, iv, *(::gpk::vcu8*)&encryptionKey, level, outputEncrypted); }
	stainli	::gpk::error_t		aesDecode						(const ::gpk::vcu8 & messageEncrypted, const ::gpk::vcu8 & iv, const ::gpk::vcc & encryptionKey, ::gpk::AES_LEVEL level, ::gpk::ac & outputDecrypted)	{ return ::gpk::aesDecode(messageEncrypted, iv, *(const ::gpk::vcu8*)&encryptionKey, level, *(::gpk::au8*)&outputDecrypted); }
}

#endif //AES_H_28937498273423049
