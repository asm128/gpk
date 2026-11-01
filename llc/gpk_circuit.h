#include "llc_log.h"

#ifndef LLC_CIRCUIT_H
#define LLC_CIRCUIT_H

namespace llc 
{
	::llc::err_t circuitResistorCalc		(::llc::u2_t milliAmp, ::llc::u2_t milliVSource, ::llc::u2_t milliVDrop, ::llc::u3_t & milliOhm);
	::llc::err_t circuitParallelResistor	(::llc::u3_t milliOhmR1, ::llc::u3_t milliOhmR2, ::llc::u3_t & milliOhmTotal);
	//::llc::err_t circuitVoltageDrop		(::llc::u2_t milliAmp, ::llc::u3_t milliOhmR1, ::llc::u2_t & milliVDrop);
	::llc::err_t circuitVoltageDivider		(::llc::u2_t milliVIn, ::llc::u3_t milliOhmR1, ::llc::u3_t milliOhmR2, ::llc::u2_t & milliVOut);
} // namespace 

#endif // LLC_CIRCUIT_H
