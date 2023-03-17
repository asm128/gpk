#include "gpk_view.h"

#ifndef GPK_ARRAY_STATIC_H_2983749823749826534465243
#define GPK_ARRAY_STATIC_H_2983749823749826534465243
namespace gpk
{
#pragma pack(push, 1)
	template<typename _tCell, uint32_t _sizeArray>
	struct array_static	{
		stacxpr	const uint32_t	SIZE						= _sizeArray;

		_tCell					Storage	[_sizeArray]		;

		operator				view<_tCell>				()																{ return {Storage, _sizeArray}; }
		operator				view<const _tCell>			()											const	noexcept	{ return {Storage, _sizeArray}; }

		inline	const _tCell&	operator[]					(uint32_t index)							const				{
			gthrow_if(index >= _sizeArray, "Invalid index: %i. Size: %i.", (int32_t)index, (int32_t)_sizeArray);
			return Storage[index];
		}
		inline	_tCell&			operator[]					(uint32_t index)												{
			gthrow_if(index >= _sizeArray, "Invalid index: %i. Size: %i.", (int32_t)index, (int32_t)_sizeArray);
			return Storage[index];
		}
		bool					operator!=					(const ::gpk::view<const _tCell>& other)	const				{ return !operator==(other); }
		bool					operator==					(const ::gpk::view<const _tCell>& other)	const				{
			if(this->size() != other.size())
				return false;
			if(this->begin() == other.begin())
				return true;
			return ::gpk::equal(other.begin(), this->begin(), this->size());
		}
		// Methods
		inline	_tCell*			begin						()													noexcept	{ return Storage;				}
		inline	_tCell*			end							()													noexcept	{ return Storage + _sizeArray;	}

		inlcxpr	const _tCell*	begin						()											const	noexcept	{ return Storage;				}
		inlcxpr	const _tCell*	end							()											const	noexcept	{ return Storage + _sizeArray;	}

		inlcxpr	const uint32_t&	size						()											const	noexcept	{ return SIZE; }
		inlcxpr	const uint32_t	byte_count					()											const	noexcept	{ return (uint32_t)(_sizeArray * sizeof(_tCell));	}
	};

	template<typename _tCell, uint32_t _sizeArray>	
	using astatic	= ::gpk::array_static<_tCell, _sizeArray>;

#pragma pack(pop)

	template <typename _tCell, size_t nSize>	stincxp	uint32_t	size		(::gpk::astatic<_tCell, nSize> /*viewToTest*/)	noexcept	{ return (uint32_t)(nSize);					}
	template <typename _tCell, size_t nSize>	stincxp	uint32_t	byte_count	(::gpk::astatic<_tCell, nSize> viewToTest)		noexcept	{ return (uint32_t)(sizeof(_tCell) * nSize);	}

	template<typename _tCell, size_t _nSize>
	::gpk::error_t			find						(const _tCell & element, const ::gpk::astatic<const _tCell, _nSize>& target, uint32_t offset = 0)	{
		return ::gpk::find(element, ::gpk::view<const _tCell>{target}, offset);
	}

	template<typename _tPOD, uint32_t _nSize> 
	::gpk::error_t			loadView					(::gpk::vcub & input, ::gpk::astatic<_tPOD, _nSize> & output) { 
		::gpk::view<const _tPOD>	readView					= {}; 
		uint32_t					bytesRead					= 0;
		gpk_necs(bytesRead = ::gpk::viewLoad(readView, input)); 
		input					= {input.begin() + bytesRead, input.size() - bytesRead}; 
		memcpy(output.begin(), readView.begin(), ::gpk::min(readView.byte_count(), ::gpk::view<_tPOD>{output}.byte_count()));
		return 0;
	}

} // namespace

#endif // GPK_ARRAY_STATIC_H_2983749823749826534465243
