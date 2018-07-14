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
							int32_t								KeyLength ;
							int32_t								KeyExpSize;
	};

	static constexpr	const SAESLevelProperties			AES_LEVEL_PROPERTIES	[3]		= 
		{ {16, 176}
		, {24, 208}
		, {32, 240}
		};

	static constexpr	const uint32_t						AES_SIZEBLOCK					= 16; // Block length in bytes AES is 128b block only
	static constexpr	const uint32_t						AES_SIZEIV						= 16; 

	struct SAESContext {
							AES_LEVEL							Level							= {};
							::gpk::array_pod<uint8_t>			RoundKey						= {};
							uint8_t								Iv			[AES_SIZEBLOCK]		= {};
	};

						void								aesInitCtx						(SAESContext* ctx, const uint8_t* key, AES_LEVEL level);
						void								aesInitCtxIV					(SAESContext* ctx, const uint8_t* key, const uint8_t* iv, AES_LEVEL level);
						void								aesCtxSetIV						(SAESContext* ctx, const uint8_t* iv);

						// ECB enables the basic ECB 16-byte block algorithm. All can be enabled simultaneously. Buffer size is exactly AES_SIZEBLOCK bytes; you need only AES_init_ctx as IV is not used in ECB. 
						// NB: ECB is considered insecure for most uses
						void								aesECBEncrypt					(SAESContext* ctx, uint8_t* buf, AES_LEVEL level);
						void								aesECBDecrypt					(SAESContext* ctx, uint8_t* buf, AES_LEVEL level);

						// CBC enables AES encryption in CBC-mode of operation.
						// buffer size MUST be mutile of AES_SIZEBLOCK; Suggest https://en.wikipedia.org/wiki/Padding_(cryptography)#PKCS7 for padding scheme.
						// NOTES: you need to set IV in ctx via aesInitCtxIV() or aesCtxSetIV() no IV should ever be reused with the same key 
						void								aesCBCEncryptBuffer				(SAESContext* ctx, uint8_t* buf, uint32_t length, AES_LEVEL level);
						void								aesCBCDecryptBuffer				(SAESContext* ctx, uint8_t* buf, uint32_t length, AES_LEVEL level);

						// CTR enables encryption in counter-mode.
						// Symmetrical operation: Same function for encrypting as for decrypting. IV is incremented for every block, and used after encryption as XOR-compliment for output; Suggesting https://en.wikipedia.org/wiki/Padding_(cryptography)#PKCS7 for padding scheme.
						// NOTES: you need to set IV in ctx with aesInitCtxIV() or aesCtxSetIV() no IV should ever be reused with the same key 
						void								aesCTRXCryptBuffer				(SAESContext* ctx, uint8_t* buf, uint32_t length, AES_LEVEL level);
}

#endif //AES_H_28937498273423049
