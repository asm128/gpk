#include "gpk_sync.h"
#include "gpk_array.h"
#include <cstdint>


static int											primalityTest				(uint64_t number)						{
	uint64_t												j							= (uint64_t)sqrt((double)number);
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

int WINAPI											WinMain						(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nShowCmd) { hInstance, hPrevInstance, lpCmdLine, nShowCmd;
	::gpk::array_pod<uint64_t>								primes;
	uint64_t												maxPrime					= 0xFFFFFFFU;
	uint64_t												totalPrimes					= 0;
	for(uint32_t i = 0; i < maxPrime; ++i) {
		if(::primalityTest(i)) {
			primes.push_back(i);
			++totalPrimes;
		}
	}
	always_printf("Primes found: %u of %u (%f%%).", primes.size(), maxPrime, (primes.size() / (float)maxPrime) * 100);
	return 0;
}