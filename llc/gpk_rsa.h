#include "gpk_array.h"

#ifndef GPK_RSA_H_23627
#define GPK_RSA_H_23627

namespace gpk
{
	struct SRSAKeyPair {
		u3_t				Public;
		u3_t				Private;
	};

	gpk::err_t			rsaKeys				(uint32_t prime1, uint32_t prime2, uint32_t offset, u3_t limit, ::gpk::apod<SRSAKeyPair> & keys);
	gpk::err_t			rsaDecode			(const ::gpk::vcu3_t & encrypted, u3_t n, u3_t key, ::gpk::au0_t & decrypted);
	gpk::err_t			rsaEncode			(const ::gpk::vcu0_t  & decrypted, u3_t n, u3_t key, u3_t testKey, ::gpk::au3_t & encrypted);
	gpk::err_t			gpcDecode			(const ::gpk::vcu3_t & encrypted, u3_t n, u3_t key, bool salt, ::gpk::au0_t & decrypted);
	gpk::err_t			gpcEncode			(const ::gpk::vcu0_t  & decrypted, u3_t n, u3_t key, u3_t testKey, bool salt, ::gpk::au3_t & encrypted);

	stin	gpk::err_t	rsaDecode			(const ::gpk::vcu64 & encrypted, u3_t n, u3_t key, ::gpk::as0_t & decrypted)								{ return ::gpk::rsaDecode(encrypted, n, key, *(::gpk::au0_t*)&decrypted); }
	stin	gpk::err_t	rsaEncode			(const ::gpk::vcs0_t  & decrypted, u3_t n, u3_t key, u3_t testKey, ::gpk::au3_t & encrypted)				{ return ::gpk::rsaEncode(*(const ::gpk::vcu0_t*)&decrypted, n, key, testKey, encrypted); }
	stin	gpk::err_t	gpcDecode			(const ::gpk::vcu3_t & encrypted, u3_t n, u3_t key, bool salt, ::gpk::as0_t & decrypted)						{ return ::gpk::gpcDecode(encrypted, n, key, salt, *(::gpk::au0_t*)&decrypted); }
	stin	gpk::err_t	gpcEncode			(const ::gpk::vcs0_t  & decrypted, u3_t n, u3_t key, u3_t testKey, bool salt, ::gpk::au3_t & encrypted)	{ return ::gpk::gpcEncode(*(const ::gpk::vcu0_t*)&decrypted, n, key, testKey, salt, encrypted); }

	stin	gpk::err_t	rsaDecode			(const ::gpk::vcu3_t & encrypted, u3_t n, u3_t key, ::gpk::asc_t & decrypted)									{ return ::gpk::rsaDecode(encrypted, n, key, *(::gpk::au0_t*)&decrypted); }
	stin	gpk::err_t	rsaEncode			(const ::gpk::vcsc_t   & decrypted, u3_t n, u3_t key, u3_t testKey, ::gpk::au3_t & encrypted)				{ return ::gpk::rsaEncode(*(const ::gpk::vcu0_t*)&decrypted, n, key, testKey, encrypted); }
	stin	gpk::err_t	gpcDecode			(const ::gpk::vcu3_t & encrypted, u3_t n, u3_t key, bool salt, ::gpk::asc_t & decrypted)						{ return ::gpk::gpcDecode(encrypted, n, key, salt, *(::gpk::au0_t*)&decrypted); }
	stin	gpk::err_t	gpcEncode			(const ::gpk::vcsc_t & decrypted, u3_t n, u3_t key, u3_t testKey, bool salt, ::gpk::au3_t & encrypted)	{ return ::gpk::gpcEncode(*(const ::gpk::vcu0_t*)&decrypted, n, key, testKey, salt, encrypted); }

	gpk::err_t			gpcDecodeWithHash	(const ::gpk::vcu3_t & encrypted, u3_t n, u3_t key, bool salt, ::gpk::au0_t & decrypted);
	gpk::err_t			gpcEncodeWithHash	(const ::gpk::vcu0_t  & decrypted, u3_t n, u3_t key, u3_t testKey, bool salt, ::gpk::au3_t & encrypted);
	stin	gpk::err_t	gpcDecodeWithHash	(const ::gpk::vcu3_t & encrypted, u3_t n, u3_t key, bool salt, ::gpk::as0_t & decrypted)						{ return ::gpk::gpcDecodeWithHash(encrypted, n, key, salt, *(::gpk::au0_t*)&decrypted); }
	stin	gpk::err_t	gpcEncodeWithHash	(const ::gpk::vcs0_t  & decrypted, u3_t n, u3_t key, u3_t testKey, bool salt, ::gpk::au3_t & encrypted)	{ return ::gpk::gpcEncodeWithHash(*(::gpk::vcu0_t*)&decrypted, n, key, testKey, salt, encrypted); }
	stin	gpk::err_t	gpcDecodeWithHash	(const ::gpk::vcu64 & encrypted, u3_t n, u3_t key, bool salt, ::gpk::asc_t & decrypted)						{ return ::gpk::gpcDecodeWithHash(encrypted, n, key, salt, *(::gpk::au0_t*)&decrypted); }
	stin	gpk::err_t	gpcEncodeWithHash	(const ::gpk::vcsc_t   & decrypted, u3_t n, u3_t key, u3_t testKey, bool salt, ::gpk::au3_t & encrypted)	{ return ::gpk::gpcEncodeWithHash(*(::gpk::vcu0_t*)&decrypted, n, key, testKey, salt, encrypted); }
} // namespace

#endif // GPK_RSA_H_23627
