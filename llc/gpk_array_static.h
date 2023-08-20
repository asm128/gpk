#include "gpk_view.h"

#ifndef GPK_ARRAY_STATIC_H_23627
#define GPK_ARRAY_STATIC_H_23627

namespace gpk
{
#pragma pack(push, 1)
	tplt<tpnm T, uint32_t _sizeArray>
	struct array_static	{
		stacxpr	const uint32_t	SIZE						= _sizeArray;

		T						Storage	[_sizeArray]		;

		operator				view<T>						()																{ return {Storage, _sizeArray}; }
		operator				view<const T>				()											const	noexcept	{ return {Storage, _sizeArray}; }

		inline	const T&		operator[]					(uint32_t index)							const				{
			gthrow_if(index >= _sizeArray, "Invalid index: %i. Size: %i.", (int32_t)index, (int32_t)_sizeArray);
			return Storage[index];
		}
		inline	T&				operator[]					(uint32_t index)												{
			gthrow_if(index >= _sizeArray, "Invalid index: %i. Size: %i.", (int32_t)index, (int32_t)_sizeArray);
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
		inline	T*				end							()													noexcept	{ return Storage + _sizeArray;	}

		inlcxpr	const T*		begin						()											const	noexcept	{ return Storage;				}
		inlcxpr	const T*		end							()											const	noexcept	{ return Storage + _sizeArray;	}

		inlcxpr	const uint32_t&	size						()											const	noexcept	{ return SIZE; }
		inlcxpr	uint32_t		byte_count					()											const	noexcept	{ return (uint32_t)(_sizeArray * sizeof(T));	}
	};

	tplt<tpnm T, uint32_t _sizeArray>	
	using astatic	= ::gpk::array_static<T, _sizeArray>;

	tplt<uint32_t _sizeArray>	using astc		= ::gpk::astatic<char     , _sizeArray>; tplt<uint32_t _sizeArray>	using astaticc		= ::gpk::astc	<_sizeArray>; tplt<uint32_t _sizeArray>	using astchar	= ::gpk::astatic<char    , _sizeArray>;
	tplt<uint32_t _sizeArray>	using astuc 	= ::gpk::astatic<uchar_t  , _sizeArray>; tplt<uint32_t _sizeArray>	using astaticuc 	= ::gpk::astuc 	<_sizeArray>; tplt<uint32_t _sizeArray>	using astuchar 	= ::gpk::astatic<uchar_t , _sizeArray>;
	tplt<uint32_t _sizeArray>	using astu8 	= ::gpk::astatic<uint8_t  , _sizeArray>; tplt<uint32_t _sizeArray>	using astaticu8 	= ::gpk::astu8 	<_sizeArray>; 
	tplt<uint32_t _sizeArray>	using astu16	= ::gpk::astatic<uint16_t , _sizeArray>; tplt<uint32_t _sizeArray>	using astaticu16	= ::gpk::astu16	<_sizeArray>; 
	tplt<uint32_t _sizeArray>	using astu32	= ::gpk::astatic<uint32_t , _sizeArray>; tplt<uint32_t _sizeArray>	using astaticu32	= ::gpk::astu32	<_sizeArray>; 
	tplt<uint32_t _sizeArray>	using astu64	= ::gpk::astatic<uint64_t , _sizeArray>; tplt<uint32_t _sizeArray>	using astaticu64	= ::gpk::astu64	<_sizeArray>; 
	tplt<uint32_t _sizeArray>	using asti8 	= ::gpk::astatic<int8_t   , _sizeArray>; tplt<uint32_t _sizeArray>	using astatici8 	= ::gpk::asti8 	<_sizeArray>; 
	tplt<uint32_t _sizeArray>	using asti16	= ::gpk::astatic<int16_t  , _sizeArray>; tplt<uint32_t _sizeArray>	using astatici16	= ::gpk::asti16	<_sizeArray>; 
	tplt<uint32_t _sizeArray>	using asti32	= ::gpk::astatic<int32_t  , _sizeArray>; tplt<uint32_t _sizeArray>	using astatici32	= ::gpk::asti32	<_sizeArray>; 
	tplt<uint32_t _sizeArray>	using asti64	= ::gpk::astatic<int64_t  , _sizeArray>; tplt<uint32_t _sizeArray>	using astatici64	= ::gpk::asti64	<_sizeArray>; 
	tplt<uint32_t _sizeArray>	using astf32	= ::gpk::astatic<float32_t, _sizeArray>; tplt<uint32_t _sizeArray>	using astaticf32	= ::gpk::astf32	<_sizeArray>; 
	tplt<uint32_t _sizeArray>	using astf64	= ::gpk::astatic<float64_t, _sizeArray>; tplt<uint32_t _sizeArray>	using astaticf64	= ::gpk::astf64	<_sizeArray>; 

	tplt<uint32_t _sizeArray>	using astvcc	= ::gpk::astatic<::gpk::vcc		, _sizeArray>; tplt<uint32_t _sizeArray>	using astaticvcc	= ::gpk::astvcc  <_sizeArray>; 
	tplt<uint32_t _sizeArray>	using astvcuc 	= ::gpk::astatic<::gpk::vcuc 	, _sizeArray>; tplt<uint32_t _sizeArray>	using astaticvcuc 	= ::gpk::astvcuc <_sizeArray>; 
	tplt<uint32_t _sizeArray>	using astvcu8 	= ::gpk::astatic<::gpk::vcu8 	, _sizeArray>; tplt<uint32_t _sizeArray>	using astaticvcu8 	= ::gpk::astvcu8 <_sizeArray>; 
	tplt<uint32_t _sizeArray>	using astvcu16	= ::gpk::astatic<::gpk::vcu16	, _sizeArray>; tplt<uint32_t _sizeArray>	using astaticvcu16	= ::gpk::astvcu16<_sizeArray>; 
	tplt<uint32_t _sizeArray>	using astvcu32	= ::gpk::astatic<::gpk::vcu32	, _sizeArray>; tplt<uint32_t _sizeArray>	using astaticvcu32	= ::gpk::astvcu32<_sizeArray>; 
	tplt<uint32_t _sizeArray>	using astvcu64	= ::gpk::astatic<::gpk::vcu64	, _sizeArray>; tplt<uint32_t _sizeArray>	using astaticvcu64	= ::gpk::astvcu64<_sizeArray>; 
	tplt<uint32_t _sizeArray>	using astvci8 	= ::gpk::astatic<::gpk::vci8 	, _sizeArray>; tplt<uint32_t _sizeArray>	using astaticvci8 	= ::gpk::astvci8 <_sizeArray>; 
	tplt<uint32_t _sizeArray>	using astvci16	= ::gpk::astatic<::gpk::vci16	, _sizeArray>; tplt<uint32_t _sizeArray>	using astaticvci16	= ::gpk::astvci16<_sizeArray>; 
	tplt<uint32_t _sizeArray>	using astvci32	= ::gpk::astatic<::gpk::vci32	, _sizeArray>; tplt<uint32_t _sizeArray>	using astaticvci32	= ::gpk::astvci32<_sizeArray>; 
	tplt<uint32_t _sizeArray>	using astvci64	= ::gpk::astatic<::gpk::vci64	, _sizeArray>; tplt<uint32_t _sizeArray>	using astaticvci64	= ::gpk::astvci64<_sizeArray>; 
	tplt<uint32_t _sizeArray>	using astvcf32	= ::gpk::astatic<::gpk::vcf32	, _sizeArray>; tplt<uint32_t _sizeArray>	using astaticvcf32	= ::gpk::astvcf32<_sizeArray>; 
	tplt<uint32_t _sizeArray>	using astvcf64	= ::gpk::astatic<::gpk::vcf64	, _sizeArray>; tplt<uint32_t _sizeArray>	using astaticvcf64	= ::gpk::astvcf64<_sizeArray>; 
#pragma pack(pop)

	tplt <tpnm T, size_t nSize>	stincxp	uint32_t	size		(::gpk::astatic<T, nSize> /*viewToTest*/)	noexcept	{ return (uint32_t)(nSize);					}
	tplt <tpnm T, size_t nSize>	stincxp	uint32_t	byte_count	(::gpk::astatic<T, nSize> viewToTest)		noexcept	{ return (uint32_t)(sizeof(T) * nSize);	}

	tplt<tpnm T, size_t _nSize>
	::gpk::error_t			find		(const T & element, const ::gpk::astatic<const T, _nSize>& target, uint32_t offset = 0)	{
		return ::gpk::find(element, ::gpk::view<const T>{target}, offset);
	}
} // namespace

#endif // GPK_ARRAY_STATIC_H_23627
