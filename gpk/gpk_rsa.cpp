#include "gpk_rsa.h"
#include "gpk_encoding.h"

#define RSA_ARDELL_KEY	1054987632

static	int									primalityTest						(uint64_t number)						{
	uint64_t										j									= (uint64_t)sqrt((double)number);
	if(number > 1 && number < 4)
		return 1;
	if(0 == number % 2)
		return 0;
	for(uint64_t i = 3; i <= j; ++i) {
		if(0 == (number % i))
			return 0;
	}
	return 1;
}

uint64_t									commonDivisor						(::gpk::SRSA & g_SRSA, uint64_t a)				{
	uint64_t										k									= 1;
	while(1) {
		k											= k + g_SRSA.t;
		if(k % a == 0)
			return(k / a);
	}
}

// function to generate encryption keys
::gpk::error_t								gpk::rsaKeys						(::gpk::SRSA & g_SRSA, uint64_t limit, ::gpk::array_pod<SRSAKeyPair>& keys)			{
	uint32_t										k									= 0;
	uint64_t										j									= 0;
	for(uint64_t i = 2; i < g_SRSA.t; ++i) {
		if(g_SRSA.t % i == 0)
			continue;
		j											= (uint64_t)sqrt((double)i);
		unsigned int									isPrime								= ::primalityTest(i);
		if(isPrime == 1 && i != g_SRSA.Primes[0] && i != g_SRSA.Primes[1]) {
			if(keys.size() > k)
				keys[k].Public								= i;
			else
				gpk_necall(keys.push_back({i}), "Out of memory?");

			uint64_t										cd									= ::commonDivisor(g_SRSA, keys[(uint32_t)k].Public);
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

::gpk::error_t								gpk::rsaInitialize					(::gpk::SRSA & g_SRSA, uint32_t prime1, uint32_t prime2) {
	ree_if(0 == ::primalityTest(prime1), "Cannot initialize RSA with a number that is not prime: %u.", prime1);
	ree_if(0 == ::primalityTest(prime2), "Cannot initialize RSA with a number that is not prime: %u.", prime2);
	g_SRSA.n									= prime1 * prime2;
	g_SRSA.t									= (prime1 - 1) * (prime2 - 1);
	g_SRSA.j									= (uint64_t)sqrt((double)g_SRSA.t);
	g_SRSA.Primes[0]							= prime1;
	g_SRSA.Primes[1]							= prime2;
	return 0;
}

static	::gpk::error_t						rsaFilterSub	(::gpk::view_array<ubyte_t>& scanline, uint32_t bpp) { 
	::gpk::array_pod<ubyte_t>						filtered;
	filtered.resize(scanline.size());
	for(uint32_t iByte = bpp; iByte < scanline.size(); ++iByte) 
		filtered[iByte]								= scanline[iByte] - scanline[iByte - bpp];	
	for(uint32_t iByte = bpp; iByte < scanline.size(); ++iByte) 
		scanline[iByte]								= filtered[iByte];	
	return 0; 
}

static	::gpk::error_t						rsaDefilterSub	(::gpk::view_array<ubyte_t>& scanline, uint32_t bpp) { 
	for(uint32_t iByte = bpp; iByte < scanline.size(); ++iByte) 
		scanline[iByte] += scanline[iByte - bpp];	
	return 0; 
}

static	::gpk::error_t						rsaDefilterSub2	(::gpk::view_array<ubyte_t>& scanline, uint32_t bpp) { 
	for(int32_t iByte = scanline.size() - bpp - 1; iByte >= 0; --iByte) 
		scanline[iByte] += scanline[iByte + bpp];	
	return 0; 
}

static	::gpk::error_t						rsaFilterSub2	(::gpk::view_array<ubyte_t>& scanline, uint32_t bpp) { 
	::gpk::array_pod<ubyte_t>						filtered;
	filtered.resize(scanline.size());
	for(int32_t iByte = scanline.size() - bpp - 1; iByte >= 0; --iByte) 
		filtered[iByte]								= scanline[iByte] - scanline[iByte + bpp];
	for(uint32_t iByte = 0; iByte < scanline.size() - bpp; ++iByte) 
		scanline[iByte]								= filtered[iByte];	
	return 0; 
}

//function to encrypt the message
::gpk::error_t								gpk::rsaEncode						(::gpk::SRSA & g_SRSA, const ::gpk::view_array<const byte_t> & decrypted, uint64_t key, ::gpk::array_pod<uint64_t>& encrypted) {
	uint32_t										offset								= encrypted.size();
	uint32_t										i									= 0;
	gpk_necall(encrypted.resize(offset + decrypted.size()), "Out of memory?");
	while(i < decrypted.size()) {			
		uint64_t										pt									= (ubyte_t)decrypted[i];
		uint64_t										k									= 1;
		for(uint64_t j = 0; j < key; ++j) {
			k											= k * pt;
			k											= k % g_SRSA.n;
		}			
		encrypted[offset + i]						= k;
		++i;
	}
	return i;
}

// function to decrypt the message
::gpk::error_t								gpk::rsaDecode						(::gpk::SRSA & g_SRSA, const ::gpk::view_array<const uint64_t> & encrypted, uint64_t key, ::gpk::array_pod<char>& decrypted) {
	uint32_t										offset								= decrypted.size();
	uint32_t										i									= 0;
	gpk_necall(decrypted.resize(offset + encrypted.size() + 1), "%s", "Out of memory?");
	while(i < encrypted.size()) {
		uint64_t										ct									= encrypted[i];
		uint64_t										k									= 1;
		for(uint64_t j = 0; j < key; ++j) {
			k											= k * ct;
			k											= k % g_SRSA.n;
		}
		decrypted[offset + i]			= (char)k;
		++i;
	}
	decrypted[decrypted.size() - 1]				= 0;
	decrypted.resize(decrypted.size() - 1);
	return (int)i;
}

//function to encrypt the message
::gpk::error_t								gpk::gpcEncode						(::gpk::SRSA & g_SRSA, const ::gpk::view_array<const byte_t> & decrypted, uint64_t key, bool salt, ::gpk::array_pod<uint64_t>& encrypted) {
	uint32_t										offset								= encrypted.size();
	uint32_t										i									= 0;

	::gpk::array_pod<byte_t>						filtered;
	if(false == salt) 		
		gpk_necall(::gpk::ardellEncode(decrypted, RSA_ARDELL_KEY, salt, filtered), "Failed to encode Ardell. %s", "Out of memory?");
	else {
		::gpk::array_pod<byte_t>						salted;
		gpk_necall(::gpk::saltDataSalt(decrypted, salted), "Failed to add salt. %s", "Out of memory?");
		gpk_necall(::gpk::ardellEncode(salted, RSA_ARDELL_KEY, salt, filtered), "Failed to encode Ardell. %s", "Out of memory?");
	}

	::gpk::view_array<ubyte_t>						filter_view							= {(ubyte_t*)filtered.begin(), filtered.size()};
	gpk_necall(::rsaFilterSub	(filter_view, 1U), "%s", "Out of memory?");
	gpk_necall(::rsaFilterSub2	(filter_view, 1U), "%s", "Out of memory?");

	gpk_necall(::gpk::rsaEncode(g_SRSA, {(byte_t*)filter_view.begin(), filter_view.size()}, key, encrypted), "%s", "Out of memory?");

	filter_view									= {(ubyte_t*)&encrypted[offset], (encrypted.size() - offset) * sizeof(uint64_t)};
	gpk_necall(::rsaFilterSub2	(filter_view, 1U), "%s", "Out of memory?");
	gpk_necall(::rsaFilterSub	(filter_view, 1U), "%s", "Out of memory?");
	return i;
}

// function to decrypt the message
::gpk::error_t								gpk::gpcDecode						(::gpk::SRSA & g_SRSA, const ::gpk::view_array<const uint64_t> & encrypted, uint64_t key, bool salt, ::gpk::array_pod<byte_t>& decrypted) {
	uint32_t										offset								= decrypted.size();
	uint32_t										i									= 0;
	::gpk::array_pod<uint64_t>						defiltered							(encrypted);
	::gpk::view_array<ubyte_t>						defilter_view						= {(ubyte_t*)defiltered.begin(), defiltered.size() * sizeof(uint64_t)};
	::rsaDefilterSub(defilter_view, 1U);
	::rsaDefilterSub2(defilter_view, 1U);

	gpk_necall(::gpk::rsaDecode(g_SRSA, defiltered, key, decrypted), "Failed to decode RSA. %s", "Out of memory?");

	::gpk::view_array<ubyte_t>						filter_view							({(ubyte_t*)&decrypted[offset], decrypted.size() - offset});
	::rsaDefilterSub2(filter_view, 1U);
	::rsaDefilterSub(filter_view, 1U);

	::gpk::array_pod<byte_t>						defiltered2							= {};
	gpk_necall(::gpk::ardellDecode({&decrypted[offset], decrypted.size() - offset}, RSA_ARDELL_KEY, salt, defiltered2), "Failed to decode Ardell. %s", "Out of memory?");
	if(false == salt) {
		for(uint32_t j=0; j < defiltered2.size(); ++j)
			decrypted[offset + j]						= defiltered2[j];
		gpk_necall(decrypted.resize(offset + defiltered2.size() + 1), "%s", "Out of memory?");
	}
	else {
		::gpk::array_pod<byte_t>						unsalted;
		gpk_necall(::gpk::saltDataUnsalt(defiltered2, unsalted), "Failed to remove salt. %s", "Out of memory?");;
		for(uint32_t j=0; j < unsalted.size(); ++j)
			decrypted[offset + j]						= unsalted[j];
		gpk_necall(decrypted.resize(offset + unsalted.size() + 1), "%s", "Out of memory?");
	}
	decrypted[decrypted.size() - 1]				= 0;
	decrypted.resize(decrypted.size() - 1);
	return (int)i;
}
