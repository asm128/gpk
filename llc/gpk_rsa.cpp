#include "gpk_rsa.h"
#include "gpk_encoding.h"
#include "gpk_noise.h"
#include <cmath>

#define RSA_ARDELL_KEY	1079150697//117515017//1054987632

static	int									primalityTest						(uint64_t number)						{
	uint64_t										j									= (uint64_t)sqrt((double)number);
	if(0 == (number & 1))
		return 0;
	if(number > 1 && number < 9)
		return 1;
	for(uint64_t i = 3; i <= j; ++i) {
		if(0 == (number % i))
			return 0;
	}
	return 1;
}

static	uint64_t							commonDivisor						(const uint64_t t, const uint64_t a)				{
	uint64_t										k									= 1;
	while(1) {
		k											= k + t;
		if(k % a == 0)
			return(k / a);
	}
}

// function to generate encryption keys
		::gpk::error_t						gpk::rsaKeys						(uint32_t prime1, uint32_t prime2, uint32_t offset, uint64_t limit, ::gpk::array_pod<SRSAKeyPair>& keys)			{
	ree_if(0 == ::primalityTest(prime1), "Cannot generate RSA keys with a number that is not prime: %u.", prime1);
	ree_if(0 == ::primalityTest(prime2), "Cannot generate RSA keys with a number that is not prime: %u.", prime2);
	uint32_t										k									= 0;
	uint64_t										j									= 0;
	uint64_t										t									= (prime1 - 1) * (prime2 - 1);
	for(uint64_t i = offset; i < t; ++i) {
		if(t % i == 0)
			continue;
		j											= (uint64_t)sqrt((double)i);
		unsigned int									isPrime								= ::primalityTest(i);
		if(isPrime == 1 && i != prime1 && i != prime2) {
			if(keys.size() > k)
				keys[k].Public								= i;
			else
				gpk_necall(keys.push_back({i}), "%s", "Out of memory?");

			uint64_t										cd									= ::commonDivisor(t, keys[(uint32_t)k].Public);
			if(cd > 0) {
				keys[k].Private								= cd;
				//break;
				++k;
			}
			if(k >= limit)
				break;
		}
	}
	return ::gpk::error_t(j);
}

//function to encrypt the message
		::gpk::error_t						gpk::rsaEncode						(const ::gpk::view_array<const byte_t> & decrypted, uint64_t n, uint64_t key, uint64_t testkey, ::gpk::array_pod<uint64_t>& encrypted) {
	uint32_t										offset								= encrypted.size();
	uint32_t										i									= 0;
	gpk_necall(encrypted.resize(offset + decrypted.size()), "%s", "Out of memory?");
	const byte_t									* pDecrypted						= decrypted.begin();
	uint64_t										* pEncrypted						= &encrypted[offset];
	while(i < decrypted.size()) {
		uint64_t										pt									= (ubyte_t)pDecrypted[i];
		uint64_t										k									= 1;
		for(uint64_t j = 0; j < key; ++j) {
			k											= k * pt;
			k											= k % n;
		}
		pEncrypted[offset + i]						= k;
		++i;
	}
	if(testkey) {
		::gpk::array_pod<byte_t>						decryptTest							= {};
		::gpk::rsaDecode({&pEncrypted[offset], encrypted.size() - offset}, n, testkey, decryptTest);
		::gpk::view_array<const byte_t>					filter_view							= {&pDecrypted[offset], decrypted.size() - offset};
		gerror_if(decryptTest.size() != filter_view.size(), "%s", "Error!");
		for(uint32_t iTest = 0; iTest < decryptTest.size(); ++iTest) {
			gerror_if(decryptTest[iTest] != filter_view[iTest], "%s", "Error!");
		}
	}
	return i;
}

// function to decrypt the message
		::gpk::error_t						gpk::rsaDecode						(const ::gpk::view_array<const uint64_t> & encrypted, uint64_t n, uint64_t key, ::gpk::array_pod<char>& decrypted) {
	uint32_t										offset								= decrypted.size();
	uint32_t										i									= 0;
	gpk_necall(decrypted.resize(offset + encrypted.size() + 1), "%s", "Out of memory?");
	byte_t											* pDecrypted						= &decrypted[offset];pDecrypted;
	const uint64_t									* pEncrypted						= encrypted.begin();pEncrypted;
	while(i < encrypted.size()) {
		uint64_t										ct									= pEncrypted[i];
		uint64_t										k									= 1;
		for(uint64_t j = 0; j < key; ++j) {
			k											= k * ct;
			k											= k % n;
		}
		pDecrypted[offset + i]						= (char)k;
		++i;
	}
	pDecrypted[decrypted.size() - 1]			= 0;
	decrypted.resize(decrypted.size() - 1);
	return (int)i;
}

		::gpk::error_t						gpcDefilterSub			(::gpk::view_array<ubyte_t>& scanline, uint32_t bpp) {
	ubyte_t											* pScanline				= scanline.begin();
	for(uint32_t iByte = bpp, countBytes = scanline.size(); iByte < countBytes; ++iByte)
		pScanline[iByte]							+= pScanline[iByte - bpp];
	return 0;
}

		::gpk::error_t						gpcDefilterSub2			(::gpk::view_array<ubyte_t>& scanline, uint32_t bpp) {
	ubyte_t											* pScanline				= scanline.begin();
	for(int32_t iByte = scanline.size() - bpp - 1; iByte >= 0; --iByte)
		pScanline[iByte]							+= pScanline[iByte + bpp];
	return 0;
}

		::gpk::error_t						gpcFilterSub			(::gpk::view_array<ubyte_t>& scanline, uint32_t bpp) {
	::gpk::array_pod<ubyte_t>						filtered;
	const uint32_t									countBytes				= scanline.size();
	gpk_necall(filtered.resize(countBytes), "%s", "Out of memory?");
	ubyte_t											* pScanline				= scanline.begin();
	{ // generate filtered bytes
		ubyte_t											* pFiltered				= filtered.begin();
		for(uint32_t iByte = bpp; iByte < countBytes; ++iByte)
			pFiltered[iByte]							= pScanline[iByte] - pScanline[iByte - bpp];
	}
	{ // copy generated bytes back to scanline
		const ubyte_t									* pFiltered				= filtered.begin();
		for(uint32_t iByte = bpp; iByte < countBytes; ++iByte)
			pScanline[iByte]							= pFiltered[iByte];
	}
	return 0;
}

		::gpk::error_t						gpcFilterSub2	(::gpk::view_array<ubyte_t>& scanline, uint32_t bpp) {
	::gpk::array_pod<ubyte_t>						filtered;
	gpk_necall(filtered.resize(scanline.size()), "%s", "Out of memory?");;
	ubyte_t											* pScanline				= scanline.begin();
	{ // generate filtered bytes
		ubyte_t											* pFiltered				= filtered.begin();
		for(int32_t iByte = scanline.size() - bpp - 1; iByte >= 0; --iByte)
			pFiltered[iByte]							= pScanline[iByte] - pScanline[iByte + bpp];
	}
	{ // copy generated bytes back to scanline
		const ubyte_t									* pFiltered				= filtered.begin();
		for(uint32_t iByte = 0; iByte < scanline.size() - bpp; ++iByte)
			pScanline[iByte]							= pFiltered[iByte];
	}
	return 0;
}

//#define DISABLE_ARDELL

static	::gpk::error_t						gpcFilter0Apply						(::gpk::view_array<ubyte_t>& scanline) { gpk_necall(::gpcFilterSub		(scanline, 1), "%s", "??"); return ::gpcFilterSub2		(scanline, 1);		}
static	::gpk::error_t						gpcFilter0Remove					(::gpk::view_array<ubyte_t>& scanline) { gpk_necall(::gpcDefilterSub2	(scanline, 1), "%s", "??"); return ::gpcDefilterSub		(scanline, 1);		}
static	::gpk::error_t						gpcFilter1Apply						(::gpk::view_array<ubyte_t>& scanline) { gpk_necall(::gpcFilterSub2		(scanline, 2), "%s", "??"); return ::gpcFilterSub		(scanline, 4);		}
static	::gpk::error_t						gpcFilter1Remove					(::gpk::view_array<ubyte_t>& scanline) { gpk_necall(::gpcDefilterSub	(scanline, 4), "%s", "??"); return ::gpcDefilterSub2	(scanline, 2);		}

//function to encrypt the message
		::gpk::error_t						gpk::gpcEncode						(const ::gpk::view_array<const byte_t> & decrypted, uint64_t n, uint64_t key, uint64_t testkey, bool salt, ::gpk::array_pod<uint64_t>& encrypted) {
	uint32_t										offset								= encrypted.size();
	uint32_t										i									= 0;

	::gpk::array_pod<byte_t>						filtered;
	if(false == salt) {
		gpk_necall(::gpk::ardellEncode(decrypted, RSA_ARDELL_KEY, salt, filtered), "Failed to encode Ardell. %s", "Out of memory?");
	}
	else { //
		::gpk::array_pod<byte_t>						salted;
		gpk_necall(::gpk::saltDataSalt(decrypted, salted), "Failed to add salt. %s", "Out of memory?");
		gpk_necall(::gpk::ardellEncode(salted, RSA_ARDELL_KEY, salt, filtered), "Failed to encode Ardell. %s", "Out of memory?");
	}
	::gpk::view_array<ubyte_t>						filter_view							= {(ubyte_t*)filtered.begin(), filtered.size()};
	::gpcFilter0Apply(filter_view);
	gpk_necall(::gpk::rsaEncode({(const byte_t*)filter_view.begin(), filter_view.size()}, n, key, testkey, encrypted), "%s", "Out of memory?");
	if(testkey) {
		::gpk::array_pod<byte_t> decryptTest = {};
		::gpk::rsaDecode(encrypted, n, testkey, decryptTest);
		gerror_if(decryptTest.size() != filter_view.size(), "%s", "Error!");
		for(uint32_t iTest =0; iTest < decryptTest.size(); ++iTest) {
			gerror_if(decryptTest[iTest] != (byte_t)filter_view[iTest], "%s", "Error!");
		}
	}
	filter_view									= {(ubyte_t*)&encrypted[offset], (encrypted.size() - offset) * (uint32_t)sizeof(uint64_t)};
	::gpcFilter1Apply(filter_view);
	return (::gpk::error_t)i;
}

// function to decrypt the message
		::gpk::error_t						gpk::gpcDecode						(const ::gpk::view_array<const uint64_t> & encrypted, uint64_t n, uint64_t key, bool salt, ::gpk::array_pod<byte_t>& decrypted) {
	uint32_t										offset								= decrypted.size();
	uint32_t										i									= 0;
	::gpk::array_pod<uint64_t>						defiltered							(encrypted);
	::gpk::view_array<ubyte_t>						defilter_view						= {(ubyte_t*)defiltered.begin(), defiltered.size() * (uint32_t)sizeof(uint64_t)};
	::gpcFilter1Remove(defilter_view);
	gpk_necall(::gpk::rsaDecode(defiltered, n, key, decrypted), "Failed to decode RSA. %s", "Out of memory?");
	::gpk::view_array<ubyte_t>						filter_view							({(ubyte_t*)&decrypted[offset], decrypted.size() - offset});
	::gpcFilter0Remove(filter_view);
	::gpk::array_pod<byte_t>						defiltered2							= {};
	gpk_necall(::gpk::ardellDecode({&decrypted[offset], decrypted.size() - offset}, RSA_ARDELL_KEY, salt, defiltered2), "Failed to decode Ardell. %s", "Out of memory?");
	if(false == salt) {
		gpk_necall(decrypted.resize(offset + defiltered2.size() + 1), "%s", "Out of memory?");
		for(uint32_t j=0; j < defiltered2.size(); ++j)
			decrypted[offset + j]						= defiltered2[j];
	}
	else {
		::gpk::array_pod<byte_t>						unsalted;
		gpk_necall(::gpk::saltDataUnsalt(defiltered2, unsalted), "Failed to remove salt. %s", "Out of memory?");;
		gpk_necall(decrypted.resize(offset + unsalted.size() + 1), "%s", "Out of memory?");
		for(uint32_t j=0; j < unsalted.size(); ++j)
			decrypted[offset + j]						= unsalted[j];
	}
	decrypted[decrypted.size() - 1]				= 0;
	decrypted.resize(decrypted.size() - 1);
	return (::gpk::error_t)i;
}

		::gpk::error_t						gpk::gpcDecodeWithHash				(const ::gpk::view_array<const uint64_t	> & encrypted, uint64_t n, uint64_t key, bool salt, ::gpk::array_pod<byte_t	>& decrypted)	{
	const uint32_t									actualSize							= encrypted.size() - 1;
	const uint64_t									posthash							= encrypted[actualSize];
	uint64_t										checkposthash						= 0;
	for(uint32_t i = 0, maxEncrypted = encrypted.size() - 2; i < maxEncrypted; ++i)
		checkposthash								+= ::gpk::noise1DBase(i * encrypted[i] + ::gpk::noise1DBase(encrypted[i + 1]), ::gpk::NOISE_SEED);
	ree_if(posthash != checkposthash, "%s", "Failed to check post hash. posthash: %llx == checkposthash: %llx", posthash, checkposthash);

	const uint32_t									decryptedStart						= decrypted.size();

	::gpk::gpcDecode({encrypted.begin(), actualSize}, n, key, salt, decrypted);
	const uint64_t									hash								= *(uint64_t*)&decrypted[decrypted.size() - sizeof(uint64_t)];
	verbose_printf("prehash found: %llx. Size of decrypted data: %u. Size of data without hash appended: %u.", hash, decrypted.size(), decrypted.size() - sizeof(uint64_t));

	uint64_t										checkhash							= 0;
	gpk_necall(decrypted.resize(decrypted.size() - sizeof(uint64_t)), "%s", "Out of memory?");
	for(uint32_t i = decryptedStart; i < decrypted.size() - 1; ++i) {
		const uint64_t									hashedChar							= ::gpk::noise1DBase((i - (uint64_t)decryptedStart) * decrypted[i] + ::gpk::noise1DBase(decrypted[i + 1]), ::gpk::NOISE_SEED);
		checkhash									+= hashedChar;
		//info_printf("Check Hashing char #%u: 0x%X '%c' (+1: 0x%X '%c') (0x%llx).", i, decrypted[i], decrypted[i], decrypted[i+1], decrypted[i+1], hashedChar);
	}
	ree_if(hash != checkhash, "Failed to check pre hash: prehash: %llx == checkprehash: %llx", hash, checkhash);
	return 0;
}

		::gpk::error_t						gpk::gpcEncodeWithHash				(const ::gpk::view_array<const byte_t	> & decrypted, uint64_t n, uint64_t key, uint64_t testkey, bool salt, ::gpk::array_pod<uint64_t	>& encrypted)	{
	::gpk::array_pod<byte_t>						prehashed, posthashed;
	prehashed.resize(decrypted.size() + sizeof(uint64_t));
	// -- Calculate the hash
	uint64_t										& hash								= *(uint64_t*)&prehashed[decrypted.size()];
	hash										= 0;
	for(uint32_t i = 0; i < decrypted.size(); ++i)
		prehashed[i]								= decrypted[i];

	for(uint32_t i = 0; i < decrypted.size() - 1; ++i) {
		const uint64_t									hashedChar							= ::gpk::noise1DBase(i * (uint64_t)decrypted[i] + ::gpk::noise1DBase(decrypted[i + 1]), ::gpk::NOISE_SEED);
		hash										+= hashedChar;
		//info_printf("Hashing char #%u: 0x%X '%c' (+1: 0x%X '%c') (0x%llx).", i, decrypted[i], decrypted[i], decrypted[i+1], decrypted[i+1], hashedChar);
	}
	verbose_printf("prehash: %llx (%llx). Size of original data: %u.Size of data with hash appended: %u.", *((uint64_t*)&prehashed[prehashed.size() - sizeof(uint64_t)]), *((uint64_t*)&prehashed[decrypted.size()]), decrypted.size(), prehashed.size());

	const uint32_t									encryptedStart						= encrypted.size();
	::gpk::gpcEncode(prehashed, n, key, testkey, salt, encrypted);
	if(testkey) {
		::gpk::array_pod<byte_t>						decryptTest							= {};
		::gpk::gpcDecode(encrypted, n, testkey, salt, decryptTest);
		gerror_if(decryptTest.size() != prehashed.size(), "%s", "Error!");
		for(uint32_t i =0; i < decryptTest.size(); ++i) {
			gerror_if(decryptTest[i] != prehashed[i], "%s", "Error!");
		}
	}

	uint64_t										posthash							= 0;
	for(uint32_t i = encryptedStart, maxEncrypted = encrypted.size() - 1; i < maxEncrypted; ++i)
		posthash									+= ::gpk::noise1DBase((i - encryptedStart) * encrypted[i] + ::gpk::noise1DBase(encrypted[i + 1]), ::gpk::NOISE_SEED);

	gpk_necall(encrypted.push_back(posthash), "%s", "Out of memory?");
	verbose_printf("posthash: %llx.", posthash);
	return 0;
}
