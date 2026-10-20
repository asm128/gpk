#include "gpk_circuit.h"

// R1 = 100kΩ (top)
// R2 = 100kΩ (bottom)
// Vout = Vin * R2 / (R1 + R2)
::gpk::err_t gpk::circuitVoltageDivider(::gpk::u2_t milliVIn, ::gpk::u3_t milliOhmR1, ::gpk::u3_t milliOhmR2, ::gpk::u2_t & milliVOut) {
	if_true_fe(0 == milliOhmR1 && 0 == milliOhmR2);
	gpk::u3_c milliOhmR = milliOhmR1 + milliOhmR2;
	milliVOut = ::gpk::u2_t(milliVIn * milliOhmR2 / milliOhmR);
	rtrn (milliOhmR1 && milliOhmR2) ? 0 
		: milliOhmR1 ? 1 
		: 2
		; 
}

// RT = R1 * R2 / (R1 + R2)
::gpk::err_t gpk::circuitParallelResistor	(::gpk::u3_t milliOhmR1, ::gpk::u3_t milliOhmR2, ::gpk::u3_t & milliOhmTotal) {
	if_true_vw(err_t(milliOhmTotal = 0) | -1, 0 == milliOhmR1 || 0 == milliOhmR2);
	rtrn err_t(milliOhmTotal = milliOhmR1 * milliOhmR2 / (milliOhmR1 + milliOhmR2));
}

// R = (Vsrc - Vdrop) / I
::gpk::err_t gpk::circuitResistorCalc		(::gpk::u2_t milliAmp, ::gpk::u2_t milliVSource, ::gpk::u2_t milliVDrop, ::gpk::u3_t & milliOhm) {
	if_true_block_log(warning_printf, 0 == milliAmp, {
		rtrn err_t(milliOhm = (u3_t)-1);
	});
	if_true_block_logf(warning_printf, milliVSource <= milliVDrop, {
		rtrn err_t(milliOhm = 0);
	}, "milliVSource(%u) <= milliVDrop(%u)", milliVSource, milliVDrop);
	rtrn err_t(milliOhm = 1000ULL * (milliVSource - milliVDrop) / (milliAmp));
}

