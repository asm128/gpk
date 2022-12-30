#include "gpk_array.h"

#ifndef AES_H_28937498273423049
#define AES_H_28937498273423049

namespace gpk
{
	enum AES_LEVEL
		{ AES_LEVEL_128 = 0
		, AES_LEVEL_192
		, AES_LEVEL_256
		};

	enum AES_MODE
		{ AES_MODE_ECB = 0
		, AES_MODE_CBC
		, AES_MODE_CTR
		};

	struct SAESLevelProperties {
							int32_t							KeyLength ;
							int32_t							KeyExpSize;
	};

	static constexpr	const SAESLevelProperties		AES_LEVEL_PROPERTIES	[3]		=
		{ {16, 176}
		, {24, 208}
		, {32, 240}
		};

	static constexpr	const uint32_t					AES_SIZEBLOCK					= 16; // Block length in bytes AES is 128b block only
	static constexpr	const uint32_t					AES_SIZEIV						= 16;

	struct SAESContext {
							AES_LEVEL						Level							= {};
							::gpk::apod<uint8_t>		RoundKey						= {};
							uint8_t							Iv			[AES_SIZEBLOCK]		= {};
	};

						void							aesInitCtx						(SAESContext* ctx, const uint8_t* key, ::gpk::AES_LEVEL level);
						void							aesInitCtxIV					(SAESContext* ctx, const uint8_t* key, ::gpk::AES_LEVEL level, const uint8_t* iv);
						void							aesCtxSetIV						(SAESContext* ctx, const uint8_t* iv);

						// ECB enables the basic ECB 16-byte block algorithm. All can be enabled simultaneously. Buffer size is exactly AES_SIZEBLOCK bytes; you need only AES_init_ctx as IV is not used in ECB.
						// NB: ECB is considered insecure for most uses
						void							aesECBEncrypt					(SAESContext* ctx, uint8_t* buf);
						void							aesECBDecrypt					(SAESContext* ctx, uint8_t* buf);

						// CBC enables AES encryption in CBC-mode of operation.
						// buffer size MUST be mutile of AES_SIZEBLOCK; Suggest https://en.wikipedia.org/wiki/Padding_(cryptography)#PKCS7 for padding scheme.
						// NOTES: you need to set IV in ctx via aesInitCtxIV() or aesCtxSetIV() no IV should ever be reused with the same key
						void							aesCBCEncryptBuffer				(SAESContext* ctx, uint8_t* buf, uint32_t length);
						void							aesCBCDecryptBuffer				(SAESContext* ctx, uint8_t* buf, uint32_t length);

						// CTR enables encryption in counter-mode.
						// Symmetrical operation: Same function for encrypting as for decrypting. IV is incremented for every block, and used after encryption as XOR-compliment for output; Suggesting https://en.wikipedia.org/wiki/Padding_(cryptography)#PKCS7 for padding scheme.
						// NOTES: you need to set IV in ctx with aesInitCtxIV() or aesCtxSetIV() no IV should ever be reused with the same key
						void							aesCTRXCryptBuffer				(SAESContext* ctx, uint8_t* buf, uint32_t length);

						::gpk::error_t					aesEncode						(const ::gpk::vcb& messageToEncrypt			, const ::gpk::vcb& encryptionKey, ::gpk::AES_LEVEL level, ::gpk::apod<byte_t>& outputEncrypted);
						::gpk::error_t					aesDecode						(const ::gpk::vcb& messageEncrypted			, const ::gpk::vcb& encryptionKey, ::gpk::AES_LEVEL level, ::gpk::apod<byte_t>& outputDecrypted);
	static inline		::gpk::error_t					aesEncode						(const byte_t* messageToEncrypt, uint32_t dataLength	, const ::gpk::vcb& encryptionKey, ::gpk::AES_LEVEL level, ::gpk::apod<byte_t>& outputEncrypted)	{ return ::gpk::aesEncode({messageToEncrypt, dataLength}, encryptionKey, level, outputEncrypted); }
	static inline		::gpk::error_t					aesDecode						(const byte_t* messageEncrypted, uint32_t dataLength	, const ::gpk::vcb& encryptionKey, ::gpk::AES_LEVEL level, ::gpk::apod<byte_t>& outputDecrypted)	{ return ::gpk::aesDecode({messageEncrypted, dataLength}, encryptionKey, level, outputDecrypted); }

						::gpk::error_t					aesEncode						(const ::gpk::vcb& messageToEncrypt			, const ::gpk::vcub& iv, const ::gpk::vcb& encryptionKey, ::gpk::AES_LEVEL level, ::gpk::apod<byte_t>& outputEncrypted);
						::gpk::error_t					aesDecode						(const ::gpk::vcb& messageEncrypted			, const ::gpk::vcub& iv, const ::gpk::vcb& encryptionKey, ::gpk::AES_LEVEL level, ::gpk::apod<byte_t>& outputDecrypted);
	static inline		::gpk::error_t					aesEncode						(const byte_t* messageToEncrypt, uint32_t dataLength	, const ::gpk::vcub& iv, const ::gpk::vcb& encryptionKey, ::gpk::AES_LEVEL level, ::gpk::apod<byte_t>& outputEncrypted)	{ return ::gpk::aesEncode({messageToEncrypt, dataLength}, iv, encryptionKey, level, outputEncrypted); }
	static inline		::gpk::error_t					aesDecode						(const byte_t* messageEncrypted, uint32_t dataLength	, const ::gpk::vcub& iv, const ::gpk::vcb& encryptionKey, ::gpk::AES_LEVEL level, ::gpk::apod<byte_t>& outputDecrypted)	{ return ::gpk::aesDecode({messageEncrypted, dataLength}, iv, encryptionKey, level, outputDecrypted); }
}

#endif //AES_H_28937498273423049
