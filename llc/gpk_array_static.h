#include "gpk_view.h"

#ifndef GPK_ARRAY_STATIC_H_2983749823749826534465243
#define GPK_ARRAY_STATIC_H_2983749823749826534465243
namespace gpk
{
#pragma pack(push, 1)
	template<typename T, uint32_t _sizeArray>
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

	template<typename T, uint32_t _sizeArray>	
	using astatic	= ::gpk::array_static<T, _sizeArray>;

	template<uint32_t _sizeArray>	using astaticc		= ::gpk::array_static<char    , _sizeArray>;
	template<uint32_t _sizeArray>	using astaticuc 	= ::gpk::array_static<uchar_t , _sizeArray>;
	template<uint32_t _sizeArray>	using astaticu8 	= ::gpk::array_static<uint8_t , _sizeArray>;
	template<uint32_t _sizeArray>	using astaticu16	= ::gpk::array_static<uint16_t, _sizeArray>;
	template<uint32_t _sizeArray>	using astaticu32	= ::gpk::array_static<uint32_t, _sizeArray>;
	template<uint32_t _sizeArray>	using astaticu64	= ::gpk::array_static<uint64_t, _sizeArray>;
	template<uint32_t _sizeArray>	using astatici8 	= ::gpk::array_static<int8_t  , _sizeArray>;
	template<uint32_t _sizeArray>	using astatici16	= ::gpk::array_static<int16_t , _sizeArray>;
	template<uint32_t _sizeArray>	using astatici32	= ::gpk::array_static<int32_t , _sizeArray>;
	template<uint32_t _sizeArray>	using astatici64	= ::gpk::array_static<int64_t , _sizeArray>;

	template<uint32_t _sizeArray>	using astc		= ::gpk::array_static<char    , _sizeArray>;
	template<uint32_t _sizeArray>	using astuc 	= ::gpk::array_static<uchar_t , _sizeArray>;
	template<uint32_t _sizeArray>	using astu8 	= ::gpk::array_static<uint8_t , _sizeArray>;
	template<uint32_t _sizeArray>	using astu16	= ::gpk::array_static<uint16_t, _sizeArray>;
	template<uint32_t _sizeArray>	using astu32	= ::gpk::array_static<uint32_t, _sizeArray>;
	template<uint32_t _sizeArray>	using astu64	= ::gpk::array_static<uint64_t, _sizeArray>;
	template<uint32_t _sizeArray>	using asti8 	= ::gpk::array_static<int8_t  , _sizeArray>;
	template<uint32_t _sizeArray>	using asti16	= ::gpk::array_static<int16_t , _sizeArray>;
	template<uint32_t _sizeArray>	using asti32	= ::gpk::array_static<int32_t , _sizeArray>;
	template<uint32_t _sizeArray>	using asti64	= ::gpk::array_static<int64_t , _sizeArray>;

#pragma pack(pop)

	template <typename T, size_t nSize>	stincxp	uint32_t	size		(::gpk::astatic<T, nSize> /*viewToTest*/)	noexcept	{ return (uint32_t)(nSize);					}
	template <typename T, size_t nSize>	stincxp	uint32_t	byte_count	(::gpk::astatic<T, nSize> viewToTest)		noexcept	{ return (uint32_t)(sizeof(T) * nSize);	}

	template<typename T, size_t _nSize>
	::gpk::error_t			find		(const T & element, const ::gpk::astatic<const T, _nSize>& target, uint32_t offset = 0)	{
		return ::gpk::find(element, ::gpk::view<const T>{target}, offset);
	}
} // namespace

#endif // GPK_ARRAY_STATIC_H_2983749823749826534465243
