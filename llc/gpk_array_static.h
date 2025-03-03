#include "gpk_cpow.h"
#include "gpk_view.h"

#ifndef GPK_ARRAY_STATIC_H_23627
#define GPK_ARRAY_STATIC_H_23627

namespace gpk
{
#pragma pack(push, 1)
	tplt<tpnm _tVal, uint32_t _elementCount>
	struct array_static	{
		stacxpr	const uint32_t	SIZE	= _elementCount;

		typedef	_tVal					T;
		typedef array_static<T, SIZE>	TAStatic;
		typedef	view<const T>			TConstView;
		typedef	view<T>					TView;

		T						Storage	[SIZE]	;

		operator				view<T>			()															{ return {Storage, SIZE}; }
		operator				view<const T>	()										const	noexcept	{ return {Storage, SIZE}; }

		inline	const T&		operator[]		(uint32_t index)						const				{ gthrow_if(index >= SIZE, GPK_FMT_U32_GE_U32, (int32_t)index, (int32_t)SIZE); return Storage[index]; }
		inline	T&				operator[]		(uint32_t index)											{ gthrow_if(index >= SIZE, GPK_FMT_U32_GE_U32, (int32_t)index, (int32_t)SIZE); return Storage[index]; }
		GPK_DEFAULT_OPERATOR(TAStatic
			, (this->size() != other.size()) ? false
			: (this->begin() == other.begin()) ? true
			: ::gpk::equal(other.begin(), this->begin(), this->size())
			);
		//bool					operator!=		(const ::gpk::view<const T>& other)		const				{ return !operator==(other); }
		//bool					operator==		(const ::gpk::view<const T>& other)		const				{
		//	return (this->size() != other.size()) ? false
		//		: (this->begin() == other.begin()) ? true
		//		: ::gpk::equal(other.begin(), this->begin(), this->size())
		//		;
		//}
		// Methods
		inlcxpr	uint32_t		byte_count		()										const	noexcept	{ return uint32_t(SIZE * sizeof(T));	}
		inlcxpr	uint32_t		bit_count		()										const	noexcept	{ return byte_count() * 8U;	}

		inlcxpr	view<uint8_t>	u0_t				()												noexcept	{ return {(uint8_t*)Storage, byte_count()};		}
		inline	view<u0_c>		u0_t				()										const	noexcept	{ return {(const uint8_t*)Storage, byte_count()};	}
		inlcxpr	view<u0_c>		cu8				()										const	noexcept	{ return {(const uint8_t*)Storage, byte_count()};	}
		inline	view<sc_t>		c				()												noexcept	{ return {(sc_t*)Storage, byte_count()};			}
		inlcxpr	view<sc_c>		cc				()										const	noexcept	{ return {(const sc_t*)Storage, byte_count()};		}

		inlcxpr	const uint32_t&	size			()										const	noexcept	{ return SIZE; }

		inline	T*				begin			()												noexcept	{ return Storage;			}
		inline	T*				end				()												noexcept	{ return Storage + SIZE;	}
		inlcxpr	const T*		begin			()										const	noexcept	{ return Storage;			}
		inlcxpr	const T*		end				()										const	noexcept	{ return Storage + SIZE;	}

		::gpk::error_t			slice			(TView & out, uint32_t offset, uint32_t count = (uint32_t)-1)				{
			ree_if(offset > SIZE, GPK_FMT_U32_GT_U32, offset, (uint32_t)SIZE);
			const uint32_t				newSize			= SIZE - offset;
			if(count != (uint32_t)-1)
				ree_if(count > newSize, GPK_FMT_U32_GT_U32, count, (uint32_t)newSize);
			out						= {&Storage[offset], ::gpk::min(newSize, count)};
			return out.size();
		}
		::gpk::error_t			slice			(TConstView & out, uint32_t offset, uint32_t count = (uint32_t)-1)	const	{
			ree_if(offset > SIZE, GPK_FMT_U32_GT_U32, offset, (uint32_t)SIZE);
			const uint32_t				newSize			= SIZE - offset;
			if(count != (uint32_t)-1)
				ree_if(count > newSize, GPK_FMT_U32_GT_U32, count, (uint32_t)newSize);
			out						= {&Storage[offset], ::gpk::min(newSize, count)};
			return out.size();
		}
	};
#pragma pack(pop)

	tplt<tpnm T, uint32_t _elementCount>
	using astatic	= ::gpk::array_static<T, _elementCount>;

	tplt<uint32_t _count>	using astvcs	= ::gpk::astatic<::gpk::vcs	, _count>; tplt<uint32_t _count>	using astaticvcs	= ::gpk::astvcs	<_count>;
	tplt<uint32_t _count>	using astvs		= ::gpk::astatic<::gpk::vs	, _count>; tplt<uint32_t _count>	using astaticvs		= ::gpk::astvs	<_count>;

	tplt<uint32_t _count>	using astc		= ::gpk::astatic<sc_t     , _count>; tplt<uint32_t _count>	using astaticc		= ::gpk::astc	<_count>; tplt<uint32_t _count>	using astchar	= ::gpk::astatic<sc_t    , _count>;
	tplt<uint32_t _count>	using astuc 	= ::gpk::astatic<uc_t  , _count>; tplt<uint32_t _count>	using astaticuc 	= ::gpk::astuc 	<_count>; tplt<uint32_t _count>	using astuchar	= ::gpk::astatic<uc_t , _count>;
	tplt<uint32_t _count>	using astu8 	= ::gpk::astatic<uint8_t  , _count>; tplt<uint32_t _count>	using astaticu8 	= ::gpk::astu8 	<_count>;
	tplt<uint32_t _count>	using astu16	= ::gpk::astatic<uint16_t , _count>; tplt<uint32_t _count>	using astaticu16	= ::gpk::astu16	<_count>;
	tplt<uint32_t _count>	using astu32	= ::gpk::astatic<uint32_t , _count>; tplt<uint32_t _count>	using astaticu32	= ::gpk::astu32	<_count>;
	tplt<uint32_t _count>	using astu64	= ::gpk::astatic<uint64_t , _count>; tplt<uint32_t _count>	using astaticu64	= ::gpk::astu64	<_count>;
	tplt<uint32_t _count>	using asti8 	= ::gpk::astatic<int8_t   , _count>; tplt<uint32_t _count>	using astatici8 	= ::gpk::asti8 	<_count>;
	tplt<uint32_t _count>	using asti16	= ::gpk::astatic<int16_t  , _count>; tplt<uint32_t _count>	using astatici16	= ::gpk::asti16	<_count>;
	tplt<uint32_t _count>	using asti32	= ::gpk::astatic<int32_t  , _count>; tplt<uint32_t _count>	using astatici32	= ::gpk::asti32	<_count>;
	tplt<uint32_t _count>	using asti64	= ::gpk::astatic<int64_t  , _count>; tplt<uint32_t _count>	using astatici64	= ::gpk::asti64	<_count>;
	tplt<uint32_t _count>	using astf32	= ::gpk::astatic<f2_t, _count>; tplt<uint32_t _count>	using astaticf32	= ::gpk::astf32	<_count>;
	tplt<uint32_t _count>	using astf64	= ::gpk::astatic<f3_t, _count>; tplt<uint32_t _count>	using astaticf64	= ::gpk::astf64	<_count>;

	tplt<uint32_t _count>	using astvcc	= ::gpk::astatic<::gpk::vcc		, _count>; tplt<uint32_t _count>	using astaticvcc	= ::gpk::astvcc  <_count>;
	tplt<uint32_t _count>	using astvcuc 	= ::gpk::astatic<::gpk::vcuc 	, _count>; tplt<uint32_t _count>	using astaticvcuc 	= ::gpk::astvcuc <_count>;
	tplt<uint32_t _count>	using astvcu8 	= ::gpk::astatic<::gpk::vcu8 	, _count>; tplt<uint32_t _count>	using astaticvcu8 	= ::gpk::astvcu8 <_count>;
	tplt<uint32_t _count>	using astvcu16	= ::gpk::astatic<::gpk::vcu16	, _count>; tplt<uint32_t _count>	using astaticvcu16	= ::gpk::astvcu16<_count>;
	tplt<uint32_t _count>	using astvcu32	= ::gpk::astatic<::gpk::vcu32	, _count>; tplt<uint32_t _count>	using astaticvcu32	= ::gpk::astvcu32<_count>;
	tplt<uint32_t _count>	using astvcu64	= ::gpk::astatic<::gpk::vcu64	, _count>; tplt<uint32_t _count>	using astaticvcu64	= ::gpk::astvcu64<_count>;
	tplt<uint32_t _count>	using astvci8 	= ::gpk::astatic<::gpk::vci8 	, _count>; tplt<uint32_t _count>	using astaticvci8 	= ::gpk::astvci8 <_count>;
	tplt<uint32_t _count>	using astvci16	= ::gpk::astatic<::gpk::vci16	, _count>; tplt<uint32_t _count>	using astaticvci16	= ::gpk::astvci16<_count>;
	tplt<uint32_t _count>	using astvci32	= ::gpk::astatic<::gpk::vci32	, _count>; tplt<uint32_t _count>	using astaticvci32	= ::gpk::astvci32<_count>;
	tplt<uint32_t _count>	using astvci64	= ::gpk::astatic<::gpk::vci64	, _count>; tplt<uint32_t _count>	using astaticvci64	= ::gpk::astvci64<_count>;
	tplt<uint32_t _count>	using astvcf32	= ::gpk::astatic<::gpk::vcf32	, _count>; tplt<uint32_t _count>	using astaticvcf32	= ::gpk::astvcf32<_count>;
	tplt<uint32_t _count>	using astvcf64	= ::gpk::astatic<::gpk::vcf64	, _count>; tplt<uint32_t _count>	using astaticvcf64	= ::gpk::astvcf64<_count>;

	tplt <tpnm T, size_t nSize>	stincxp	uint32_t	size		(::gpk::astatic<T, nSize> /*viewToTest*/)	noexcept	{ return uint32_t(nSize);				}
	tplt <tpnm T, size_t nSize>	stincxp	uint32_t	byte_count	(::gpk::astatic<T, nSize> viewToTest)		noexcept	{ return uint32_t(sizeof(T) * nSize);	}

	// Is this really worth it?
	tplt<uint32_t _elementCount>	stincxp	uint32_t	size(astc	<_elementCount> /*viewToTest*/)	noexcept	{ return uint32_t(_elementCount); }
	//tplt<uint32_t _elementCount>	stincxp	uint32_t	size(astuc 	<_elementCount> /*viewToTest*/)	noexcept	{ return uint32_t(_elementCount); }
	tplt<uint32_t _elementCount>	stincxp	uint32_t	size(astu8 	<_elementCount> /*viewToTest*/)	noexcept	{ return uint32_t(_elementCount); }
	tplt<uint32_t _elementCount>	stincxp	uint32_t	size(astu16	<_elementCount> /*viewToTest*/)	noexcept	{ return uint32_t(_elementCount); }
	tplt<uint32_t _elementCount>	stincxp	uint32_t	size(astu32	<_elementCount> /*viewToTest*/)	noexcept	{ return uint32_t(_elementCount); }
	tplt<uint32_t _elementCount>	stincxp	uint32_t	size(astu64	<_elementCount> /*viewToTest*/)	noexcept	{ return uint32_t(_elementCount); }
	tplt<uint32_t _elementCount>	stincxp	uint32_t	size(asti8 	<_elementCount> /*viewToTest*/)	noexcept	{ return uint32_t(_elementCount); }
	tplt<uint32_t _elementCount>	stincxp	uint32_t	size(asti16	<_elementCount> /*viewToTest*/)	noexcept	{ return uint32_t(_elementCount); }
	tplt<uint32_t _elementCount>	stincxp	uint32_t	size(asti32	<_elementCount> /*viewToTest*/)	noexcept	{ return uint32_t(_elementCount); }
	tplt<uint32_t _elementCount>	stincxp	uint32_t	size(asti64	<_elementCount> /*viewToTest*/)	noexcept	{ return uint32_t(_elementCount); }
	tplt<uint32_t _elementCount>	stincxp	uint32_t	size(astf32	<_elementCount> /*viewToTest*/)	noexcept	{ return uint32_t(_elementCount); }
	tplt<uint32_t _elementCount>	stincxp	uint32_t	size(astf64	<_elementCount> /*viewToTest*/)	noexcept	{ return uint32_t(_elementCount); }

	tplt<uint32_t _elementCount>	stincxp	uint32_t	size(astvcc	 < _elementCount> /*viewToTest*/)	noexcept	{ return uint32_t(_elementCount); }
	//tplt<uint32_t _elementCount>	stincxp	uint32_t	size(astvcuc < _elementCount> /*viewToTest*/)	noexcept	{ return uint32_t(_elementCount); }
	tplt<uint32_t _elementCount>	stincxp	uint32_t	size(astvcu8 < _elementCount> /*viewToTest*/)	noexcept	{ return uint32_t(_elementCount); }
	tplt<uint32_t _elementCount>	stincxp	uint32_t	size(astvcu16< _elementCount> /*viewToTest*/)	noexcept	{ return uint32_t(_elementCount); }
	tplt<uint32_t _elementCount>	stincxp	uint32_t	size(astvcu32< _elementCount> /*viewToTest*/)	noexcept	{ return uint32_t(_elementCount); }
	tplt<uint32_t _elementCount>	stincxp	uint32_t	size(astvcu64< _elementCount> /*viewToTest*/)	noexcept	{ return uint32_t(_elementCount); }
	tplt<uint32_t _elementCount>	stincxp	uint32_t	size(astvci8 < _elementCount> /*viewToTest*/)	noexcept	{ return uint32_t(_elementCount); }
	tplt<uint32_t _elementCount>	stincxp	uint32_t	size(astvci16< _elementCount> /*viewToTest*/)	noexcept	{ return uint32_t(_elementCount); }
	tplt<uint32_t _elementCount>	stincxp	uint32_t	size(astvci32< _elementCount> /*viewToTest*/)	noexcept	{ return uint32_t(_elementCount); }
	tplt<uint32_t _elementCount>	stincxp	uint32_t	size(astvci64< _elementCount> /*viewToTest*/)	noexcept	{ return uint32_t(_elementCount); }
	tplt<uint32_t _elementCount>	stincxp	uint32_t	size(astvcf32< _elementCount> /*viewToTest*/)	noexcept	{ return uint32_t(_elementCount); }
	tplt<uint32_t _elementCount>	stincxp	uint32_t	size(astvcf64< _elementCount> /*viewToTest*/)	noexcept	{ return uint32_t(_elementCount); }

	tplt<tpnm T, size_t _nSize>
	::gpk::error_t			find		(const T & element, const ::gpk::astatic<const T, _nSize>& target, uint32_t offset = 0)	{
		return ::gpk::find(element, ::gpk::view<const T>{target}, offset);
	}


	stainli gpk::astchar<12>	str			(f2_t	arg) { gpk::astchar<12> dest = {}; sprintf_s(dest.Storage, "%f", arg); return dest; } 
	stainli gpk::astchar<22>	str			(f3_t	arg) { gpk::astchar<22> dest = {}; sprintf_s(dest.Storage, "%f", arg); return dest; } 
	stainli gpk::astchar<7>		str			(int16_t	arg) { gpk::astchar< 7> dest = {}; sprintf_s(dest.Storage, "%i", arg); return dest; } 
	stainli gpk::astchar<7>		str			(uint16_t	arg) { gpk::astchar< 7> dest = {}; sprintf_s(dest.Storage, "%u", arg); return dest; } 
	stainli gpk::astchar<12>	str			(int32_t	arg) { gpk::astchar<12> dest = {}; sprintf_s(dest.Storage, "%" GPK_FMT_I32, arg); return dest; } 
	stainli gpk::astchar<22>	str			(int64_t	arg) { gpk::astchar<22> dest = {}; sprintf_s(dest.Storage, "%" GPK_FMT_I64, arg); return dest; } 
	stainli gpk::astchar<12>	str			(uint32_t	arg) { gpk::astchar<12> dest = {}; sprintf_s(dest.Storage, "%" GPK_FMT_U32, arg); return dest; } 
	stainli gpk::astchar<22>	str			(uint64_t	arg) { gpk::astchar<22> dest = {}; sprintf_s(dest.Storage, "%" GPK_FMT_U64, arg); return dest; } 

	stacxpr	gpk::astchar<5 >	str			(uint8_t	arg) { 
		return (arg >= 100) ? gpk::astchar<5>
				{ digit<2>(arg)
				, digit<1>(arg)
				, digit<0>(arg)
				, 0
				}
			: (arg >= 10) ? gpk::astchar<5>
				{ digit<1>(arg)
				, digit<0>(arg)
				, 0
				}
			: gpk::astchar<5>
				{ digit<0>(arg)
				, 0
				}
			;
	}
	stacxpr	gpk::astchar<5 >	str			(int8_t		arg) { 
	return (arg >= 0) ? str(uint8_t(arg))
		: (arg <= -100) ? gpk::astchar<5>
			{ '-'
			, ::gpk::digit<2>(-arg)
			, ::gpk::digit<1>(-arg)
			, ::gpk::digit<0>(-arg)
			, 0
			}
		: (arg <= -10) ? gpk::astchar<5>
			{ ::gpk::digit<1>(-arg)
			, ::gpk::digit<0>(-arg)
			, 0
			}
		: gpk::astchar<5>
			{ ::gpk::digit<0>(-arg)
			, 0
			}
		;
	}
} // namespace

#endif // GPK_ARRAY_STATIC_H_23627
