#include "gpk_array.h"

#ifndef GPK_RSA_H_23627
#define GPK_RSA_H_23627

namespace gpk
{
	struct SRSAKeyPair {
		uint64_t				Public;
		uint64_t				Private;
	};

	::gpk::error_t			rsaKeys				(uint32_t prime1, uint32_t prime2, uint32_t offset, uint64_t limit, ::gpk::apod<SRSAKeyPair> & keys);
	::gpk::error_t			rsaDecode			(const ::gpk::vcu3_t & encrypted, uint64_t n, uint64_t key, ::gpk::au0_t & decrypted);
	::gpk::error_t			rsaEncode			(const ::gpk::vcu0_t  & decrypted, uint64_t n, uint64_t key, uint64_t testKey, ::gpk::au3_t & encrypted);
	::gpk::error_t			gpcDecode			(const ::gpk::vcu3_t & encrypted, uint64_t n, uint64_t key, bool salt, ::gpk::au0_t & decrypted);
	::gpk::error_t			gpcEncode			(const ::gpk::vcu0_t  & decrypted, uint64_t n, uint64_t key, uint64_t testKey, bool salt, ::gpk::au3_t & encrypted);

	stainli	::gpk::error_t	rsaDecode			(const ::gpk::vcu64 & encrypted, uint64_t n, uint64_t key, ::gpk::as0_t & decrypted)								{ return ::gpk::rsaDecode(encrypted, n, key, *(::gpk::au0_t*)&decrypted); }
	stainli	::gpk::error_t	rsaEncode			(const ::gpk::vcs0_t  & decrypted, uint64_t n, uint64_t key, uint64_t testKey, ::gpk::au3_t & encrypted)				{ return ::gpk::rsaEncode(*(const ::gpk::vcu0_t*)&decrypted, n, key, testKey, encrypted); }
	stainli	::gpk::error_t	gpcDecode			(const ::gpk::vcu3_t & encrypted, uint64_t n, uint64_t key, bool salt, ::gpk::as0_t & decrypted)						{ return ::gpk::gpcDecode(encrypted, n, key, salt, *(::gpk::au0_t*)&decrypted); }
	stainli	::gpk::error_t	gpcEncode			(const ::gpk::vcs0_t  & decrypted, uint64_t n, uint64_t key, uint64_t testKey, bool salt, ::gpk::au3_t & encrypted)	{ return ::gpk::gpcEncode(*(const ::gpk::vcu0_t*)&decrypted, n, key, testKey, salt, encrypted); }

	stainli	::gpk::error_t	rsaDecode			(const ::gpk::vcu3_t & encrypted, uint64_t n, uint64_t key, ::gpk::asc_t & decrypted)									{ return ::gpk::rsaDecode(encrypted, n, key, *(::gpk::au0_t*)&decrypted); }
	stainli	::gpk::error_t	rsaEncode			(const ::gpk::vcsc_t   & decrypted, uint64_t n, uint64_t key, uint64_t testKey, ::gpk::au3_t & encrypted)				{ return ::gpk::rsaEncode(*(const ::gpk::vcu0_t*)&decrypted, n, key, testKey, encrypted); }
	stainli	::gpk::error_t	gpcDecode			(const ::gpk::vcu3_t & encrypted, uint64_t n, uint64_t key, bool salt, ::gpk::asc_t & decrypted)						{ return ::gpk::gpcDecode(encrypted, n, key, salt, *(::gpk::au0_t*)&decrypted); }
	stainli	::gpk::error_t	gpcEncode			(const ::gpk::vcsc_t & decrypted, uint64_t n, uint64_t key, uint64_t testKey, bool salt, ::gpk::au3_t & encrypted)	{ return ::gpk::gpcEncode(*(const ::gpk::vcu0_t*)&decrypted, n, key, testKey, salt, encrypted); }

	::gpk::error_t			gpcDecodeWithHash	(const ::gpk::vcu3_t & encrypted, uint64_t n, uint64_t key, bool salt, ::gpk::au0_t & decrypted);
	::gpk::error_t			gpcEncodeWithHash	(const ::gpk::vcu0_t  & decrypted, uint64_t n, uint64_t key, uint64_t testKey, bool salt, ::gpk::au3_t & encrypted);
	stainli	::gpk::error_t	gpcDecodeWithHash	(const ::gpk::vcu3_t & encrypted, uint64_t n, uint64_t key, bool salt, ::gpk::as0_t & decrypted)						{ return ::gpk::gpcDecodeWithHash(encrypted, n, key, salt, *(::gpk::au0_t*)&decrypted); }
	stainli	::gpk::error_t	gpcEncodeWithHash	(const ::gpk::vcs0_t  & decrypted, uint64_t n, uint64_t key, uint64_t testKey, bool salt, ::gpk::au3_t & encrypted)	{ return ::gpk::gpcEncodeWithHash(*(::gpk::vcu0_t*)&decrypted, n, key, testKey, salt, encrypted); }
	stainli	::gpk::error_t	gpcDecodeWithHash	(const ::gpk::vcu64 & encrypted, uint64_t n, uint64_t key, bool salt, ::gpk::asc_t & decrypted)						{ return ::gpk::gpcDecodeWithHash(encrypted, n, key, salt, *(::gpk::au0_t*)&decrypted); }
	stainli	::gpk::error_t	gpcEncodeWithHash	(const ::gpk::vcsc_t   & decrypted, uint64_t n, uint64_t key, uint64_t testKey, bool salt, ::gpk::au3_t & encrypted)	{ return ::gpk::gpcEncodeWithHash(*(::gpk::vcu0_t*)&decrypted, n, key, testKey, salt, encrypted); }
} // namespace

#endif // GPK_RSA_H_23627
