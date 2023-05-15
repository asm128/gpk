#include "gpk_view.h"

#ifndef GPK_ARRAY_STATIC_H_2983749823749826534465243
#define GPK_ARRAY_STATIC_H_2983749823749826534465243
namespace gpk
{
#pragma pack(push, 1)
	template<typename Val, uint32_t _sizeArray>
	struct array_static	{
		stacxpr	const uint32_t	SIZE						= _sizeArray;

		Val					Storage	[_sizeArray]		;

		operator				view<Val>				()																{ return {Storage, _sizeArray}; }
		operator				view<const Val>			()											const	noexcept	{ return {Storage, _sizeArray}; }

		inline	const Val&	operator[]					(uint32_t index)							const				{
			gthrow_if(index >= _sizeArray, "Invalid index: %i. Size: %i.", (int32_t)index, (int32_t)_sizeArray);
			return Storage[index];
		}
		inline	Val&			operator[]					(uint32_t index)												{
			gthrow_if(index >= _sizeArray, "Invalid index: %i. Size: %i.", (int32_t)index, (int32_t)_sizeArray);
			return Storage[index];
		}
		bool					operator!=					(const ::gpk::view<const Val>& other)	const				{ return !operator==(other); }
		bool					operator==					(const ::gpk::view<const Val>& other)	const				{
			if(this->size() != other.size())
				return false;
			if(this->begin() == other.begin())
				return true;
			return ::gpk::equal(other.begin(), this->begin(), this->size());
		}
		// Methods
		inline	Val*			begin						()													noexcept	{ return Storage;				}
		inline	Val*			end							()													noexcept	{ return Storage + _sizeArray;	}

		inlcxpr	const Val*	begin						()											const	noexcept	{ return Storage;				}
		inlcxpr	const Val*	end							()											const	noexcept	{ return Storage + _sizeArray;	}

		inlcxpr	const uint32_t&	size						()											const	noexcept	{ return SIZE; }
		inlcxpr	const uint32_t	byte_count					()											const	noexcept	{ return (uint32_t)(_sizeArray * sizeof(Val));	}
	};

	template<typename Val, uint32_t _sizeArray>	
	using astatic	= ::gpk::array_static<Val, _sizeArray>;

	template<uint32_t _sizeArray>	using astaticu8 	= ::gpk::array_static<uint8_t, _sizeArray>;
	template<uint32_t _sizeArray>	using astaticu16	= ::gpk::array_static<uint16_t, _sizeArray>;
	template<uint32_t _sizeArray>	using astaticu32	= ::gpk::array_static<uint32_t, _sizeArray>;
	template<uint32_t _sizeArray>	using astaticu64	= ::gpk::array_static<uint64_t, _sizeArray>;
	template<uint32_t _sizeArray>	using astatici8 	= ::gpk::array_static<int8_t, _sizeArray>;
	template<uint32_t _sizeArray>	using astatici16	= ::gpk::array_static<int16_t, _sizeArray>;
	template<uint32_t _sizeArray>	using astatici32	= ::gpk::array_static<int32_t, _sizeArray>;
	template<uint32_t _sizeArray>	using astatici64	= ::gpk::array_static<int64_t, _sizeArray>;

#pragma pack(pop)

	template <typename Val, size_t nSize>	stincxp	uint32_t	size		(::gpk::astatic<Val, nSize> /*viewToTest*/)	noexcept	{ return (uint32_t)(nSize);					}
	template <typename Val, size_t nSize>	stincxp	uint32_t	byte_count	(::gpk::astatic<Val, nSize> viewToTest)		noexcept	{ return (uint32_t)(sizeof(Val) * nSize);	}

	template<typename Val, size_t _nSize>
	::gpk::error_t			find						(const Val & element, const ::gpk::astatic<const Val, _nSize>& target, uint32_t offset = 0)	{
		return ::gpk::find(element, ::gpk::view<const Val>{target}, offset);
	}

	template<typename _tPOD, uint32_t _nSize> 
	::gpk::error_t			loadView					(::gpk::vcu8 & input, ::gpk::astatic<_tPOD, _nSize> & output) { 
		::gpk::view<const _tPOD>	readView					= {}; 
		uint32_t					bytesRead					= 0;
		gpk_necs(bytesRead = ::gpk::viewRead(readView, input)); 
		input					= {input.begin() + bytesRead, input.size() - bytesRead}; 
		memcpy(output.begin(), readView.begin(), ::gpk::min(readView.byte_count(), ::gpk::view<_tPOD>{output}.byte_count()));
		return 0;
	}

} // namespace

#endif // GPK_ARRAY_STATIC_H_2983749823749826534465243
