#include "gpk_log.h"

#include "gpk_functional.h"

#if defined(GPK_ATMEL)
#	include <string.h>
#	ifdef max
#		undef max
#	endif
#	ifdef min
#		undef min
#	endif
#else
#	include <cstring>
#endif

#ifndef GPK_ARRAY_VIEW_H_23627
#define GPK_ARRAY_VIEW_H_23627

namespace gpk
{
#pragma pack(push, 1)
	tplt <tpnm _tVal>
	class view {
	protected:
		// Properties / Member Variables
		_tVal					* Data			= 0;
		uint32_t				Count			= 0;
	public:
		typedef	_tVal			T;
		typedef	view<const T>	TConstView;
		typedef	view<T>			TView;

		// Constructors
		inlcxpr					view			()									noexcept	= default;
		inlcxpr					view			(const view<T> & other)				noexcept	= default;

		tplt<size_t Len>
		inlcxpr					view			(T (&elements)[Len])				noexcept	: Data(elements), Count(Len)										{}

		tplt<size_t Len>
		inlcxpr					view			(uint32_t elementCount, T (&elements)[Len])		: Data(elements), Count(::gpk::min((uint32_t)Len, elementCount))	{}
		inline					view			(T * elements, uint32_t elementCount)			: Data(elements), Count(elementCount)								{
			gthrow_if(0 == elements && 0 != elementCount, "%u -> 0.", elementCount);	// Crash if we received invalid parameters in order to prevent further malfunctioning.
		}

		tplt <size_t Len>
		inline					view			(T (&elements)[Len], uint32_t elementCount)		: Data(elements), Count(::gpk::min((uint32_t)Len, elementCount))	{
			gthrow_if(elementCount > Len, "%u > %u", elementCount, (uint32_t)Len);
		}

		// Operators
		inlcxpr	operator		view<const T>	()							const	noexcept	{ return {Data, Count}; }

		T&						operator[]		(uint32_t index)								{
			gsthrow_if(0 == Data);
			gthrow_if(index >= Count, "%i >= %i.", index, Count);
			return Data[index];
		}
		const T&				operator[]		(uint32_t index)			const				{
			gsthrow_if(0 == Data);
			gthrow_if(index >= Count, "%i >= %i.", index, Count);
			return Data[index];
		}
		bool					operator!=		(const TConstView & other)	const				{ return  !operator==(other); } // I had to add this for the android build not supporting C++20.
		bool					operator==		(const TConstView & other)	const				{
			if(this->size() != other.size())
				return false;
			if(this->begin() == other.begin())
				return true;
			return ::gpk::equal(other.begin(), this->begin(), this->size());
		}

		// Methods
		inlcxpr	uint32_t		byte_count		()							const	noexcept	{ return uint32_t(Count * sizeof(T));	}
		inlcxpr	uint32_t		bit_count		()							const	noexcept	{ return byte_count() * 8U;	}

		inline	view<uint8_t>	u8				()									noexcept	{ return {(uint8_t*)Data, byte_count()};		}
		inlcxpr	view<cuint8_t>	u8				()							const	noexcept	{ return {(const uint8_t*)Data, byte_count()};	}
		inlcxpr	view<cuint8_t>	cu8				()							const	noexcept	{ return {(const uint8_t*)Data, byte_count()};	}
		inline	view<char>		c				()									noexcept	{ return {(char*)Data, byte_count()};			}
		inlcxpr	view<cchar_t>	cc				()							const	noexcept	{ return {(const char*)Data, byte_count()};		}

		inlcxpr	const T*		begin			()							const	noexcept	{ return Data;			}
		inlcxpr	const T*		end				()							const	noexcept	{ return Data + Count;	}
		inlcxpr	const uint32_t&	size			()							const	noexcept	{ return Count;			}

		inline	T*				begin			()									noexcept	{ return Data;			}
		inline	T*				end				()									noexcept	{ return Data + Count;	}

		::gpk::error_t			slice			(TView & out, uint32_t offset, uint32_t count = (uint32_t)-1)				{
			reterr_gerror_if(offset > Count, "%u > %u", offset, (uint32_t)Count);
			const uint32_t				newSize			= Count - offset;
			if(count != (uint32_t)-1)
				ree_if(count > newSize, "%u > %u", count, (uint32_t)newSize);
			out						= {&Data[offset], ::gpk::min(newSize, count)};
			return out.size();
		}
		::gpk::error_t			slice			(TConstView & out, uint32_t offset, uint32_t count = (uint32_t)-1)	const	{
			ree_if(offset > Count, "%u > %u", offset, (uint32_t)Count);
			const uint32_t				newSize			= Count - offset;
			if(count != (uint32_t)-1)
				ree_if(count > newSize, "%u > %u", count, (uint32_t)newSize);
			out						= {&Data[offset], ::gpk::min(newSize, count)};
			return out.size();
		}
		::gpk::error_t			revert			()																			{
			const uint32_t				lastElement		= Count - 1;
			for(uint32_t i = 0, swapCount = Count / 2; i < swapCount; ++i) {
				T							old				= Data[i];
				Data[i]					= Data[lastElement - i];
				Data[lastElement - i]	= old;
			}
			return 0;
		}

		inline	::gpk::error_t	fill			(const T & value, uint32_t offset = 0, uint32_t stop = 0xFFFFFFFFU)		{ for(; offset < ::gpk::min(Count, stop); ++offset) Data[offset] = value; return Count; }


		::gpk::error_t			for_each		(const ::gpk::TFuncForEach       <T> & funcForEach, uint32_t offset = 0)			{ for(; offset < Count; ++offset) funcForEach(Data[offset]); return offset; }
		::gpk::error_t			for_each		(const ::gpk::TFuncForEachConst  <T> & funcForEach, uint32_t offset = 0)	const	{ for(; offset < Count; ++offset) funcForEach(Data[offset]); return offset; }
		::gpk::error_t			enumerate		(const ::gpk::TFuncEnumerate     <T> & funcForEach, uint32_t offset = 0)			{ for(; offset < Count; ++offset) funcForEach(offset, Data[offset]); return offset; }
		::gpk::error_t			enumerate		(const ::gpk::TFuncEnumerateConst<T> & funcForEach, uint32_t offset = 0)	const	{ for(; offset < Count; ++offset) funcForEach(offset, Data[offset]); return offset; }
		//
		::gpk::error_t			for_each		(const ::gpk::TFuncForEach       <T> & funcForEach, uint32_t offset, uint32_t stop)			{ for(stop = ::gpk::min(stop, Count); offset < stop; ++offset) funcForEach(Data[offset]); return offset; }
		::gpk::error_t			for_each		(const ::gpk::TFuncForEachConst  <T> & funcForEach, uint32_t offset, uint32_t stop)	const	{ for(stop = ::gpk::min(stop, Count); offset < stop; ++offset) funcForEach(Data[offset]); return offset; }
		::gpk::error_t			enumerate		(const ::gpk::TFuncEnumerate     <T> & funcForEach, uint32_t offset, uint32_t stop)			{ for(stop = ::gpk::min(stop, Count); offset < stop; ++offset) funcForEach(offset, Data[offset]); return offset; }
		::gpk::error_t			enumerate		(const ::gpk::TFuncEnumerateConst<T> & funcForEach, uint32_t offset, uint32_t stop)	const	{ for(stop = ::gpk::min(stop, Count); offset < stop; ++offset) funcForEach(offset, Data[offset]); return offset; }

		::gpk::error_t			find			(const FBool<T&>		& funcForEach, uint32_t offset = 0)			{ for(; offset < Count; ++offset) if(funcForEach(Data[offset])) return (::gpk::error_t)offset; return -1; }
		::gpk::error_t			find			(const FBool<const T&>	& funcForEach, uint32_t offset = 0)	const	{ for(; offset < Count; ++offset) if(funcForEach(Data[offset])) return (::gpk::error_t)offset; return -1; }
		::gpk::error_t			find			(const T & value, uint32_t offset = 0)						const	{ for(; offset < Count; ++offset) if(Data[offset] == value) return (::gpk::error_t)offset; return -1; }

		tplt<tpnm _tMax> ::gpk::error_t	max	(_tMax & maxFound, const FTransform<_tMax, const T &> & funcComparand, uint32_t offset = 0)	const	{ int32_t iMax = 0; for(; offset < Count; ++offset) { _tMax value = funcComparand(Data[offset]); if(value > maxFound) { iMax = offset; maxFound = value; } } return iMax; }
		tplt<tpnm _tMax> ::gpk::error_t	min	(_tMax & minFound, const FTransform<_tMax, const T &> & funcComparand, uint32_t offset = 0)	const	{ int32_t iMin = 0; for(; offset < Count; ++offset) { _tMax value = funcComparand(Data[offset]); if(value < minFound) { iMin = offset; minFound = value; } } return iMin; }
		tplt<tpnm _tMax> ::gpk::error_t	max	(const FTransform<_tMax, const T &> & funcComparand, uint32_t offset = 0)					const	{ _tMax maxFound; return max(maxFound, funcComparand, offset); }
		tplt<tpnm _tMax> ::gpk::error_t	min	(const FTransform<_tMax, const T &> & funcComparand, uint32_t offset = 0)					const	{ _tMax minFound; return min(minFound, funcComparand, offset); }
	}; // view<>

	tplt<tpnm T>	using	view_array	= ::gpk::view<T>;
	tplt<tpnm T>	using	view1d		= ::gpk::view<T>;
	tplt<tpnm T>	using	v1			= ::gpk::view<T>;

	tplt<tpnm T>	ndstinx	uint32_t	size		(const ::gpk::view<T> & viewToTest)	noexcept	{ return viewToTest.size();			}
	tplt<tpnm T>	ndstinx	uint32_t	byte_count	(const ::gpk::view<T> & viewToTest)	noexcept	{ return viewToTest.byte_count();	}

#pragma pack(pop)

	typedef	::gpk::view<::gpk::uchar_t		>	vuc, vuchar;
	typedef	::gpk::view<char				>	vc, vchar;
	typedef	::gpk::view<float				>	vf32;
	typedef	::gpk::view<double				>	vf64;
	typedef	::gpk::view<uint8_t				>	vu8;
	typedef	::gpk::view<uint16_t			>	vu16;
	typedef	::gpk::view<uint32_t			>	vu32;
	typedef	::gpk::view<uint64_t			>	vu64;
	typedef	::gpk::view<int8_t				>	vi8;
	typedef	::gpk::view<int16_t				>	vi16;
	typedef	::gpk::view<int32_t				>	vi32;
	typedef	::gpk::view<int64_t				>	vi64;

	typedef	::gpk::view<const ::gpk::uchar_t>	vcuc, vcuchar;
	typedef	::gpk::view<const char			>	vcc, vcchar;
	typedef	::gpk::view<const float			>	vcf32;
	typedef	::gpk::view<const double		>	vcf64;
	typedef	::gpk::view<const uint8_t		>	vcu8;
	typedef	::gpk::view<const uint16_t		>	vcu16;
	typedef	::gpk::view<const uint32_t		>	vcu32;
	typedef	::gpk::view<const uint64_t		>	vcu64;
	typedef	::gpk::view<const int8_t		>	vci8;
	typedef	::gpk::view<const int16_t		>	vci16;
	typedef	::gpk::view<const int32_t		>	vci32;
	typedef	::gpk::view<const int64_t		>	vci64;

// Use this to initialize a constexpr vcs from a string literal
#define GPK_CXS(constexpr_string_literal) ::gpk::vcc{uint32_t(sizeof(constexpr_string_literal) - 1), constexpr_string_literal}

	typedef	::gpk::view<vuc		>	vvuc;
	typedef	::gpk::view<vc		>	vvc;
	typedef	::gpk::view<vu8		>	vvu8;
	typedef	::gpk::view<vu16	>	vvu16;
	typedef	::gpk::view<vu32	>	vvu32;
	typedef	::gpk::view<vu64	>	vvu64;
	typedef	::gpk::view<vi8		>	vvi8;
	typedef	::gpk::view<vi16	>	vvi16;
	typedef	::gpk::view<vi32	>	vvi32;
	typedef	::gpk::view<vi64	>	vvi64;
	typedef	::gpk::view<vf32	>	vvf32;
	typedef	::gpk::view<vf64	>	vvf64;

	typedef	::gpk::view<const vcuc	>	vcvcuc;
	typedef	::gpk::view<const vcc	>	vcvcc;
	typedef	::gpk::view<const vcu8	>	vcvcu8;
	typedef	::gpk::view<const vcu16	>	vcvcu16;
	typedef	::gpk::view<const vcu32	>	vcvcu32;
	typedef	::gpk::view<const vcu64	>	vcvcu64;
	typedef	::gpk::view<const vci8	>	vcvci8;
	typedef	::gpk::view<const vci16	>	vcvci16;
	typedef	::gpk::view<const vci32	>	vcvci32;
	typedef	::gpk::view<const vci64	>	vcvci64;
	typedef	::gpk::view<const vcf32	>	vcvcf32;
	typedef	::gpk::view<const vcf64	>	vcvcf64;

	typedef	::gpk::view<vcuc	>	vvcuc;
	typedef	::gpk::view<vcc		>	vvcc;
	typedef	::gpk::view<vcu8	>	vvcu8;
	typedef	::gpk::view<vcu16	>	vvcu16;
	typedef	::gpk::view<vcu32	>	vvcu32;
	typedef	::gpk::view<vcu64	>	vvcu64;
	typedef	::gpk::view<vci8	>	vvci8;
	typedef	::gpk::view<vci16	>	vvci16;
	typedef	::gpk::view<vci32	>	vvci32;
	typedef	::gpk::view<vci64	>	vvci64;
	typedef	::gpk::view<vcf32	>	vvcf32;
	typedef	::gpk::view<vcf64	>	vvcf64;

	stacxpr	::gpk::vcc	VCC_NULL		= {4, "null"};
	stacxpr	::gpk::vcc	VCC_TRUE		= {4, "true"};
	stacxpr	::gpk::vcc	VCC_FALSE		= {5, "false"};

	ndstinx	::gpk::vcc	bool2vcc		(bool b)		{ return b ? ::gpk::VCC_TRUE : ::gpk::VCC_FALSE; }
	ndstinx	const char*	bool2char		(bool b)		{ return b ? ::gpk::VCC_TRUE.begin() : ::gpk::VCC_FALSE.begin(); }
	ndstinx	uint8_t		bool2u8			(bool b)		{ return b ? 1 : 0; }
	ndstinx	uint8_t		bool2i8			(bool b)		{ return b ? 1 : 0; }
	stainli	const char*	bool2char		(bool b, ::gpk::vcc & output)	{ return (output = b ? ::gpk::VCC_TRUE : ::gpk::VCC_FALSE).begin(); }
	stainli	bool		vcc2bool		(::gpk::vcc b)	{ return b.size() && b != VCC_FALSE; }


	struct view_string : public view<char> {
		inlcxpr			view_string				()													= default;
		inlcxpr			view_string				(const view<char> & other)				noexcept	: view(other)				{}
						view_string				(char * inputString, uint32_t length)				: view(inputString, length)	{ Count = (length == (uint32_t)-1) ? (uint32_t)strlen(inputString) : length;					}
		tplt<u32 Len>	view_string				(char (&inputString)[Len])				noexcept	: view(inputString)			{ Count = (uint32_t)strnlen(inputString, (uint32_t)Len);								}
		tplt<u32 Len>	view_string				(char (&inputString)[Len], uint32_t length)			: view(inputString, length)	{ if(length == (uint32_t)-1) Count = (uint32_t)strnlen(inputString, (uint32_t)Len);	}
	};

	struct view_const_string : public view<const char> {
		using vcc			::begin;

		inlcxpr				view_const_string		()													: view(0, "") 				{}
		inlcxpr				view_const_string		(const view<const char> & other)		noexcept	: view(other)				{}
		tplt<u32 Len>		view_const_string		(const char (&inputString)[Len])		noexcept	: view(inputString)			{ Count = (uint32_t)strnlen(inputString, (uint32_t)Len);								}
		tplt<u32 Len>		view_const_string		(const char (&inputString)[Len], uint32_t length)	: view(inputString, length)	{ if(length == (uint32_t)-1) Count = (uint32_t)strnlen(inputString, (uint32_t)Len);	}
							view_const_string		(const char* inputString, uint32_t length)			: view(inputString ? inputString : "", length)	{
			if(length == (uint32_t)-1) 
				Count 				= (uint32_t)strlen(begin());
		}

		inlcxpr	operator	const char*				()	const	{ return begin(); }
	};

	typedef	::gpk::view_string					vs;
	typedef	::gpk::view_const_string			vcs;

	stainli gpk::vcs		str					(const gpk::vcs & arg)	{ return arg; } 
	stainli gpk::vcs		str					(const gpk::vs & arg)	{ return arg.cc(); } 

	typedef	::gpk::view<::gpk::vs			>	vvs;
	typedef	::gpk::view<::gpk::vcs			>	vvcs;
	typedef	::gpk::view<const ::gpk::vs		>	vcvs;
	typedef	::gpk::view<const ::gpk::vcs	>	vcvcs;

	stacxpr	::gpk::vcc		TRIM_CHARACTERS		= " \t\b\n\r";

	::gpk::error_t			rtrim				(::gpk::vcc & trimmed, const ::gpk::vcc & original, const ::gpk::vcc & characters = ::gpk::TRIM_CHARACTERS);
	::gpk::error_t			ltrim				(::gpk::vcc & trimmed, const ::gpk::vcc & original, const ::gpk::vcc & characters = ::gpk::TRIM_CHARACTERS);
	::gpk::error_t			trim				(::gpk::vcc & trimmed, const ::gpk::vcc & original, const ::gpk::vcc & characters = ::gpk::TRIM_CHARACTERS);
	stainli	::gpk::error_t	rtrim				(::gpk::vcc & trimmed) 	{ return rtrim(trimmed, trimmed); }
	stainli	::gpk::error_t	ltrim				(::gpk::vcc & trimmed) 	{ return ltrim(trimmed, trimmed); }
	stainli	::gpk::error_t	trim				(::gpk::vcc & trimmed) 	{ return trim(trimmed, trimmed); }

	tplt <tpnm T>
	::gpk::error_t			reverse				(::gpk::view<T> elements)													{
		const uint32_t				lastElement			= elements.size() - 1;
		for(uint32_t i = 0, swapCount = elements.size() / 2; i < swapCount; ++i) {
			T							old						= elements[i];
			elements[i]					= elements[lastElement - i];
			elements[lastElement - i]	= old;
		}
		return 0;
	}

	tplt<tpnm T>
	::gpk::error_t			find					(const T & valueToFind, const ::gpk::view<const T> & target, uint32_t offset = 0)		{
		for(uint32_t iOffset = offset, offsetStop = target.size(); iOffset < offsetStop; ++iOffset)
			if(valueToFind == target[iOffset])
				return (int32_t)iOffset;
		return -1;
	}

	tplt<tpnm T>
	::gpk::error_t					rfind					(const T & valueToFind, const ::gpk::view<const T> & target, int32_t offset = 0)		{
		for(uint32_t iOffset = target.size() - 1 - offset; iOffset < target.size(); --iOffset)
			if(valueToFind == target[iOffset])
				return iOffset;
		return -1;
	}

	tplt<tpnm T>
	::gpk::error_t					find_sequence_obj		(const ::gpk::view<T> & sequence, const ::gpk::view<T> & target, uint32_t offset = 0)	{
		for(int32_t iOffset = (int32_t)offset, offsetStop = ((int32_t)target.size() - sequence.size()) + 1; iOffset < offsetStop; ++iOffset) {
			bool								equal					= true;
			for(uint32_t iSequenceElement = 0; iSequenceElement < sequence.size(); ++iSequenceElement) {
				if(sequence[iSequenceElement] != target[iOffset + iSequenceElement]) {
					equal							= false;
					break;
				}
			}
			if(equal)
				return iOffset;
		}
		return -1;
	}

	tplt<tpnm T>
	::gpk::error_t					rfind_sequence_obj		(const ::gpk::view<T> & sequence, const ::gpk::view<T> & target, uint32_t offset = 0)	{
		for(int32_t iOffset = (int32_t)(target.size() - sequence.size() - offset); iOffset >= 0; --iOffset) {
			bool								equal					= true;
			for(uint32_t iSequenceElement = 0; iSequenceElement < sequence.size(); ++iSequenceElement) {
				if(sequence[iSequenceElement] != target[iOffset + iSequenceElement]) {
					equal							= false;
					break;
				}
			}
			if(equal)
				return iOffset;
		}
		return -1;
	}

	tplt<tpnm T>
	::gpk::error_t					find_sequence_pod		(const ::gpk::view<T> & sequence, const ::gpk::view<T> & target, uint32_t offset = 0)	{
		for(int32_t iOffset = (int32_t)offset, offsetStop = ((int32_t)target.size() - sequence.size()) + 1; iOffset < offsetStop; ++iOffset)
			if(0 == memcmp(sequence.begin(), &target[iOffset], sequence.size() * sizeof(T)))
				return iOffset;
		return -1;
	}

	tplt<tpnm T>
	::gpk::error_t					rfind_sequence_pod		(const ::gpk::view<T> & sequence, const ::gpk::view<T>& target, uint32_t offset = 0)	{
		for(int32_t iOffset = (int32_t)(target.size() - sequence.size() - offset); iOffset >= 0; --iOffset)
			if(0 == memcmp(sequence.begin(), &target[iOffset], sequence.size() * sizeof(T)))
				return iOffset;
		return -1;
	}

	stainli	::gpk::error_t			find_string				(const ::gpk::vcs & toFind, const ::gpk::vcc & target, uint32_t offset = 0) { return ::gpk::find_sequence_pod (toFind, target, offset); }
	stainli	::gpk::error_t			rfind_string			(const ::gpk::vcs & toFind, const ::gpk::vcc & target, uint32_t offset = 0) { return ::gpk::rfind_sequence_pod(toFind, target, offset); }

	tplt<tpnm T>
	::gpk::error_t					split					(const T & valueToFind, const ::gpk::view<T> & original, ::gpk::view<T> & left, ::gpk::view<T> & right) {
		const ::gpk::error_t				iValue					= ::gpk::find(valueToFind, original);
		if(0 > iValue) {
			left							= original;
			right							= {};
		}
		else {
			gpk_necs(original.slice(left, 0, iValue));
			const uint32_t						offsetRight				= iValue + 1;
			gpk_necs(original.slice(right, offsetRight, original.size() - offsetRight));
		}
		return iValue;
	}

	tplt<tpnm T>
	::gpk::error_t					splitAt					(const T & valueToFind, const ::gpk::view<T> & original, ::gpk::view<T> & left, ::gpk::view<T> & right) {
		const ::gpk::error_t				iValue					= ::gpk::find(valueToFind, original);
		if(0 > iValue) { // Read until the end unless fragment is found.
			left							= original;
			right							= {};
		}
		else {
			gpk_necs(original.slice(left, 0, (uint32_t)iValue));
			gpk_necs(original.slice(right, iValue, (uint32_t)original.size() - iValue));
		}
		return iValue;
	}

	// Returns the index of the start of the sequence if the latter found.
	tplt<tpnm T>
	::gpk::error_t					split					(const ::gpk::view<T> & sequenceToFind, const ::gpk::view<T> & original, ::gpk::view<T> & left, ::gpk::view<T> & right) {
		const ::gpk::error_t				iValue					= ::gpk::find_sequence_pod(sequenceToFind, original);
		if(0 > iValue) {
			left							= original;
			right							= {};
		}
		else {
			gpk_necs(original.slice(right, iValue + sequenceToFind.size()));
			gpk_necs(original.slice(left, 0, iValue));
		}
		return iValue;
	}

	tplt<tpnm T>
	inline	::gpk::error_t			split					(const ::gpk::view<T> & sequenceToFind, ::gpk::view<T> & inputOrLeft, ::gpk::view<T> & right) {
		return ::gpk::split(sequenceToFind, inputOrLeft, inputOrLeft, right);
	}

	tplt<tpnm T>
	::gpk::error_t					splitAt					(const ::gpk::view<T> & sequenceToFind, const ::gpk::view<T> & original, ::gpk::view<T> & left, ::gpk::view<T> & right) {
		const ::gpk::error_t				iValue					= ::gpk::find_sequence_pod(sequenceToFind, original);
		if(0 > iValue) { // Read until the end unless fragment is found.
			left							= original;
			right							= {};
		}
		else {
			gpk_necall(original.slice(left, 0, (uint32_t)iValue), "%s", "Invalid slice");
			gpk_necall(original.slice(right, iValue, (uint32_t)original.size() - iValue), "%s", "Invalid slice");
		}
		return iValue;
	}

	tplt<tpnm T>
	::gpk::error_t					max						(::gpk::view<T> input, T ** result) {
		ree_if(0 == input.size(), "%s", "Cannot get reference to max element of an empty array.");
		*result							= &input[0];
		int32_t								iMax					= 0;
		for(uint32_t iElement = 1; iElement < input.size(); ++iElement) {
			const T						& currentElement		= input[iElement];
			if(currentElement > **result) {
				*result							= &currentElement;
				iMax							= iElement;
			}
		}
		return iMax;
	}

	tplt<tpnm T>
	::gpk::error_t					min						(::gpk::view<T> input, T ** result) {
		ree_if(0 == input.size(), "%s", "Cannot get reference to max element of an empty array.");
		*result							= &input[0];
		int32_t								iMin					= 0;
		for(uint32_t iElement = 1; iElement < input.size(); ++iElement) {
			const T						& currentElement		= input[iElement];
			if(currentElement < **result) {
				*result							= &currentElement;
				iMin							= iElement;
			}
		}
		return iMin;
	}

	tplt<tpnm T>
	T&							max						(::gpk::view<T> input) {
		T								* result				= 0;
		gthrow_if(errored(::gpk::max(input, &result)), "%s", "");
		return *result;
	}

	tplt<tpnm T>
	T&							min						(::gpk::view<T> input) {
		T								* result				= 0;
		gthrow_if(errored(::gpk::min(input, &result)), "%s", "");
		return *result;
	}

	tplt<tpnm TNumber>	
	TNumber					sum						(::gpk::view<const TNumber> input)			{
		TNumber 				result 					= 0;
		input.for_each([&result](TNumber number) { result += number; });
		return result;
	}
	

#define be2le_16(number) ::gpk::reverse<uint8_t>({(uint8_t*)&number, 2})
#define be2le_32(number) ::gpk::reverse<uint8_t>({(uint8_t*)&number, 4})
#define be2le_64(number) ::gpk::reverse<uint8_t>({(uint8_t*)&number, 8})
} // namespace

#endif // GPK_ARRAY_VIEW_H_23627
