#include "gpk_array.h"

#ifndef GPK_RSA_H_2018_10_23
#define GPK_RSA_H_2018_10_23

namespace gpk
{
	struct SRSAKeyPair {
		uint64_t									Public;
		uint64_t									Private;
	};

	::gpk::error_t								rsaKeys								(uint32_t prime1, uint32_t prime2, uint32_t offset, uint64_t limit, ::gpk::apod<SRSAKeyPair>& keys);
	::gpk::error_t								rsaDecode							(const ::gpk::view<const uint64_t	> & encrypted, uint64_t n, uint64_t key, ::gpk::apod<char>& decrypted);
	::gpk::error_t								rsaEncode							(const ::gpk::view<const byte_t		> & decrypted, uint64_t n, uint64_t key, uint64_t testKey, ::gpk::apod<uint64_t>& encrypted);
	::gpk::error_t								gpcDecode							(const ::gpk::view<const uint64_t	> & encrypted, uint64_t n, uint64_t key, bool salt, ::gpk::apod<byte_t>& decrypted);
	::gpk::error_t								gpcEncode							(const ::gpk::view<const byte_t		> & decrypted, uint64_t n, uint64_t key, uint64_t testKey, bool salt, ::gpk::apod<uint64_t>& encrypted);
																									
	::gpk::error_t								gpcDecodeWithHash					(const ::gpk::view<const uint64_t	> & encrypted, uint64_t n, uint64_t key, bool salt, ::gpk::apod<byte_t>& decrypted);
	::gpk::error_t								gpcEncodeWithHash					(const ::gpk::view<const byte_t		> & decrypted, uint64_t n, uint64_t key, uint64_t testKey, bool salt, ::gpk::apod<uint64_t>& encrypted);
} // namespace

#endif // GPK_RSA_H_2018_10_23
