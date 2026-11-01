#include "gpk_string.h"

#ifndef GPK_RSA_H_23627
#define GPK_RSA_H_23627

namespace gpk
{
	struct SRSAKeyPair {
		u3_t				Public;
		u3_t				Private;
	};

	err_t			rsaKeys				(u2_t prime1, u2_t prime2, u2_t offset, u3_t limit, apod<SRSAKeyPair> & keys);
	err_t			rsaDecode			(const vcu3_t	& encrypted, u3_t n, u3_t key, au0_t & decrypted);
	err_t			rsaEncode			(const vcu0_t	& decrypted, u3_t n, u3_t key, u3_t testKey, au3_t & encrypted);
	err_t			gpcDecode			(const vcu3_t	& encrypted, u3_t n, u3_t key, bool salt, au0_t & decrypted);
	err_t			gpcEncode			(const vcu0_t	& decrypted, u3_t n, u3_t key, u3_t testKey, bool salt, au3_t & encrypted);

	stin	err_t	rsaDecode			(const vcu3_t	& encrypted, u3_t n, u3_t key, as0_t & decrypted)							{ return rsaDecode(encrypted, n, key, *(au0_t*)&decrypted); }
	stin	err_t	rsaEncode			(const vcs0_t	& decrypted, u3_t n, u3_t key, u3_t testKey, au3_t & encrypted)				{ return rsaEncode(*(const vcu0_t*)&decrypted, n, key, testKey, encrypted); }
	stin	err_t	gpcDecode			(const vcu3_t	& encrypted, u3_t n, u3_t key, bool salt, as0_t & decrypted)				{ return gpcDecode(encrypted, n, key, salt, *(au0_t*)&decrypted); }
	stin	err_t	gpcEncode			(const vcs0_t	& decrypted, u3_t n, u3_t key, u3_t testKey, bool salt, au3_t & encrypted)	{ return gpcEncode(*(const vcu0_t*)&decrypted, n, key, testKey, salt, encrypted); }

	stin	err_t	rsaDecode			(const vcu3_t	& encrypted, u3_t n, u3_t key, string & decrypted)							{ return rsaDecode(encrypted, n, key, *(au0_t*)&decrypted); }
	stin	err_t	rsaEncode			(const vcst_t	& decrypted, u3_t n, u3_t key, u3_t testKey, au3_t & encrypted)				{ return rsaEncode(*(const vcu0_t*)&decrypted, n, key, testKey, encrypted); }
	stin	err_t	gpcDecode			(const vcu3_t	& encrypted, u3_t n, u3_t key, bool salt, string & decrypted)				{ return gpcDecode(encrypted, n, key, salt, *(au0_t*)&decrypted); }
	stin	err_t	gpcEncode			(const vcst_t	& decrypted, u3_t n, u3_t key, u3_t testKey, bool salt, au3_t & encrypted)	{ return gpcEncode(*(const vcu0_t*)&decrypted, n, key, testKey, salt, encrypted); }

	err_t			gpcDecodeWithHash	(vcu3_t	encrypted, u3_t n, u3_t key, bool salt, au0_t & decrypted);
	err_t			gpcEncodeWithHash	(vcu0_t	decrypted, u3_t n, u3_t key, u3_t testKey, bool salt, au3_t & encrypted);
	stin	err_t	gpcDecodeWithHash	(vcu3_t	encrypted, u3_t n, u3_t key, bool salt, as0_t & decrypted)							{ return gpcDecodeWithHash(encrypted, n, key, salt, *(au0_t*)&decrypted); }
	stin	err_t	gpcEncodeWithHash	(vcs0_t	decrypted, u3_t n, u3_t key, u3_t testKey, bool salt, au3_t & encrypted)			{ return gpcEncodeWithHash(*(vcu0_t*)&decrypted, n, key, testKey, salt, encrypted); }
	stin	err_t	gpcDecodeWithHash	(vcu3_t	encrypted, u3_t n, u3_t key, bool salt, string & decrypted)							{ return gpcDecodeWithHash(encrypted, n, key, salt, *(au0_t*)&decrypted); }
	stin	err_t	gpcEncodeWithHash	(vcst_t	decrypted, u3_t n, u3_t key, u3_t testKey, bool salt, au3_t & encrypted)			{ return gpcEncodeWithHash(*(vcu0_t*)&decrypted, n, key, testKey, salt, encrypted); }
} // namespace

#endif // GPK_RSA_H_23627
