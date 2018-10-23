#include "gpk_rsa.h"
#include "gpk_encoding.h"

#define RSA_ARDELL_KEY	987654321

static	int									primalityTest						(uint64_t number)						{
	uint64_t										j									= (uint64_t)sqrt((double)number);
	if(number > 1 && number < 4)
		return 1;
	if(0 == number % 2)
		return 0;
	for(uint64_t i = 5; i <= j; i += 2) {
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
				keys[k].Public						= i;
			else
				keys.push_back({i});

			uint64_t										cd									= ::commonDivisor(g_SRSA, keys[(uint32_t)k].Public);
			if(cd > 0) {
				keys[k].Private								= cd;
				break;
				//++k;
			}
			else 
				if(k == limit)
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

static	::gpk::error_t						rsaFilterSub2	(::gpk::view_array<ubyte_t>& scanline, uint32_t bpp) { 
	::gpk::array_pod<ubyte_t>						filtered;
	filtered.resize(scanline.size());
	for(int32_t iByte = scanline.size() - bpp - 1; iByte >= 0; --iByte) 
		filtered[iByte]								= scanline[iByte] - scanline[iByte + bpp];
	for(uint32_t iByte = 0; iByte < scanline.size() - bpp; ++iByte) 
		scanline[iByte]								= filtered[iByte];	
	return 0; 
}

static	::gpk::error_t						rsaDefilterSub2	(::gpk::view_array<ubyte_t>& scanline, uint32_t bpp) { 
	for(int32_t iByte = scanline.size() - bpp - 1; iByte >= 0; --iByte) 
		scanline[iByte] += scanline[iByte + bpp];	
	return 0; 
}

//function to encrypt the message
::gpk::error_t								gpk::rsaEncode						(::gpk::SRSA & g_SRSA, ::gpk::view_array<const byte_t> decrypted, uint64_t key, ::gpk::array_pod<uint64_t>& encrypted) {
	uint32_t										offset								= encrypted.size();
	uint32_t										i									= 0;
	encrypted.resize(offset + decrypted.size());
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
::gpk::error_t								gpk::rsaDecode						(::gpk::SRSA & g_SRSA, ::gpk::view_const_uint64 encrypted, uint64_t key, ::gpk::array_pod<char>& decrypted) {
	uint32_t										offset								= decrypted.size();
	uint32_t										i									= 0;
	decrypted.resize(offset + encrypted.size() + 1);
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
::gpk::error_t								gpk::rsaSafeEncode						(::gpk::SRSA & g_SRSA, ::gpk::view_array<const byte_t> decrypted, uint64_t key, bool salt, ::gpk::array_pod<uint64_t>& encrypted) {
	salt;																																		  
	uint32_t										offset								= encrypted.size();
	uint32_t										i									= 0;
	::gpk::array_pod<byte_t>						filtered;//							(decrypted);
	::gpk::ardellEncode(decrypted, RSA_ARDELL_KEY, salt, filtered);

	::gpk::view_array<ubyte_t>						filter_view							({(ubyte_t*)filtered.begin(), filtered.size()});
	::rsaFilterSub(filter_view, 1U);
	::rsaFilterSub2(filter_view, 1U);

	::gpk::rsaEncode(g_SRSA, {(byte_t*)filter_view.begin(), filter_view.size()}, key, encrypted);

	filter_view									= {(ubyte_t*)&encrypted[offset], (encrypted.size() - offset) * sizeof(uint64_t)};
	::rsaFilterSub2(filter_view, 1U);
	::rsaFilterSub(filter_view, 1U);
	return i;
}

// function to decrypt the message
::gpk::error_t								gpk::rsaSafeDecode						(::gpk::SRSA & g_SRSA, ::gpk::view_const_uint64 encrypted, uint64_t key, bool salt, ::gpk::array_pod<char>& decrypted) {
	salt;
	uint32_t										offset								= decrypted.size();
	uint32_t										i									= 0;
	::gpk::array_pod<uint64_t>						defiltered							(encrypted);
	::gpk::view_array<ubyte_t>						defilter_view						= {(ubyte_t*)defiltered.begin(), defiltered.size() * sizeof(uint64_t)};
	::rsaDefilterSub(defilter_view, 1U);
	::rsaDefilterSub2(defilter_view, 1U);

	::gpk::rsaDecode(g_SRSA, defiltered, key, decrypted);

	::gpk::view_array<ubyte_t>						filter_view							({(ubyte_t*)&decrypted[offset], decrypted.size() - offset});
	::rsaDefilterSub2(filter_view, 1U);
	::rsaDefilterSub(filter_view, 1U);

	::gpk::array_pod<byte_t>						defiltered2							= {};
	::gpk::ardellDecode({&decrypted[offset], decrypted.size() - offset}, RSA_ARDELL_KEY, salt, defiltered2);

	for(uint32_t j=0; j < defiltered2.size(); ++j)
		decrypted[offset + j]						= defiltered2[j];

	decrypted.resize(offset + defiltered2.size() + 1);
	decrypted[decrypted.size() - 1]				= 0;
	decrypted.resize(decrypted.size() - 1);
	return (int)i;
}
