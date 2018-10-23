#include "gpk_array.h"

#ifndef GPK_RSA_H_2018_10_23
#define GPK_RSA_H_2018_10_23

namespace gpk
{
	struct SRSAKeyPair {
		uint64_t	Public;
		uint64_t	Private;
	};

	struct SRSA {
		uint64_t									n = 0, t = 0, j			= 0;
		uint32_t									Primes[2]				= {};
	};

	::gpk::error_t								rsaInitialize						(::gpk::SRSA & g_SRSA, uint32_t prime1, uint32_t prime2);
	::gpk::error_t								rsaKeys								(::gpk::SRSA & g_SRSA, uint64_t limit, ::gpk::array_pod<SRSAKeyPair>& keys);
	::gpk::error_t								rsaDecode							(::gpk::SRSA & g_SRSA, ::gpk::view_array<const uint64_t> encrypted, uint64_t key, ::gpk::array_pod<char>& decrypted);
	::gpk::error_t								rsaEncode							(::gpk::SRSA & g_SRSA, ::gpk::view_array<const byte_t>	 decrypted, uint64_t key, ::gpk::array_pod<uint64_t>& encrypted);
	::gpk::error_t								rsaSafeDecode						(::gpk::SRSA & g_SRSA, ::gpk::view_array<const uint64_t> encrypted, uint64_t key, bool salt, ::gpk::array_pod<char>& decrypted);
	::gpk::error_t								rsaSafeEncode						(::gpk::SRSA & g_SRSA, ::gpk::view_array<const byte_t>	 decrypted, uint64_t key, bool salt, ::gpk::array_pod<uint64_t>& encrypted);
} // namespace 

#endif // GPK_RSA_H_2018_10_23
