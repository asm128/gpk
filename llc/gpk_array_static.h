#include "gpk_view.h"

#ifndef GPK_ARRAY_STATIC_H_23627
#define GPK_ARRAY_STATIC_H_23627

namespace gpk
{
#pragma pack(push, 1)
	tplt<tpnm _tVal, uint32_t _elementCount>
	struct array_static	{
		typedef	_tVal			T;
		typedef	view<const T>	TConstView;
		typedef	view<T>			TView;

		stacxpr	const uint32_t	SIZE						= _elementCount;

		T						Storage	[_elementCount]		;

		operator				view<T>						()																{ return {Storage, _elementCount}; }
		operator				view<const T>				()											const	noexcept	{ return {Storage, _elementCount}; }

		inline	const T&		operator[]					(uint32_t index)							const				{
			gthrow_if(index >= _elementCount, "Invalid index: %i. Size: %i.", (int32_t)index, (int32_t)_elementCount);
			return Storage[index];
		}
		inline	T&				operator[]					(uint32_t index)												{
			gthrow_if(index >= _elementCount, "Invalid index: %i. Size: %i.", (int32_t)index, (int32_t)_elementCount);
			return Storage[index];
		}
		bool					operator!=					(const ::gpk::view<const T>& other)	const				{ return !operator==(other); }
		bool					operator==					(const ::gpk::view<const T>& other)	const				{
			if(this->size() != other.size())
				return false;
			if(this->begin() == other.begin())
				return true;
			return ::gpk::equal(other.begin(), this->begin(), this->size());
		}
		// Methods
		inlcxpr	uint32_t		byte_count		()							const	noexcept	{ return uint32_t(size() * sizeof(T));	}
		inlcxpr	uint32_t		bit_count		()							const	noexcept	{ return byte_count() * 8U;	}

		inlcxpr	view<uint8_t>	u8				()									noexcept	{ return {(uint8_t*)Storage, byte_count()};		}
		inline	view<cuint8_t>	u8				()							const	noexcept	{ return {(const uint8_t*)Storage, byte_count()};	}
		inlcxpr	view<cuint8_t>	cu8				()							const	noexcept	{ return {(const uint8_t*)Storage, byte_count()};	}
		inline	view<char>		c				()									noexcept	{ return {(char*)Storage, byte_count()};			}
		inlcxpr	view<cchar_t>	cc				()							const	noexcept	{ return {(const char*)Storage, byte_count()};		}

		inline	T*				begin						()						noexcept	{ return Storage;				}
		inline	T*				end							()						noexcept	{ return Storage + _elementCount;	}

		inlcxpr	const T*		begin						()				const	noexcept	{ return Storage;				}
		inlcxpr	const T*		end							()				const	noexcept	{ return Storage + _elementCount;	}

		inlcxpr	const uint32_t&	size						()				const	noexcept	{ return SIZE; }

		::gpk::error_t			slice			(TView & out, uint32_t offset, uint32_t count = (uint32_t)-1)				{
			ree_if(offset > SIZE, "Out of range. Max offset: %u. Requested: %u.", (uint32_t)SIZE, offset);
			const uint32_t				newSize			= SIZE - offset;
			if(count != (uint32_t)-1)
				ree_if(count > newSize, "Out of range. Max size: %u. Requested: %u.", (uint32_t)newSize, count);
			out						= {&Storage[offset], ::gpk::min(newSize, count)};
			return out.size();
		}
		::gpk::error_t			slice			(TConstView & out, uint32_t offset, uint32_t count = (uint32_t)-1)	const	{
			ree_if(offset > SIZE, "Out of range. Max offset: %u. Requested: %u.", (uint32_t)SIZE, offset);
			const uint32_t				newSize			= SIZE - offset;
			if(count != (uint32_t)-1)
				ree_if(count > newSize, "Out of range. Max size: %u. Requested: %u.", (uint32_t)newSize, count);
			out						= {&Storage[offset], ::gpk::min(newSize, count)};
			return out.size();
		}
	};

	tplt<tpnm T, uint32_t _elementCount>
	using astatic	= ::gpk::array_static<T, _elementCount>;

	tplt<uint32_t _count>	using astvcs	= ::gpk::astatic<::gpk::vcs	, _count>; tplt<uint32_t _count>	using astaticvcs	= ::gpk::astvcs	<_count>;
	tplt<uint32_t _count>	using astvs		= ::gpk::astatic<::gpk::vs	, _count>; tplt<uint32_t _count>	using astaticvs		= ::gpk::astvs	<_count>;

	tplt<uint32_t _count>	using astc		= ::gpk::astatic<char     , _count>; tplt<uint32_t _count>	using astaticc		= ::gpk::astc	<_count>; tplt<uint32_t _count>	using astchar	= ::gpk::astatic<char    , _count>;
	tplt<uint32_t _count>	using astuc 	= ::gpk::astatic<uchar_t  , _count>; tplt<uint32_t _count>	using astaticuc 	= ::gpk::astuc 	<_count>; tplt<uint32_t _count>	using astuchar	= ::gpk::astatic<uchar_t , _count>;
	tplt<uint32_t _count>	using astu8 	= ::gpk::astatic<uint8_t  , _count>; tplt<uint32_t _count>	using astaticu8 	= ::gpk::astu8 	<_count>;
	tplt<uint32_t _count>	using astu16	= ::gpk::astatic<uint16_t , _count>; tplt<uint32_t _count>	using astaticu16	= ::gpk::astu16	<_count>;
	tplt<uint32_t _count>	using astu32	= ::gpk::astatic<uint32_t , _count>; tplt<uint32_t _count>	using astaticu32	= ::gpk::astu32	<_count>;
	tplt<uint32_t _count>	using astu64	= ::gpk::astatic<uint64_t , _count>; tplt<uint32_t _count>	using astaticu64	= ::gpk::astu64	<_count>;
	tplt<uint32_t _count>	using asti8 	= ::gpk::astatic<int8_t   , _count>; tplt<uint32_t _count>	using astatici8 	= ::gpk::asti8 	<_count>;
	tplt<uint32_t _count>	using asti16	= ::gpk::astatic<int16_t  , _count>; tplt<uint32_t _count>	using astatici16	= ::gpk::asti16	<_count>;
	tplt<uint32_t _count>	using asti32	= ::gpk::astatic<int32_t  , _count>; tplt<uint32_t _count>	using astatici32	= ::gpk::asti32	<_count>;
	tplt<uint32_t _count>	using asti64	= ::gpk::astatic<int64_t  , _count>; tplt<uint32_t _count>	using astatici64	= ::gpk::asti64	<_count>;
	tplt<uint32_t _count>	using astf32	= ::gpk::astatic<float32_t, _count>; tplt<uint32_t _count>	using astaticf32	= ::gpk::astf32	<_count>;
	tplt<uint32_t _count>	using astf64	= ::gpk::astatic<float64_t, _count>; tplt<uint32_t _count>	using astaticf64	= ::gpk::astf64	<_count>;

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
#pragma pack(pop)

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
} // namespace

#endif // GPK_ARRAY_STATIC_H_23627
