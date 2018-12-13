#include "gpk_sync.h"
#include "gpk_array.h"
#include "gpk_timer.h"
#include <cstdint>

static	int											primalityTest						(uint64_t number)						{
	uint64_t												j									= (uint64_t)sqrt((double)number);
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

int WINAPI											WinMain						(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nShowCmd) { hInstance, hPrevInstance, lpCmdLine, nShowCmd;
	::gpk::array_pod<uint64_t>								primes;
	uint64_t												maxPrime					= 0xFFFFFU;
	uint64_t												totalPrimes					= 0;
	::gpk::STimer											timer;
	for(uint32_t i = 0; i < maxPrime; ++i) {
		if(::primalityTest(i)) {
			primes.push_back(i);
			++totalPrimes;
		}
	}
	timer.Frame();
	always_printf("Primes found in %g seconds: %u of %llu (%g%%).", timer.LastTimeSeconds, primes.size(), maxPrime, (primes.size() / (float)maxPrime) * 100);
	for(uint32_t i = 3000; i < primes.size(); ++i)
		always_printf("Prime found: %llu.", primes[i]);
	return 0;
}
