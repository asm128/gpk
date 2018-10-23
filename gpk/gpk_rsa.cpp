#include "gpk_rsa.h"

struct SRSA {
	uint64_t									n = 0, t = 0, j			= 0;
	uint32_t									Primes[2]				= {};
	::gpk::array_pod<uint64_t>					KeyPublic, KeyPrivate	= {};	//, temp;
};

int											primalityTest						(SRSA & g_SRSA, uint64_t number)			{
	g_SRSA.j										= (uint64_t)sqrt((double)number);
	if(number > 1 && number < 4)
		return 1;
	if(0 == number % 2)
		return 0;
	for(uint64_t i = 5; i <= g_SRSA.j; i += 2) {
		if(0 == (number % i))
			return 0;
	}
	return 1;
}


uint64_t									commonDivisor						(SRSA & g_SRSA, uint64_t a)				{
	uint64_t										k									= 1;
	while(1) {
		k											= k + g_SRSA.t;
		if(k % a == 0)
			return(k / a);
	}
}

// function to generate encryption keys
uint64_t									encryption_key						(SRSA & g_SRSA, uint64_t limit)			{
	uint64_t										k									= 0;
	for(uint64_t i = 2; i < g_SRSA.t; ++i) {
		if(g_SRSA.t % i == 0)
			continue;
		unsigned int									isPrime								= primalityTest(g_SRSA, i);
		if(isPrime == 1 && i != g_SRSA.Primes[0] && i != g_SRSA.Primes[1]) {
			if(g_SRSA.KeyPublic.size() > k)
				g_SRSA.KeyPublic[(uint32_t)k]									= i;
			else
				g_SRSA.KeyPublic.push_back(i);

			uint64_t										cd									= commonDivisor(g_SRSA, g_SRSA.KeyPublic[(uint32_t)k]);
			if(cd > 0) {
				g_SRSA.KeyPrivate.push_back(cd);
				break;
				//++k;
			}
			else 
				if(k == limit)
					break;
		}
	}
	return 0;
}

//function to encrypt the message
int											rsaEncode							(SRSA & g_SRSA, const char* decrypted, unsigned int messageSize, ::gpk::array_pod<uint64_t>& encrypted) {
	unsigned int									offset								= encrypted.size();
	encrypted.resize(offset + messageSize);
	uint64_t										key									= g_SRSA.KeyPublic[0];
	uint64_t										i									= 0;
	while(i < messageSize) {			
		uint64_t										pt									= (unsigned char)decrypted[i];
		//unsigned int									pt									= pt - 96;
		uint64_t										k									= 1;
		for(g_SRSA.j = 0; g_SRSA.j < key; ++g_SRSA.j) {
			k											= k * pt;
			k											= k % g_SRSA.n;
		}			
		//encrypted[offset + i]						= k + 96;
		encrypted[(uint32_t)(offset + i)]			= k;
		++i;
	}
	return (int)i;
}

// function to decrypt the message
int											rsaDecode							(SRSA & g_SRSA, const uint64_t* encrypted, unsigned int messageSize, ::gpk::array_pod<char>& decrypted) {
	unsigned int									offset								= decrypted.size();
	decrypted.resize(offset + messageSize + 1);

	uint64_t										key									= g_SRSA.KeyPrivate[0];
	uint64_t										i									= 0;

	while(i < messageSize) {
		//int ct = encrypted[i] - 96; 
		uint64_t										ct									= encrypted[i];
		uint64_t										k									= 1;
		for(g_SRSA.j = 0; g_SRSA.j < key; ++g_SRSA.j) {
			k											= k * ct;
			k											= k % g_SRSA.n;
		}
		decrypted[(uint32_t)(offset + i)]			= (char)k;
		//decrypted[offset + i] = (char)k + 96;
		++i;
	}
	decrypted[decrypted.size() - 1]				= 0;
	decrypted.resize(decrypted.size() - 1);
	return (int)i;
}
