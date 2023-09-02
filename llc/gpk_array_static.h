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
		inline	T*				begin						()													noexcept	{ return Storage;				}
		inline	T*				end							()													noexcept	{ return Storage + _elementCount;	}

		inlcxpr	const T*		begin						()											const	noexcept	{ return Storage;				}
		inlcxpr	const T*		end							()											const	noexcept	{ return Storage + _elementCount;	}

		inlcxpr	const uint32_t&	size						()											const	noexcept	{ return SIZE; }
		inlcxpr	uint32_t		byte_count					()											const	noexcept	{ return (uint32_t)(_elementCount * sizeof(T));	}

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

	tplt<uint32_t _elementCount>	using astc		= ::gpk::astatic<char     , _elementCount>; tplt<uint32_t _elementCount>	using astaticc		= ::gpk::astc	<_elementCount>; tplt<uint32_t _elementCount>	using astchar	= ::gpk::astatic<char    , _elementCount>;
	tplt<uint32_t _elementCount>	using astuc 	= ::gpk::astatic<uchar_t  , _elementCount>; tplt<uint32_t _elementCount>	using astaticuc 	= ::gpk::astuc 	<_elementCount>; tplt<uint32_t _elementCount>	using astuchar 	= ::gpk::astatic<uchar_t , _elementCount>;
	tplt<uint32_t _elementCount>	using astu8 	= ::gpk::astatic<uint8_t  , _elementCount>; tplt<uint32_t _elementCount>	using astaticu8 	= ::gpk::astu8 	<_elementCount>; 
	tplt<uint32_t _elementCount>	using astu16	= ::gpk::astatic<uint16_t , _elementCount>; tplt<uint32_t _elementCount>	using astaticu16	= ::gpk::astu16	<_elementCount>; 
	tplt<uint32_t _elementCount>	using astu32	= ::gpk::astatic<uint32_t , _elementCount>; tplt<uint32_t _elementCount>	using astaticu32	= ::gpk::astu32	<_elementCount>; 
	tplt<uint32_t _elementCount>	using astu64	= ::gpk::astatic<uint64_t , _elementCount>; tplt<uint32_t _elementCount>	using astaticu64	= ::gpk::astu64	<_elementCount>; 
	tplt<uint32_t _elementCount>	using asti8 	= ::gpk::astatic<int8_t   , _elementCount>; tplt<uint32_t _elementCount>	using astatici8 	= ::gpk::asti8 	<_elementCount>; 
	tplt<uint32_t _elementCount>	using asti16	= ::gpk::astatic<int16_t  , _elementCount>; tplt<uint32_t _elementCount>	using astatici16	= ::gpk::asti16	<_elementCount>; 
	tplt<uint32_t _elementCount>	using asti32	= ::gpk::astatic<int32_t  , _elementCount>; tplt<uint32_t _elementCount>	using astatici32	= ::gpk::asti32	<_elementCount>; 
	tplt<uint32_t _elementCount>	using asti64	= ::gpk::astatic<int64_t  , _elementCount>; tplt<uint32_t _elementCount>	using astatici64	= ::gpk::asti64	<_elementCount>; 
	tplt<uint32_t _elementCount>	using astf32	= ::gpk::astatic<float32_t, _elementCount>; tplt<uint32_t _elementCount>	using astaticf32	= ::gpk::astf32	<_elementCount>; 
	tplt<uint32_t _elementCount>	using astf64	= ::gpk::astatic<float64_t, _elementCount>; tplt<uint32_t _elementCount>	using astaticf64	= ::gpk::astf64	<_elementCount>; 

	tplt<uint32_t _elementCount>	using astvcc	= ::gpk::astatic<::gpk::vcc		, _elementCount>; tplt<uint32_t _elementCount>	using astaticvcc	= ::gpk::astvcc  <_elementCount>; 
	tplt<uint32_t _elementCount>	using astvcuc 	= ::gpk::astatic<::gpk::vcuc 	, _elementCount>; tplt<uint32_t _elementCount>	using astaticvcuc 	= ::gpk::astvcuc <_elementCount>; 
	tplt<uint32_t _elementCount>	using astvcu8 	= ::gpk::astatic<::gpk::vcu8 	, _elementCount>; tplt<uint32_t _elementCount>	using astaticvcu8 	= ::gpk::astvcu8 <_elementCount>; 
	tplt<uint32_t _elementCount>	using astvcu16	= ::gpk::astatic<::gpk::vcu16	, _elementCount>; tplt<uint32_t _elementCount>	using astaticvcu16	= ::gpk::astvcu16<_elementCount>; 
	tplt<uint32_t _elementCount>	using astvcu32	= ::gpk::astatic<::gpk::vcu32	, _elementCount>; tplt<uint32_t _elementCount>	using astaticvcu32	= ::gpk::astvcu32<_elementCount>; 
	tplt<uint32_t _elementCount>	using astvcu64	= ::gpk::astatic<::gpk::vcu64	, _elementCount>; tplt<uint32_t _elementCount>	using astaticvcu64	= ::gpk::astvcu64<_elementCount>; 
	tplt<uint32_t _elementCount>	using astvci8 	= ::gpk::astatic<::gpk::vci8 	, _elementCount>; tplt<uint32_t _elementCount>	using astaticvci8 	= ::gpk::astvci8 <_elementCount>; 
	tplt<uint32_t _elementCount>	using astvci16	= ::gpk::astatic<::gpk::vci16	, _elementCount>; tplt<uint32_t _elementCount>	using astaticvci16	= ::gpk::astvci16<_elementCount>; 
	tplt<uint32_t _elementCount>	using astvci32	= ::gpk::astatic<::gpk::vci32	, _elementCount>; tplt<uint32_t _elementCount>	using astaticvci32	= ::gpk::astvci32<_elementCount>; 
	tplt<uint32_t _elementCount>	using astvci64	= ::gpk::astatic<::gpk::vci64	, _elementCount>; tplt<uint32_t _elementCount>	using astaticvci64	= ::gpk::astvci64<_elementCount>; 
	tplt<uint32_t _elementCount>	using astvcf32	= ::gpk::astatic<::gpk::vcf32	, _elementCount>; tplt<uint32_t _elementCount>	using astaticvcf32	= ::gpk::astvcf32<_elementCount>; 
	tplt<uint32_t _elementCount>	using astvcf64	= ::gpk::astatic<::gpk::vcf64	, _elementCount>; tplt<uint32_t _elementCount>	using astaticvcf64	= ::gpk::astvcf64<_elementCount>; 
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
