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
	::gpk::error_t			rsaDecode			(const ::gpk::vcu64 & encrypted, uint64_t n, uint64_t key, ::gpk::au8 & decrypted);
	::gpk::error_t			rsaEncode			(const ::gpk::vcu8  & decrypted, uint64_t n, uint64_t key, uint64_t testKey, ::gpk::au64 & encrypted);
	::gpk::error_t			gpcDecode			(const ::gpk::vcu64 & encrypted, uint64_t n, uint64_t key, bool salt, ::gpk::au8 & decrypted);
	::gpk::error_t			gpcEncode			(const ::gpk::vcu8  & decrypted, uint64_t n, uint64_t key, uint64_t testKey, bool salt, ::gpk::au64 & encrypted);

	stainli	::gpk::error_t	rsaDecode			(const ::gpk::vcu64 & encrypted, uint64_t n, uint64_t key, ::gpk::ai8 & decrypted)									{ return ::gpk::rsaDecode(encrypted, n, key, *(::gpk::au8*)&decrypted); }
	stainli	::gpk::error_t	rsaEncode			(const ::gpk::vci8  & decrypted, uint64_t n, uint64_t key, uint64_t testKey, ::gpk::au64 & encrypted)				{ return ::gpk::rsaEncode(*(const ::gpk::vcu8*)&decrypted, n, key, testKey, encrypted); }
	stainli	::gpk::error_t	gpcDecode			(const ::gpk::vcu64 & encrypted, uint64_t n, uint64_t key, bool salt, ::gpk::ai8 & decrypted)						{ return ::gpk::gpcDecode(encrypted, n, key, salt, *(::gpk::au8*)&decrypted); }
	stainli	::gpk::error_t	gpcEncode			(const ::gpk::vci8  & decrypted, uint64_t n, uint64_t key, uint64_t testKey, bool salt, ::gpk::au64 & encrypted)	{ return ::gpk::gpcEncode(*(const ::gpk::vcu8*)&decrypted, n, key, testKey, salt, encrypted); }

	stainli	::gpk::error_t	rsaDecode			(const ::gpk::vcu64 & encrypted, uint64_t n, uint64_t key, ::gpk::ac & decrypted)									{ return ::gpk::rsaDecode(encrypted, n, key, *(::gpk::au8*)&decrypted); }
	stainli	::gpk::error_t	rsaEncode			(const ::gpk::vcc   & decrypted, uint64_t n, uint64_t key, uint64_t testKey, ::gpk::au64 & encrypted)				{ return ::gpk::rsaEncode(*(const ::gpk::vcu8*)&decrypted, n, key, testKey, encrypted); }
	stainli	::gpk::error_t	gpcDecode			(const ::gpk::vcu64 & encrypted, uint64_t n, uint64_t key, bool salt, ::gpk::ac & decrypted)						{ return ::gpk::gpcDecode(encrypted, n, key, salt, *(::gpk::au8*)&decrypted); }
	stainli	::gpk::error_t	gpcEncode			(const ::gpk::vcc   & decrypted, uint64_t n, uint64_t key, uint64_t testKey, bool salt, ::gpk::au64 & encrypted)	{ return ::gpk::gpcEncode(*(const ::gpk::vcu8*)&decrypted, n, key, testKey, salt, encrypted); }

	::gpk::error_t			gpcDecodeWithHash	(const ::gpk::vcu64 & encrypted, uint64_t n, uint64_t key, bool salt, ::gpk::au8 & decrypted);
	::gpk::error_t			gpcEncodeWithHash	(const ::gpk::vcu8  & decrypted, uint64_t n, uint64_t key, uint64_t testKey, bool salt, ::gpk::au64 & encrypted);
	stainli	::gpk::error_t	gpcDecodeWithHash	(const ::gpk::vcu64 & encrypted, uint64_t n, uint64_t key, bool salt, ::gpk::ai8 & decrypted)						{ return ::gpk::gpcDecodeWithHash(encrypted, n, key, salt, *(::gpk::au8*)&decrypted); }
	stainli	::gpk::error_t	gpcEncodeWithHash	(const ::gpk::vci8  & decrypted, uint64_t n, uint64_t key, uint64_t testKey, bool salt, ::gpk::au64 & encrypted)	{ return ::gpk::gpcEncodeWithHash(*(::gpk::vcu8*)&decrypted, n, key, testKey, salt, encrypted); }
	stainli	::gpk::error_t	gpcDecodeWithHash	(const ::gpk::vcu64 & encrypted, uint64_t n, uint64_t key, bool salt, ::gpk::ac & decrypted)						{ return ::gpk::gpcDecodeWithHash(encrypted, n, key, salt, *(::gpk::au8*)&decrypted); }
	stainli	::gpk::error_t	gpcEncodeWithHash	(const ::gpk::vcc   & decrypted, uint64_t n, uint64_t key, uint64_t testKey, bool salt, ::gpk::au64 & encrypted)	{ return ::gpk::gpcEncodeWithHash(*(::gpk::vcu8*)&decrypted, n, key, testKey, salt, encrypted); }
} // namespace

#endif // GPK_RSA_H_23627
