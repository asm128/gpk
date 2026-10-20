#include "gpk_log.h"

#ifndef GPK_CIRCUIT_H
#define GPK_CIRCUIT_H

namespace gpk 
{
	::gpk::err_t circuitResistorCalc		(::gpk::u2_t milliAmp, ::gpk::u2_t milliVSource, ::gpk::u2_t milliVDrop, ::gpk::u3_t & milliOhm);
	::gpk::err_t circuitParallelResistor	(::gpk::u3_t milliOhmR1, ::gpk::u3_t milliOhmR2, ::gpk::u3_t & milliOhmTotal);
	//::gpk::err_t circuitVoltageDrop		(::gpk::u2_t milliAmp, ::gpk::u3_t milliOhmR1, ::gpk::u2_t & milliVDrop);
	::gpk::err_t circuitVoltageDivider		(::gpk::u2_t milliVIn, ::gpk::u3_t milliOhmR1, ::gpk::u3_t milliOhmR2, ::gpk::u2_t & milliVOut);
} // namespace 

#endif // GPK_CIRCUIT_H


