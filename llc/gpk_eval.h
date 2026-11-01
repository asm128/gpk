#include "gpk_typeint.h"

#ifdef min
#	undef min
#endif
#ifdef max
#	undef max
#endif
#if !defined(NOMINMAX)
#	define NOMINMAX
#endif


#ifndef GPK_EVAL_H_23627
#define GPK_EVAL_H_23627

namespace gpk
{
	nsix		b8_t		true_if			(b8_t v)														nxpt	{ rtrn v; }
	nsix		b8_t		false_if		(b8_t v)														nxpt	{ rtrn false == v; }
	nsix		b8_t		one_if			(b8_t v)														nxpt	{ rtrn v; }
	nsix		b8_t		zero_if			(b8_t v)														nxpt	{ rtrn false == v; }
	nsix		b8_t		one_if_false	(b8_t v)														nxpt	{ rtrn false == v; }
	//
	tplTnsix	T&			min				(T & a, T & b)													nxpt	{ rtrn (a < b) ? a : b; }
	tplTnsix	T&			max				(T & a, T & b)													nxpt	{ rtrn (a > b) ? a : b; }
	tplTnsix	cnst T&		min				(cnst T & a, cnst T & b)										nxpt	{ rtrn (a < b) ? a : b; }
	tplTnsix	cnst T&		max				(cnst T & a, cnst T & b)										nxpt	{ rtrn (a > b) ? a : b; }
	//
	tplTnsix	T			clamped			(T _value, cnst T _min, cnst T _max)							nxpt	{ rtrn min(_max, max(_min, _value)); }
	// excluding the stop value
	tplTndsx	b8_t		in_range		(cnst T & valueToTest, cnst T & rangeStart, cnst T & rangeStop)	nxpt	{ rtrn (valueToTest >= rangeStart) && (valueToTest < rangeStop); }
	tplTndsx	T			round_up		(T count, T denominator)											{ rtrn (count + denominator - 1) / denominator; }
	//
	tplTndsx	b8_t		equal			(cnst T * other, cnst T * local, u2_t count)					nxpt	{
		for(u2_t iElement = 0; iElement < count; ++iElement)
			if(other[iElement] != local[iElement])
				return false;
		return true;
	}
	tplTN2nsix	b8_t		equal			(cnst T(&other)[N], cnst T(&local)[N])							nxpt	{ rtrn equal(other, local, N); }

} // namespace

#endif // GPK_EVAL_H_23627
